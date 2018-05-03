#include <QMenu>
#include "netdialog.h"
#include "ui_netdialog.h"

NetDialog::NetDialog(QList<QString> clientNames, QList<CASock *> clients, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::NetDialog)
{
    m_ui->setupUi(this);
    QList<QString> s;
    for (int x = 0; x < clientNames.size() && x < clients.size(); x++)
      s << tr("%1\t%2").arg(clientNames.at(x), -40).arg(((clients.at(x)->isTcp())?clients.at(x)->tcpSocket()->peerAddress().toString():QString::number(clients.at(x)->web())));
    QStringListModel *m = new QStringListModel(s, this);
    m_ui->netList->setModel(m);
}

NetDialog::~NetDialog()
{
    delete m_ui;
}

void NetDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void NetDialog::updateList(QList<QString> clientNames, QList<CASock*> clients)
{
  QItemSelectionModel *oldm = m_ui->netList->selectionModel();
  QList<QString> s;
  for (int x = 0; x < clientNames.size() && x < clients.size(); x++)
    s << tr("%1\t%2").arg(clientNames.at(x), -40).arg(((clients.at(x)->isTcp())?clients.at(x)->tcpSocket()->peerAddress().toString():QString::number(clients.at(x)->web())));
  QStringListModel *m = new QStringListModel(s, this);
  m_ui->netList->setModel(m);
  delete oldm;
}

void NetDialog::menuDisconnect()
{
  emit disconnectUser(mIndex.row());
}

void NetDialog::on_netList_customContextMenuRequested(QPoint pos)
{
  QMenu m(this);
  m.addAction("Disconnect", this, SLOT(menuDisconnect()));
  mIndex = m_ui->netList->indexAt(pos);
  if (mIndex.isValid()) m.exec(this->mapToGlobal(pos));
}
