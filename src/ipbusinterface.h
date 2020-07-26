#ifndef IPBUSINTERFACE_H
#define IPBUSINTERFACE_H

#include "ipbusheaders.h"
const quint16 maxPacket = 368; //368 words, limit from ethernet MTU of 1500 bytes

class IPbusTarget: public QObject {
    Q_OBJECT
public:
    QString IPaddress = "172.20.75.180";
    QUdpSocket *qsocket = new QUdpSocket(this);

    StatusPacket statusPacket;
    QList<quint32 *> transactionsList; //will contain only pointers to data
    quint16 requestSize = 0, responseSize = 0; //values are measured in words
    quint32 request[maxPacket], response[maxPacket];
    char *pRequest = reinterpret_cast<char *>(request);
    char *pResponse = reinterpret_cast<char *>(response);
    quint32 dt[2]; //temporary data
    struct TypePM {
        union { //part0
            quint32 registers[36] = {0};
            char pointer[36 * wordSize];
            struct {
                struct RawTDCdata {			 //┐RAW_TDC_DATA
                    quint32					 //│
                        val1            : 7, //│
                                        : 1, //│40-4B
                        val2            : 6, //│
                                        :18; //│
                } RAW_TDC_DATA[12];
                struct ADCzeroLevel {		 //┐ADC0_ZEROLVL, ADC1_ZEROLVL
                    quint32					 //│
                        ADC0            :12, //│
                                        :20, //│4C-63
                        ADC1            :12, //│
                                        :20; //│
                } ZEROLVL[12];
            };
        };
    } PM;
    quint32 PMaddress = 0x200;

    IPbusTarget() {
        connect(qsocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, [=](QAbstractSocket::SocketError socketError) {emit networkError("Socket Error " + QString::number(socketError) + ": " + qsocket->errorString());});
        connect(qsocket, &QUdpSocket::connected, this, &IPbusTarget::requestStatus);
        connect(qsocket, &QUdpSocket::disconnected, this, &IPbusTarget::reinit);
        connect(this, &IPbusTarget::networkError, this, &IPbusTarget::error);
		connect(this, &IPbusTarget::IPbusError, this, &IPbusTarget::error);
		connect(this, &IPbusTarget::logicError, this, &IPbusTarget::error);
        qsocket->bind(QHostAddress::AnyIPv4, 50009);
    }

    ~IPbusTarget() {
        qsocket->disconnectFromHost();
    }

signals:
	void networkError(QString);
    void IPbusError(QString);
	void logicError(QString);
    void IPbusStatusOK();
    void statusDataReady();
    void successfulRead(quint8 nWords);
    void successfulWrite(quint8 nWords);

private:
    quint32 *masks(quint32 mask0, quint32 mask1) { //for convinient adding RMWbit transaction
        dt[0] = mask0; //for writing 0's: AND term
        dt[1] = mask1; //for writing 1's: OR term
        return dt;
    }

    void addTransaction(TransactionType type, quint32 address, quint32 *data, quint8 nWords = 1) {
        request[requestSize++] = quint32(TransactionHeader(type, nWords, transactionsList.size()));
        request[requestSize++] = address;
        switch (type) {
            case                read:
            case nonIncrementingRead:
            case   configurationRead:
                responseSize += 1 + nWords;
                break;
            case                write:
            case nonIncrementingWrite:
            case   configurationWrite:
                for (quint8 i=0; i<nWords; ++i) request[requestSize++] = data[i];
                ++responseSize;
                break;
            case RMWbits:
                request[requestSize++] = data[0];
                request[requestSize++] = data[1];
                responseSize += 2;
                break;
            case RMWsum:
                request[requestSize++] = *data; //addend
                responseSize += 2;
        }
        if (requestSize > maxPacket || responseSize > maxPacket) {
            emit IPbusError("packet size exceeded");
            return;
        } else transactionsList.append(data);
    }

	bool transceive(bool analyze = true) { //send request, wait for response, receive it and check correctness
		qint32 n = qint32(qsocket->write(pRequest, requestSize * wordSize));
		if (n < 0) {
			emit networkError("Socket write error: " + qsocket->errorString());
			return false;
		} else if (n != requestSize * wordSize) {
			emit networkError("Sending packet failed");
			return false;
		} else if (!qsocket->waitForReadyRead(100)) {
			emit networkError("no response");
			return false;
		} else {
			n = qint32(qsocket->read(pResponse, responseSize * wordSize));
			qint32 m = qint32(qsocket->bytesAvailable());
			if (m > 0) qsocket->readAll();
			if (n < 0) {
				emit networkError("Socket read error: " + qsocket->errorString());
				return false;
			} else if (n == 0) {
				emit networkError("empty response, no IPbus");
				return false;
			} else if (response[0] != request[0] || n % wordSize > 0) {
				emit networkError(QString::asprintf(": incorrect response (%d bytes)", n + m));
				return false;
			} else {
				responseSize = quint16(n / wordSize);
/*//debug print
				printf("request:\n");
				for (quint16 i=0; i<requestSize; ++i)  printf("%08X\n", request[i]);
				printf("        response:\n");
				for (quint16 i=0; i<responseSize; ++i) printf("        %08X\n", response[i]);
				printf("\n"); //*/
				if (analyze)
					return analyzeResponse();
				else
					return true;
			}
		}
	}

	bool analyzeResponse() { //analyze response
		for (quint16 j = 1, n = 0; !transactionsList.isEmpty() && j < responseSize; ++j, ++n) {
		   TransactionHeader th = response[j];
		   if (th.ProtocolVersion != 2 || th.TransactionID != n) {
			   emit IPbusError("unexpected transaction header");
			   return false;
		   }
		   quint8 nWords = th.Words;
		   quint32 *data = transactionsList.takeFirst();
		   if (nWords > 0) {
			   switch (th.TypeID) {
				   case                read:
				   case nonIncrementingRead:
				   case   configurationRead:
					   if (j + nWords >= responseSize) { //response too short to contain nWords values
						   nWords = quint8(responseSize - j - 1);
						   for (quint8 i=0; i<nWords; ++i) data[i] = response[++j];
						   emit successfulRead(nWords);
						   emit IPbusError("read transaction truncated");
						   return false;
					   } else {
                           if(data!=nullptr) for (quint8 i=0; i<nWords; ++i) data[i] = response[++j];
						   emit successfulRead(nWords);
					   }
					   break;
				   case RMWbits:
				   case RMWsum :
					   if (th.Words != 1) {
						   emit IPbusError("wrong RMW transaction");
						   return false;
					   }
					   ++j;     //skipping received value
					   emit successfulRead(1);
					   /* fall through */ //[[fallthrough]];
				   case                write:
				   case nonIncrementingWrite:
				   case   configurationWrite:
					   emit successfulWrite(nWords);
					   break;
				   default:
					   emit IPbusError("unknown transaction type");
					   return false;
			   }
		   }
		   if (th.InfoCode != 0) {
			   emit IPbusError(th.infoCodeString());
			   return false;
		   }
		}
		resetTransactions();
		return true;
	}

private slots:
    void resetTransactions() { //return to initial (default) state
        request[0] = quint32(PacketHeader(control, 0));
        requestSize = 1;
        responseSize = 1;
        transactionsList.clear();
    }

    void error(QString st) {
		resetTransactions();
        printf("Error: %s\n", qPrintable(st));
	}

public slots:
    void reinit() {
        if (qsocket->state() == QAbstractSocket::ConnectedState)
            qsocket->disconnectFromHost();
        else {
            qsocket->connectToHost(IPaddress, 50001, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
        }
    }

    void requestStatus() {
        requestSize = 16; //size of statusPacket in words
        memcpy(pRequest, &statusPacket, sizeof(statusPacket));
        responseSize = requestSize;
        if (transceive(false)) {
            //nextPacketID = PacketHeader(qFromBigEndian(response[3])).PacketID;
            emit IPbusStatusOK();
            printf("%s: IPbus OK\n", qPrintable(IPaddress));
            resetTransactions();
            writeRegister(0x400, PMaddress + 0x7F);
        }
    }

    void sync() { //update all parameters
//        addTransaction(read, PMaddress + 0x40, PM.registers, 36);
//        writeRegister(0x400, PMaddress + 0x7F);
    }

    void writeRegister(quint32 data, quint32 address) {
        addTransaction(write, address, &data, 1);
        transceive();
    }

	void setBit(quint8 n, quint32 address) {
		addTransaction(RMWbits, address, masks(0xFFFFFFFF, 1 << n));
		if (transceive()) sync();
	}

	void clearBit(quint8 n, quint32 address) {
		addTransaction(RMWbits, address, masks(~(1 << n), 0x00000000));
		if (transceive()) sync();
	}

    void writeNbits(quint32 data, quint32 address, quint8 nbits = 16, quint8 shift = 0) {
        quint32 mask = (1 << nbits) - 1; //e.g. 0x00000FFF for nbits==12
        addTransaction(RMWbits, address, masks( ~quint32(mask << shift), quint32((data & mask) << shift) ));
		if (transceive()) sync();
    }

    //----------------------------------------------------

    void readRegister(quint32* data, quint32 address){
        addTransaction(read, address, data, 1);
        transceive();
    }

    void zero_read_fast(){
//        quint32 not_iszero;
//        do{
//            addTransaction(read, PMaddress + 0x7F, &not_iszero, 1);
            addTransaction(read, PMaddress + 0xd, PM.registers+12, 24);
            transceive();
//            qDebug() << not_iszero;
//        }while(!not_iszero);
//        writeRegister(0x400, PMaddress + 0x7F);
    }

    void read_raw_tdc(){
        addTransaction(read, PMaddress + 0x40, PM.registers, 12);
        transceive();
    }

};

#endif // IPBUSINTERFACE_H
