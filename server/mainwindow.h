#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QLabel>
#include <QFile>
#include <QTimer>
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
    void on_pushButton_clicked();
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

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QTcpSocket *client;

    QString Files;

    int clientsCounter = 0;
    int YForLabel = 0;

    QLabel *pcNameLabel;
    QLabel *userNameLabel;
    QLabel *installDateLabel;
    QLabel *osLabel;
    QLabel *osLanguageLabel;
    QLabel *countryLabel;
    QLabel *camLabel;
};
#endif // MAINWINDOW_H
