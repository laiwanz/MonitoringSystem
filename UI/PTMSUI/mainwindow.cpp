#include "mainwindow.h"
#include "Widget.h"
#include "ui_mainwindow.h"
#include "cbl/util/LogFile.h"
#include <QColor>
#include <QComboBox>
#include <QAbstractItemView>
#include <QVBoxLayout>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QMenu>
#include <QMutex>
#include <QMouseEvent>
#include <QPushButton>

MainWindow::MainWindow(const std::string& sHost, int nPort, QWidget *parent)
        :QMainWindow(parent),
         m_PCMaster(sHost, nPort, ""),
         ui(new Ui::MainWindow),
         m_buttonClicked(false),
         m_multiClickEnabled(true),
         m_isCmd(false),
         m_bGotoUrl(false),
         m_nSortStatus(0),
         m_nStatus(0),
         m_bConnected(false),
         m_nTimerNum(0)
{

    this->m_Task = new Task();
    this->m_CCmd = new Widget();
    this->config();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::MonitorConnection);
    timer->start(1000);

    m_DisconnectionTimer = new QTimer(this);
    connect(m_DisconnectionTimer, &QTimer::timeout, this, &MainWindow::checkDisconnection);
    m_DisconnectionTimer->start(60000);

    setUI();
}


MainWindow::~MainWindow()
{
    delete ui;
    delete m_DisconnectionTimer;
    delete m_Task;
    delete m_CCmd;
}


void MainWindow::setUI(){

    ui->setupUi(this);

    QStringList options;
    options << "PI3070ie" << "PI3070ig" << "PI8000ig" << "PI3070i" ;
    ui->comboBox->addItems(options);
    m_CCmd->initUI();

    ui->tableWidget->setColumnWidth(1, 120);
    ui->tableWidget->setColumnWidth(2, 60);
    ui->tableWidget->setColumnWidth(3, 240);
    ui->tableWidget->setColumnWidth(4, 120);
    ui->tableWidget->setColumnWidth(5, 90);
    ui->tableWidget->setColumnWidth(6, 120);
    ui->tableWidget->setColumnWidth(7, 130);
    ui->tableWidget->setColumnWidth(8, 100);
    ui->tableWidget->setColumnWidth(9, 130);
    ui->tableWidget->setColumnWidth(10, 130);

    ui->tableWidget_2->setColumnWidth(1, 120);
    ui->tableWidget_2->setColumnWidth(2, 60);
    ui->tableWidget_2->setColumnWidth(3, 120);
    ui->tableWidget_2->setColumnWidth(4, 120);
    ui->tableWidget_2->setColumnWidth(5, 90);
    ui->tableWidget_2->setColumnWidth(6, 100);
    ui->tableWidget_2->setColumnWidth(7, 100);
    ui->tableWidget_2->setColumnWidth(8, 130);
    ui->tableWidget_2->setColumnWidth(9, 100);
    ui->tableWidget_2->setColumnWidth(10, 100);
    ui->tableWidget_2->setColumnWidth(11, 100);
    ui->tableWidget_2->setColumnWidth(12, 100);
    ui->tableWidget_2->setSortingEnabled(true);

    ui->tableWidget_3->setColumnWidth(1, 120);
    ui->tableWidget_3->setColumnWidth(2, 60);
    ui->tableWidget_3->setColumnWidth(3, 240);
    ui->tableWidget_3->setColumnWidth(4, 120);
    ui->tableWidget_3->setColumnWidth(5, 90);
    ui->tableWidget_3->setColumnWidth(6, 120);
    ui->tableWidget_3->setColumnWidth(7, 130);
    ui->tableWidget_3->setColumnWidth(8, 100);
    ui->tableWidget_3->setColumnWidth(9, 130);
    ui->tableWidget_3->setColumnWidth(10, 130);

    ui->tableWidget_4->setColumnWidth(1, 120);
    ui->tableWidget_4->setColumnWidth(2, 60);
    ui->tableWidget_4->setColumnWidth(3, 240);
    ui->tableWidget_4->setColumnWidth(4, 120);
    ui->tableWidget_4->setColumnWidth(5, 90);
    ui->tableWidget_4->setColumnWidth(6, 120);
    ui->tableWidget_4->setColumnWidth(7, 130);
    ui->tableWidget_4->setColumnWidth(8, 100);
    ui->tableWidget_4->setColumnWidth(9, 130);
    ui->tableWidget_4->setColumnWidth(10, 130);

    ui->tabWidget->setTabText(0, "all");
    ui->tabWidget->setTabText(1, "sort");
    ui->tabWidget->setTabText(2, "group");
    ui->tabWidget->setTabText(3, "single");
    ui->tabWidget->setCurrentIndex(0);
    ui->listWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    connect(ui->tableWidget, &QTableWidget::itemClicked, this, &MainWindow::gotoURL);
}


void MainWindow::config(){
    this->m_Task->config([this](const string & TaskName, const string & TaskInfo){
        if(! m_Tasks.count(TaskName)){
            m_Tasks[TaskName] = TaskInfo;
        }
        QStringList options;
        for(const auto&  task : m_Tasks){
            options << task.first.c_str();
        }
        ui->comboBox_2->clear();
        ui->comboBox_2->addItems(options);
        return 0;
    }),
    this->m_PCMaster.setConfig();
    this->m_PCMaster.setFuncConfig([this](REDIS_REPORT_INFO_T Info){
        return this->printMachineInfo(Info);
    },  NULL, [this](const string &channel, const string &model, const string &name, int type,
    int time, int interval, const string &postscript){
        std::string sTaskInfo;
        this->m_PCMaster.newATask(sTaskInfo, channel, model, name, type, time, interval, postscript);
        return this->m_PCMaster.publish(channel, sTaskInfo);
    });

}

static bool isMemberOfGroup(const std::vector<std::string>& members, const std::string& target) {
    return std::find(members.begin(), members.end(), target) != members.end();
}

static bool isIPFormat(const QString &ip) {

    QString pattern = QString("^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$");

    QRegularExpression regex(pattern);
    return regex.match(ip).hasMatch();
}


void MainWindow::gotoURL(QTableWidgetItem *item){
    if(m_bGotoUrl){
        if(!isIPFormat(item->text())){
            return;
        }
        QUrl url = QUrl::fromUserInput(QString("http://%1").arg(item->text()));
        QDesktopServices::openUrl(url);
        m_bGotoUrl = false;
        QString recond =  "你选择了";
        recond += item->text();
        recond += "进行进入局域网后台的操作";
        operationRecord(recond);
    }
    QFont font;
    font.setFamily("微软雅黑 Light");
    font.setPointSize(14);
    ui->pushButton_15->setFont(font);
    ui->pushButton_15->setStyleSheet(QString());
}

void MainWindow::on_pushButton_clicked()
{
    this->m_Task->show();
}


void MainWindow::on_pushButton_3_clicked()
{
    qDebug() << "ALL Device";
    m_vMembers.clear();
    m_nStatus = 0;
    ui->tabWidget->setCurrentIndex(0);
    ui->tableWidget->update();

}



int MainWindow::start(){

    m_PCMaster.init();
    m_PCMaster.masterStop();
    m_PCMaster.masterOpen();
    return 0;
}


void MainWindow::operationRecord(const QString &arg1){
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString dateTimeStr = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");

    QString record = dateTimeStr + "  " + arg1;
    ui->listWidget->insertItem(0, record);
}


void MainWindow::checkDisconnection()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    const int disconnectThreshold = 60 * 1000;


    for (auto it = m_LastReportTimeMap.begin(); it != m_LastReportTimeMap.end(); )
    {
        const QDateTime& lastReportTime = it.value();

        if (lastReportTime.msecsTo(currentTime) >= disconnectThreshold)
        {

            cbl::LOG_SET_WITH_LIMIT("./Log/Redis_Server_Log.txt", LOG_DEFAULT_LIMIT_FILE_SIZE, LOG_DEFAULT_LIMIT_BACKUP_AMOUNT, LOG_MODE_FILE);
            LOGI("%s 断开连接", it.key().toStdString().c_str());
            it = m_LastReportTimeMap.erase(it);
        }
        else
        {
            ++it;
        }
    }
}


void MainWindow::showTheAllDate(const REDIS_REPORT_INFO_T reportInfo, QTableWidget* widget){
    std::unique_lock<std::mutex> lock(mtx); // 使用 std::unique_lock 来锁定互斥锁
	QColor lightRed(255, 128, 128);
    QString qsDeviceID = QString::fromStdString(reportInfo.sDeviceId);

   for(int row = 0; row < widget->rowCount(); ++row){
       QTableWidgetItem *item = widget->item(row, 3);
       if(item && item->text() == qsDeviceID){

           QTableWidgetItem *nCpuItem = new QTableWidgetItem;
           nCpuItem->setData(Qt::EditRole, reportInfo.nCpu);
           if(reportInfo.nCpu >= 30){
               QBrush brush(lightRed);
               nCpuItem->setBackground(brush);
               cbl::LOGW("%s : CPU 占用率为 %d %", reportInfo.sIp.c_str(), reportInfo.nCpu);

           }
           widget->setItem(row, 5, nCpuItem);


           QTableWidgetItem *nFreeMemoryItem = new QTableWidgetItem(QString::number((reportInfo.nTotalMemory / 1024) / 1024));

           if(reportInfo.nFreeMemory <= 30000){
               QBrush brush(lightRed);
               nFreeMemoryItem->setBackground(brush);
               cbl::LOGW("%s : 内存剩余过低！剩余内存为 %d", reportInfo.sIp.c_str(), reportInfo.nFreeMemory);
           }
           widget->setItem(row, 6, nFreeMemoryItem);
           widget->setItem(row, 7, new QTableWidgetItem(QString::number((reportInfo.nTotalMemory / 1024) / 1024)));

//           QTableWidgetItem *nDiskSizeItem = new QTableWidgetItem(QString::number((reportInfo.nDiskSize) / 1024));
//           nDiskSizeItem->setData(Qt::UserRole, reportInfo.nDiskSize / 1024);
           QTableWidgetItem *nDiskSizeItem = new QTableWidgetItem;
           nDiskSizeItem->setData(Qt::EditRole, reportInfo.nDiskSize / 1024);

           if(reportInfo.nDiskSize <= 3000){
               QBrush brush(lightRed);
               nDiskSizeItem->setBackground(brush);
              cbl::LOGW("%s : /mnt/data 硬盘内存剩余过低！剩余内存为 %d MB", reportInfo.sIp.c_str(), reportInfo.nDiskSize / 1024);
           }
           widget->setItem(row, 8, nDiskSizeItem);
           widget->setItem(row, 9, new QTableWidgetItem(QString::number((reportInfo.nTotalSize) / 1024)));

           QString nsPID = QString::number(reportInfo.nPID);
           QTableWidgetItem *nPIDItem = new QTableWidgetItem(nsPID);
           if(reportInfo.nPID == 0){
               QBrush brush(lightRed);
               nPIDItem->setBackground(brush);

               cbl::LOGW("%s : HMIUI的句柄数为%d", reportInfo.sIp.c_str(),reportInfo.nPID);

           }
           widget->setItem(row, 10, nPIDItem);


           QString nsCurMem = QString::number(reportInfo.ulCurMem / 1024);
           QTableWidgetItem *nCurMemmItem = new QTableWidgetItem(nsCurMem);
           if(reportInfo.ulCurMem == 0){
               QBrush brush(lightRed);
               nCurMemmItem->setBackground(brush);
           }

           widget->setItem(row, 11, nCurMemmItem);

           QString nsPeakMem = QString::number(reportInfo.ulPeakMem / 1024);
           QTableWidgetItem *nPeakMemItem = new QTableWidgetItem(nsPeakMem);
           if(reportInfo.ulPeakMem == 0){
               QBrush brush(lightRed);
               nPeakMemItem->setBackground(brush);

           };
           widget->setItem(row, 12, nPeakMemItem);
           widget->update();
           //QApplication::processEvents();
           return;
       }
    }
       int nRow = widget->rowCount();
       widget->insertRow(nRow);
       widget->setItem(nRow, 0, new QTableWidgetItem(QString::fromStdString(reportInfo.sIp)));
       widget->setItem(nRow, 1, new QTableWidgetItem(QString::fromStdString(reportInfo.sMAC)));
       widget->setItem(nRow, 2, new QTableWidgetItem(QString::fromStdString(reportInfo.sModel)));
       widget->setItem(nRow, 3, new QTableWidgetItem(QString::fromStdString(reportInfo.sDeviceId)));
       widget->setItem(nRow, 4, new QTableWidgetItem(QString::fromStdString(reportInfo.sVersion)));

       QTableWidgetItem *nCpuItem = new QTableWidgetItem;
       nCpuItem->setData(Qt::EditRole, reportInfo.nCpu);
       if(reportInfo.nCpu >= 30){
           QBrush brush(lightRed);
           nCpuItem->setBackground(brush);
           cbl::LOGW("%s : CPU 占用率为 %d %", reportInfo.sIp.c_str(), reportInfo.nCpu);

       }
       widget->setItem(nRow, 5, nCpuItem);


       QTableWidgetItem *nFreeMemoryItem = new QTableWidgetItem(QString::number((reportInfo.nFreeMemory / 1024) / 1024));
       if(reportInfo.nFreeMemory <= 30000){
           QBrush brush(lightRed);
           nFreeMemoryItem->setBackground(brush);
           cbl::LOGW("%s : 内存剩余过低！剩余内存为 %d", reportInfo.sIp.c_str(), reportInfo.nFreeMemory);
       }
       widget->setItem(nRow, 6, nFreeMemoryItem);
       widget->setItem(nRow, 7, new QTableWidgetItem(QString::number((reportInfo.nTotalMemory / 1024) / 1024)));


//       QTableWidgetItem *nDiskSizeItem = new QTableWidgetItem(QString::number(reportInfo.nDiskSize / 1024));

       QTableWidgetItem *nDiskSizeItem = new QTableWidgetItem;
       nDiskSizeItem->setData(Qt::EditRole, reportInfo.nDiskSize / 1024);
       if(reportInfo.nDiskSize <= 3000){
           QBrush brush(lightRed);
           nDiskSizeItem->setBackground(brush);
           cbl::LOGW("%s : /mnt/data 硬盘内存剩余过低！剩余内存为 %d MB", reportInfo.sIp.c_str(), reportInfo.nDiskSize / 1024);
            }
       widget->setItem(nRow, 8, nDiskSizeItem);
       widget->setItem(nRow, 9, new QTableWidgetItem(QString::number(reportInfo.nTotalSize / 1024)));

       QString nsPID = QString::number(reportInfo.nPID);
       QTableWidgetItem *nPIDItem = new QTableWidgetItem(nsPID);
       if(reportInfo.nPID == 0){
           QBrush brush(lightRed);
           nPIDItem->setBackground(brush);

           cbl::LOGW("%s : HMIUI的句柄数为%d", reportInfo.sIp.c_str(),reportInfo.nPID);

       }
       widget->setItem(nRow, 10, nPIDItem);

       QString nsCurMem = QString::number(reportInfo.ulCurMem / 1024);
       QTableWidgetItem *nCurMemmItem = new QTableWidgetItem(nsCurMem);
       if(reportInfo.ulCurMem == 0){
           QBrush brush(lightRed);
           nCurMemmItem->setBackground(brush);
       }

       widget->setItem(nRow, 11, nCurMemmItem);

       QString nsPeakMem = QString::number(reportInfo.ulPeakMem / 1024);
       QTableWidgetItem *nPeakMemItem = new QTableWidgetItem(nsPeakMem);
       if(reportInfo.ulPeakMem == 0){
           QBrush brush(lightRed);
           nPeakMemItem->setBackground(brush);

       };
       widget->setItem(nRow, 12, nPeakMemItem);
       widget->update();
       //QApplication::processEvents();
       return;

}



void MainWindow::showTheDate(const REDIS_REPORT_INFO_T reportInfo, QTableWidget* widget){
    std::unique_lock<std::mutex> lock(mtx); // 使用 std::unique_lock 来锁定互斥锁
    QString qsDeviceID = QString::fromStdString(reportInfo.sDeviceId);
    QColor lightRed(255, 128, 128);
    for(int row = 0; row < widget->rowCount(); ++row){
        QTableWidgetItem *item = widget->item(row, 3);
        if(item && item->text() == qsDeviceID){

            QTableWidgetItem *nCpuItem = new QTableWidgetItem;
            nCpuItem->setData(Qt::EditRole, reportInfo.nCpu);
            if(reportInfo.nCpu >= 30){
                QBrush brush(lightRed);
                nCpuItem->setBackground(brush);
                cbl::LOGW("%s : CPU 占用率为 %d %", reportInfo.sIp.c_str(), reportInfo.nCpu);

            }
            widget->setItem(row, 5, nCpuItem);

            std::string sMemory;
            sMemory += to_string(reportInfo.nFreeMemory / 1024);
            sMemory += " / ";
            sMemory += to_string(reportInfo.nTotalMemory / 1024);

            QTableWidgetItem *nFreeMemoryItem = new QTableWidgetItem(QString::fromStdString(sMemory));
            if(reportInfo.nFreeMemory <= 30000){
                QBrush brush(lightRed);
                nFreeMemoryItem->setBackground(brush);
                cbl::LOGW("%s : 内存剩余过低！剩余内存为 %d", reportInfo.sIp.c_str(), reportInfo.nFreeMemory);
            }
            widget->setItem(row, 6, nFreeMemoryItem);

            std::string sSize;
            sSize += to_string(reportInfo.nDiskSize / 1024);
            sSize += " / ";
            sSize += to_string(reportInfo.nTotalSize/ 1024);

            widget->setItem(row, 7, new QTableWidgetItem(QString::fromStdString(sSize)));


            QString nsPID = QString::number(reportInfo.nPID);
            QTableWidgetItem *nPIDItem = new QTableWidgetItem(nsPID);
            if(reportInfo.nPID == 0){
                QBrush brush(lightRed);
                nPIDItem->setBackground(brush);

                cbl::LOGW("%s : HMIUI的句柄数为%d", reportInfo.sIp.c_str(),reportInfo.nPID);

            }
            widget->setItem(row, 8, nPIDItem);


            QString nsCurMem = QString::number(reportInfo.ulCurMem / 1024);
            QTableWidgetItem *nCurMemmItem = new QTableWidgetItem(nsCurMem);
            if(reportInfo.ulCurMem == 0){
                QBrush brush(lightRed);
                nCurMemmItem->setBackground(brush);
            }

            widget->setItem(row, 9, nCurMemmItem);

            QString nsPeakMem = QString::number(reportInfo.ulPeakMem / 1024);
            QTableWidgetItem *nPeakMemItem = new QTableWidgetItem(nsPeakMem);
            if(reportInfo.ulPeakMem == 0){
                QBrush brush(lightRed);
                nPeakMemItem->setBackground(brush);

            };
            widget->setItem(row, 10, nPeakMemItem);

            return;
        }
    }

    int nRow = widget->rowCount();
    widget->insertRow(nRow);
    QTableWidgetItem *sIpItem = new QTableWidgetItem;
    sIpItem->setText(QString::fromStdString(reportInfo.sIp));
    sIpItem->setData(Qt::UserRole, QString("http://%1").arg(sIpItem->text()));
    widget->setItem(nRow, 0, sIpItem);

    widget->setItem(nRow, 1, new QTableWidgetItem(QString::fromStdString(reportInfo.sMAC)));
    widget->setItem(nRow, 2, new QTableWidgetItem(QString::fromStdString(reportInfo.sModel)));
    widget->setItem(nRow, 3, new QTableWidgetItem(QString::fromStdString(reportInfo.sDeviceId)));
    widget->setItem(nRow, 4, new QTableWidgetItem(QString::fromStdString(reportInfo.sVersion)));
    QString nsCpu = QString::number(reportInfo.nCpu);
    QTableWidgetItem *nCpuItem = new QTableWidgetItem(nsCpu);
    if(reportInfo.nCpu >= 30){
        QBrush brush(lightRed);
        nCpuItem->setBackground(brush);
        cbl::LOGW("%s : CPU 占用率为 %d %", reportInfo.sIp.c_str(), reportInfo.nCpu);

    }
    widget->setItem(nRow, 5, nCpuItem);


    std::string sMemory;
    sMemory += to_string(reportInfo.nFreeMemory / 1024);
    sMemory += " / ";
    sMemory += to_string(reportInfo.nTotalMemory / 1024);

    QString nsFreeMemory = (QString::fromStdString(sMemory)) ;
    QTableWidgetItem *nFreeMemoryItem = new QTableWidgetItem(nsFreeMemory);
    if(reportInfo.nFreeMemory <= 30000){
        QBrush brush(lightRed);
        nFreeMemoryItem->setBackground(brush);
        cbl::LOGW("%s : 内存剩余过低！剩余内存为 %d", reportInfo.sIp.c_str(), reportInfo.nFreeMemory);
    }
    widget->setItem(nRow, 6, nFreeMemoryItem);


    std::string sSize;
    sSize += to_string(reportInfo.nDiskSize/ 1024);
    sSize += " / ";
    sSize += to_string(reportInfo.nTotalSize / 1024);

    QString nsDiskSize = (QString::fromStdString(sSize)) ;
    QTableWidgetItem *nDiskSizeItem = new QTableWidgetItem(nsDiskSize);
    if(reportInfo.nDiskSize <= 3000){
        QBrush brush(lightRed);
        nDiskSizeItem->setBackground(brush);
        cbl::LOGW("%s : /mnt/data 硬盘内存剩余过低！剩余内存为 %d MB", reportInfo.sIp.c_str(), reportInfo.nDiskSize / 1024);
    }
    widget->setItem(nRow, 7, nDiskSizeItem);

    QString nsPID = QString::number(reportInfo.nPID);
    QTableWidgetItem *nPIDItem = new QTableWidgetItem(nsPID);
    if(reportInfo.nPID == 0 || reportInfo.nPID >= 100){
        QBrush brush(lightRed);
        nPIDItem->setBackground(brush);
        cbl::LOGW("%s : HMIUI的句柄数为%d", reportInfo.sIp.c_str(),reportInfo.nPID);

    }
    widget->setItem(nRow, 8, nPIDItem);

    QString nsCurMem = QString::number(reportInfo.ulCurMem / 1024);
    QTableWidgetItem *nCurMemmItem = new QTableWidgetItem(nsCurMem);
    if(reportInfo.ulCurMem == 0){
        QBrush brush(lightRed);
        nCurMemmItem->setBackground(brush);
    };
    widget->setItem(nRow, 9, nCurMemmItem);

    QString nsPeakMem = QString::number(reportInfo.ulPeakMem / 1024);
    QTableWidgetItem *nPeakMemItem = new QTableWidgetItem(nsPeakMem);
    if(reportInfo.ulPeakMem == 0){
        QBrush brush(lightRed);
        nPeakMemItem->setBackground(brush);

    };
    widget->setItem(nRow, 10, nPeakMemItem);
    widget->update();


    return;

}




int MainWindow::printMachineInfo(const REDIS_REPORT_INFO_T reportInfo){


//    qDebug() << "get the Info";
    cbl::LOG_SET_WITH_LIMIT("./Log/Report_Log.txt", LOG_DEFAULT_LIMIT_FILE_SIZE, LOG_DEFAULT_LIMIT_BACKUP_AMOUNT, LOG_MODE_FILE);
    QString qsDeviceID = QString::fromStdString(reportInfo.sDeviceId);
    m_LastReportTimeMap[QString::fromStdString(reportInfo.sIp)] = QDateTime::currentDateTime();


    if(m_nStatus == 2){
        if(!isMemberOfGroup(m_Groups[m_sGroupName], reportInfo.sDeviceId)){
            return -2;
        }
    } else if (m_nStatus == 1){
        if(m_sSingleDevice != reportInfo.sDeviceId){
            return -1;
        }
    } else if (m_nStatus == 3){
        if(reportInfo.sModel != "PI3070ie"){
            return -3;
        }
    } else if (m_nStatus == 4){
        if(reportInfo.sModel != "PI3070ig"){
            return -4;
        }
    } else if (m_nStatus == 5){
        if(reportInfo.sModel != "PI3070i"){
            return -5;
        }
    } else if (m_nStatus == 6){
        if((reportInfo.sModel != "PI8102ig") && (reportInfo.sModel != "PI8070ig")){
            return -6;
        }
    } else if (m_nStatus == 7) {
        return -7;
    }


    if(ui->tabWidget->currentIndex() == 0){
        showTheDate(reportInfo, ui->tableWidget);

    } else if (ui->tabWidget->currentIndex() == 1){
        showTheAllDate(reportInfo, ui->tableWidget_2);

    } else if (ui->tabWidget->currentIndex() == 2){
        showTheDate(reportInfo, ui->tableWidget_3);

    }else if (ui->tabWidget->currentIndex() == 3){
        showTheDate(reportInfo, ui->tableWidget_4);
    }
    return 0;

}
bool MainWindow::isRedisServerRunning(){
    int result = this->m_PCMaster.ping();
    qDebug() << "ping : " << result << "  isConnected : " << this->isConnected() << "  State : " << ((result == 1) && this->isConnected());
    return (result == 1) && this->isConnected();
}

void MainWindow::MonitorConnection(){

    if(ui->tabWidget->currentIndex() == 0){
        ui->tableWidget->viewport()->update();
        QApplication::processEvents();
    }else if (ui->tabWidget->currentIndex() == 1){
        ui->tableWidget_2->viewport()->update();
        QApplication::processEvents();
    }else if (ui->tabWidget->currentIndex() == 2){
        ui->tableWidget_3->viewport()->update();
        QApplication::processEvents();
    }else if (ui->tabWidget->currentIndex() == 3){
        ui->tableWidget_4->viewport()->update();
        QApplication::processEvents();
    }
    if(!m_bConnected){
        qDebug() << "Waiting for Redis Server to start...";
        if(isRedisServerRunning()){
            qDebug() << "Redis Server is online. Connecting...";
            cbl::LOG_SET_WITH_LIMIT("./Log/Redis_Server_Log.txt", LOG_DEFAULT_LIMIT_FILE_SIZE, LOG_DEFAULT_LIMIT_BACKUP_AMOUNT, LOG_MODE_FILE);
            LOGI("Redis服务连接");
            this->start();
            m_bConnected = true;
        }

    }else{
        if (!isRedisServerRunning()) {
            cbl::LOG_SET_WITH_LIMIT("./Log/Redis_Server_Log.txt", LOG_DEFAULT_LIMIT_FILE_SIZE, LOG_DEFAULT_LIMIT_BACKUP_AMOUNT, LOG_MODE_FILE);
            qDebug() << "Redis Server is offline. Waiting for it to come back online...";
            LOGW("Redis服务器关闭");
            m_bConnected = false;
        }
    }
}


void MainWindow::on_pushButton_4_clicked()
{
    //ui->pushButton_4->setStyleSheet("background-color: #99FF99;font: 25 14pt;");
    if(m_vMembers.size() == 0){
        m_vMembers.push_back("ALL");
        operationRecord("你向所有设备发送了 更新的任务");
    }
    string sUpdateTask = R"+*({
                         "Tasks":{
                             "Channel": "TaskFromServer",
                             "Model": "rk3308",
                             "Members":[],
                             "Name": "Update",
                             "TaskType": 1,
                             "Time":1,
                             "Interval":0,
                             "Postscript":"sssssss"
                         }
                     }
                 )+*";
    size_t pos = sUpdateTask.find("\"Members\":[");

    if (pos != std::string::npos) {
        std::string sMembers;
        for (const auto& str : m_vMembers) {
            sMembers += "\"" + str + "\",";
        }
        sMembers.pop_back();

        sUpdateTask.insert(pos + 11, sMembers);
    }
        QString recond = "你向 ";
        if(m_nStatus == 1){
            recond += QString::fromStdString(m_vMembers[0]);
            recond += "发送了更新的任务";
            operationRecord(recond);
        } else if(m_nStatus == 2){
            recond += QString::fromStdString(m_sGroupName);
            recond += "发送了更新的任务";
            operationRecord(recond);
        }
        QFont font;
        font.setFamily("微软雅黑 Light");
        font.setPointSize(14);

    this->m_PCMaster.publish("TaskFromServer", sUpdateTask);

}

void MainWindow::on_pushButton_7_clicked()
{

    if(m_vMembers.size() == 0){
        m_vMembers.push_back("ALL");
    }        operationRecord("你向所有设备发送了 更新程序的任务");
    string sPingTask = R"+*({
                         "Tasks":{
                             "Channel": "TaskFromServer",
                             "Model": "rk3308",
                             "Members":[],
                             "Name": "Reinstall",
                             "TaskType": 3,
                             "Time":1,
                             "Interval":0,
                             "Postscript":""
                         }
                     }
                 )+*";
    size_t pos = sPingTask.find("\"Members\":[");

    if (pos != std::string::npos) {
        std::string sMembers;
        for (const auto& str : m_vMembers) {
            sMembers += "\"" + str + "\",";
        }
        sMembers.pop_back();

        sPingTask.insert(pos + 11, sMembers);
    }
    QString recond = "你向 ";
    if(m_nStatus == 1){
        recond += QString::fromStdString(m_vMembers[0]);
        recond += "发送了更新程序的任务";
        operationRecord(recond);
    } else if(m_nStatus == 2){
        recond += QString::fromStdString(m_sGroupName);
        recond += "发送了更新程序的任务";
        operationRecord(recond);
    }
    this->m_PCMaster.publish("TaskFromServer", sPingTask);
}

void MainWindow::on_pushButton_10_clicked()
{
    operationRecord("你进行了刷新机器信息操作");
    if(ui->tabWidget->currentIndex() == 0){
        ui->tableWidget->update();
    }else if(ui->tabWidget->currentIndex() == 1){
        ui->tableWidget_2->update();
    }

}

int MainWindow::recviveACK(const REDIS_RESPOSE_INFO_T &respondInfo){
    QString qsDeviceID = QString::fromStdString(respondInfo.sDeviceId);

//    for(int row = 0; row < ui->tableWidget->rowCount(); ++row){
//        QTableWidgetItem *item = ui->tableWidget->item(row, 3);
//        if(item->text() == qsDeviceID){
//            if(respondInfo.bRes){
//                ui->tableWidget->setItem(row, 15, new QTableWidgetItem(QString:: fromStdString("收到")));
//            }else{
//                ui->tableWidget->setItem(row, 15, new QTableWidgetItem(QString:: fromStdString("未收到")));
//            }
//            return 0;
//        }
//    }
    return 0;
}

void MainWindow::on_pushButton_5_clicked()
{

    if(m_vMembers.size() == 0){
        m_vMembers.push_back("ALL");
        operationRecord("你向所有设备发送了 重启的任务");
    }
    string sRebootTask = R"+*({
                         "Tasks":{
                             "Channel": "TaskFromServer",
                             "Model": "rk3308",
                             "Members":[],
                             "Name": "Reboot",
                             "TaskType": 0,
                             "Time":1,
                             "Interval":0,
                             "Postscript":"sssssss"
                         }
                     }
                 )+*";
    size_t pos = sRebootTask.find("\"Members\":[");

    if (pos != std::string::npos) {
        std::string sMembers;
        for (const auto& str : m_vMembers) {
            sMembers += "\"" + str + "\",";
        }
        sMembers.pop_back();
        sRebootTask.insert(pos + 11, sMembers);
    }

    QString recond = "你向 ";
    if(m_nStatus == 1){
        recond += QString::fromStdString(m_vMembers[0]);
        recond += "发送了重启的任务";
        operationRecord(recond);
    } else if(m_nStatus == 2){
        recond += QString::fromStdString(m_sGroupName);
        recond += "发送了重启的任务";
        operationRecord(recond);
    }
    this->m_PCMaster.publish("TaskFromServer", sRebootTask);
}


void MainWindow::on_pushButton_11_clicked()
{
    /*
     * delete folder
     */

    int ret = QMessageBox::question(this, "警告",
                                         "是否真的要进行程序自毁",
                                         QMessageBox::Yes | QMessageBox::No,
                                         QMessageBox::No);
    if (ret == QMessageBox::Yes) {


    if(m_vMembers.size() == 0){
        m_vMembers.push_back("ALL");
                operationRecord("你向所有设备发送了 自毁的任务");
    }
    string sDestructTask = R"+*({
                         "Tasks":{
                             "Channel": "TaskFromServer",
                             "Model": "rk3308",
                             "Members":[],
                             "Name": "Destruct",
                             "TaskType": 5,
                             "Time":1,
                             "Interval":0,
                             "Postscript":""
                         }
                     }
                 )+*";

    size_t pos = sDestructTask.find("\"Members\":[");

    if (pos != std::string::npos) {
        std::string sMembers;
        for (const auto& str : m_vMembers) {
            sMembers += "\"" + str + "\",";
        }
        sMembers.pop_back();

        sDestructTask.insert(pos + 11, sMembers);
    }
    QString recond = "你向 ";
    if(m_nStatus == 1){
        recond += QString::fromStdString(m_vMembers[0]);
        recond += "发送了自毁的任务";
        operationRecord(recond);
    } else if(m_nStatus == 2){
        recond += QString::fromStdString(m_sGroupName);
        recond += "发送了自毁的任务";
        operationRecord(recond);
    }
    this->m_PCMaster.publish("TaskFromServer", sDestructTask);
    }
}


void MainWindow::on_pushButton_13_clicked()
{
    if(m_vMembers.size() == 0){
        m_vMembers.push_back("ALL");
        operationRecord("你向所有设备发送了 蜂鸣的任务");
    }
    string sDestructTask = R"+*({
                         "Tasks":{
                             "Channel": "TaskFromServer",
                             "Model": "rk3308",
                             "Members":[],
                             "Name": "Beep",
                             "TaskType": 7,
                             "Time":1,
                             "Interval":0,
                             "Postscript":""
                         }
                     }
                 )+*";

    size_t pos = sDestructTask.find("\"Members\":[");

    if (pos != std::string::npos) {
        std::string sMembers;
        for (const auto& str : m_vMembers) {
            sMembers += "\"" + str + "\",";
        }
        sMembers.pop_back();

        sDestructTask.insert(pos + 11, sMembers);
    }
    QString recond = "你向 ";
    if(m_nStatus == 1){
        recond += QString::fromStdString(m_vMembers[0]);
        recond += "发送了提高蜂鸣的任务";
        operationRecord(recond);
    } else if(m_nStatus == 2){
        recond += QString::fromStdString(m_sGroupName);
        recond += "发送了提高蜂鸣的任务";
        operationRecord(recond);
    }

    this->m_PCMaster.publish("TaskFromServer", sDestructTask);
}

void MainWindow::on_pushButton_14_clicked()
{
    if(m_vMembers.size() == 0){
        m_vMembers.push_back("ALL");
        operationRecord("你向所有设备发送了 提高背光的任务");
    }
    string sDestructTask = R"+*({
                         "Tasks":{
                             "Channel": "TaskFromServer",
                             "Model": "rk3308",
                             "Members":[],
                             "Name": "LightUp",
                             "TaskType": 8,
                             "Time":1,
                             "Interval":0,
                             "Postscript":"100"
                         }
                     }
                 )+*";

    size_t pos = sDestructTask.find("\"Members\":[");

    if (pos != std::string::npos) {
        std::string sMembers;
        for (const auto& str : m_vMembers) {
            sMembers += "\"" + str + "\",";
        }
        sMembers.pop_back();

        sDestructTask.insert(pos + 11, sMembers);
    }
    QString recond = "你向 ";
    if(m_nStatus == 1){
        recond += QString::fromStdString(m_vMembers[0]);
        recond += "发送了提高背光的任务";
        operationRecord(recond);
    } else if(m_nStatus == 2){
        recond += QString::fromStdString(m_sGroupName);
        recond += "发送了提高背光的任务";
        operationRecord(recond);
    }

    this->m_PCMaster.publish("TaskFromServer", sDestructTask);
}


void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    column = 1;
    if (m_buttonClicked) {
        ui->tableWidget->selectRow(row);
        QString qsDeviceId = ui->tableWidget->item(row, 3)->text();

        QString qsDeviceIP = ui->tableWidget->item(row, 0)->text();
        m_vMembers.push_back(qsDeviceId.toStdString());
        m_sSingleDevice = qsDeviceId.toStdString();
        QString recond  = "你选择了：";
        recond += qsDeviceIP;
        recond += "作为单台设备进行进行后续操作";
        operationRecord(recond);
        m_nStatus = 1;
        ui->tableWidget->setRowCount(0);
        m_buttonClicked = false;
        QFont font;
        font.setFamily("微软雅黑 Light");
        font.setPointSize(14);
        ui->pushButton_9->setFont(font);
        ui->pushButton_9->setStyleSheet(QString());
        ui->tabWidget->setCurrentIndex(3);

    } else if (m_isCmd){
        ui->tableWidget->selectRow(row);
        QString qsDeviceIP = ui->tableWidget->item(row, 0)->text();
        m_CCmd->set(qsDeviceIP);
        m_CCmd->show();
        QFont font;
        font.setFamily("微软雅黑 Light");
        font.setPointSize(14);
        ui->pushButton_12->setFont(font);
        ui->pushButton_12->setStyleSheet(QString());
        m_isCmd = false;
        QString recond =  "你选择了";
        recond += qsDeviceIP;
        recond += "进行SSH的命令行查询的操作";
        operationRecord(recond);
    }

}

void MainWindow::on_pushButton_9_clicked()
{
    operationRecord("你选择单台设备进行后续的操作");
    QMessageBox::information(this, "提示", "请双击点击列表中想要进行操作的设备");
    m_vMembers.clear();
    m_nStatus = 0;
    ui->pushButton_9->setStyleSheet("background-color: #99FF99;font: 25 14pt;");
    m_buttonClicked = true;
}

void MainWindow::on_pushButton_8_clicked()
{
    if(ui->tabWidget->currentIndex() != 0){
        ui->tabWidget->setCurrentIndex(0);
    }
    QMessageBox messageBox;
        messageBox.setText("请选择一个选项");
        messageBox.addButton("新建分组", QMessageBox::ActionRole);
        messageBox.addButton("已有分组", QMessageBox::ActionRole);
        messageBox.exec();
        if (messageBox.clickedButton() == messageBox.buttons().at(0)) {
            bool ok;
            m_multiClickEnabled = true;
            m_nStatus = 0;
            m_vMembers.clear();
            QString groupName = QInputDialog::getText(this, "新建分组", "请输入新建分组的名称:", QLineEdit::Normal, "", &ok);
            QString recond = "你新建了一个分组， 分组名称为：";
            ui->pushButton_8->setStyleSheet("background-color: #99FF99;font: 25 14pt;");
            recond += groupName;
            operationRecord(recond);
            if (ok && !groupName.isEmpty()) {
                QMessageBox::information(this, "提示", "请双击点击列表中想要添加入组的设备"
                                                     " 添加完设备后请点击分组设备中的已有分组来查看所创建的分组");
                m_sGroupName = groupName.toStdString();
                connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) {
                    column = 1;
                    QTableWidgetItem* item = ui->tableWidget->item(row, 3);

                    if(m_multiClickEnabled){
                        if (item) {

                           auto it = std::find(m_vMembers.begin(), m_vMembers.end(), (item->text().toStdString()));
                           if( it != m_vMembers.end()){
                               QMessageBox::information(this, "提示", "请勿重复添加该设备");
                           } else{
                               QString data = item->text();
                               QString recond = "你选择将 ";
                               recond += ui->tableWidget->item(row, 0)->text();
                               recond += "添加入";
                               recond += QString::fromStdString(m_sGroupName);
                               operationRecord(recond);
                               m_vMembers.push_back(data.toStdString());
                           }
                        }
                    }
                });
            }

        } else if (messageBox.clickedButton() == messageBox.buttons().at(1)) {

           m_multiClickEnabled = false;
           if(! m_Groups.count(m_sGroupName)){
               m_Groups[m_sGroupName] = m_vMembers;
           }
           QStringList options;
           options << "PI3070ie" << "PI3070ig" << "PI8000ig" << "PI3070i";
           for(const auto&  group : m_Groups){
               options << group.first.c_str();
           }
           QFont font;
           font.setFamily("微软雅黑 Light");
           font.setPointSize(14);

           ui->pushButton_8->setFont(font);
           ui->pushButton_8->setStyleSheet(QString());
           ui->comboBox->clear();
           ui->comboBox->addItems(options);
        }

}

void MainWindow::on_comboBox_activated(const QString &arg1)
{

    m_vMembers.clear();
    this->m_sGroupName = arg1.toStdString();
    for(const auto& member : m_Groups[m_sGroupName]){
        m_vMembers.push_back(member);
    }

    m_nStatus = 2;
    if(arg1 == "PI3070ie"){
        m_nStatus = 3;
    } else if (arg1 == "PI3070ig"){
        m_nStatus = 4;
    } else if (arg1 == "PI3070i"){
        m_nStatus = 5;
    } else if (arg1 == "PI8000ig"){
        m_nStatus = 6;
    }
    this->ui->tableWidget->setRowCount(m_Groups[m_sGroupName].size());
    QString recond =  "你选择了";
    recond += arg1;
    recond += "分组进行后续的操作";
    ui->tabWidget->setCurrentIndex(2);
    ui->tableWidget_3->setRowCount(0);
    operationRecord(recond);
}



void MainWindow::on_pushButton_2_clicked()
{
    if(ui->tabWidget->currentIndex() != 0){
        ui->tabWidget->setCurrentIndex(0);
    }
    if(!m_sTaskInfo.empty()){
        if(m_vMembers.size() == 0){
            m_vMembers.push_back("ALL");
            QString recond = "你向所有设备发送了: 任务名称为";
            recond += QString::fromStdString(m_sTaskName);
            recond += "的任务";
            operationRecord(recond);
        }
        std::string sTaskInfoTemp = m_sTaskInfo;
        size_t pos = sTaskInfoTemp.find("\"Members\":[");
        if (pos != std::string::npos) {
            std::string sMembers;
            for (const auto& str : m_vMembers) {
                sMembers += "\"" + str + "\",";
            }
            sMembers.pop_back();

            sTaskInfoTemp.insert(pos + 11, sMembers);
        }

        int ret = QMessageBox::question(this, "提示",
                                             "是否要发布此任务？ \n\n" + QString::fromStdString(sTaskInfoTemp),
                                             QMessageBox::Yes | QMessageBox::No,
                                             QMessageBox::No);
        if(ret == QMessageBox::Yes){
            QString recond = "你向 ";
            if(m_nStatus == 1){
                recond += QString::fromStdString(m_vMembers[0]);
            } else {
                recond += QString::fromStdString(m_sGroupName);
            }
            recond += "发送了: 任务名称为";
            recond += QString::fromStdString(m_sTaskName);
            recond += "的任务";
            operationRecord(recond);

            this->m_PCMaster.publish("TaskFromServer", sTaskInfoTemp);
        }
    } else {
        QMessageBox::information(this, "提示", "并未选择自建任务进行发布");
    }
}


int MainWindow::ping(){
    return this->m_PCMaster.ping();
}
bool MainWindow::isConnected(){
    return this->m_PCMaster.isConnected();
}


void MainWindow::on_comboBox_2_activated(const QString &arg1)
{
    if(ui->tabWidget->currentIndex() != 0){
        ui->tabWidget->setCurrentIndex(0);
    }
    m_sTaskInfo.clear();
    this->m_sTaskName = arg1.toStdString();
    m_sTaskInfo = m_Tasks[m_sTaskName];
    QString recond =  "你选择了";
    recond += arg1 ;
    recond += "任务进行发布";
    operationRecord(recond);
}

void MainWindow::on_pushButton_12_clicked()
{
    if(ui->tabWidget->currentIndex() != 0){
        ui->tabWidget->setCurrentIndex(0);
    }
    QMessageBox::information(this, "提示", "请双击选择设备进行命令行操作查询");
    m_isCmd =true;
    ui->pushButton_12->setStyleSheet("background-color: #99FF99;font: 25 14pt;");
}


void MainWindow::on_pushButton_15_clicked()
{
    if(ui->tabWidget->currentIndex() != 0){
        ui->tabWidget->setCurrentIndex(0);
    }
    QMessageBox::information(this, "提示", "请单机选择设备ip进入局域网后台进行操作查询");
    m_bGotoUrl = true;
    ui->pushButton_15->setStyleSheet("background-color: #99FF99;font: 25 14pt;");
}




void MainWindow::on_tabWidget_currentChanged(int index)
{

}

void MainWindow::on_toolButton_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}
