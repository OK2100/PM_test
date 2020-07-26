#ifndef RAWTDCHISTOS_H
#define RAWTDCHISTOS_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QStateMachine>

#include "../../src/ipbusheaders.h"
#include "../../src/ipbusinterface.h"
#include "../../src/qcustomplot.h"


namespace Ui {
class RawTDChistos;
}

class RawTDChistos : public QMainWindow
{
    Q_OBJECT

public:
    explicit RawTDChistos(QWidget *parent = nullptr);
    ~RawTDChistos();

private:
    Ui::RawTDChistos *ui;

    inline bool validIPaddress(QString text) {return QRegExp("(0?0?[1-9]|0?[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.(([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\\.){2}(0?0?[1-9]|0?[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])").exactMatch(text);}

    IPbusTarget FEE;

    bool ok;

    QMap<quint16, quint16> TDC[12];
    quint32 n = 1000;

    QFile out;
    QTextStream fout;

    QVector<QCustomPlot*> h1;    // 12 histos
    QVector<QCustomPlot*> h2;    // 12 histos

    QVector<QCPBars*> b0;
    QVector<QCPBars*> b[2];        // 12 bars for LSB + 0; 12 bars for LSB + 1

    QVector<quint16> arr_raw_tdc[12];         // array for sample all inputs


    QStateMachine* machine = new QStateMachine(this);
    QState* selecting = new QState(machine);
    QState* processing = new QState(machine);
        QState* sampling = new QState(processing);
        QState* storing = new QState(processing);
        QState* calculating = new QState(processing);

    QAction* readfileAction = new QAction("Read files...",this);

    QTimer* responseTimer = new QTimer(this);
    QTimer* workTimer = new QTimer(this);

    QVector<double> ticks[12];
    QVector<QString> labels[12];

    // Technical functions
    void rescaleRange(const QCPRange&);
    void setup_hist();
    void prepare_labels();

private slots:
    void on_spinBox_editingFinished();
    void changeIP();
    void recheck();

    void read_files();

signals:
    void samplingReady();
    void storingReady();
    void calculatingReady();





};

#endif // RAWTDCHISTOS_H
