#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <string>
#include <fstream>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

//This function will extract the output from the console.
std::string execute(const std::string& command) {
    system((command + " > temp.txt").c_str());

    std::ifstream ifs("temp.txt");
    std::string ret{ std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>() };
    ifs.close(); // must close the inout stream so the file can be cleaned up
    if (std::remove("temp.txt") != 0) {
        perror("Error deleting temporary file");
    }
    return ret;
}

int checkCOM(int number) {
    std::string stuff, systemCommand, command, portNumber, status, systemOutput;

    //Displays system status, changes system settings, or reconfigures ports or devices
    command = "mode ";

    //Specifies the name of the device for which you want to display the status.
    portNumber = "com" + std::to_string(number); //Converts the integer to a string

    //Requests the status of any redirected parallel printers.
    status = " /status";

    systemCommand = command + portNumber + status;

    //This will convert the string to a constant character for use in system();
    const char* systemCommandChar = systemCommand.c_str();


    systemOutput = execute(systemCommandChar);

    //This will remove the whitespaces from the above command.
    systemOutput.erase(remove_if(systemOutput.begin(), systemOutput.end(), isspace), systemOutput.end());

    //This sets two conditions, general for valid inptus of numbers, and default if the user inputs a value like 'a' the console automatically outputs Illegaldevicename-COM0.
    std::string conditionGeneral = "Illegaldevicename-COM" + std::to_string(number);
    std::string conditionDefault = "Illegaldevicename-COM0";

    //This will check if the system ouput is equal to the two conditions. If one condition is true it will say "Device not connected". If the device is connected it will say "Device Connected"
    if ((systemOutput == conditionGeneral) || (systemOutput == conditionDefault)) {
        return 0;
    }
    else {
        return 1;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    esp = new QSerialPort(this);
}

MainWindow::~MainWindow()
{
    if(esp->isOpen()){
        esp->close();
    }
    delete ui;
}


void MainWindow::on_validButton_clicked()
{
    int comValue = checkCOM(ui->comBox->value()); // This will set the returned value of 0 or 1 to comValue.
    int portValue = ui->comBox->value();
    QString portValueString = QString::number(portValue);

    if(comValue == 1){
        esp->setPortName("COM" + portValueString);
        esp->open(QSerialPort::ReadOnly);
        esp->setBaudRate(QSerialPort::Baud115200);
        esp->setDataBits(QSerialPort::Data8);
        esp->setParity(QSerialPort::NoParity);
        esp->setStopBits(QSerialPort::OneStop);
        esp->setFlowControl(QSerialPort::NoFlowControl);
        ui->status_entry->setText("Connection Succesful. ESP Device Connected.");
        QObject::connect(esp, SIGNAL(readyRead()), this, SLOT(serialRead()));
        esp->setReadBufferSize(150);
    }else{
        ui->status_entry->setText("Connection Unsuccessful. Please Check COM Ports.");
    }
}

//Goes to next page
void MainWindow::on_startButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

//Closes the application
void MainWindow::on_exitButton_clicked()
{
    close();
}

//This will read the serial data
void MainWindow::serialRead(){

    //Splits data at the space (based on arduino code)
    QStringList bufferSplit = serialBuffer.split(" ");

    if(bufferSplit.length() < 7){
        serialData = esp->readLine();
        serialBuffer += QString::fromStdString(serialData.toStdString());
    }else{
        //bufferSplit[1] is a good value
        //This is so I can obtain the full value of the temperature every time. It will check the length and if that is 5 digits it will print
        if(bufferSplit[6].length() == 5){
            qDebug() << bufferSplit;
            //These 3 lines will update the respective lcdmonitors by sending data
            MainWindow::updateTemp(bufferSplit[6]);

            MainWindow::updateAccel(bufferSplit[0],bufferSplit[1],bufferSplit[2]);

            MainWindow::updateGyro(bufferSplit[3],bufferSplit[4],bufferSplit[5]);

            serialBuffer = "";
        }else{
            serialBuffer = "";
        }
    }
}

//This will update the temperature
void MainWindow::updateTemp(const QString sensor_temp){
    QPalette pal = ui->Temp->palette();

    if(sensor_temp.toDouble() >= 30){
        pal.setColor(QPalette::Background, Qt::red);
        ui->Temp->setAutoFillBackground(true);
        ui->Temp->setPalette(pal);
        ui->Temp->show();
        ui->Temp->display(sensor_temp);
    }else if (sensor_temp.toDouble() >= 25 and sensor_temp.toDouble() < 30){
        pal.setColor(QPalette::Background, Qt::yellow);
        ui->Temp->setAutoFillBackground(true);
        ui->Temp->setPalette(pal);
        ui->Temp->show();
        ui->Temp->display(sensor_temp);
    }else if (sensor_temp.toDouble() >= 10 and sensor_temp.toDouble() < 25){
        pal.setColor(QPalette::Background, Qt::green);
        ui->Temp->setAutoFillBackground(true);
        ui->Temp->setPalette(pal);
        ui->Temp->show();
        ui->Temp->display(sensor_temp);
    }else{
        pal.setColor(QPalette::Background, Qt::blue);
        ui->Temp->setAutoFillBackground(true);
        ui->Temp->setPalette(pal);
        ui->Temp->show();
        ui->Temp->display(sensor_temp);
    }
}

//Updates acceleration data
void MainWindow::updateAccel(const QString sensor_accelX,const QString sensor_accelY,const QString sensor_accelZ){
    ui->Accel_X->display(sensor_accelX);
    ui->Accel_Y->display(sensor_accelY);
    ui->Accel_Z->display(sensor_accelZ);
}

//Updates gyro data
void MainWindow::updateGyro(const QString sensor_gyroX,const QString sensor_gyroY,const QString sensor_gyroZ){
    ui->Gyro_X->display(sensor_gyroX);
    ui->Gyro_Y->display(sensor_gyroY);
    ui->Gyro_Z->display(sensor_gyroZ);
}

//Closes the application
void MainWindow::on_exitButton2_clicked()
{
    close();
}

