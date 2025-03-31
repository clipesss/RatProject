#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
#include <QSysInfo>
#include <QDir>
#include <QLocale>
#include <QtMultimedia/QMediaDevices>
#include <QtMultimedia>
#include <QFile>
#include <QListWidgetItem>
#include <iostream>
#include <string.h>

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
    void ClientStartup();
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QString SelectedPath;
};
#endif // MAINWINDOW_H
