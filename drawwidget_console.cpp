#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtMath>
#include <QDebug>


//#include "console.h"
#include <QColor>

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
extern bool check_Debug, check_Log;

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::ConsoleInit(void)
{
    // Main Consol
    Main_console = new Console;
    Main_console->setEnabled(false);
    Main_console->setLocalEchoEnabled(false);
    m_ui->Main_verticalLayout->addWidget(Main_console);

    connect(Main_console, &Console::getData, this, &MainWindow::Main_writeData);


}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::ConsolePutString(select_group group, QString str, QColor color)
{
    switch(group)
    {
    case select_group::MAIN_GROUP:
        // Main_console->putString(str, color);
        Main_console->write(str);
        break;


    }
}

//-----------------------------------------------------------------------------
//
//_____________________________________________________________________________
void MainWindow::ConsolePutData(select_group group, const QByteArray &data, QColor color)
{
    switch(group)
    {
    case select_group::MAIN_GROUP:
        // Main_console->putData(data, color);
        Main_console->write(QString(data.data()));
        break;

    }
}
