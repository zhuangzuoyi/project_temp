#include "dialog.h"
#include "ui_dialog.h"
#include "QFileDialog"
#include "QDebug"
#include "QtSerialPort/QSerialPortInfo"

#include <QList>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    update_com();
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    serial.setParity(QSerialPort::NoParity);
    connect(&serial, &QSerialPort::readyRead, this, &Dialog::readResponse);
    received_data.clear();
}

Dialog::~Dialog()
{
    delete ui;
    serial.close();
}


void Dialog::update_com(void)
{
    const auto infos = QSerialPortInfo::availablePorts();
    ui->com->clear();
    for (const QSerialPortInfo &info : infos) {
        ui->com->addItem(info.portName());
    }
}

void Dialog::on_open_file_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Text Files(*.bin);;Text Files(*.hex)"));
    QList <QString> path_temp = path.split("/");
    //int index = path_temp.length()-1;
    //ui->file_path->setText(path_temp[index]);
    ui->file_path->setText(path);
    qDebug()<<"The file is:"+path;
    QFile file(path);

    file.open(QIODevice::ReadOnly);
    fireware_data = file.readAll();
    qDebug()<<"Firmware's lenght is:"<<fireware_data.length();
    file.close();
    qDebug()<<"The dats is:"<<fireware_data;
}



void Dialog::keyPressEvent(QKeyEvent * e)
{
    if(e->key() == Qt::Key_F5)
    {
           qDebug()<<"F5 press";
           update_com();
    }
}

void Dialog::on_pushButton_clicked()
{
    serial.setPortName(ui->com->currentText());

    if (!serial.open(QIODevice::ReadWrite)) {
        qDebug()<<tr("Can't open %1, error code %2").arg(serial.portName()).arg(serial.error());
        return;
    }
    qDebug()<<"Open port success";
    send_get_mcu_cmd();
}

void Dialog::send_get_mcu_cmd(void)
{
    unsigned char get_mcu_cmd[6]={0xaa,0x00,0x01,0x00,0x01,0x00};
    unsigned char checksum=0;
    for(int i=1;i<5;i++)
    {
        checksum +=get_mcu_cmd[i];
    }
    checksum = ~checksum + 1;
    get_mcu_cmd[5] = checksum;
    serial.write((const char *)get_mcu_cmd,6);
}


void Dialog::send_get_start_add_cmd(void)
{
    unsigned char get_mcu_cmd[6]={0xaa,0x00,0x01,0x00,0x02,0x00};
    unsigned char checksum=0;
    for(int i=1;i<5;i++)
    {
        checksum +=get_mcu_cmd[i];
    }
    checksum = ~checksum + 1;
    get_mcu_cmd[5] = checksum;
    serial.write((const char *)get_mcu_cmd,6);
}


void Dialog::send_get_bootloader_version_cmd(void)
{
    unsigned char get_mcu_cmd[6]={0xaa,0x00,0x01,0x00,0x00,0x00};
    unsigned char checksum=0;
    for(int i=1;i<5;i++)
    {
        checksum +=get_mcu_cmd[i];
    }
    checksum = ~checksum + 1;
    get_mcu_cmd[5] = checksum;
    serial.write((const char *)get_mcu_cmd,6);
}

void Dialog::send_erea_app_cmd(void)
{
    unsigned char get_mcu_cmd[6]={0xaa,0x00,0x01,0x02,0x00,0x00};
    unsigned char checksum=0;
    for(int i=1;i<5;i++)
    {
        checksum +=get_mcu_cmd[i];
    }
    checksum = ~checksum + 1;
    get_mcu_cmd[5] = checksum;
    serial.write((const char *)get_mcu_cmd,6);
}



void Dialog::readResponse()
{
    QByteArray dat = serial.readAll();
    received_data.append(dat);
    qDebug()<<"read"<<dat.length()<<"data";
}

void Dialog::on_close_clicked()
{
    serial.close();
}

void Dialog::on_comboBox_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        send_get_start_add_cmd();
        break;
    case 1:
        send_get_bootloader_version_cmd();
        break;
    case 2:
        send_erea_app_cmd();
        break;
    case 3:
        break;
    default:
        break;
    }
}
