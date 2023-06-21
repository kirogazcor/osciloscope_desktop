#include "oscilloscope.h"
#include "ui_oscilloscope.h"

Oscilloscope::Oscilloscope(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Oscilloscope)
{
    // Настройка графика
    ui->setupUi(this);
    ui->GraphicField->xAxis->setLabel("us");
    ui->GraphicField->yAxis->setLabel("mV");
    ui->GraphicField->xAxis->setRange(0,2000);
    ui->GraphicField->yAxis->setRange(0,5000);
    ui->GraphicField->clearGraphs();
    ui->GraphicField->addGraph();
    DisconnectedHandler();
    // Создание потока порта
    QThread *thread_New = new QThread;//Создаем поток для порта платы
    Port *PortNew = new Port();//Создаем обьект по классу
    PortNew->moveToThread(thread_New);//помешаем класс  в поток
    PortNew->thisPort.moveToThread(thread_New);//Помещаем сам порт в поток
    connect(thread_New, SIGNAL(started()), PortNew, SLOT(process_Port()));//Переназначения метода run
    connect(PortNew, SIGNAL(finished_Port()), thread_New, SLOT(quit()));//Переназначение метода выход
    connect(thread_New, SIGNAL(finished()), PortNew, SLOT(deleteLater()));//Удалить к чертям поток
    connect(PortNew, SIGNAL(finished_Port()), thread_New, SLOT(deleteLater()));//Удалить к чертям поток
    connect(this, SIGNAL(connectDevice(QString)),PortNew,SLOT(ConnectPort(QString)));
    connect(this, SIGNAL(disconnectDevice()),PortNew,SLOT(DisconnectPort()));
    connect(PortNew, SIGNAL(connected()), this, SLOT(ConnectedHandler()));
    connect(PortNew, SIGNAL(disconnected()), this, SLOT(DisconnectedHandler()));
    connect(PortNew, SIGNAL(outData(QByteArray)), this, SLOT(ProcessData(QByteArray)));//Лог ошибок
    connect(this,SIGNAL(writeData(QByteArray)),PortNew,SLOT(WriteToPort(QByteArray)));
    thread_New->start();
}

Oscilloscope::~Oscilloscope()
{
    delete ui;
}

void Oscilloscope::ConnectedHandler()
{
    QPalette p = ui->checkBox->palette();
    p.setColor(QPalette::WindowText,Qt::darkGreen);
    ui->checkBox->setPalette(p);
}

void Oscilloscope::DisconnectedHandler()
{
    QPalette p = ui->checkBox->palette();
    p.setColor(QPalette::WindowText,Qt::red);
    ui->checkBox->setPalette(p);
}

void Oscilloscope::on_pushButton_clicked()
{
    ui->comboBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->comboBox->addItem(info.portName());
    }
}


void Oscilloscope::on_checkBox_toggled(bool checked)
{
    if(checked) emit connectDevice(ui->comboBox->currentText());
    else emit disconnectDevice();
}

void Oscilloscope::ProcessData(QByteArray data)
{
    static int  start = 0,
                step = 0;
    if(++step == 8)
    {
        start += sync_inc;
        if (start >= sync_period) start -= sync_period;
        QVector<double> x(num_samples), y(num_samples);
        double x_point = 0;
        for(int i = 0; i < num_samples; ++i)
        {
            x[i] = x_point;
            y[i] = ((uint16_t*)(data.data()))[i + start];
            x_point += 5;
        }

        ui->GraphicField->graph(0)->setData(x,y);
        step = 0;
        ui->GraphicField->replot();
    }
}


void Oscilloscope::on_checkBox_2_toggled(bool checked)
{
    QByteArray data;
    if(checked) data.append(0xA5);
    else data.append(0xC3);

    emit writeData(data);
}


void Oscilloscope::on_spinBox_valueChanged(int arg1)
{
    if(arg1 > 0)
    {
        sync_inc = arg1 - (8000 % arg1);
        sync_period = (500 / arg1) * arg1;
    }
    else
    {
        sync_inc = 0;
        sync_period = 500;
    }
}

