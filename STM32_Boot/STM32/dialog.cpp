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

}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::update_com(void)
{
    const auto infos = QSerialPortInfo::availablePorts();
    ui->com->clear();
    for (const QSerialPortInfo &info : infos) {
        ui->com->addItem(info.portName());
        /*
        QString s = QObject::tr("Port: ") + info.portName() + "\n"
                    + QObject::tr("Location: ") + info.systemLocation() + "\n"
                    + QObject::tr("Description: ") + info.description() + "\n"
                    + QObject::tr("Manufacturer: ") + info.manufacturer() + "\n"
                    + QObject::tr("Serial number: ") + info.serialNumber() + "\n"
                    + QObject::tr("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
                    + QObject::tr("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
                    + QObject::tr("Busy: ") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";
        */
    }
}

void Dialog::on_open_file_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Text Files(*.bin);;Text Files(*.hex)"));
    QList <QString> path_temp = path.split("/");
    int index = path_temp.length()-1;
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
    serial = QSerialPort(ui->com->currentText());
}
