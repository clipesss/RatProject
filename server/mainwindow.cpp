#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ServerStartup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ServerStartup()
{
    server = new QTcpServer(this);

    QObject::connect(server,&QTcpServer::newConnection,this,[&](){
        client = server->nextPendingConnection();
        qDebug() << "[+] Client succesful connected to the server. Client IP: " << client->peerAddress();


        QObject::connect(client,&QTcpSocket::readyRead,this,[&](){
            qDebug() << "[+] New message from the client. Client IP: " << client->localAddress();
        });

        QObject::connect(client,&QTcpSocket::disconnected,this,[&](){
            qDebug() << "[-] Client has been disconnected from the server. Client IP: " << client->localAddress();
            client->deleteLater();
        });
    });

    if(server->listen(QHostAddress::Any,4444))
    {
        qDebug() << "[+] Server succesful connect to port.";
    }
    else
    {
        qDebug() << "[-] Server cant connect to port.";
    }
}

void MainWindow::on_pushButton_clicked()
{
    QByteArray dataForClient = "Hello Client!";
    client->write(dataForClient);
}

