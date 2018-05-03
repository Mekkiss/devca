#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QFile>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if (a.arguments().size() < 4) return 0;
    QString vMajor = a.arguments().at(1);
    QString vMinor = a.arguments().at(2);
    QString fTarget = a.arguments().at(3);
    QString vBld = QString::null;

    QFile bldfile("_buildfile");
    if (bldfile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QTextStream in(&bldfile);
      vBld = in.readLine();
    }
    if (vBld.isNull()) vBld.sprintf("0");
    bldfile.close();

    int version = vBld.toInt();

    if (!bldfile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) return 0;
    QTextStream bldOut(&bldfile);
    bldOut << QString::number(++version) << "\n";
    bldfile.close();

    QFile file(fTarget);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return 0;

    QTextStream out(&file);
    out << "#ifndef VERSION_H\n";
    out << "#define VERSION_H\n";
    out << "\n";
    out << "namespace Version\n";
    out << "{\n";
    out << "  static const int MAJOR = " << vMajor << ";\n";
    out << "  static const int MINOR = " << vMinor << ";\n";
    out << "  static const int BUILD = " << QString::number(version) << ";\n";
    out << "}\n";
    out << "\n";
    out << "#endif // VERSION_H\n";
    file.close();
}
