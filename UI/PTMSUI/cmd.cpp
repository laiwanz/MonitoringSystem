#include "cmd.h"
#include "ui_cmd.h"
#include "RedisPubSuber.h"
#include <QDebug>

Cmd::Cmd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Cmd)
{
    ui->setupUi(this);
}

Cmd::~Cmd()
{
    delete ui;
}

void Cmd::Connect(const QString& m_sIP){
    QString strUser = "root";
    QString strPwd = "we@con&*";
    m_sshSocket = new CConnectionForSshClient(m_sIP,22,strPwd,strUser);
    m_sshSocket->init();
    connect(m_sshSocket,SIGNAL(sigDataArrived(QString ,QString , int )),
            this,SLOT(slotDataArrived(QString ,QString , int )));
    connect(this,SIGNAL(sigSend(QString)),m_sshSocket,SLOT(slotSend(QString)));

    qDebug() << "Successful" ;
}

void Cmd::on_pushButton_clicked()
{
    QString strCmd = ui->lineEdit->text();
    strCmd += "\n"; //添加回车
    emit sigSend(strCmd);
}


void Cmd::on_pushButton_2_clicked()
{
    ui->textEdit->clear();
}

void Cmd::slotDataArrived(QString strMsg, QString strIp, int nPort)
{
    Q_UNUSED(strIp)
    Q_UNUSED(nPort)

    ui->textEdit->append(strMsg);
    ui->textEdit->moveCursor(QTextCursor::End);
}

