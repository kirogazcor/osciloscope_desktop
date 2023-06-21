#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPalette>
#include <QColor>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "port.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Oscilloscope; }
QT_END_NAMESPACE

class Oscilloscope : public QMainWindow
{
    Q_OBJECT

public:
    Oscilloscope(QWidget *parent = nullptr);
    ~Oscilloscope();
public slots:
    void ConnectedHandler();
    void DisconnectedHandler();
    void ProcessData(QByteArray data);

private slots:
    void on_pushButton_clicked();

    void on_checkBox_toggled(bool checked);

    void on_checkBox_2_toggled(bool checked);

    void on_spinBox_valueChanged(int arg1);

private:
    Ui::Oscilloscope *ui;

    int num_samples = 401,
        sync_inc = 0,
        sync_period = 500;

signals:
    void connectDevice(QString port_name);
    void disconnectDevice();
    void writeData(QByteArray);
};
#endif // MAINWINDOW_H
