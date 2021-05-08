#include "mainwindow.h"

#include <QApplication>
#include<QTranslator>
#include<QDir>
#include<QFile>


void setTranslator(QString settingDir, QTranslator* translator) {
    QFile file(settingDir + "/setting.ini");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    QString qmFileName = file.readLine();
    if (qmFileName == "") {
        translator = nullptr;
    } else {
        translator->load(settingDir + "/" +qmFileName);
    }
    file.close();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator* translator = new QTranslator;

    setTranslator(qApp->applicationDirPath() + "/setting", translator);
    if (translator) {
        a.installTranslator(translator);
    }

    MainWindow w;
    w.show();
    return a.exec();
}


