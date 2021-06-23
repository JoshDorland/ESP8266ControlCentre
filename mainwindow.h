#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_validButton_clicked();

    void on_startButton_clicked();

    void on_exitButton_clicked();

    void serialRead();

    void updateTemp(const QString sensor_reading);

    void updateAccel(const QString sensor_accelX,const QString sensor_accelY,const QString sensor_accelZ);

    void updateGyro(const QString sensor_gyroX,const QString sensor_gyroY,const QString sensor_gyroZ);

    void on_exitButton2_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *esp;
    QByteArray serialData;
    QString serialBuffer;
};
#endif // MAINWINDOW_H
