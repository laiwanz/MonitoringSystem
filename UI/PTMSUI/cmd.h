#ifndef CMD_H
#define CMD_H

#include <QWidget>
#include <QString>
#include <string>
#include "CConnectionForSshClient.h"
namespace Ui {
class Cmd;
}

class Cmd : public QWidget
{
    Q_OBJECT

public:
    explicit Cmd(QWidget *parent = nullptr);
    ~Cmd();
    void Connect(const QString& m_sIP);

signals:
    void sigSend(QString strMsg);
private slots:

    void slotDataArrived(QString strMsg,QString strIp, int nPort);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();



private:
    QString m_sIP;
    Ui::Cmd *ui;
    std::string m_CommandInfo;
    CConnectionForSshClient *m_sshSocket;
    bool m_bConnectState;
};

#endif // CMD_H
