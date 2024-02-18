#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include "CConnectionForSshClient.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
    void initUI();
    void set(const QString sIP);

private:
    void createConnection();
    void disConnection();
    void setConnectState(bool bState);
private slots:
    void slotSshConnect();
    void slotSshSendCmd();
    void slotConnectStateChanged(bool bState,QString strIp,int nPort);
    void slotDataArrived(QString strMsg,QString strIp, int nPort);
signals:
    void sigSend(QString strMsg);
    void sigDisconnected();
private:
    QTextEdit *m_pTextEdit;

    QComboBox *m_pComBoxIp;
    QComboBox *m_pComBoxUser;
    QLineEdit *m_pLineEditPwd;

    QPushButton *m_pBtnConnect;
    QLabel *m_pLabelState;

    QComboBox *m_pComBoxCmd;
    QPushButton *m_pBtnSend;
    bool m_bConnectState;
    CConnectionForSshClient *m_sshSocket;
};

#endif // WIDGET_H
