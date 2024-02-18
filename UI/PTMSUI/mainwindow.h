#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QTimer>
#include "task.h"
#include "cmd.h"
#include "Widget.h"
#include "master.h"
#include "RedisRecDefine.h"
#include <string>
#include <QTimer>
#include <vector>
#include <map>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const std::string& sHost, int nPort, QWidget *parent = nullptr);
    ~MainWindow();
public:

    int ping();
    bool isConnected();
    int start();
    Master m_PCMaster;
private slots:
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_14_clicked();

private slots:

    void setUI();
    int printMachineInfo(const REDIS_REPORT_INFO_T reportInfo);
    int recviveACK(const REDIS_RESPOSE_INFO_T &respondInfo);

    void on_comboBox_activated(const QString &arg1);
    void MonitorConnection();


    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_comboBox_2_activated(const QString &arg1);

    void config();

    bool isRedisServerRunning();

    void checkDisconnection();

    void operationRecord(const QString &arg1);

    void gotoURL(QTableWidgetItem *item);

    void on_pushButton_15_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_toolButton_clicked();

    void showTheDate(const REDIS_REPORT_INFO_T reportInfo, QTableWidget* widget);

    void showTheAllDate(const REDIS_REPORT_INFO_T reportInfo, QTableWidget* widget);
private:
    Ui::MainWindow *ui;
    QTimer *m_DisconnectionTimer;
    Task* m_Task;
    //Cmd* m_Cmd;
    Widget* m_CCmd;

    QMap<QString, QDateTime> m_LastReportTimeMap;
    QMap<QString, REDIS_REPORT_INFO_T> m_DATE;
    bool m_buttonClicked;
    bool m_multiClickEnabled;
    bool m_simple;
    bool m_isCmd;
    bool m_bGotoUrl;
    int m_nSortStatus;
    int m_nStatus;

    std::string m_sSingleDevice;
    std::vector<std::string> m_vMembers;

    std::string m_sGroupName;
    std::map<std::string, std::vector<std::string>> m_Groups;

    std::string m_sTaskInfo;
    std::string m_sTaskName;
    std::map<std::string, std::string> m_Tasks;
    bool m_bConnected;

    int m_nTimerNum;
    std::mutex mtx; // 全局互斥锁
};
#endif // MAINWINDOW_H
