#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include "QtSerialPort/QtSerialPort"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_open_file_clicked();
    void readResponse();
    void on_pushButton_clicked();

    void on_close_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::Dialog *ui;
    QByteArray fireware_data;
    QSerialPort serial;
    void update_com(void);
    void send_get_mcu_cmd(void);
    void keyPressEvent(QKeyEvent*);
    void send_get_start_add_cmd(void);
    void send_erea_app_cmd(void);
    void send_get_bootloader_version_cmd(void);
    QByteArray  received_data;
};

#endif // DIALOG_H
