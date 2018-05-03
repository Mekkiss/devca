#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QtPlugin>

//#ifdef WINDOWS
//Q_IMPORT_PLUGIN(qjpeg)
//Q_IMPORT_PLUGIN(qgif)
//#endif /*WIN32 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationDomain("dnd.ballig.net");
    a.setOrganizationName("Ballig");
    a.setApplicationName("CombatAssistant");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "./");
    QSettings settings("ca.ini", QSettings::IniFormat);
    bool ovStyle = settings.value("overrideStyle", true).toBool();
    if (ovStyle) a.setStyle("windows");
    a.setStyleSheet("QStatusBar::item { border: 0px solid black; }");
    MainWindow w;
    w.show();
    return a.exec();
}
