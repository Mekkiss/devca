#ifndef NETDIALOG_H
#define NETDIALOG_H

#include <QStringListModel>
#include <QStandardItem>
#include <QtGui/QDialog>
#include <QtNetwork>
#include "network.h"

class CASock;

namespace Ui {
    class NetDialog;
}

class NetDialog : public QDialog {
    Q_OBJECT
public:
    NetDialog(QList<QString> clientNames, QList<CASock*> clients, QWidget *parent = 0);
    ~NetDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::NetDialog *m_ui;
    QModelIndex mIndex;

private slots:
    void on_netList_customContextMenuRequested(QPoint pos);
    void menuDisconnect();

public slots:
    void updateList(QList<QString> clientNames, QList<CASock *> clients);

signals:
    void disconnectUser(int d);
};

#endif // NETDIALOG_H
