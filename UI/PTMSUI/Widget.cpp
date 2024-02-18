#include "Widget.h"
#include <QDebug>
#include <QGridLayout>
#include <iostream>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{

}

Widget::~Widget()
{

}

void Widget::initUI()
{
    int nItemHeight = 30;
    QFont font;
    font.setFamily("微软雅黑");
    font.setPointSize(10);
    m_bConnectState = false;
    resize(1000,800);

    QLabel *pLabelIp = new QLabel(this);
    pLabelIp->setText("IP");
    pLabelIp->setMinimumHeight(nItemHeight);
    pLabelIp->setFont(font);

    QLabel *pLabelUser = new QLabel(this);
    pLabelUser->setText("UserName");
    pLabelUser->setFont(font);
    pLabelUser->setMinimumHeight(nItemHeight);

    QLabel *pLabelPort = new QLabel(this);
    pLabelPort->setText("Password");
    pLabelPort->setMinimumHeight(nItemHeight);
    pLabelPort->setFont(font);

    m_pComBoxIp = new QComboBox(this);
    m_pComBoxIp->addItem("192.168.55.173");
    m_pComBoxIp->setFont(font);
    m_pComBoxIp->setMinimumHeight(nItemHeight);

    m_pComBoxIp->setEditable(true);

    m_pComBoxUser = new QComboBox(this);
    m_pComBoxUser->addItem("root");
    m_pComBoxUser->setFont(font);
    m_pComBoxUser->setMinimumHeight(nItemHeight);
    m_pComBoxUser->setEditable(true);

    m_pLineEditPwd = new QLineEdit(this);
    m_pLineEditPwd->setEchoMode(QLineEdit::Password);
    m_pLineEditPwd->setMinimumHeight(nItemHeight);
    m_pLineEditPwd->setFont(font);
    m_pLineEditPwd->setText("we@con&*");

    m_pLabelState = new QLabel(this);
    m_pLabelState->setFixedSize(20,20);
    setConnectState(m_bConnectState);

    m_pBtnConnect = new QPushButton(this);
    m_pBtnConnect->setText("Connect");
    m_pBtnConnect->setMinimumHeight(nItemHeight);
    m_pBtnConnect->setFont(font);
    connect(m_pBtnConnect,SIGNAL(clicked()),this,SLOT(slotSshConnect()));

    m_pComBoxCmd = new QComboBox(this);
    m_pComBoxCmd->addItem("ls -la");
    m_pComBoxCmd->addItem("top");
    m_pComBoxCmd->addItem("cd /mnt/data");
    m_pComBoxCmd->setEditable(true);
    m_pComBoxCmd->setFont(font);
    m_pComBoxCmd->setMinimumHeight(nItemHeight*2);

    m_pBtnSend = new QPushButton(this);
    m_pBtnSend->setText("Publish");
    m_pBtnSend->setFont(font);
    m_pBtnSend->setMinimumHeight(nItemHeight*2);
    connect(m_pBtnSend,SIGNAL(clicked()),this,SLOT(slotSshSendCmd()));

    m_pTextEdit = new QTextEdit(this);
    m_pTextEdit->setFont(font);


    QGridLayout *layout = new QGridLayout;
    layout->addWidget(pLabelIp,     0,0,1,1); //IP地址Label  坐标第0行0列  所占空间大小 1行1列
    layout->addWidget(pLabelUser,   1,0,1,1);
    layout->addWidget(pLabelPort,   2,0,1,1);
    layout->addWidget(m_pLabelState,3,0,1,1);//连接状态Label  坐标第3行0列  所占空间大小 1行1列

    layout->addWidget(m_pComBoxIp,   0,1,1,2);
    layout->addWidget(m_pComBoxUser, 1,1,1,2);
    layout->addWidget(m_pLineEditPwd,2,1,1,2);
    layout->addWidget(m_pBtnConnect, 3,1,1,2);

    layout->addWidget(m_pTextEdit, 0,3,4,8);

    layout->addWidget(m_pComBoxCmd, 4,3,1,7);
    layout->addWidget(m_pBtnSend,  4,10,1,1);

    this->setLayout(layout);
}

void Widget::createConnection()
{

    QString strIp = m_pComBoxIp->currentText();
    QString strUser = m_pComBoxUser->currentText();
    QString strPwd = m_pLineEditPwd->text();

    m_sshSocket = new CConnectionForSshClient(strIp,22,strPwd,strUser);

    m_sshSocket->init();

    connect(m_sshSocket,SIGNAL(sigConnectStateChanged(bool,QString,int)),
            this,SLOT(slotConnectStateChanged(bool,QString,int)));

    connect(m_sshSocket,SIGNAL(sigDataArrived(QString ,QString , int )),
            this,SLOT(slotDataArrived(QString ,QString , int )));

    connect(this,SIGNAL(sigSend(QString)),m_sshSocket,SLOT(slotSend(QString)));

    connect(this,SIGNAL(sigDisconnected()),m_sshSocket,SLOT(slotDisconnected()));

}

void Widget::disConnection()
{

    emit sigDisconnected();
}

void Widget::setConnectState(bool bState)
{
    if(!bState)
    {
        m_pLabelState->setStyleSheet("QLabel{background-color:#ff0000;border-radius:5px;}");
    }
    else
    {
        m_pLabelState->setStyleSheet("QLabel{background-color:#00ff00;border-radius:5px;}");
    }
}

void Widget::set(const QString sIP){

    if(m_bConnectState){
        slotSshConnect();
    }
    m_pComBoxIp->insertItem(0, sIP);
    m_pComBoxIp->setCurrentText(sIP);
    slotSshConnect();
    m_pTextEdit->clear();
}


void Widget::slotSshConnect()
{

    if(!m_bConnectState){
        m_pBtnConnect->setText("Connecting...");
        createConnection();
    }else{
        m_pBtnConnect->setText("Connect");
        m_bConnectState = false;
        emit sigDisconnected();
        setConnectState(m_bConnectState);
    }
}

void Widget::slotSshSendCmd()
{

    if(m_bConnectState){
        QString strCmd = m_pComBoxCmd->currentText();
        strCmd += "\n"; //添加回车
        if(strCmd == "clear\n"){
            m_pTextEdit->clear();
        }else{
            emit sigSend(strCmd);
        }
    }
}


void Widget::slotConnectStateChanged(bool bState, QString strIp, int nPort)
{

    Q_UNUSED(strIp)
    Q_UNUSED(nPort)

    m_bConnectState = bState;
    setConnectState(m_bConnectState);
    if(m_bConnectState)
        m_pBtnConnect->setText("Disconnect");
    else
        m_pBtnConnect->setText("Connect");
}

void Widget::slotDataArrived(QString strMsg, QString strIp, int nPort)
{
    Q_UNUSED(strIp)
    Q_UNUSED(nPort)

    m_pTextEdit->append(strMsg);
    m_pTextEdit->moveCursor(QTextCursor::End);
}
