#include "rawtdchistos.h"
#include "ui_rawtdchistos.h"

RawTDChistos::RawTDChistos(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RawTDChistos)
{
    ui->setupUi(this);

    // Menu
    QMenu *networkMenu = menuBar()->addMenu("&Network");
    networkMenu->addAction("&Recheck target status", this, SLOT(recheck()));
    networkMenu->addAction("&Change target IP adress...", this, SLOT(changeIP()));

    // File
    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Read files ...", this, &RawTDChistos::read_files,tr("Ctrl+f"));

    // General preparations
    for(quint8 i=0; i<12; i++){
        arr_raw_tdc[i].reserve(ui->spinBox->maximum());
        ticks[i].reserve(50);
        labels[i].reserve(50);
    }

    setup_hist();
    workTimer->setSingleShot(0);
    workTimer->setInterval(0);

    // IPbusTarget preparation
    connect(&FEE,&IPbusTarget::IPbusStatusOK,this,[=](){
        responseTimer->stop();
        statusBar()->showMessage(FEE.IPaddress + ": connected");
    });

    responseTimer->setSingleShot(true);
    responseTimer->callOnTimeout(this, [=]() {
        FEE.qsocket->disconnect(SIGNAL(readyRead()));
        statusBar()->showMessage(FEE.IPaddress + ": no response");
    });


    // Machine logic
    machine->setInitialState(selecting);

    selecting->addTransition(ui->startBtn,&QPushButton::clicked,processing);
    processing->addTransition(ui->stopBtn,&QPushButton::clicked,selecting);
    processing->addTransition(&FEE,&IPbusTarget::networkError,selecting);

    processing->setInitialState(sampling);
    sampling->addTransition(this,&RawTDChistos::samplingReady,storing);
    storing->addTransition(this,&RawTDChistos::storingReady,calculating);

    // Selecting state
    connect(selecting,&QState::entered,this,[=](){
        ui->lineEdit->setEnabled(1);
        ui->spinBox->setEnabled(1);
        ui->startBtn->setEnabled(1);
        ui->stopBtn->setEnabled(0);
        statusBar()->showMessage("Selecting");
        qDebug() << "Selecting";

    });

    // Processing state

    connect(processing,&QState::entered,this,[=](){
        for(quint8 i=0;i<12; ++i){

            b0[i]->data().data()->clear();
            b[0][i]->data().data()->clear();
            b[1][i]->data().data()->clear();
            ticks[i].clear();
            labels->clear();

            arr_raw_tdc[i].clear();
            TDC[i].clear();
        }
    });

    connect(sampling,&QState::entered,this,[=](){
        ui->lineEdit->setEnabled(0);
        ui->spinBox->setEnabled(0);
        ui->startBtn->setEnabled(0);
        ui->stopBtn->setEnabled(1);
        statusBar()->showMessage("Sampling",500);
        qDebug() << "Sampling";

        QString PMname = ui->lineEdit->text();
        if (QRegExp("[AC][0-9]").exactMatch(PMname)) {
            FEE.PMaddress = PMname.at(0) == 'C' ? 0x1600 : 0x200;
            FEE.PMaddress += (PMname.at(1).toLatin1() - '0') * 0x200;
        }
        else if(PMname == "00"){
            FEE.IPaddress = "172.20.75.175";
            FEE.PMaddress = 0x00000000;
            FEE.reinit();
        }
        n = ui->spinBox->value();

        workTimer->callOnTimeout([=](){
            if(n--){
                FEE.read_raw_tdc();
                for (quint8 i=0; i<12; ++i) {
                    arr_raw_tdc[i].push_back(FEE.PM.RAW_TDC_DATA[i].val1 + 0x100*FEE.PM.RAW_TDC_DATA[i].val2);
                }
                statusBar()->showMessage(tr("%1/%2").arg(ui->spinBox->value()-n).arg(ui->spinBox->value()));
            }
            else{
                workTimer->stop();
                emit samplingReady();
            }
        });

        statusBar()->showMessage(QString::asprintf("Reading %d samples...", n));
        workTimer->start();

    });

    connect(sampling,&QState::exited,this,[=](){
        workTimer->disconnect();
    });

    connect(storing,&QState::entered,this,[=](){

        statusBar()->showMessage("Start storing ...",500);
        qDebug() << "Storing";
        for(quint8 i=0;i<12;++i){
            for(quint16 v : arr_raw_tdc[i]){ ++TDC[i][v]; }
        }

        out.setFileName("results_tdc.txt");
        out.open(QIODevice::WriteOnly);
        fout.setDevice(&out);

        for(qint8 i=0;i<12;++i){
            fout << endl;
            fout << "Ch:\t" << i+1 << endl;
            fout << "T:\t" << "TDC_RAW" << endl;
            fout << "Value\tCounts" << endl;

            for(quint16 j=0; j< TDC[i].keys().size();++j){
                b0[i]->addData(j+1,TDC[i][TDC[i].keys()[j]]);
                ticks[i].push_back(TDC[i].keys()[j]);
                fout << hex << TDC[i].keys()[j] << "\t" << dec << TDC[i][TDC[i].keys()[j]] << endl;
            }

            fout << endl;

            h1[i]->rescaleAxes();
            h1[i]->replot();
        }
        out.close();


        out.setFileName("results_tdc_lsb.txt");
        out.open(QIODevice::WriteOnly);
        fout.setDevice(&out);

        for(quint8 i=0; i<12; ++i){
            fout << "Ch:\t" << i+1 << endl;
            fout << "T:\t" << "TDC_RAW_LSByte" << endl;
            fout << "Time\tVal2\tVal1"<< endl;

            foreach (quint16 k, TDC[i].keys()) {
                if(k&1<<10) b[1][i]->addData(k&0x7f,TDC[i][k]);
                else    b[0][i]->addData(k&0x7f,TDC[i][k]);

                fout << QString::asprintf( "%d\t%X\t%d\n", (k&1<<10)? 1:0, quint8(k), TDC[i][k]);
            }

            fout << endl;

            h2[i]->rescaleAxes();
            h2[i]->replot();
        }

        out.close();

        emit storingReady();
    });

    connect(calculating,&QState::entered,this,[=](){
        emit ui->stopBtn->clicked();
    });


    FEE.IPaddress = "127.0.0.1";
    machine->start();
    setWindowState(Qt::WindowMaximized);
    recheck();


}

RawTDChistos::~RawTDChistos()
{
    for(quint8 i=0; i<12; i++){
        h1[i]->clearPlottables();
        h2[i]->clearPlottables();

        arr_raw_tdc[i].squeeze();
    }
    delete ui;
}

void RawTDChistos::setup_hist()
{

    h1.reserve(12);
    h1.push_back(ui->h1);
    h1.push_back(ui->h2);
    h1.push_back(ui->h3);
    h1.push_back(ui->h4);
    h1.push_back(ui->h5);
    h1.push_back(ui->h6);
    h1.push_back(ui->h7);
    h1.push_back(ui->h8);
    h1.push_back(ui->h9);
    h1.push_back(ui->h10);
    h1.push_back(ui->h11);
    h1.push_back(ui->h12);

    h2.reserve(12);
    h2.push_back(ui->h1_2);
    h2.push_back(ui->h2_2);
    h2.push_back(ui->h3_2);
    h2.push_back(ui->h4_2);
    h2.push_back(ui->h5_2);
    h2.push_back(ui->h6_2);
    h2.push_back(ui->h7_2);
    h2.push_back(ui->h8_2);
    h2.push_back(ui->h9_2);
    h2.push_back(ui->h10_2);
    h2.push_back(ui->h11_2);
    h2.push_back(ui->h12_2);

    b0.resize(12);
    b[0].resize(12);
    b[1].resize(12);

    QFont legendFont = font();
    legendFont.setPointSize(7);

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    fixedTicker->setTickStep(1.0);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);


    for (quint8 i=0;i<12;++i) {

        b0[i] = new QCPBars(h1[i]->xAxis,h1[i]->yAxis);
        b0[i]->setBrush(QColor(QColor(0,0,0,220)));
        b0[i]->setPen(Qt::NoPen);
        b0[i]->setWidth(1);
        b0[i]->setStackingGap(0);
        b0[i]->setName("LSB + 0 (10th bit)");


        b[0][i] = new QCPBars(h2[i]->xAxis,h2[i]->yAxis);
        b[0][i]->setBrush(QColor(QColor(200,0,0,200)));
        b[0][i]->setPen(Qt::NoPen);
        b[0][i]->setWidth(1);
        b[0][i]->setStackingGap(0);
        b[0][i]->setName("LSB + 0 (10th bit)");

        b[1][i] = new QCPBars(h2[i]->xAxis,h2[i]->yAxis);
        b[1][i]->setBrush(QColor(QColor(0,0,200,200)));
        b[1][i]->setWidth(1);
        b[1][i]->setPen(Qt::NoPen);
        b[1][i]->setStackingGap(0);
//        b[1][i]->moveAbove(b[i]);
        b[1][i]->setName("LSB + 1 (10th bit)");

        h1[i]->plotLayout()->insertRow(0);
        h1[i]->plotLayout()->addElement(0,0, new QCPTextElement(h1[i],tr("Channel %1").arg(i+1)));
        h1[i]->xAxis->setTicker(fixedTicker);
        h1[i]->setInteractions(QCP::iRangeZoom|QCP::iRangeDrag);
        h1[i]->axisRect()->setRangeDrag(Qt::Horizontal);
        h1[i]->axisRect()->setRangeZoom(Qt::Horizontal);
//        h1[i]->autoAddPlottableToLegend();
//        h1[i]->legend->setVisible(1);

//        h1[i]->legend->setFont(legendFont);
//        h1[i]->legend->setIconSize(10,10);
//        h1[i]->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignTop);
//        h1[i]->axisRect()->insetLayout()->setMinimumMargins(QMargins(0,0,0,0));
//        h1[i]->axisRect()->insetLayout()->setMargins(QMargins(0,0,0,0));

        ticks[i] << 1;
        labels[i] << "00\n01";
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(ticks[i], labels[i]);
        h1[i]->xAxis->setTicker(textTicker);
//        h1[i]->xAxis->setTickLabelRotation(60);
        h1[i]->xAxis->setSubTicks(false);
//        h1[i]->xAxis->setTickLength(0, 4);


        connect(h1[i]->xAxis,static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged),this,&RawTDChistos::rescaleRange);

        h2[i]->plotLayout()->insertRow(0);
        h2[i]->plotLayout()->addElement(0,0, new QCPTextElement(h2[i],tr("Channel %1").arg(i+1)));
        h2[i]->xAxis->setTicker(fixedTicker);
        h2[i]->setInteractions(QCP::iRangeZoom|QCP::iRangeDrag);
        h2[i]->axisRect()->setRangeDrag(Qt::Horizontal);
        h2[i]->axisRect()->setRangeZoom(Qt::Horizontal);
        h2[i]->autoAddPlottableToLegend();
        h2[i]->legend->setVisible(1);

        h2[i]->legend->setFont(legendFont);
        h2[i]->legend->setIconSize(10,10);
        h2[i]->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignTop);
        h2[i]->axisRect()->insetLayout()->setMinimumMargins(QMargins(0,0,0,0));
        h2[i]->axisRect()->insetLayout()->setMargins(QMargins(0,0,0,0));
        connect(h2[i]->xAxis,static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged),this,&RawTDChistos::rescaleRange);
    }

}


void RawTDChistos::on_spinBox_editingFinished()
{
    ui->spinBox->setValue(pow(2,round(std::log2(ui->spinBox->value()))));
}

void RawTDChistos::rescaleRange(const QCPRange& newRange)
{
    bool fr=false;
    quint16 upper = 0;
    quint16 upper1 = 0;

    for(quint8 i=0;i<12;++i){
        if(sender() == h2[i]->xAxis){
            if(newRange.upper > 127) h2[i]->xAxis->setRangeUpper(127);
            if(newRange.lower < 0) h2[i]->xAxis->setRangeLower(0);

            upper=static_cast<QCPBars*>(h2[i]->plottable(0))->data().data()->valueRange(fr,QCP::sdBoth,newRange).upper;
            upper1=static_cast<QCPBars*>(h2[i]->plottable(1))->data().data()->valueRange(fr,QCP::sdBoth,newRange).upper;
            if((upper+upper1) > h2[i]->yAxis->range().upper){
                h2[i]->yAxis->rescale();
                h2[i]->yAxis->setRange(0,h2[i]->yAxis->range().upper*1.1);
            } else h2[i]->yAxis->setRange(0,(upper+upper1)*1.1);

            return;
        }
        else if(sender() == h1[i]->xAxis){
//            if(newRange.upper > 127) h1[i]->xAxis->setRangeUpper(127);
//            if(newRange.lower < 0) h1[i]->xAxis->setRangeLower(0);

            upper=static_cast<QCPBars*>(h1[i]->plottable(0))->data().data()->valueRange(fr,QCP::sdBoth,newRange).upper;
//            upper1=static_cast<QCPBars*>(h1[i]->plottable(1))->data().data()->valueRange(fr,QCP::sdBoth,newRange).upper;
//            if((upper) > h1[i]->yAxis->range().upper){
//                h1[i]->yAxis->rescale();
//                h1[i]->yAxis->setRange(0,h1[i]->yAxis->range().upper*1.1);
//            } else
                h1[i]->yAxis->setRange(0,(upper)*1.1);

            return;
        }

    }
}


void RawTDChistos::changeIP() {
    bool ok;
    QString text = QInputDialog::getText(this, "Changing target IP address", "Enter new address", QLineEdit::Normal, FEE.IPaddress, &ok);
    if (ok && !text.isEmpty()) {
        if (validIPaddress(text)) {
            FEE.IPaddress = text;
            recheck();
        } else statusBar()->showMessage(text + ": invalid IP address");
    }
}

void RawTDChistos::recheck() {
    statusBar()->showMessage(FEE.IPaddress + ": status requested...");

    responseTimer->start(500);
    FEE.reinit();

}



void RawTDChistos::read_files()
{
    statusBar()->showMessage("Reading file ...");

    for(quint8 i=0;i<12; ++i){
        b0[i]->data().data()->clear();
        b[0][i]->data().data()->clear();
        b[0][i]->data().data()->clear();
        ticks[i].clear();
        labels->clear();

//        arr_raw_tdc[i].clear();
//        TDC[i].clear();
    }


    quint32 cnt=0;

    QFile in;
    in.setFileName("results_tdc.txt");

    if ( !in.open(QFile::ReadOnly | QFile::Text) ) {
        qDebug() << "File not exists";
    }
    else {
        quint8 idx;
        quint16 cnt =1;
        QStringList wordList;

        while (!in.atEnd()) {
            QString line = in.readLine();
            wordList = line.split("\t");

            if(line.length()<=1) continue;
            else if(wordList[0].left(2) == "Ch"){
                cnt = 1;
                idx = wordList[1].toUShort()-1;
                continue;
            }
            else if(wordList[0].left(2) == "T:") {
                if(wordList[1].left(3)=="TDC") {in.readLine();}
                else {
                    qDebug() << "Error reading target's name";
                    break;
                }
                continue;
            }
            else{
                b0[idx]->addData(cnt++,wordList[1].toUInt());
                ticks[idx].push_back(wordList[0].toUInt(&ok,16));
            }
        }
        in.close();
    }

    in.setFileName("results_tdc_lsb.txt");
    cnt = 0;

    if ( !in.open(QFile::ReadOnly | QFile::Text) ) {
        qDebug() << "File not exists";
    }
    else {
        quint8 idx;
        QStringList wordList;
        while (!in.atEnd()) {
            QString line = in.readLine();
            wordList = line.split("\t");

            if(line.length()<=1) continue;
            else if(wordList[0].left(2) == "Ch"){
                idx = wordList[1].toUShort()-1;
                continue;
            }
            else if(wordList[0].left(2) == "T:") {
                if (wordList[1].left(3)=="TDC") {}
                else {
                    qDebug() << "Error reading target's name" << wordList[1].left(4);
                    break;
                }
                QString line = in.readLine();
                continue;
            }
            else{
                if(wordList[0].toUInt()) b[1][idx]->addData(wordList[1].toUInt(&ok,16),wordList[2].toUInt());
                else b[0][idx]->addData(wordList[1].toUInt(&ok,16),wordList[2].toUInt());
            }
        }
        in.close();
    }

    prepare_labels();

    for(qint8 i=0;i<12;++i){

        h2[i]->rescaleAxes();
        h2[i]->replot();
    }


}

void RawTDChistos::prepare_labels()
{
    for(quint8 i=0; i<12; i++){
//        qDebug() << i << ticks[i].size();

//        qDebug() << ticks[i];

        labels[i].clear();
        for(double el : ticks[i]){
            qDebug() << "=" << hex << quint16(el) << (quint16(el)>> 8) << quint8(el);
            labels[i].push_back(QString::asprintf("%02X\n%02X",quint16(el) >> 8, quint8(el)));
        }
        qDebug() << labels[i];

        ticks[i].clear();
        for(quint16 j=1;j<=labels[i].size();++j){
            ticks[i] << j;
        }
        qDebug() << ticks[i];

        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(ticks[i], labels[i]);
        h1[i]->xAxis->setTicker(textTicker);
        h1[i]->rescaleAxes();
        h1[i]->replot();

    }



}
