#include "zerolevelhistos.h"
#include "ui_zerolevelhistos.h"

ZeroLevelHistos::ZeroLevelHistos(QWidget *parent) :
    QMainWindow(parent)
    ,ui(new Ui::ZeroLevelHistos)
{
    ui->setupUi(this);

        // Menu
        QMenu *networkMenu = menuBar()->addMenu("&Network");
        networkMenu->addAction("&Recheck target status", this, SLOT(recheck()));
        networkMenu->addAction("&Change target IP adress...", this, SLOT(changeIP()));

        // File
        QMenu *fileMenu = menuBar()->addMenu("&File");
        fileMenu->addAction("&Read files ...", this, &ZeroLevelHistos::read_files,tr("Ctrl+f"));

        // General preparations
        for(quint8 i=0; i<12; i++){
            arr_adc0[i].reserve(ui->spinBox->maximum());
            arr_adc1[i].reserve(ui->spinBox->maximum());
            allan_sigma0[i].reserve(20);
            allan_sigma1[i].reserve(20);
        }
        time_interval.reserve(ui->spinBox->maximum());

        setup_hist();
        workTimer->setSingleShot(0);
        workTimer->setInterval(0);

//        readfileAction->setShortcut(tr("Ctrl+r"));
//        connect(readfileAction,&QAction::triggered,this,&ZeroLevelHistos::read_files);

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
        sampling->addTransition(this,&ZeroLevelHistos::samplingReady,storing);
        storing->addTransition(this,&ZeroLevelHistos::storingReady,calculating);

        // Selecting state
        connect(selecting,&QState::entered,this,[=](){
            ui->lineEdit->setEnabled(1);
            ui->spinBox->setEnabled(1);
            ui->startBtn->setEnabled(1);
            ui->stopBtn->setEnabled(0);
            statusBar()->showMessage("Selecting");
            qDebug() << "Selecting";

        });
        connect(selecting,&QState::exited,this,[=](){

        });

        // Processing state

        connect(processing,&QState::entered,this,[=](){

            for(quint8 i=0;i<12; ++i){
                b[i]->data().data()->clear();
                b1[i]->data().data()->clear();
                hg[i]->graph(0)->data().data()->clear();
                hg[i]->graph(1)->data().data()->clear();
                ui->allanPlot->graph(2*i)->data().data()->clear();
                ui->allanPlot->graph(2*i+1)->data().data()->clear();

                arr_adc0[i].clear();
                arr_adc1[i].clear();
                ADC0[i].clear();
                ADC1[i].clear();
                allan_sigma0[i].clear();
                allan_sigma1[i].clear();
            }
            time_interval.clear();
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
                FEE.PMaddress = 0x0;
                FEE.reinit();
            }

            n = ui->spinBox->value();

            workTimer->callOnTimeout([=](){
                if(n--){

                    FEE.zero_read_fast();
                    for (quint8 i=0; i<12; ++i) {
                        arr_adc0[i].push_back(FEE.PM.ZEROLVL[i].ADC0);
                        arr_adc1[i].push_back(FEE.PM.ZEROLVL[i].ADC1);
                    }
                    time_interval.push_back(t.elapsed());
                    statusBar()->showMessage(tr("%1/%2").arg(ui->spinBox->value()-n).arg(ui->spinBox->value()));
//                    workTimer->start();
                }
                else{
                    workTimer->stop();
//                    qDebug() << t.elapsed();
                    emit samplingReady();
                }
            });

            statusBar()->showMessage(QString::asprintf("Reading %d samples...", n));
            workTimer->start();
            t.start();
        });

        connect(sampling,&QState::exited,this,[=](){
            workTimer->disconnect();
        });

        connect(storing,&QState::entered,this,[=](){
            statusBar()->showMessage("Start storing ...",500);
            qDebug() << "Storing";
            for(quint8 i=0;i<12;++i){
                for(quint16 v : arr_adc0[i]){ ++ADC0[i][v]; }
                for(quint16 v : arr_adc1[i]){ ++ADC1[i][v]; }
            }

            out.setFileName("results.txt");
            out.open(QIODevice::WriteOnly);
            fout.setDevice(&out);

            for(qint8 i=0;i<12;++i){
                fout << endl;
                fout << "Ch:\t" << i+1 << endl;
                fout << "T:\t" << "ADC0_ZEROLVL" << endl;
                fout << "Value\tCounts" << endl;

                foreach (quint16 k, ADC0[i].keys()) {
                    b[i]->addData(k,ADC0[i][k]);
                    fout << k << "\t" << ADC0[i][k] << endl;
                }

                fout << endl;
                fout << "Ch:\t" << i+1 << endl;
                fout << "T:\t" << "ADC1_ZEROLVL" << endl;
                fout << "Value\tCounts"<< endl;

                foreach (quint16 k, ADC1[i].keys()) {
                    b1[i]->addData(k,ADC1[i][k]);
                    fout << k << "\t" << ADC1[i][k] << endl;
                }
                hh[i]->rescaleAxes();
                hh[i]->replot();
            }
            out.close();


            out.setFileName("results_raw.txt");
            out.open(QIODevice::WriteOnly);
            fout.setDevice(&out);

            for(quint8 i=0; i<12; ++i){
                fout << "Ch:\t" << i+1 << endl;
                fout << "T:\t" << "ADC0_ZEROLVL" << endl;
                fout << "Time\tValue"<< endl;
                for(quint16 j=0;j<time_interval.size();++j){
                    hg[i]->graph(0)->addData(time_interval[j],arr_adc0[i][j]);
                    fout << time_interval[j] << "\t" << arr_adc0[i][j] << endl;
                }

                fout << endl;

                fout << "Ch:\t" << i+1 << endl;
                fout << "T:\t" << "ADC1_ZEROLVL" << endl;
                fout << "Time\tValue"<< endl;
                for(quint16 j=0;j<time_interval.size();++j){
                    hg[i]->graph(1)->addData(time_interval[j],arr_adc1[i][j]);
                    fout << time_interval[j] << "\t" << arr_adc1[i][j] << endl;
                }

                hg[i]->rescaleAxes();
                hg[i]->replot();

                fout << endl;

            }

            out.close();

            emit storingReady();
        });

        connect(calculating,&QState::entered,this,[=](){
            statusBar()->showMessage("Start calculating ...",500);
            qDebug() << "Calculating";

            QVector<double> tmp_arr[2];
            tmp_arr[0].reserve(ui->spinBox->value()+1);
            tmp_arr[1].reserve(ui->spinBox->value()+1);

            quint32 N = std::log2(ui->spinBox->value());

            for(quint8 k=0;k<12;k++){
                for(qint8 pw = N;pw>=0;pw-- ){

                    for(quint32 i=0;i<pow(2,N);i+=pow(2,N-pw)){
                        double m = 0;
                        double m1 = 0;
                        for(qint32 j=0;j<pow(2,N-pw);++j){
                            m+= arr_adc0[k][i+j];
                            m1+= arr_adc1[k][i+j];
                        }
                        m = m/(pow(2,N-pw));
                        m1 = m1/(pow(2,N-pw));
                        tmp_arr[0].push_back(m);
                        tmp_arr[1].push_back(m1);
                    }

                    for(quint8 l=0;l<2;++l){
                        double mean = 0;
                        double mean2 = 0;
                        double sigma = 0;

                        for(double v : tmp_arr[l]){
                            mean += v/tmp_arr[l].size();
                            mean2 += v*v/tmp_arr[l].size();
                        }
//                        qDebug() << "Channel:" << k+1 << "ADC" << l << "_ZEROLVL" << endl
//                                 << "Elements:" << tmp_arr[l].size() << endl
//                                 << "Mean:\t" <<  mean << endl
//                                 << "Sigm:\t" << pow(mean2-mean*mean,0.5) << endl;
                        sigma = pow(mean2-mean*mean,0.5);
                        (l==0)?allan_sigma0[k].push_back(sigma):
                               allan_sigma1[k].push_back(sigma);

                        ui->allanPlot->graph(2*k+l)->addData(pw,sigma);
                    }

                    tmp_arr[0].clear();
                    tmp_arr[1].clear();

                }
            }

            ui->allanPlot->rescaleAxes();
            ui->allanPlot->replot();

            out.setFileName("results_allan.txt");
            out.open(QIODevice::WriteOnly);
            fout.setDevice(&out);

            for(quint8 i=0;i<12;++i){
                fout << "Ch:\t" << i+1 << endl;
                fout << "T:\t" << "ADC0_Allan's_sigma" << endl;
                fout << "Log_2\tValue" << endl;

                for(qint8 pw = N;pw>=0;pw-- ){
//                    fout << time_interval[pow(2,pw)-1] << "\t" << allan_sigma0[i][pw-1] << endl;
                    fout << pw << "\t" << allan_sigma0[i][N-pw] << endl;
                }

                fout << endl;

                fout << "Ch:\t" << i+1 << endl;
                fout << "T:\t" << "ADC1_Allan's_sigma" << endl;
                fout << "Log_2\tValue"<< endl;

                for(qint8 pw = N;pw>=0;pw-- ){
//                    fout << time_interval[pow(2,pw)-1] << "\t" << allan_sigma1[i][pw-1] << endl;
                    fout << pw << "\t" << allan_sigma1[i][N-pw] << endl;
                }

                fout << endl;

            }
            out.close();

            emit ui->stopBtn->clicked();
        });


//        FEE.IPaddress = "127.0.0.1";
        machine->start();
        setWindowState(Qt::WindowMaximized);
        recheck();
}

ZeroLevelHistos::~ZeroLevelHistos()
{
    for(quint8 i=0;i<12;i++){
        arr_adc0[i].squeeze();
        arr_adc1[i].squeeze();
    }
    time_interval.squeeze();
    delete ui;
}


void ZeroLevelHistos::setup_hist()
{
    hh.reserve(12);
    hh.push_back(ui->h1);
    hh.push_back(ui->h2);
    hh.push_back(ui->h3);
    hh.push_back(ui->h4);
    hh.push_back(ui->h5);
    hh.push_back(ui->h6);
    hh.push_back(ui->h7);
    hh.push_back(ui->h8);
    hh.push_back(ui->h9);
    hh.push_back(ui->h10);
    hh.push_back(ui->h11);
    hh.push_back(ui->h12);

    hg.reserve(12);
    hg.push_back(ui->h1_2);
    hg.push_back(ui->h2_2);
    hg.push_back(ui->h3_2);
    hg.push_back(ui->h4_2);
    hg.push_back(ui->h5_2);
    hg.push_back(ui->h6_2);
    hg.push_back(ui->h7_2);
    hg.push_back(ui->h8_2);
    hg.push_back(ui->h9_2);
    hg.push_back(ui->h10_2);
    hg.push_back(ui->h11_2);
    hg.push_back(ui->h12_2);


    b.resize(12);
    b1.resize(12);

    QFont legendFont = font();
    legendFont.setPointSize(7);

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    fixedTicker->setTickStep(1.0);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);


    for (quint8 i=0;i<12;++i) {
        b[i] = new QCPBars(hh[i]->xAxis,hh[i]->yAxis);
        b[i]->setBrush(QColor(QColor(200,0,0,200)));
        b[i]->setPen(Qt::NoPen);
        b[i]->setWidth(1);
        b[i]->setStackingGap(0);
        b[i]->setName("ADC0_ZEROLVL");

        b1[i] = new QCPBars(hh[i]->xAxis,hh[i]->yAxis);
        b1[i]->setBrush(QColor(QColor(0,0,200,200)));
        b1[i]->setWidth(1);
        b1[i]->setPen(Qt::NoPen);
        b1[i]->setStackingGap(0);
//        b1[i]->moveAbove(b[i]);
        b1[i]->setName("ADC1_ZEROLVL");

        hh[i]->plotLayout()->insertRow(0);
        hh[i]->plotLayout()->addElement(0,0, new QCPTextElement(hh[i],tr("Channel %1").arg(i+1)));
        hh[i]->xAxis->setTicker(fixedTicker);
        hh[i]->setInteractions(QCP::iRangeZoom|QCP::iRangeDrag);
        hh[i]->axisRect()->setRangeDrag(Qt::Horizontal);
        hh[i]->axisRect()->setRangeZoom(Qt::Horizontal);
        hh[i]->autoAddPlottableToLegend();
        hh[i]->legend->setVisible(1);

        hh[i]->legend->setFont(legendFont);
        hh[i]->legend->setIconSize(10,10);
        hh[i]->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignTop);
        hh[i]->axisRect()->insetLayout()->setMinimumMargins(QMargins(0,0,0,0));
        hh[i]->axisRect()->insetLayout()->setMargins(QMargins(0,0,0,0));
        connect(hh[i]->xAxis,static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged),this,&ZeroLevelHistos::rescaleRange);

        hg[i]->plotLayout()->insertRow(0);
        hg[i]->plotLayout()->addElement(0,0, new QCPTextElement(hg[i],tr("Channel %1").arg(i+1)));
        hg[i]->xAxis->setTicker(fixedTicker);
        hg[i]->setInteractions(QCP::iRangeZoom|QCP::iRangeDrag);
        hg[i]->axisRect()->setRangeDrag(Qt::Horizontal);
        hg[i]->autoAddPlottableToLegend();
        hg[i]->legend->setVisible(1);

        hg[i]->legend->setFont(legendFont);
        hg[i]->legend->setIconSize(10,10);
        hg[i]->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignTop);
        hg[i]->axisRect()->insetLayout()->setMinimumMargins(QMargins(0,0,0,0));
        hg[i]->axisRect()->insetLayout()->setMargins(QMargins(0,0,0,0));

        hg[i]->addGraph();
        hg[i]->addGraph();

//        hg[i]->graph(0)->setBrush(QColor(QColor(200,0,0)));
//        hg[i]->graph(1)->setBrush(QColor(QColor(0,0,200)));

        hg[i]->graph(0)->setPen( QPen (QColor(200,0,0,200)));
//        hg[i]->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,QColor(200,0,0,200),QColor(200,0,0,200),2));
        hg[i]->graph(1)->setPen( QPen (QColor(0,0,200,200)));
//        hg[i]->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,QColor(0,0,200,200),QColor(0,0,200,200),2));

        hg[i]->graph(0)->setName("ADC0_ZEROLVL");
        hg[i]->graph(1)->setName("ADC1_ZEROLVL");

        ui->allanPlot->addGraph();
        ui->allanPlot->addGraph();

    }
    ui->allanPlot->setInteractions(QCP::iRangeZoom|QCP::iRangeDrag);
}



void ZeroLevelHistos::rescaleRange(const QCPRange& newRange)
{
    bool fr=false;
    quint16 upper = 0;
    quint16 upper1 = 0;

    for(quint8 i=0;i<12;++i){
        if(sender() == hh[i]->xAxis){
            if(newRange.upper > 4095) hh[i]->xAxis->setRangeUpper(4095);
            if(newRange.lower < -4096) hh[i]->xAxis->setRangeLower(-4096);

            upper=static_cast<QCPBars*>(hh[i]->plottable(0))->data().data()->valueRange(fr,QCP::sdBoth,newRange).upper;
            upper1=static_cast<QCPBars*>(hh[i]->plottable(1))->data().data()->valueRange(fr,QCP::sdBoth,newRange).upper;
            if((upper+upper1) > hh[i]->yAxis->range().upper){
                hh[i]->yAxis->rescale();
                hh[i]->yAxis->setRange(0,hh[i]->yAxis->range().upper*1.1);
            } else hh[i]->yAxis->setRange(0,(upper+upper1)*1.1);

            return;
        }
    }
}


void ZeroLevelHistos::calc_variance(const QVector<double>& arr, quint8 ich,quint8 iz, QVector<double>& dest)
{
    double mean = 0;
    double mean2 = 0;

    for(auto v : arr){
        mean += v/arr.size();
        mean2 += v*v/arr.size();
    }
    qDebug() << "Channel:" << ich << "ADC" << iz << "_ZEROLVL" << endl
             << "Elements:" << arr.size() << endl
             << "Mean:\t" <<  mean << endl
             << "Sigm:\t" << pow(mean2-mean*mean,0.5) << endl;

    dest.push_back(pow(mean2-mean*mean,0.5));
}


void ZeroLevelHistos::changeIP() {
    bool ok;
    QString text = QInputDialog::getText(this, "Changing target IP address", "Enter new address", QLineEdit::Normal, FEE.IPaddress, &ok);
    if (ok && !text.isEmpty()) {
        if (validIPaddress(text)) {
            FEE.IPaddress = text;
            recheck();
        } else statusBar()->showMessage(text + ": invalid IP address");
    }
}

void ZeroLevelHistos::recheck() {
    statusBar()->showMessage(FEE.IPaddress + ": status requested...");

    responseTimer->start(500);
    FEE.reinit();

}


void ZeroLevelHistos::on_spinBox_editingFinished()
{
    ui->spinBox->setValue(pow(2,round(std::log2(ui->spinBox->value()))));
}


void ZeroLevelHistos::read_files()
{
    statusBar()->showMessage("Reading file ...");

    for(quint8 i=0;i<12; ++i){
        b[i]->data().data()->clear();
        b1[i]->data().data()->clear();
        hg[i]->graph(0)->data().data()->clear();
        hg[i]->graph(1)->data().data()->clear();
        ui->allanPlot->graph(2*i)->data().data()->clear();
        ui->allanPlot->graph(2*i+1)->data().data()->clear();

//        arr_adc0[i].clear();
//        arr_adc1[i].clear();
//        ADC0[i].clear();
//        ADC1[i].clear();
//        allan_sigma0[i].clear();
//        allan_sigma1[i].clear();
    }
//    time_interval.clear();


    quint32 cnt=0;

    QFile in;
    in.setFileName("results.txt");

    if ( !in.open(QFile::ReadOnly | QFile::Text) ) {
        qDebug() << "File not exists";
    }
    else {
        quint8 idx;
        QStringList wordList;
        QCPBars* bar;

        while (!in.atEnd()) {
            QString line = in.readLine();
            wordList = line.split("\t");

            if(line.length()<=1) continue;
            else if(wordList[0].left(2) == "Ch"){
                idx = wordList[1].toUShort()-1;
                continue;
            }
            else if(wordList[0].left(2) == "T:") {
                if(wordList[1].left(4)=="ADC0") bar = b[idx];
                else if (wordList[1].left(4)=="ADC1") bar = b1[idx];
                else {
                    qDebug() << "Error reading target's name";
                    break;
                }
                continue;
            }
            else{
                bar->addData(wordList[0].toUInt(),wordList[1].toUInt());
            }
//            if(!(++cnt%500)){
//                hh[idx]->rescaleAxes();
//                hh[idx]->replot();
//                qApp->processEvents();
//            }
        }
        in.close();
    }


    in.setFileName("results_raw.txt");
    cnt = 0;

    if ( !in.open(QFile::ReadOnly | QFile::Text) ) {
        qDebug() << "File not exists";
    }
    else {
        quint8 idx;
        QStringList wordList;
        QCPGraph* graph;
        while (!in.atEnd()) {
            QString line = in.readLine();
            wordList = line.split("\t");

            if(line.length()<=1) continue;
            else if(wordList[0].left(2) == "Ch"){
                idx = wordList[1].toUShort()-1;
                continue;
            }
            else if(wordList[0].left(2) == "T:") {
                if (wordList[1].left(4)=="ADC0") graph = hg[idx]->graph(0);
                else if (wordList[1].left(4)=="ADC1") graph = hg[idx]->graph(1);
                else {
                    qDebug() << "Error reading target's name" << wordList[1].left(4);
                    break;
                }
                QString line = in.readLine();
                continue;
            }
            else{
                graph->addData(wordList[0].toUInt(),wordList[1].toUInt());
            }
//            if(!(++cnt%500)){
//                hg[idx]->rescaleAxes();
////                hg[idx]->xAxis->setRange(hg[idx]->xAxis->range().upper,150,Qt::AlignRight);
//                hg[idx]->replot();
//                qApp->processEvents();
//            }
        }
        in.close();
    }



    in.setFileName("results_allan.txt");
    cnt = 0;

    if ( !in.open(QFile::ReadOnly | QFile::Text) ) {
        qDebug() << "File not exists";
    }
    else {
        quint8 idx;
        QStringList wordList;
        QCPGraph* graph;
        while (!in.atEnd()) {
            QString line = in.readLine();
            wordList = line.split("\t");

            if(line.length()<=1) continue;
            else if(wordList[0].left(2) == "Ch"){
                idx = wordList[1].toUShort()-1;
                continue;
            }
            else if(wordList[0].left(2) == "T:") {
                if (wordList[1].left(4)=="ADC0") graph = ui->allanPlot->graph(2*idx);
                else if (wordList[1].left(4)=="ADC1") graph = ui->allanPlot->graph(2*idx+1);
                else {
                    qDebug() << "Error reading target's name" << wordList[1].left(4);
                    break;
                }
                QString line = in.readLine();
                continue;
            }
            else{
                graph->addData(wordList[0].toUInt(),wordList[1].toDouble());
            }

        }
        in.close();
    }


    for(qint8 i=0;i<12;++i){
        hh[i]->rescaleAxes();
        hh[i]->replot();

        hg[i]->rescaleAxes();
        hg[i]->replot();
    }

    ui->allanPlot->rescaleAxes();
    ui->allanPlot->replot();

    statusBar()->showMessage("Done");


}
