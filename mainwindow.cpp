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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"

#include <QLabel>
#include <QMessageBox>

#include <iostream>
#include "thread.h"

using namespace std;

#define _CRT_SECURE_NO_WARNINGS
#define DELAY_BASE_TIME              200
#define DELAY_TIMEOUT_COUNTER        10
#define __APP_NAME_INFO__            QString("NPIO_Counter")

#define STR_OUT_DIRECTION            QString("->")
#define STR_IN_DIRECTION             QString("<-")

#define STR_MAIN_PRIFIX              QString("main ")

#define STR_SHELL_CMD_SP             QString("sp")

#define STR_SHELL_CMD_SN             QString("sn")

#define M_STATE_SHELL_CMD_SP         0x00000001
#define STR_SELFSTART                QString("selfstart")
#define STR_SELFSTOP                 QString("selfstop")

#define STR_READIN                   QString("readin")

#define STR_CMD_SH_SYSINFO           QString ("sh_sysinfo")
#define STR_COMPLEAT                 QString ("COMPLEAT")

#define STR_CMD_SET_CONFIG           QString ("set_config")
#define STR_CMD_0                    QString ("0")

#define STR_CMD_SERVERIP             QString ("175.213.26.195")
#define STR_CMD_SERVERPORT           QString ("18900")
#define STR_CMD_NURI                 QString ("NURI")

#define STR_CMD_5                    QString ("5")    // 디폴트 시리얼 번호

#define STR_CMD_10                   QString ("10")    // 디폴트 전송주기

#define STR_CMD_18                   QString ("18")    // 디폴트 한도(threshold)
#define STR_CMD_Y                    QString ("Y")

#define STR_CMD_SOC_RECON            QString ("soc_recon")

#define STR_CMD_GATE_1_IN            QString ("GATE [1] IN!")
#define STR_CMD_GATE_2_IN            QString ("GATE [2] IN!")
#define STR_CMD_GATE_3_IN            QString ("GATE [3] IN!")
#define STR_CMD_GATE_4_IN            QString ("GATE [4] IN!")
#define STR_CMD_GATE_1_OUT           QString ("GATE [1] OUT!")
#define STR_CMD_GATE_2_OUT           QString ("GATE [2] OUT!")
#define STR_CMD_GATE_3_OUT           QString ("GATE [3] OUT!")
#define STR_CMD_GATE_4_OUT           QString ("GATE [4] OUT!")

int g_M_State = 0x00000000;
int g_J_State = 0x00000000;
int g_S_State = 0x00000000; // step
int g_E_State = 0x00000000;

#define M_STATE_SELFSTART       0x00000001
#define M_STATE_SELFSTOP        0x00000002

#define M_STATE_READIN_ERR      0x40000000
#define M_STATE_READIN_OK       0x80000000

#define J_STATE_SELFSTART       0x00000001
#define J_STATE_SELFSTOP        0x00000002

#define S_STATE_INIT            0x00000000
#define S_STATE_SELFSTART       0x00000001
#define S_STATE_SELFSTOP        0x00000002

#define SET_BIT(REG, BIT)       ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)      ((REG) & (BIT))

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
extern QString logcurtime;
extern bool check_Debug, check_Log;

bool g_SelfTestStart;
bool g_SelfTestStop;

bool g_get_readsn = false;

QString ServerIPDefault;
QString ServerPortDefault;
QString DeviceCodeDefault;
QString SerialNODefault;
QString ReportPeriodDefault;
QString ThresholdDefault;

QStringList g_aCurrentServerInformation;

QString g_ServerIP_String;
QString g_ServerPort_String;
QString g_SerialNumber_String;

QByteArray rcvStr;

// 계수기 센서 정보 데이터
bool g_bIsContainedDET_IN_0;
bool g_bIsContainedDET_IN_1;
bool g_bIsContainedDET_IN_2;
bool g_bIsContainedDET_IN_3;
bool g_bIsContainedDET_OUT_0;
bool g_bIsContainedDET_OUT_1;
bool g_bIsContainedDET_OUT_2;
bool g_bIsContainedDET_OUT_3;
//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),

    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    makeServerConfigFile();
    loadServerConfigFile();

    m_Main_Title = tr("계수기 생산 v1.0.0") + QString(__APP_VESION_INFO__);
    setWindowTitle(m_Main_Title);

    // 윈도우 Size 고정
    setFixedSize(785, 654);

    m_FilePath = qApp->applicationDirPath() + "/";

    //-------------------------------------------
    // log Dir
    QDir dir(m_FilePath + "log");
    if(!dir.exists())
        dir.mkpath(".");

    logcurtime = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss");

    m_iniFileName = m_FilePath + __APP_NAME_INFO__ + ".ini";

    ini_Read();

    ConsoleInit();

    init_ui();

    initSerialPortInfo();
    configSerialPort();

    // connect signals / slots
    initActionsConnections();

    Left_status = new QLabel;
    // Center_status = new QLabel;
    // Right_status = new QLabel;

    m_ui->statusbar->addPermanentWidget(Left_status);
    // m_ui->statusBar->addPermanentWidget(Center_status, 1);
    // m_ui->statusBar->addPermanentWidget(Right_status);

    m_status = new QLabel;
    m_ui->statusbar->addWidget(m_status);

    statusBar()->setFocus();


    // putString(MAIN_GROUP, monitor_txColor::BLACK_, outmsg.asprintf("[OK ] Start Program !!\n"));
    qDebug() << "Start Program !!";

}

MainWindow::~MainWindow()
{
    btn_Disconnect();

    ini_Write();

    delete m_ui;

    putString(MAIN_GROUP, monitor_txColor::BLACK_, outmsg.asprintf("[OK ] End Program !!\n"));

    qDebug() << "End Program !!";
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::init_ui()
{
     // 엔터 입력시 다음 줄 이동
     connect(m_ui->lineEdit_ServerIP, &QLineEdit::returnPressed, m_ui->lineEdit_ServerPort, QOverload<>::of(&QLineEdit::setFocus));
     connect(m_ui->lineEdit_ServerPort, &QLineEdit::returnPressed, m_ui->lineEdit_SerialNumber, QOverload<>::of(&QLineEdit::setFocus));
     connect(m_ui->lineEdit_SerialNumber, &QLineEdit::returnPressed, m_ui->lineEdit_ReportPeriod, QOverload<>::of(&QLineEdit::setFocus));

     // 줄바꿈,우측정렬
     m_ui->lineEdit_ServerIP->setStyleSheet("Text-align: right");
     m_ui->lineEdit_ServerPort->setStyleSheet("Text-align: right");
     m_ui->lineEdit_DeviceCode->setStyleSheet("Text-align: right");
     m_ui->lineEdit_SerialNumber->setStyleSheet("Text-align: right");
     m_ui->lineEdit_ReportPeriod->setStyleSheet("Text-align: right");
     m_ui->lineEdit_Threshold->setStyleSheet("Text-align: right");

     connect(this, SIGNAL(enableButton(bool)), m_ui->btn_Connect, SLOT(setEnabled(bool)));
     connect(this, SIGNAL(enableButton(bool)), m_ui->btn_Disconnect, SLOT(setEnabled(bool)));
     connect(this, SIGNAL(enableButton(bool)), m_ui->btn_ServerReset, SLOT(setEnabled(bool)));
     connect(this, SIGNAL(enableButton(bool)), m_ui->btn_ConnectTest, SLOT(setEnabled(bool)));
     // connect(this, SIGNAL(enableButton(bool)), m_ui->btn_ServerSetting, SLOT(setEnabled(bool)));

     DisableButton();
     m_ui->btn_ServerSetting->setEnabled(false);
}

//-----------------------------------------------------------------------------
// 볼랜드의 Application->ProcessMessages() 와 동일한 기능
//_____________________________________________________________________________
void MainWindow::process_Delay(int millisecondsToWait)
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::showStatusMessage(int location, const QString &message)
{
    // status->setText(message);

    switch(location)
    {
    case LEFT_:
        Left_status->setText(message);
        break;

/*
    case CENTER_:
        Center_status->setText(message);
        break;

    case RIGHT_:
        Right_status->setText(message);
        break;
*/
    }

    statusBar()->setFocus();
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
int MainWindow::Main_openSerialPort()
{
    int retVal = OK;

    CommSettings p = read_Settings(MAIN_GROUP);
    //  CommSettings p = *pMain_Serial;
    Main_serial->setPortName(p.name);
    //  Main_serial->setBaudRate(p.baudRate);
    Main_serial->setBaudRate(921600);
    Main_serial->setDataBits(QSerialPort::Data8);
    Main_serial->setParity(QSerialPort::NoParity);
    Main_serial->setStopBits(QSerialPort::OneStop);
    Main_serial->setFlowControl(QSerialPort::NoFlowControl);

    if (Main_serial->open(QIODevice::ReadWrite)) {
        Main_console->setEnabled(true);
        Main_console->setLocalEchoEnabled(p.localEchoEnabled);
        outmsg = Left_status->text() + " | " +
                tr("Main Connected to %1 : %2, %3, %4, %5, %6")
                .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl);
        showStatusMessage(LEFT_, outmsg);
        qDebug() << outmsg;

        // Test
        m_ui->cb_ComPort->setEnabled(true);

        m_ui->btn_Connect->setEnabled(true);
        m_ui->btn_Disconnect->setEnabled(true);

        m_ui->lineEdit_ServerIP->setEnabled(true);
        m_ui->lineEdit_ServerPort->setEnabled(true);
        m_ui->lineEdit_DeviceCode->setEnabled(true);
        m_ui->lineEdit_SerialNumber->setEnabled(true);
        m_ui->lineEdit_ReportPeriod->setEnabled(true);
        m_ui->lineEdit_Threshold->setEnabled(true);

        m_ui->btn_ServerReset->setEnabled(true);
        m_ui->btn_ServerInquiry->setEnabled(true);
        // m_ui->btn_ServerSetting->setEnabled(true);

        m_ui->Gate1_IN_label->setEnabled(true);
        m_ui->Gate2_IN_label->setEnabled(true);
        m_ui->Gate3_IN_label->setEnabled(true);
        m_ui->Gate4_IN_label->setEnabled(true);

        m_ui->Gate1_OUT_label->setEnabled(true);
        m_ui->Gate2_OUT_label->setEnabled(true);
        m_ui->Gate3_OUT_label->setEnabled(true);
        m_ui->Gate4_OUT_label->setEnabled(true);

        m_ui->network_check_label->setEnabled(true);
        m_ui->btn_CountReset->setEnabled(true);
        m_ui->btn_ConnectTest->setEnabled(true);
    }
    else
    {
        QMessageBox::critical(this, tr("Main_openSerialPort() Error"), Main_serial->errorString());

        qDebug() << "Main_openSerialPort() : Open error";

        retVal = ERR;
    }

    return retVal;
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::Main_closeSerialPort()
{
    if (Main_serial->isOpen())
    {
        Main_writeData(QString(STR_SELFSTOP+_CR_).toStdString().c_str());
        Main_serial->close();
    }
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::Main_writeData(const QByteArray &data)
{
    QByteArray write_buf;
    write_buf.clear();

    write_buf.append(STX);
    write_buf.append(data);
    write_buf.append(ETX);

    Main_serial->write(write_buf);
    Main_serial->write(data);

    qDebug() << write_buf.toHex();
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::Main_readData()
{
    QByteArray data = Main_serial->readAll();

    // qDebug() << "data.length :" << data.length();

    Main_console->putData(data);

    for(int i=0; i<data.count(); i++)
    {
        if(data[i] == _CR_)
        // if(data[i] == _LF_)
        { // 0x0d
            // qDebug() << "data.length[i] : " << i;
            emit msg_Main_readData(data);
        }
    }
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::Main_fnc_readData(QByteArray rcvStr)
{
   // qDebug() << "Main_fnc_readData() rcvStr" << rcvStr.toHex() << rcvStr.count();
   // qDebug() << "Main_fnc_readData() rcvStr" << rcvStr.toHex() << Main_SerialQ.count();
    qDebug() << "[Main_fnc_readData] rcvStr" << "[" << rcvStr.length() << "]" << Main_console->toPlainText();

    int index = -1;
    QString tx_prifix;
    int length;

    tx_prifix = STR_MAIN_PRIFIX + Main_tx_msg;

}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::DetectingNetworkSignal(QByteArray rcvStr)
{

    if (Main_serial->isOpen())
    {
        QString tempNetworkString = Main_console->toPlainText();

        // 네트워크 접속 정보 데이터 들어올 시
        bool bIsContainedETH_Link_Up = tempNetworkString.contains( "ETH Link Up", Qt::CaseInsensitive);
        bool bIsContainedETH_Link_Down = tempNetworkString.contains( "ETH Link Down", Qt::CaseInsensitive);

        //  네트워크 접속 확인
        if( true == bIsContainedETH_Link_Up )
        {
            m_ui->network_check_label->setStyleSheet("color: rgb(85, 255, 0);"
                                                            "background-color: rgb(0, 0, 0);");
            m_ui->network_check_label->setText("CONNECT!");

            tempNetworkString = QString();
        }
        else if( true == bIsContainedETH_Link_Down )
        {
            m_ui->network_check_label->setStyleSheet("color: rgb(126, 25, 27);"
                                                     "background-color: rgb(255, 255, 255);"
                                                     "border: 1px;"
                                                     "border-style: outset;");
            m_ui->network_check_label->setText("DISCONNET!");

            tempNetworkString = QString();
        }
        else
        {
            return;
        }
    }
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::DetectingGateSignal(QByteArray rcvStr)
{

    if (Main_serial->isOpen())
    {
        QString tempSensorString = Main_console->toPlainText();

        // 계수기 센서 정보 데이터
        g_bIsContainedDET_IN_0 = tempSensorString.contains( "DET IN [0]", Qt::CaseInsensitive);
        g_bIsContainedDET_IN_1 = tempSensorString.contains( "DET IN [1]", Qt::CaseInsensitive);
        g_bIsContainedDET_IN_2 = tempSensorString.contains( "DET IN [2]", Qt::CaseInsensitive);
        g_bIsContainedDET_IN_3 = tempSensorString.contains( "DET IN [3]", Qt::CaseInsensitive);
        g_bIsContainedDET_OUT_0 = tempSensorString.contains( "DET OUT [0]", Qt::CaseInsensitive);
        g_bIsContainedDET_OUT_1 = tempSensorString.contains( "DET OUT [1]", Qt::CaseInsensitive);
        g_bIsContainedDET_OUT_2 = tempSensorString.contains( "DET OUT [2]", Qt::CaseInsensitive);
        g_bIsContainedDET_OUT_3 = tempSensorString.contains( "DET OUT [3]", Qt::CaseInsensitive);

        if(! m_ui->Gate1_IN_label->text().isEmpty() )
        {
            g_bIsContainedDET_IN_0 = false;
        }
        if(! m_ui->Gate2_IN_label->text().isEmpty() )
        {
            g_bIsContainedDET_IN_1 = false;
        }
        if(! m_ui->Gate3_IN_label->text().isEmpty() )
        {
            g_bIsContainedDET_IN_2 = false;
        }
        if(! m_ui->Gate4_IN_label->text().isEmpty() )
        {
            g_bIsContainedDET_IN_3 = false;
        }
        if(! m_ui->Gate1_OUT_label->text().isEmpty() )
        {
            g_bIsContainedDET_OUT_0 = false;
        }
        if(! m_ui->Gate2_OUT_label->text().isEmpty() )
        {
            g_bIsContainedDET_OUT_1 = false;
        }
        if(! m_ui->Gate3_OUT_label->text().isEmpty() )
        {
            g_bIsContainedDET_OUT_2 = false;
        }
        if(! m_ui->Gate4_OUT_label->text().isEmpty() )
        {
            g_bIsContainedDET_OUT_3 = false;
        }



        // 계수기 센서
        if( true == g_bIsContainedDET_IN_0 )
        {
            m_ui->Gate1_IN_label->setStyleSheet("color: rgb(85, 255, 0);"
                                                "background-color: rgb(0, 0, 0);");
            m_ui->Gate1_IN_label->setText("IN #1");

        }
        else if( true == g_bIsContainedDET_IN_1 )
        {
            m_ui->Gate2_IN_label->setStyleSheet("color: rgb(85, 255, 0);"
                                                "background-color: rgb(0, 0, 0);");
            m_ui->Gate2_IN_label->setText("IN #2");

            g_bIsContainedDET_IN_1 = false;
        }
        else if( true == g_bIsContainedDET_IN_2 )
        {
            m_ui->Gate3_IN_label->setStyleSheet("color: rgb(85, 255, 0);"
                                                "background-color: rgb(0, 0, 0);");
            m_ui->Gate3_IN_label->setText("IN #3");

            g_bIsContainedDET_IN_2 = false;
        }
        else if( true == g_bIsContainedDET_IN_3 )
        {
            m_ui->Gate4_IN_label->setStyleSheet("color: rgb(85, 255, 0);"
                                                "background-color: rgb(0, 0, 0);");
            m_ui->Gate4_IN_label->setText("IN #4");

            g_bIsContainedDET_IN_3 = false;
        }
        else if( true == g_bIsContainedDET_OUT_0 )
        {
            m_ui->Gate1_OUT_label->setStyleSheet("color: rgb(85, 255, 0);"
                                                 "background-color: rgb(0, 0, 0);");
            m_ui->Gate1_OUT_label->setText("OUT #1");

            g_bIsContainedDET_OUT_0 = false;
        }
        else if( true == g_bIsContainedDET_OUT_1 )
        {
            m_ui->Gate2_OUT_label->setStyleSheet("color: rgb(85, 255, 0);"
                                                 "background-color: rgb(0, 0, 0);");
            m_ui->Gate2_OUT_label->setText("OUT #2");

            g_bIsContainedDET_OUT_1 = false;
        }
        else if( true == g_bIsContainedDET_OUT_2 )
        {
            m_ui->Gate3_OUT_label->setStyleSheet("color: rgb(85, 255, 0);"
                                                 "background-color: rgb(0, 0, 0);");
            m_ui->Gate3_OUT_label->setText("OUT #3");

            g_bIsContainedDET_OUT_2 = false;
        }
        if( true == g_bIsContainedDET_OUT_3 )
        {
            m_ui->Gate4_OUT_label->setStyleSheet("color: rgb(85, 255, 0);"
                                                 "background-color: rgb(0, 0, 0);");
            m_ui->Gate4_OUT_label->setText("OUT #4");

            g_bIsContainedDET_OUT_3 = false;
        }
        else
        {
            return;
        }
    }
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::fnc_readData(QByteArray rcvStr)
{
    qDebug() << "[fnc_readData] rcvStr" << rcvStr;

    int index, index2;
    int i, num;
    int length;
    QString strRes;

    index = -1;
    index = rcvStr.indexOf(STR_MAIN_PRIFIX, Qt::CaseInsensitive);
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::Main_handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), Main_serial->errorString());
        Main_closeSerialPort();
    }
}

void MainWindow::initActionsConnections()
{
    // [MAIN_COMM]
    connect(Main_serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error), this, &MainWindow::Main_handleError);
    connect(Main_serial, &QSerialPort::readyRead, this, &MainWindow::Main_readData);
    connect(this,  &MainWindow::msg_Main_readData, this, &MainWindow::Main_fnc_readData);

    connect(this,  &MainWindow::msg_Main_readData, this, &MainWindow::Server_Info_arr);
    connect(this,  &MainWindow::msg_Main_readData, this, &MainWindow::DetectingGateSignal);
    connect(this,  &MainWindow::msg_Main_readData, this, &MainWindow::DetectingNetworkSignal);

    connect(m_ui->btn_Connect, &QPushButton::clicked, this, &MainWindow::btn_Connect);
    connect(m_ui->btn_Disconnect, &QPushButton::clicked, this, &MainWindow::btn_Disconnect);

    connect(m_ui->btn_ServerInquiry, &QPushButton::clicked, this, &MainWindow::AbleButtonServerSetting);
    connect(m_ui->btn_ServerSetting, &QPushButton::clicked, this, &MainWindow::DisableButtonServerSetting);

    //
    connect(this,  &MainWindow::msg_readData, this, &MainWindow::fnc_readData);


}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::makeServerConfigFile()
{

    if (QFile::exists(ServerConfigFile()))  return;

    ServerIPDefault = "175.213.26.195";
    ServerPortDefault = "18900";
    DeviceCodeDefault = "NURI";
    SerialNODefault = "5";
    ReportPeriodDefault = "10";
    ThresholdDefault = "18";

    QSettings settings(ServerConfigFile(), QSettings::IniFormat);

    // [SERVER_INFO]
    settings.setValue("SERVER_INFO/ServerIPDefault", ServerIPDefault);
    settings.setValue("SERVER_INFO/ServerPortDefault", ServerPortDefault);
    settings.setValue("SERVER_INFO/DeviceCodeDefault", DeviceCodeDefault);
    settings.setValue("SERVER_INFO/SerialNODefault", SerialNODefault);
    settings.setValue("SERVER_INFO/ReportPeriodDefault", ReportPeriodDefault);
    settings.setValue("SERVER_INFO/ThresholdDefault", ThresholdDefault);
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
QString MainWindow::ServerConfigFile()
{
    QString ApplicationPath = QApplication::applicationDirPath();
    QString retn;

    QString configBasePath =
    qApp->applicationName();

    retn = ApplicationPath + "/Server_config.ini";

    return retn;
  }

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::loadServerConfigFile()
{
    if (!QFile::exists(ServerConfigFile()))
    {
        QMessageBox::information(this,"경고", "Server_Config.ini 파일이 존재 하지 않습니다.\n 새로운 파일을 생성합니다.");
        makeServerConfigFile();
    }

    QSettings settings(ServerConfigFile(), QSettings::IniFormat);

    // [SERVER_INFO]
    ServerIPDefault = settings.value("SERVER_INFO/ServerIPDefault").toString();
    ServerPortDefault = settings.value("SERVER_INFO/ServerPortDefault").toString();
    DeviceCodeDefault = settings.value("SERVER_INFO/DeviceCodeDefault").toString();
    SerialNODefault = settings.value("SERVER_INFO/SerialNODefault").toString();
    ReportPeriodDefault = settings.value("SERVER_INFO/ReportPeriodDefault").toString();
    ThresholdDefault = settings.value("SERVER_INFO/ThresholdDefault").toString();

}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________

void MainWindow::ini_Read()
{
    QSettings settings(m_iniFileName, QSettings::IniFormat);

    // [MAIN_COMM]
    settings.beginGroup("MAIN_COMM");
    m_MainComPort = settings.value("Main_ComPort").toString();
    m_MainBaudRate = settings.value("Main_BaudRate").toString();
    m_MainParity = settings.value("Main_Parity").toString();
    settings.endGroup();

    // [SCREEN_INFO]
    settings.beginGroup("SCREEN_INFO");
    check_Debug = settings.value("Debug_Check").toBool(); // bool 값 읽기
    // m_ui->check_Debug->setChecked(check_Debug);
    check_Log = settings.value("Log_Check").toBool();
    // m_ui->check_Log->setChecked(check_Log);



}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::ini_Write()
{
    QSettings settings(m_iniFileName, QSettings::IniFormat);

    CommSettings p;

    // [MAIN_COMM]
    p = read_Settings(MAIN_GROUP);
    qDebug() << p.name << p.stringBaudRate << p.stringParity;
    m_MainComPort  = p.name;
    m_MainBaudRate = p.stringBaudRate;
    m_MainParity   = p.stringParity;
    settings.setValue("MAIN_COMM/Debug_Check", check_Debug);
    settings.setValue("MAIN_COMM/Log_Check", check_Log);
    settings.setValue("MAIN_COMM/Main_ComPort", m_MainComPort);
    settings.setValue("MAIN_COMM/Main_BaudRate", m_MainBaudRate);
    settings.setValue("MAIN_COMM/Main_Parity", m_MainParity);

    // [SCREEN_INFO]
    settings.setValue("SCREEN_INFO/Debug_Check", check_Debug);
    settings.setValue("SCREEN_INFO/Log_Check", check_Log);


}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::initSerialPortInfo()
{
    // [MAIN_COMM]
    m_ui->cb_ComPort->clear();
    m_ui->lineEdit_ServerIP->clear();
    m_ui->lineEdit_ServerPort->clear();
    m_ui->lineEdit_DeviceCode->clear();
    m_ui->lineEdit_SerialNumber->clear();
    m_ui->lineEdit_ReportPeriod->clear();
    m_ui->lineEdit_Threshold->clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        m_ui->cb_ComPort->addItem(info.portName());
    }
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::configSerialPort()
{
    // [MAIN_COMM]
    pMain_Serial = new CommSettings;
    pMain_Serial->name = m_MainComPort;
    pMain_Serial->baudRate = m_MainBaudRate.toInt();
    // pMain_Serial->parity = static_cast<QSerialPort::Parity>(m_ui->Main_parityBox->findText(m_MainParity));

    serial_Settings(MAIN_GROUP);

    Main_serial = new QSerialPort(this);
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::serial_Settings(select_group group)
{
    int idx;

    switch(group)
    {
    case select_group::MAIN_GROUP:
        {
            idx = m_ui->cb_ComPort->findText(m_MainComPort);
            if(idx < 0)
            {
                qDebug() << pMain_Serial->name << "찾을 수 없습니다.";
            }
            else
            {
                m_ui->cb_ComPort->setCurrentIndex(idx);
            }


       // serial_UpdateSettings(MAIN_GROUP);
        }
        break;
    }
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
MainWindow::CommSettings MainWindow::read_Settings(select_group group) const
{
    CommSettings currentSettings;

    switch(group)
    {
    case select_group::MAIN_GROUP:
         currentSettings = *pMain_Serial;
        break;
    }

    return currentSettings;
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::putString(select_group group, int type, QString str)
{
    switch(type)
    {

    case monitor_txColor::BLACK_:
        ConsolePutString(group, str, Qt::black);
        break;

    case monitor_txColor::DARKRED_:
        ConsolePutString(group, str, Qt::darkRed);
        break;

    case monitor_txColor::DARKGREEN_: // Sending
        ConsolePutString(group, str, Qt::darkGreen);
        break;

    case monitor_txColor::DARKBLUE_: // Received
        ConsolePutString(group, str, Qt::darkBlue);
        break;

    case monitor_txColor::GREEN_:
        ConsolePutString(group, str, QColor(0, 128, 0, 255));
        break;

    }
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::Main_btn_Enable(bool mode)
{

    m_ui->cb_ComPort->setEnabled(mode);

    m_ui->btn_Connect->setEnabled(mode);
    m_ui->btn_Disconnect->setEnabled(mode);

    m_ui->btn_ClearConsole->setEnabled(mode);
    m_ui->btn_CountReset->setEnabled(mode);
    m_ui->btn_ConnectTest->setEnabled(mode);

    m_ui->btn_ServerReset->setEnabled(mode);
    m_ui->btn_ServerInquiry->setEnabled(mode);
    // m_ui->btn_ServerSetting->setEnabled(mode);


}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::btn_Connect()
{
    if (Main_serial == NULL || Main_serial->isOpen())
        return;

    // serial_UpdateSettings(MAIN_GROUP);

    if(Main_openSerialPort() == OK)
    {
        m_ui->btn_Connect->setEnabled(false);
        m_ui->btn_Disconnect->setEnabled(true);

        Main_tx_msg = STR_SELFSTART;
        tx_msg = Main_tx_msg + _CR_;
        serial_writeData(MAIN_GROUP, tx_msg.toStdString().c_str());
    }

}

void MainWindow::btn_Disconnect()
{
    Main_closeSerialPort();

    if(Main_serial->isOpen())
    {
        m_ui->btn_Connect->setEnabled(true);
        m_ui->btn_Disconnect->setEnabled(false);
    }

    m_ui->btn_Connect->setEnabled(true);
    m_ui->btn_Disconnect->setEnabled(false);


}

void MainWindow::on_btn_ClearConsole_clicked()
{
    Main_console->clear();
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::openSerialPort()
{
    btn_Connect();

    if(Main_serial->isOpen())
    {

            btn_Disconnect();
    }

    int counter = 0;
    do
    {
        //qDebug() << "counter : " << counter;
        process_Delay(100);
        counter++;
        if(READ_BIT(g_M_State, M_STATE_SELFSTART) && READ_BIT(g_J_State, J_STATE_SELFSTART))

        {
            break;
        }

    }while(counter != DELAY_TIMEOUT_COUNTER);

    if(counter == DELAY_TIMEOUT_COUNTER)
    {

    }
    else
    {
        Main_btn_Enable(true);
        consoleClear();
    }
}

void MainWindow::closeSerialPort()
{
    btn_Disconnect();

    consoleClear();

    Main_btn_Enable(false);
    m_ui->btn_ServerSetting->setEnabled(false);

    State_Clear();
}

void MainWindow::State_Clear()
{
    g_M_State = 0;
    g_J_State = 0;
    g_S_State = 0;
    g_E_State = 0;

    g_SelfTestStart = false;
    g_SelfTestStop  = false;
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::consoleClear()
{
    Main_console->clear();
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::serial_writeData(select_group group, const QByteArray &data)
{
    QByteArray write_buf;
    write_buf.clear();
    write_buf.append(data);

    switch(group)
    {
    case select_group::MAIN_GROUP:
        if(Main_serial->isOpen())
        {
            Main_serial->write(write_buf);
        }
        break;
    }
    // qDebug() << "["<<group<<" serial_writeData]" << data << write_buf.toHex();
    qDebug() << "["<<group<<" serial_writeData]" << data << write_buf;
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::getServerInformationString()
{

    g_ServerIP_String = m_ui->lineEdit_ServerIP->text();

    g_ServerPort_String = m_ui->lineEdit_ServerPort->text();

    g_SerialNumber_String = m_ui->lineEdit_SerialNumber->text();
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::Server_Info_Check()
{
    // 연결되어있으면
    if (Main_serial->isOpen())
    {
        // 서버 설정 조회
        serial_writeData(MAIN_GROUP, QString(STR_CMD_SH_SYSINFO+_CR_).toStdString().c_str());
    }

}


//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::DisableButton()
{
    m_ui->btn_Disconnect->setEnabled(false);
    // m_ui->btn_ServerSetting->setEnabled(false);
    m_ui->btn_ServerInquiry->setEnabled(false);
    m_ui->btn_ConnectTest->setEnabled(false);
    m_ui->btn_ServerReset->setEnabled(false);
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::AbleButtonServerSetting()
{
    m_ui->btn_ServerSetting->setEnabled(true);
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::DisableButtonServerSetting()
{
    m_ui->btn_ServerSetting->setEnabled(false);
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::Server_Info_arr(QByteArray rcvStr)
{
    QString tempString = Main_console->toPlainText();

    // 서버 정보 데이터 들어올 시
    bool bIsContainedBuildDate = tempString.contains( "Build Date", Qt::CaseInsensitive);

    // 서버 정보
    if( true == bIsContainedBuildDate )
    {
        if( NULL == tempString )
        {
            return;
        }

        //(\n + 공백)으로 쪼개서 리스트 저장
        g_aCurrentServerInformation = tempString.split(QRegularExpression("\\s+"));

        m_ui->lineEdit_ServerIP->setText(g_aCurrentServerInformation[4]);
        m_ui->lineEdit_ServerPort->setText(g_aCurrentServerInformation[7]);
        m_ui->lineEdit_DeviceCode->setText(g_aCurrentServerInformation[17]);
        m_ui->lineEdit_SerialNumber->setText(g_aCurrentServerInformation[21]);
        m_ui->lineEdit_ReportPeriod->setText(g_aCurrentServerInformation[30]);
        m_ui->lineEdit_Threshold->setText(g_aCurrentServerInformation[58]);

        Main_console->clear();

        // 초기화
        tempString = QString();

        QThread::msleep(10);

        serial_writeData(MAIN_GROUP, QString(STR_COMPLEAT+_CR_).toStdString().c_str());


    }


}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::on_btn_Connect_clicked()
{
    // 서버 설정버튼 제외
    m_ui->btn_ServerSetting->setEnabled(false);
    // m_ui->btn_Disconnect->setEnabled(false);
    // m_ui->btn_ServerInquiry->setEnabled(false);
    // m_ui->btn_ConnectTest->setEnabled(false);
    // m_ui->btn_ServerReset->setEnabled(false);
}

void MainWindow::on_btn_Disconnect_clicked()
{
    DisableButton();

    ServerInfo_Clear();

    on_btn_CountReset_clicked();

    m_ui->btn_ServerSetting->setEnabled(false);
    // m_ui->btn_Disconnect->setEnabled(false);
    // m_ui->btn_ServerInquiry->setEnabled(false);
    // m_ui->btn_ConnectTest->setEnabled(false);
    // m_ui->btn_ServerReset->setEnabled(false);
}

void MainWindow::on_btn_ServerInquiry_clicked()
{
    Main_console->clear();

    loadServerConfigFile();

    ServerInfo_Clear();

    // 서버 조회
    Server_Info_Check();

    //서버 정보를 문자열에 입력
    Server_Info_arr(rcvStr);

    g_aCurrentServerInformation = QStringList();

    m_ui->btn_ServerSetting->setEnabled(true);
}

void MainWindow::ServerInfo_Clear()
{
    m_ui->lineEdit_ServerIP->clear();
    m_ui->lineEdit_ServerPort->clear();
    m_ui->lineEdit_DeviceCode->clear();
    m_ui->lineEdit_SerialNumber->clear();
    m_ui->lineEdit_ReportPeriod->clear();
    m_ui->lineEdit_Threshold->clear();
}


void MainWindow::on_btn_ServerReset_clicked()
{
    ServerInfo_Clear();

    loadServerConfigFile();


    m_ui->lineEdit_ServerIP->setText(ServerIPDefault);

    m_ui->lineEdit_ServerPort->setText(ServerPortDefault);

    m_ui->lineEdit_DeviceCode->setText(DeviceCodeDefault);

    m_ui->lineEdit_SerialNumber->setText(SerialNODefault);

    m_ui->lineEdit_ReportPeriod->setText(ReportPeriodDefault);

    m_ui->lineEdit_Threshold->setText(ThresholdDefault);

}


void MainWindow::on_btn_ServerSetting_clicked()
{
    // 서버 설정 명령어 입력
    if (Main_serial->isOpen())
    {
        Server_Info_arr(rcvStr);

        // QlineEdit에 입력된 문자열 저장
        getServerInformationString();

        serial_writeData(MAIN_GROUP, QString(STR_CMD_SET_CONFIG+_CR_).toStdString().c_str());

        QThread::msleep(10);

        serial_writeData(MAIN_GROUP, QString(g_ServerIP_String+_CR_).toStdString().c_str());

        QThread::msleep(10);

        serial_writeData(MAIN_GROUP, QString(g_ServerPort_String+_CR_).toStdString().c_str());

        QThread::msleep(10);

        serial_writeData(MAIN_GROUP, QString(_CR_).toStdString().c_str());

        QThread::msleep(10);

        serial_writeData(MAIN_GROUP, QString(STR_CMD_NURI+_CR_).toStdString().c_str());

        QThread::msleep(10);

        serial_writeData(MAIN_GROUP, QString(g_SerialNumber_String+_CR_).toStdString().c_str());

        QThread::msleep(10);

        serial_writeData(MAIN_GROUP, QString(STR_CMD_10+_CR_).toStdString().c_str());

        QThread::msleep(10);

        serial_writeData(MAIN_GROUP, QString(STR_CMD_18+_CR_).toStdString().c_str());

        QThread::msleep(10);

        serial_writeData(MAIN_GROUP, QString(STR_CMD_Y+_CR_).toStdString().c_str());

        if( g_ServerIP_String != g_aCurrentServerInformation[4] )
        {
           QMessageBox::information(this,"Chage", "[Changed Server IP] " + g_aCurrentServerInformation[4] + " -> " + g_ServerIP_String);
        }

        if( g_ServerPort_String != g_aCurrentServerInformation[7] )
        {
           QMessageBox::information(this,"Chage", "[Changed Server Port] " + g_aCurrentServerInformation[7] + " -> " + g_ServerPort_String);
        }

        if( g_SerialNumber_String != g_aCurrentServerInformation[21] )
        {
           QMessageBox::information(this,"Chage", "[Changed Serial Number] " + g_aCurrentServerInformation[21] + " -> " + g_SerialNumber_String);
        }
        else
        {
            return;
        }
    }


    g_ServerIP_String = QString();
    g_ServerPort_String = QString();
    g_SerialNumber_String = QString();

    g_aCurrentServerInformation = QStringList();

    m_ui->btn_ServerSetting->setEnabled(false);

}

void MainWindow::on_btn_CountReset_clicked()
{
    Main_console->clear();

    ServerInfo_Clear();

    // TEXT는 UI에서 시그널 슬롯 기능으로 설정
    m_ui->Gate1_IN_label->setStyleSheet("color: rgb(85, 255, 0);"
                                        "background-color: rgb(255, 255, 255);"
                                        "border: 1px;"
                                        "border-style: outset;");

    m_ui->Gate2_IN_label->setStyleSheet("color: rgb(85, 255, 0);"
                                        "background-color: rgb(255, 255, 255);"
                                        "border: 1px;"
                                        "border-style: outset;");

    m_ui->Gate3_IN_label->setStyleSheet("color: rgb(85, 255, 0);"
                                        "background-color: rgb(255, 255, 255);"
                                        "border: 1px;"
                                        "border-style: outset;");

    m_ui->Gate4_IN_label->setStyleSheet("color: rgb(85, 255, 0);"
                                        "background-color: rgb(255, 255, 255);"
                                        "border: 1px;"
                                        "border-style: outset;");

    m_ui->Gate1_OUT_label->setStyleSheet("color: rgb(85, 255, 0);"
                                         "background-color: rgb(255, 255, 255);"
                                         "border: 1px;"
                                         "border-style: outset;");

    m_ui->Gate2_OUT_label->setStyleSheet("color: rgb(85, 255, 0);"
                                         "background-color: rgb(255, 255, 255);"
                                         "border: 1px;"
                                         "border-style: outset;");

    m_ui->Gate3_OUT_label->setStyleSheet("color: rgb(85, 255, 0);"
                                         "background-color: rgb(255, 255, 255);"
                                         "border: 1px;"
                                         "border-style: outset;");

    m_ui->Gate4_OUT_label->setStyleSheet("color: rgb(85, 255, 0);"
                                         "background-color: rgb(255, 255, 255);"
                                         "border: 1px;"
                                         "border-style: outset;");

    // 네트워크 라벨
    m_ui->network_check_label->setStyleSheet("color: rgb(85, 255, 0);"
                                             "background-color: rgb(255, 255, 255);"
                                             "border: 1px;"
                                             "border-style: outset;");

}


void MainWindow::on_btn_Network_Check_clicked()
{
    // 연결되어있으면
    if (Main_serial->isOpen())
    {
        // 서버 연결 조회
        serial_writeData(MAIN_GROUP, QString(STR_CMD_SOC_RECON+_CR_).toStdString().c_str());
    }
}
