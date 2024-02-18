#include "task.h"
#include "ui_task.h"
#include <QMap>
#include "cbl/cbl.h"
#include <vector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QString>

Task::Task(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Task)
{
    ui->setupUi(this);
}

Task::~Task()
{
    delete ui;
}

void Task::on_pushButton_clicked()
{
    QStringList options;
    options << "TaskFromServer" << "OrderFromServer" ;

    bool ok;
    QString selectedOption = QInputDialog::getItem(this, "选择频道", "请选择一个频道：", options, 0, false, &ok);

    if (ok && !selectedOption.isEmpty())
    {
        m_sChannel = selectedOption.toStdString();
        ui->tableWidget->setItem(0, 1, new QTableWidgetItem(selectedOption));
//        QMessageBox::information(this, "选中的参数", "您选择了：" + selectedOption);
    };
}

void Task::on_pushButton_2_clicked()
{
    QStringList options;
    options << "a8plus" << "rk3308" << "x86";

    bool ok;
    QString selectedOption = QInputDialog::getItem(this, "选择参数", "请选择一个参数：", options, 0, false, &ok);

    if (ok && !selectedOption.isEmpty())
    {
        m_sModel = selectedOption.toStdString();
        ui->tableWidget->setItem(0, 2, new QTableWidgetItem(selectedOption));
//        QMessageBox::information(this, "选中的参数", "您选择了：" + selectedOption);
    };
}

void Task::on_pushButton_7_clicked()
{
    bool inputValid = false;
    while (!inputValid)
    {
        bool ok;
        QString inputText = QInputDialog::getText(this, tr("任务的名称"), tr("请输入名称："), QLineEdit::Normal, "", &ok);

        if (ok)
        {
            std::string inputString = inputText.toStdString();
            if (!inputText.isEmpty())
            {
                ui->tableWidget->setItem(0, 0 ,new QTableWidgetItem(inputText));
                m_sTaskName = inputString;
                inputValid = true;
            }
        }
        else
        {
            break;
        }
    }
}

void Task::on_pushButton_3_clicked()
{
    QStringList options;
    options << "Reboot" << "Update" << "Reinstall" << "Add Channel" << "Print" << "Close" << "Order" << "Beep" << "BackLisght";
    QMap<QString, int> optionMap;
    optionMap["Reboot"] = 0;
    optionMap["Update"] = 1;
    optionMap["Reinstall"] = 2;
    optionMap["Add Channel"] = 3;
    optionMap["Print"] = 4;
    optionMap["Close"] = 5;
    optionMap["Order"] = 6;
    optionMap["Beep"] = 7;
    optionMap["BackLight"] = 8;

    bool ok;
    QString selectedOption = QInputDialog::getItem(this, "选择任务类型", "请选择一个任务：", options, 0, false, &ok);

    if (ok && !selectedOption.isEmpty())
    {
        if (optionMap.contains(selectedOption)) {
            int taskType = optionMap[selectedOption];
            m_nTaskType = taskType;
            // 进行其他操作
            ui->tableWidget->setItem(0, 3, new QTableWidgetItem(selectedOption));
//            QMessageBox::information(this, "选中的参数", "您选择了：" + selectedOption);
         };
    };

}

void Task::on_pushButton_4_clicked()
{
    bool inputValid = false;
    while (!inputValid)
    {
        bool ok;
        QString inputText = QInputDialog::getText(this, tr("执行任务的次数:"), tr("请输入执行次数:"), QLineEdit::Normal, "", &ok);

        if (ok)
        {
            if (!inputText.isEmpty()){
                bool convertOk;
                int inputValue = inputText.toInt(&convertOk);

                if (convertOk)
                {
                    ui->tableWidget->setItem(0, 4 ,new QTableWidgetItem(QString::number(inputValue)));
                    m_nTimes = inputValue;
                    inputValid = true;
                }else{
                    (QMessageBox::warning(this, ("错误"), ("输入的内容不是有效的数字")));
                }
            }else{
                QMessageBox::warning(this, ("错误"), ("请输入一个数字请重新输入。"));
            }
        }else{
            break;
        }
    }
}

void Task::on_pushButton_5_clicked()
{
    bool inputValid = false;
    while (!inputValid)
    {
        bool ok;
        QString inputText = QInputDialog::getText(this, tr("任务执行的间隔时间"), tr("请输入间隔时间："), QLineEdit::Normal, "", &ok);

        if (ok)
        {
            if (!inputText.isEmpty())
            {
                bool convertOk;
                int inputValue = inputText.toInt(&convertOk);

                if (convertOk)
                {
                    ui->tableWidget->setItem(0, 5 ,new QTableWidgetItem(QString::number(inputValue)));
                    m_nInterval = inputValue;
                    inputValid = true; // 输入有效，结束循环
                }
                else
                {
                    QMessageBox::warning(this, tr("错误"), tr("输入的内容不是有效的数字！"));
                }
            }
            else
            {
                QMessageBox::warning(this, tr("错误"), tr("请输入一个数字！"));
            }
        }
        else
        {
            break;
        }

    }

}

void Task::on_pushButton_6_clicked()
{
    std::cout << "setMessage" << std::endl;
    bool inputValid = false;
    while (!inputValid)
    {
        bool ok;
        QString inputText = QInputDialog::getText(this, tr("任务执行的附带信息"), tr("请输入附加信息："), QLineEdit::Normal, "", &ok);

        if (ok)
        {
            std::string inputString = inputText.toStdString();
            if (!inputText.isEmpty())
            {
                ui->tableWidget->setItem(0, 6 ,new QTableWidgetItem(inputText));
                m_sPostscript = inputString;
                inputValid = true;
            }
        }
        else
        {
            break;
        }
    }
}


int Task::config(FUNCTION_NEWTASK_FUNCTION func){
    m_func = func;
    return 0;
}

static QString fillJsonString(const QString& jsonString,
                       const QString& channel,
                       const QString& model,
                       const QString& taskName,
                       int taskType,
                       int times,
                       int interval,
                       const QString& postscript)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!doc.isObject()) {
        return jsonString;
    }
    QJsonObject obj = doc.object();
    QJsonValue tasksValue = obj.value("Tasks");
    if (!tasksValue.isObject()) {
        return jsonString;
    }
    QJsonObject tasksObj = tasksValue.toObject();
    tasksObj["Channel"] = channel;
    tasksObj["Model"] = model;
    tasksObj["Name"] = taskName;
    tasksObj["TaskType"] = taskType;
    tasksObj["Time"] = times;
    tasksObj["Interval"] = interval;
    tasksObj["Postscript"] = postscript;
    obj["Tasks"] = tasksObj;
    doc.setObject(obj);
    return doc.toJson(QJsonDocument::Compact);
}

void Task::on_pushButton_8_clicked()
{
    /* create the task infomation
     */
    QString qsTaskInfo = R"+*({
        "Tasks": {
            "Channel": "",
            "Model": "",
            "Members": [],
            "Name": "",
            "TaskType": 9,
            "Time": 0,
            "Interval": 0,
            "Postscript": ""
        }
    })+*";

    QString TaskJsonString = fillJsonString(qsTaskInfo, QString::fromStdString(m_sChannel), QString::fromStdString(m_sModel),
                                              QString::fromStdString(m_sTaskName), m_nTaskType, m_nTimes, m_nInterval,
                                              QString::fromStdString(m_sPostscript));

    m_taskInfo = TaskJsonString.toStdString();
    m_func(m_sTaskName, m_taskInfo);
    this->close();
}

void Task::on_tableWidget_cellActivated(int row, int column)
{
    std::cout << row << column << std::endl;
}
