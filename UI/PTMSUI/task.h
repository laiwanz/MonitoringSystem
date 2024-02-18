#ifndef TASK_H
#define TASK_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <iostream>

#include <QListView>
#include <QStringList>
#include <QInputDialog>
#include <QMessageBox>
#include <QMainWindow>
#include <QModelIndexList>
#include <QStandardItemModel>
#include <QTableWidget>
#include <string>
namespace Ui {
class Task;
}

using FUNCTION_NEWTASK_FUNCTION = std::function<int(const std::string &, const std::string &)>;

class Task : public QWidget
{
    Q_OBJECT

public:
    explicit Task(QWidget *parent = nullptr);
    ~Task();

    int config(FUNCTION_NEWTASK_FUNCTION func);

private slots:


    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_tableWidget_cellActivated(int row, int column);

    void on_pushButton_8_clicked();

private:
    Ui::Task *ui;
    std::string m_taskInfo;

    std::string m_sChannel;
    std::string m_sModel;
    std::string m_sTaskName;

    int m_nTaskType;
    int m_nTimes;
    int m_nInterval;
    std::string m_sPostscript;

    FUNCTION_NEWTASK_FUNCTION m_func;
};

#endif // TASK_H
