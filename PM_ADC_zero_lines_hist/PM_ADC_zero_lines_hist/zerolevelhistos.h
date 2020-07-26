#ifndef ZEROLEVELHISTOS_H
#define ZEROLEVELHISTOS_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QStateMachine>


#include "../../src/ipbusinterface.h"
#include "../../src/qcustomplot.h"


namespace Ui {
class ZeroLevelHistos;
}

class ZeroLevelHistos : public QMainWindow
{
    Q_OBJECT

public:
    explicit ZeroLevelHistos(QWidget *parent = nullptr);
    ~ZeroLevelHistos();

private:
    Ui::ZeroLevelHistos *ui;

    inline bool validIPaddress(QString text) {return QRegExp("(0?0?[1-9]|0?[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.(([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\\.){2}(0?0?[1-9]|0?[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])").exactMatch(text);}

    IPbusTarget FEE;
    QMap<quint16, quint16> ADC0[12], ADC1[12];
    quint32 n = 1000;

    QFile out;
    QTextStream fout;

    QVector<QCustomPlot*> hh;    // 12 histos
    QVector<QCustomPlot*> hg;    // 12 histos
    QVector<QCPBars*> b;        // 12 bars for ADC0_ZEROLVL
    QVector<QCPBars*> b1;       // 12 bars for ADC1_ZEROLVL

    QVector<double> allan_sigma0[12];
    QVector<double> allan_sigma1[12];

    QVector<quint16> arr_adc0[12];         // array for sample all inputs
    QVector<quint16> arr_adc1[12];         // array for sample all inputs
    QVector<quint64> time_interval;

    QTimer* responseTimer = new QTimer(this);
    QTimer* workTimer = new QTimer(this);
    QElapsedTimer t;

    QStateMachine* machine = new QStateMachine(this);
    QState* selecting = new QState(machine);
    QState* processing = new QState(machine);
        QState* sampling = new QState(processing);
        QState* storing = new QState(processing);
        QState* calculating = new QState(processing);

    QAction* readfileAction = new QAction("Read files...",this);

    // for Allan diviation
    void calc_variance(const QVector<double>&, quint8 ,quint8, QVector<double>&);

    // Technical functions
    void rescaleRange(const QCPRange&);
    void setup_hist();

signals:
    void samplingReady();
    void storingReady();
    void calculatingReady();

private slots:
    void changeIP();
    void recheck();
    void on_spinBox_editingFinished();

    void read_files();

//    void on_startBtn_clicked();
//    void on_stopBtn_clicked();
};

#endif // ZEROLEVELHISTOS_H
