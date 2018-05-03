#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QtGui/QDialog>
#include <QSettings>

namespace Ui {
    class ConnectDialog;
}

class ConnectDialog : public QDialog {
    Q_OBJECT
public:
    ConnectDialog(QWidget *parent = 0);
    ~ConnectDialog();
    QString getAddress();
    int getPort();
    QString getUserName();
    bool useWebServer();
    bool useIRC();
    QString getIrcNick();
    QString getIrcServer();
    int getIrcPort();
    QString getIrcGM();
    QString getIrcChannel();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ConnectDialog *m_ui;

private slots:
    void on_listenButton_released();
    void on_connectButton_released();
};

#endif // CONNECTDIALOG_H
