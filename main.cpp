#include "mainwindow.h"
#include <QApplication>


MainWindow *MainWin;

QString logcurtime;
bool check_Debug, check_Log;
//-----------------------------------------------------------------------------
// 디버깅 로그 파일로 저장 : .log
//_____________________________________________________________________________
void LogToFile(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //if(!check_Debug)
    //    return;


    QString m_FilePath = qApp->applicationDirPath() + "/";

    // log Dir
    QDir dir(m_FilePath + "log");
    if(!dir.exists())
        dir.mkpath(".");

    //QString LogFileName = m_FilePath + "log/" + "Debug.log";
    QString LogFileName = m_FilePath + "log/Debug_" + logcurtime + ".log";

    QFile file(LogFileName);
    if(!file.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);

    QString curtime = QDateTime::currentDateTime().toString("yyyy.MM.dd|hh:mm:ss:zzz ");
    //context.file, context.line, context.function
    //QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        out << "[Debug] " << curtime << msg << " " << context.file << " " << context.line << "\n";
        break;
    case QtInfoMsg:
        out << "[Info] " << curtime << msg << "\n";
        break;
    case QtWarningMsg:
        out << "[Warning] " << curtime << msg << "\n";
        break;
    case QtCriticalMsg:
        out << "[Critical] " << curtime << msg << " " << context.function << " " << context.line << "\n";
        break;
    case QtFatalMsg:
        out << "[Fatal]" << curtime << msg << "\n";
        abort();
    }
    file.close();
}


int main(int argc, char *argv[])
{
    //qInstallMessageHandler(LogToFile); // Handler 등록

    //qSetMessagePattern("%{file}(%{line})%{time yyyy.MM.dd|hh:mm:ss:zzz} %{message}"); // 디버그 메시지 패턴 설정

    QApplication a(argc, argv);
    MainWindow w;

    MainWin = &w;

    w.show();

    return a.exec();
}
