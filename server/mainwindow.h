#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QLabel>
#include <QFile>
#include <QTimer>
#include <QFileDialog>
#include <QFileInfo>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void ServerStartup();
    void AuthClientToTable(QTcpSocket *socket);
    void getClientInformation(QTcpSocket *socket);
    void workWithFiles();
    void on_filesButton_clicked();
    void on_showC_clicked();
    void on_getPath_clicked();
    void on_deleteFile_clicked();
    void on_deleteDir_clicked();
    void on_createFile_clicked();
    void on_deleteDir_2_clicked();
    void on_copyFile_clicked();
    QString selectDirectory();


    void on_inputFile_clicked();

    void on_SaveFile_clicked();

    void on_openLink_clicked();

    void on_backToMain_clicked();

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QTcpSocket *client;

    QByteArray data;
    QString Files;
    QByteArray bufferTest;
    int clientsCounter = 0;
    int YForLabel = 0;

    QString pathCopyFile;
    QLabel *pcNameLabel;
    QLabel *userNameLabel;
    QLabel *installDateLabel;
    QLabel *osLabel;
    QLabel *osLanguageLabel;
    QLabel *countryLabel;
    QLabel *camLabel;

    bool openLink = 0;
    bool openFilesBool = 0;
    bool openClientPanel = 0;

};
#endif // MAINWINDOW_H
