/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>

#include <QtWidgets> // QDir, QFile

#include <QProcess>

#include <QThread>

#include <QTextStream>

#define __APP_VESION_INFO__ "1.0.0.0"
#define __APP_NAME_INFO__ "NPIO_Counter"

#include "console.h"
#include "thread.h"

#ifndef OK
#define OK 0
#define ERR (-1)
#endif

#define STX             0x02
#define ETX             0x03

#define _CR_            '\r' // 0d
#define _LF_            '\n' // 0a

typedef enum
{
    LEFT_,
    // CENTER_,
    // RIGHT_
} StatusBar_Location;

typedef enum
{
    BLACK_,        // Info
    DARKRED_,      // Error
    DARKGREEN_,    // Sending
    DARKBLUE_,     // Received
    GREEN_
} monitor_txColor;


typedef enum
{
    MAIN_GROUP

} select_group;

typedef struct RT_INSTANCE {
    QString folder;
    QString url;
    QProcess* process;
    QString binary;
} RT_INSTANCE;



QT_BEGIN_NAMESPACE

class QLabel;

namespace Ui
{
    class MainWindow;
}

QT_END_NAMESPACE

class Console;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    struct CommSettings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };

    CommSettings *pMain_Serial;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    QString m_Main_Title;

    void process_Delay(int millisecondsToWait);
    void init_ui();

    void getServerInformationString();

    QString m_MainComPort;
    QString m_MainBaudRate;
    QString m_MainParity;
    QSerialPort *Main_serial;
    QQueue<char> Main_Serial_Q;
    Console *Main_console;

    QString cb_ComPort_String;
    QString cb_BaudRate_String;
    QString lineEdit_ServerIP_String;
    QString lineEdit_ServerPort_String;
    QString lineEdit_DeviceCode_String;
    QString lineEdit_SerialNumber_String;
    QString lineEdit_ReportPeriod_String;
    QString lineEdit_Threhold_String;

    QString rx_msg;
    QString tx_msg;
    QString outmsg;

    QQueue<char> Main_SerialQ;

    QString Main_rx_msg;
    QString Main_tx_msg;


signals:
    void msg_Main_readData(QByteArray rcvStr);
    void msg_readData(QByteArray rcvStr);

    void enableButton(bool);

    // void Send_Signal(QString CurrentServerInformation);

private slots:
    void about();
    void openSerialPort();
    void closeSerialPort();
    void consoleClear();

    int Main_openSerialPort();
    void Main_closeSerialPort();
    void Main_writeData(const QByteArray &data);
    void Main_readData();
    void Main_fnc_readData(QByteArray rcvStr);
    void Main_handleError(QSerialPort::SerialPortError error);

    void fnc_readData(QByteArray rcvStr);

private:
    QLabel *Left_status;
    // QLabel *Center_status;
    // QLabel *Right_status;

private:
    void initActionsConnections();

private:
    void showStatusMessage(const QString &message);

    Ui::MainWindow *m_ui = nullptr;
    QLabel *m_status = nullptr;

    Console *m_console = nullptr;
    QSerialPort *m_serial = nullptr;
    SettingsDialog *m_settings = nullptr;

    QString m_FilePath;
    QString m_iniFileName;
    void ini_Read();
    void ini_Write();

    void makeServerConfigFile();
    QString ServerConfigFile();
    void loadServerConfigFile();


private:

public:
    void showStatusMessage(int location, const QString &message);

    void ConsoleInit(void);
    void ConsolePutString(select_group group, QString str, QColor color);
    void ConsolePutData(select_group group, const QByteArray &data, QColor color);

    void Server_Info_Check();

signals:

private slots:
    void putString(select_group group, int type, const QString str);

    void DisableButton();

    void btn_Connect();
    void btn_Disconnect();

    void on_btn_Connect_clicked();

    void on_btn_Disconnect_clicked();

    void on_btn_ClearConsole_clicked();

    void on_btn_ServerInquiry_clicked();

    void on_btn_ServerReset_clicked();

    void on_btn_ServerSetting_clicked();

    void Server_Info_arr(QByteArray rcvStr);

    void on_btn_CountReset_clicked();

    void AbleButtonServerSetting();
    void DisableButtonServerSetting();

    void DetectingGateSignal(QByteArray rcvStr);

    void DetectingNetworkSignal(QByteArray rcvStr);

    void on_btn_Network_Check_clicked();

protected:



private:


       void State_Clear();

       void Main_btn_Enable(bool mode);

       void initSerialPortInfo();
       void configSerialPort();
       void serial_Settings(select_group group);
       CommSettings read_Settings(select_group group) const;
       // void serial_UpdateSettings(select_group group);

       void serial_writeData(select_group group, const QByteArray &data);

       void Display_Result(QLabel *lable, int result);

       void ServerInfo_Clear();

};



#endif // MAINWINDOW_H
