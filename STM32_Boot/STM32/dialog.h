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

    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
    QByteArray fireware_data;
    QSerialPort serial;
    void update_com(void);

    void keyPressEvent(QKeyEvent*);
};

#endif // DIALOG_H
