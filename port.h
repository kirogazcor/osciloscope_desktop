#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class Port : public QObject
{
    Q_OBJECT

public:
    explicit Port(QObject *parent = 0);
    ~Port();
    QSerialPort thisPort;

signals:
    void finished_Port();
    void outData(QByteArray data);
    void connected();
    void disconnected();

public slots:
    void  DisconnectPort();
    void ConnectPort(QString name);
    void process_Port();
    void WriteToPort(QByteArray data);
    void ReadInPort();

private slots:
    void handleError(QSerialPort::SerialPortError error);
};

#endif // PORT_H
