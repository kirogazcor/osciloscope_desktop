#include "port.h"
#include <qdebug.h>

Port::Port(QObject *parent) :
    QObject(parent)
{
}

Port::~Port()
{
    qDebug("Port is dead");
    emit finished_Port();
}

void Port :: process_Port(){
    qDebug("Port is born");
    connect(&thisPort,SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(&thisPort, SIGNAL(readyRead()),this,SLOT(ReadInPort()));
}

void Port :: ConnectPort(QString name)
{
    thisPort.setPortName(name);
    if (thisPort.open(QIODevice::ReadWrite))
    {
        if (thisPort.setBaudRate(12582912)  // 12 mbps
            && thisPort.setDataBits(QSerialPort::Data8)
            && thisPort.setParity(QSerialPort::NoParity)
            && thisPort.setStopBits(QSerialPort::OneStop)
            && thisPort.setFlowControl(QSerialPort::NoFlowControl))
        {
            if (thisPort.isOpen()){
                emit connected();
            }
        } else {
            thisPort.close();
            qDebug() << thisPort.errorString();
          }
    } else {
        thisPort.close();
        qDebug() << thisPort.errorString();
    }
}

// Обработка ошибок
void Port::handleError(QSerialPort::SerialPortError error)//
{
    if ( (thisPort.isOpen()) && (error == QSerialPort::ResourceError)) {
        qDebug() << thisPort.errorString();
        DisconnectPort();
    }
}

// Отключение от порта
void  Port::DisconnectPort(){
    if(thisPort.isOpen()){
        thisPort.close();
        emit disconnected();
    }
}
// Запись данных в порт
void Port :: WriteToPort(QByteArray data){
    if(thisPort.isOpen()){
        thisPort.write(data);
    }
}
// Получение данных из порта
void Port :: ReadInPort(){
    QByteArray data;

    data.append(thisPort.readAll());
    emit outData(data);
}

