#ifndef WINDOWS
#ifdef WIN32
 #define WINDOWS
#elif WIN64
 #define WINDOWS
#endif
#endif /* WINDOWS */
#ifdef WINDOWS
#include <windows.h>
#endif /* WIN32 */
#include <QMessageBox>
#include <QSound>
#include <QImageReader>
#include "network.h"
#include "netdialog.h"
#include <IrcCommand>
#define _CHUNK 22528

CANetwork::CANetwork(QGridTab *t, QObject *p)
{
  m_Client = false;
  m_Parent = p;
  tabWidget = t;
  useIRC = false;
  connect(tabWidget, SIGNAL(redrawGrid(int,int,int,int,QColor,QColor,QColor,int)), this, SLOT(redrawGrid(int,int,int,int,QColor,QColor,QColor,int)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(tabAdded()), this, SLOT(tabAdded()), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(renameTab(QString,int)), this, SLOT(renameTab(QString,int)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(tabRemoved(int)), this, SLOT(tabRemoved(int)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(circleDrawn(int,int,int,QColor,int,QString)), this, SLOT(circleDrawn(int,int,int,QColor,int,QString)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(blockDrawn(int,int,int,QColor,QString)), this, SLOT(blockDrawn(int,int,int,QColor,QString)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(crossDrawn(int,int,int,QColor)), this, SLOT(crossDrawn(int,int,int,QColor)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(triangleDrawn(int,int,int,QColor,int)), this, SLOT(triangleDrawn(int,int,int,QColor,int)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(itemRemoved(int,int,int)), this, SLOT(itemRemoved(int,int,int)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(itemRemoved(int,quint16)), this, SLOT(itemRemoved(int,quint16)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(dragStarted(int,int,int)), this, SLOT(startDrag(int,int,int)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(dragFinished(int,int,int)), this, SLOT(finishDrag(int,int,int)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(lineDrawn(int,double,double,double,double,QPen)), this, SLOT(lineDrawn(int,double,double,double,double,QPen)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(caItemDrawn(int,CAGraphicsItem*)), this, SLOT(caItemDrawn(int,CAGraphicsItem*)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(newBackground(CABackground*,int)), this, SLOT(changeBackground(CABackground*,int)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(updateDownload(qint64,qint64)), this, SLOT(updateDownload(qint64,qint64)), Qt::AutoConnection);
  connect(tabWidget, SIGNAL(newBackgroundOffset(int,int,int)), this, SLOT(backgroundOffset(int,int,int)), Qt::AutoConnection);
  m_serverTimer = new QTimer(this);
  m_webTimer = new QTimer(this);
  m_sendTimer = new QTimer(this);
  connect(m_sendTimer, SIGNAL(timeout()), this, SLOT(checkSend()), Qt::AutoConnection);
  m_Host = QString("(unknown)");
  userName = QString("");
  tcpSocket = new CASock(true, new QTcpSocket(this));
  tcpServer = new QTcpServer(this);
  m_HTTP = new QNetworkAccessManager(this);


}

CANetwork::~CANetwork()
{
  delete m_serverTimer;
  delete m_webTimer;
  delete tcpSocket;
  delete tcpServer;
  delete m_HTTP;
}

void CANetwork::cancelDownload()
{
  if (isServer() && !m_Send.isEmpty())
    for (int x = m_Send.size(); x > 0; x--)
    {
      m_Send.at((x-1))->cancelSend();
      delete m_Send.takeAt((x-1));
    }
  else if (!isServer()) clientWrite(QString::number(N_CANCEL_IMG));
}

int CANetwork::numClients()
{
  return clients.size();
}
void CANetwork::processConnectDialog(ConnectDialog *cd)
{
    QSettings settings("ca.ini", QSettings::IniFormat);
#ifdef WEB_SERVER
    bool wb = settings.value("webBackup", true).toBool();
#endif
    QString server = settings.value("serverScript", "http://www.pfrpg.com.au/devca.php5").toString();
    if (cd->result() == 2) // Listen Button
    {
        userName = cd->getUserName().replace(",",".");
        networkDisconnect();
        if (!cd->useWebServer() && !cd->useIRC())
        {
            QHostAddress h = cd->getAddress().isEmpty() ? QHostAddress(cd->getAddress()) : QHostAddress::Any;
            if (!tcpServer->listen(h, (quint16)cd->getPort()))
            {
               QMessageBox::critical(0, tr("CA Listen"), tr("Unable to listen for connections: %1.").arg(tcpServer->errorString()));
               return;
            }
            connect(tcpServer, SIGNAL(newConnection()), this, SLOT(serverConnectionMade()), Qt::AutoConnection);
            emit changeNetworkLabel(tr("Listening on port %1").arg(tcpServer->serverPort()));
        }
#ifdef WEB_SERVER
        else emit changeNetworkLabel(tr("Listening to web server"));
        if (cd->useWebServer() || wb)
        {
          connect(m_serverTimer, SIGNAL(timeout()), this, SLOT(ping()), Qt::AutoConnection);
          m_serverTimer->start(120000);
          connect(m_HTTP, SIGNAL(finished(QNetworkReply*)), this, SLOT(HTTPServer(QNetworkReply*)), Qt::AutoConnection);
          HTTPSend(QString(tr("%1,%2").arg(N_WEB_SERVER).arg(userName)));
          connect(m_webTimer, SIGNAL(timeout()), this, SLOT(webMsg()), Qt::AutoConnection);
          webMsg();
          m_webTimer->start(5000);
        }
#endif
        /* IRC additions below */
        if (cd->useIRC())
        {
            useIRC = true;
            m_ircGM = true;
            emit changeNetworkLabel(tr("Connecting to IRC server"));
            m_irc = new IrcNetwork();
            m_irc->setHost(cd->getIrcServer());
            m_irc->setPort(cd->getIrcPort());
            m_irc->setUserName(cd->getIrcGM());
            m_irc->setNickName(cd->getIrcGM());
            m_irc->setRealName("Combat Assistant with Mekkis' Hacks (GM)");
            m_irc->setChannel(cd->getIrcChannel());
            m_irc->open();
            emit changeNetworkLabel(tr("Connected to IRC server as GM"));
            connect(m_irc, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(ircServerParse(IrcMessage*)), Qt::AutoConnection);

        }
    }
      else if (cd->result() == QDialog::Accepted) //Connect Button
    {
        userName = cd->getUserName();
        networkDisconnect();
        if (cd->useIRC())
        {
            useIRC = true;
            m_ircGM = false;
            tcpSocket->setTcp(false);
            emit changeNetworkLabel(tr("Connecting to IRC server"));
            m_irc = new IrcNetwork();
            m_irc->setHost(cd->getIrcServer());
            m_irc->setPort(cd->getIrcPort());
            m_irc->setUserName((cd->getIrcNick()));
            m_irc->setNickName((cd->getIrcNick()));
            m_irc->setRealName(("Combat Assistant with Mekkis' Hacks (Client)"));
            m_irc->setChannel(cd->getIrcChannel());
            m_irc->open();
            emit changeNetworkLabel(tr("Connected to IRC server as client"));
            m_Client = true;
            emit clientConnected(true);
            m_GMNick = cd->getIrcGM();
            connect(m_irc, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(ircClientParse(IrcMessage*)), Qt::AutoConnection);

        } else  //use TCP socket
        {
            connect(tcpSocket->tcpSocket(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)), Qt::AutoConnection);
            connect(tcpSocket->tcpSocket(), SIGNAL(readyRead()), this, SLOT(clientRead()), Qt::AutoConnection);
            connect(tcpSocket->tcpSocket(), SIGNAL(connected()), this, SLOT(clientConnected()), Qt::AutoConnection);
            connect(tcpSocket->tcpSocket(), SIGNAL(disconnected()), this, SLOT(clientDisconnected()), Qt::AutoConnection);
            blockSize = 0;
            currentReceive = QString("");
            emit changeNetworkLabel("Connecting...");
            tcpSocket->tcpSocket()->connectToHost(cd->getAddress(), cd->getPort());
            tcpSocket->setHost(cd->getAddress());
        }
#ifdef WEB_SERVER
        else //Use web server
        {
            connect(m_HTTP, SIGNAL(finished(QNetworkReply*)), this, SLOT(HTTPClient(QNetworkReply*)), Qt::AutoConnection);
            tcpSocket->setTcp(false);
            HTTPSend(QString(tr("%1,%2,%3").arg(N_WEB_CLIENT).arg(cd->getAddress()).arg(userName)));
            connect(m_webTimer, SIGNAL(timeout()), this, SLOT(webMsg()), Qt::AutoConnection);
            webMsg();
            m_webTimer->start(5000);
            emit changeNetworkLabel("Connecting to web server...");
        }
#endif /* WEB_SERVER */
    }

}

void CANetwork::processConnectDialog(QString u, QString a, quint16 p, int r, bool w)
{
  QSettings settings("ca.ini", QSettings::IniFormat);
  bool wb = settings.value("webBackup", true).toBool();
  QString server = settings.value("serverScript", "http://www.pfrpg.com.au/devca.php5").toString();
  if (r == 2) // Listen Button
  {
    userName = u.replace(",",".");
    networkDisconnect();
    if (!w)
    {
      QString bind(((a.isEmpty())?"0.0.0.0":a));
      if (!tcpServer->listen(QHostAddress(bind), (quint16) p))
      {
         QMessageBox::critical(0, tr("CA Listen"), tr("Unable to listen for connections: %1.").arg(tcpServer->errorString()));
         return;
      }
      connect(tcpServer, SIGNAL(newConnection()), this, SLOT(serverConnectionMade()), Qt::AutoConnection);
      emit changeNetworkLabel(tr("Listening on port %1").arg(tcpServer->serverPort()));
      m_irc = new IrcNetwork();
      m_irc->setHost(tr("irc.austnet.org"));
      m_irc->setPort(6667);
      m_irc->setUserName((userName));
      m_irc->setNickName((userName));
      m_irc->setRealName(("Combat Assistant with Mekkis' Hacks (GM)"));
      m_irc->setChannel(("#caIRCtest"));
      m_irc->open();
    }
    else emit changeNetworkLabel(tr("Listening to web server"));
    if (w || wb)
    {
      connect(m_serverTimer, SIGNAL(timeout()), this, SLOT(ping()), Qt::AutoConnection);
      m_serverTimer->start(120000);
      connect(m_HTTP, SIGNAL(finished(QNetworkReply*)), this, SLOT(HTTPServer(QNetworkReply*)), Qt::AutoConnection);
      HTTPSend(QString(tr("%1,%2").arg(N_WEB_SERVER).arg(userName)));
      connect(m_webTimer, SIGNAL(timeout()), this, SLOT(webMsg()), Qt::AutoConnection);
      webMsg();
      m_webTimer->start(5000);
    }
  }
  else if (r == QDialog::Accepted) //Connect Button
  {
    userName = u.replace(",",".");
    networkDisconnect();
    if (w)
    {
      connect(m_HTTP, SIGNAL(finished(QNetworkReply*)), this, SLOT(HTTPClient(QNetworkReply*)), Qt::AutoConnection);
      tcpSocket->setTcp(false);
      HTTPSend(QString(tr("%1,%2,%3").arg(N_WEB_CLIENT).arg(a).arg(userName)));
      connect(m_webTimer, SIGNAL(timeout()), this, SLOT(webMsg()), Qt::AutoConnection);
      webMsg();
      m_webTimer->start(5000);
      emit changeNetworkLabel("Connecting to web server...");
    }
    else
    {
        m_irc = new IrcNetwork();
        m_irc->setHost(tr("irc.austnet.org"));
        m_irc->setPort(6667);
        m_irc->setUserName((userName));
        m_irc->setNickName((userName));
        m_irc->setRealName(("Combat Assistant with Mekkis' Hacks (Client)"));
        m_irc->setChannel(("#caIRCtest"));
        m_irc->open();
        connect(m_irc, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(ircClientParse(IrcMessage*)), Qt::AutoConnection);
        // Attempting IRC connection...
        /*
      connect(tcpSocket->tcpSocket(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)), Qt::AutoConnection);
      connect(tcpSocket->tcpSocket(), SIGNAL(readyRead()), this, SLOT(clientRead()), Qt::AutoConnection);
      connect(tcpSocket->tcpSocket(), SIGNAL(connected()), this, SLOT(clientConnected()), Qt::AutoConnection);
      connect(tcpSocket->tcpSocket(), SIGNAL(disconnected()), this, SLOT(clientDisconnected()), Qt::AutoConnection);
      blockSize = 0;
      currentReceive = QString("");
      emit changeNetworkLabel("Connecting...");
      tcpSocket->tcpSocket()->connectToHost(a, p);
      tcpSocket->setHost(a); */
    }
  }
}

void CANetwork::HTTPSend(QString m)
{
  if (!m.isEmpty() && !m.isNull()) webBuffer.append(m);
}

bool CANetwork::isServer()
{
  if (m_ircGM) return true;
  if (tcpServer->isListening() || (!m_Client && m_webTimer->isActive())) return true;
  return false;
}

void CANetwork::HTTPServer(QNetworkReply* reply)
{
  QString r = QString(reply->readAll());
  if (r.length() > 0)
  {
    QList<QString> msg = r.split('\n');
    for (int x = 0; x < msg.count(); x++)
    {
      QStringList m = msg.at(x).split(",");
      int s = hasWebClient(m.at(0).toLongLong());
      m.removeFirst();
      msg.replace(x, m.join(","));
      serverParseReceive(msg.at(x), ((s>=0)?clients.at(s):NULL));
    }
  }
}

void CANetwork::HTTPClient(QNetworkReply *reply)
{
  QVariant h(reply->header(QNetworkRequest::ContentTypeHeader));
  if (h.isValid() && h.toString().startsWith("image/", Qt::CaseInsensitive))
  {
    QString suffix("jpg");
    QVariant t;
    if (reply->hasRawHeader("Content-Disposition") && (t = QVariant(reply->rawHeader("Content-Disposition"))) != QVariant())
    {
      QRegExp rx("^inline; filename=\"(\\w+)\"$");
      rx.exactMatch(t.toString());
      QStringList sl = rx.capturedTexts();
      if (sl.size() >= 2 && !sl.at(1).isEmpty()) suffix = sl.at(1);
    }
    if (!suffix.compare("jpeg", Qt::CaseInsensitive)) suffix = QString(tr("jpg"));
    QDir dir("images/");
    if (!dir.exists()) QDir().mkdir("images/");
    QString newFileName(tr("images/%1.%2").arg(epoch_s(), suffix));
    QFile *file = new QFile(newFileName, reply);
    if (!file->open(QIODevice::WriteOnly)) return;
    file->write(reply->readAll());
    file->close();
  }
  else
  {
    QString r = QString(reply->readAll());
    if (r.length() > 0)
    {
      QList<QString> msg = r.split('\n');
      for (int x = 0; x < msg.count(); x++) clientParseReceive(msg.at(x));
    }
  }
}

void CANetwork::webMsg()
{
  QSettings settings("ca.ini", QSettings::IniFormat);
  QString server = settings.value("serverScript", "http://www.pfrpg.com.au/devca.php5").toString();
  HTTPSend(QString(tr("%1").arg(N_WEB_MSG).toAscii()));
  if (webBuffer.size() > 0)
  {
    QNetworkRequest request;
    request.setUrl(QUrl(server));
    request.setRawHeader("User-Agent", "CombatAssistant 2.1 352");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    m_HTTP->post(request, QByteArray(webBuffer.join("\n").toAscii()));
    webBuffer.clear();
  }
}

void CANetwork::clientConnected()
{
  m_Client = true;
  emit clientConnected(true);
  emit changeNetworkLabel(tr("Connected to %1").arg(m_Host.mid(0, 30)));
}

void CANetwork::clientDisconnected()
{
  m_Client = false;
  emit clientConnected(false);
  emit changeNetworkLabel("Not connected");
  tcpSocket->tcpSocket()->disconnect(this, SLOT(displayError(QAbstractSocket::SocketError)));
  tcpSocket->tcpSocket()->disconnect(this, SLOT(clientRead()));
  tcpSocket->tcpSocket()->disconnect(this, SLOT(clientConnected()));
  tcpSocket->tcpSocket()->disconnect(this, SLOT(clientDisconnected()));
  networkDisconnect();
}

void CANetwork::serverConnectionMade()
{
  QTcpSocket *sock = tcpServer->nextPendingConnection();
  bool old = false;
  for (int x = 0; x < m_Banlist.size(); x++)
  {
    if (m_Banlist.at(x) == sock->peerAddress())
    {
      sock->close();
      delete sock;
      return;
    }
  }
  for (int x = 0; x < oldClients.size(); x++)
  {
    if (oldClients.at(x)->isTcp() && oldClients.at(x)->address() == sock->peerAddress())
    {
      old = true;
      clients.append(oldClients.takeAt(x));
      clients.last()->setSock(sock);
      m_ClientNames.append("New User");
      break;
    }
  }
  if (!old)
  {
    clients.append(new CASock(true, sock));
    m_ClientNames.append("New User");
  }
  clientBlockSize.append(0);
  clientCurrentReceive.append(QString(""));
  connect(clients.last()->tcpSocket(), SIGNAL(readyRead()), this, SLOT(serverRead()), Qt::AutoConnection);
  connect(clients.last()->tcpSocket(), SIGNAL(disconnected()), this, SLOT(serverClientDisconnected()), Qt::AutoConnection);
  clientBlockSize.append((quint16)0);
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_0);
  out << (quint16)0;
  out << QString::number(N_REQUEST_USERNAME)+","+QString::number(clients.size()-1)+","+userName.replace(',','.');
  out.device()->seek(0);
  out << (quint16)(block.size() - sizeof(quint16));
  sock->write(block);
  emit changeNetworkLabel(tr("Connected: %1").arg(clients.size()));
  resync(clients.last());
}

void CANetwork::serverClientDisconnected()
{
  for (int x = 0; x < clients.size(); x++)
  {
    if (clients.at(x)->isTcp() && clients.at(x)->tcpSocket()->state() == QAbstractSocket::UnconnectedState)
    {
      oldClients.append(clients.takeAt(x));
      m_ClientNames.removeAt(x);
      clientBlockSize.removeAt(x);
      clientCurrentReceive.removeAt(x);
      if (clients.size()==0)
      {
        m_ClientNames.clear();
        clientBlockSize.clear();
        clientCurrentReceive.clear();
        emit changeNetworkLabel(((tcpServer->isListening())?tr("Listening on port %1").arg(tcpServer->serverPort()):tr("Listening to web server")));
      }
      else emit changeNetworkLabel(tr("Connected: %1").arg(clients.size()));
      emit clientChange(m_ClientNames, clients);
      return;
    }
  }
}

void CANetwork::serverWriteToSocket(CASock *sock, QString msg)
{
  if (sock->isTcp())
  {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << msg;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    sock->tcpSocket()->write(block);
  }
  else HTTPSend(QString(tr("%1,%2,%3").arg(N_WEB_SEND).arg(QString::number(sock->web())).arg(msg)));
}

void CANetwork::sendImage(QString chx)
{
  QString path = findFileByHash(chx, "images/");
  if (!path.isNull() && !path.isEmpty() && !QPixmap(path).isNull())
  {
    QSettings settings("ca.ini", QSettings::IniFormat);
    QString server = settings.value("serverScript", "http://www.pfrpg.com.au/devca.php5").toString();
    QFile *file = new QFile(path);
    file->open(QIODevice::ReadOnly);
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(QString(tr("image/%1")).arg(QString(QImageReader::imageFormat(path)))));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString(tr("form-data; name=\"image\"; filename=\"%1.%2\"").arg(chx).arg(QFileInfo(path).baseName()))));
    imagePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(imagePart);
    QNetworkRequest request;
    request.setUrl(QUrl(server));
    request.setRawHeader("User-Agent", "CombatAssistant 2.1 352");
    QNetworkReply *reply = m_HTTP->post(request, multiPart);
    multiPart->setParent(reply);
  }
}

void CANetwork::serverWrite(QString msg)
{
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  bool w = false;
  out.setVersion(QDataStream::Qt_4_0);
  out << (quint16)0;
  out << msg;
  out.device()->seek(0);
  out << (quint16)(block.size() - sizeof(quint16));
  for (int x = 0; x < clients.size(); x++)
  {
    if (clients.at(x)->isTcp()) clients.at(x)->tcpSocket()->write(block);
    else w = true;
  }
  m_irc->sendCommand(IrcCommand::createMessage(m_irc->channel(), msg));

  if (w) HTTPSend(QString(tr("%1,%3").arg(N_WEB_SENDALL).arg(msg)));
}

void CANetwork::serverWriteToUser(QString u, QString msg)
{
  CASock *sock = NULL;
  if (m_ClientNames.contains(u) && (sock = clients.value(m_ClientNames.indexOf(u, 0), NULL)) != NULL)
  {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << msg;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    if (sock->isTcp()) sock->tcpSocket()->write(block);
    else HTTPSend(QString(tr("%1,%2,%3").arg(N_WEB_SEND).arg(QString::number(sock->web())).arg(msg)));
  }
  else if (m_irc->isConnected()) //Try sending over IRC
  {
      m_irc->sendCommand(IrcCommand::createMessage(u, msg));
  }
}

void CANetwork::serverRead()
{
  for (int x = 0; x < clients.size(); x++)
  {
    if (clients.at(x)->isTcp())
    {
      QDataStream in(clients.at(x)->tcpSocket());
      in.setVersion(QDataStream::Qt_4_0);
      clients.at(x)->addGarbage(clients.at(x)->tcpSocket()->bytesAvailable());
      while (clients.at(x)->tcpSocket()->bytesAvailable() > 0)
      {
        if (clientBlockSize.at(x) == 0)
        {
          if (clients.at(x)->tcpSocket()->bytesAvailable() < (int)sizeof(quint16)) break;

          in >> clientBlockSize[x];
        }

        if (clients.at(x)->tcpSocket()->bytesAvailable() < clientBlockSize.at(x)) break;
        QString nextReceive;
        in >> nextReceive;

        if (nextReceive == clientCurrentReceive.at(x)) break;
        clientCurrentReceive[x] = nextReceive;
        serverParseReceive(clientCurrentReceive.at(x), clients.at(x));
        clientCurrentReceive[x].clear();
        clientBlockSize[x] = 0;
      }
      if (clients.at(x)->garbage() > 31457280) banUser(x);
    }
  }
}

/**
  * Write message from client -> server.
  * Expanded to attempt over IRC if signaled that TCP socket
  * (or webserver hack) is unavailable.
  *
  */
void CANetwork::clientWrite(QString msg)
{
  if (useIRC)
  {
      m_irc->sendCommand(IrcCommand::createMessage(m_GMNick, msg));
      m_irc->sendCommand(IrcCommand::createMessage("#caIRCtest", msg));
      return;
  }
  if (tcpSocket->isTcp())
  {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << msg;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    tcpSocket->tcpSocket()->write(block);
  }
  else HTTPSend(QString(tr("%1,%2,%3").arg(N_WEB_SEND).arg(QString::number(tcpSocket->web())).arg(msg)));
}

void CANetwork::clientRead()
{
  QDataStream in(tcpSocket->tcpSocket());
  in.setVersion(QDataStream::Qt_4_0);

  while (tcpSocket->tcpSocket()->bytesAvailable() > 0)
  {
    if (blockSize == 0)
    {
      if (tcpSocket->tcpSocket()->bytesAvailable() < (int)sizeof(quint16)) return;

      in >> blockSize;
    }

    if (tcpSocket->tcpSocket()->bytesAvailable() < blockSize) return;
    QString nextReceive;
    in >> nextReceive;

    if (nextReceive == currentReceive) return;

    currentReceive = nextReceive;

    clientParseReceive(currentReceive);
    currentReceive.clear();
    blockSize = 0;
  }
}

void CANetwork::displayError(QAbstractSocket::SocketError socketError)
{
  QSettings settings("ca.ini", QSettings::IniFormat);
  bool wb = settings.value("webBackup", true).toBool();
  switch (socketError)
  {
    case QAbstractSocket::RemoteHostClosedError:
      return;
    case QAbstractSocket::HostNotFoundError:
      tcpSocket->setError(tr("The host was not found. Please check the host name and port settings."));
      break;
    case QAbstractSocket::ConnectionRefusedError:
      tcpSocket->setError(tr("The connection was refused by the peer. Please check the host name and port settings."));
      break;
    default:
      tcpSocket->setError(tr("An error occurred: %1.").arg(tcpSocket->tcpSocket()->errorString()));
      break;
  }
  if (wb)
  {
    connect(m_HTTP, SIGNAL(finished(QNetworkReply*)), this, SLOT(HTTPClient(QNetworkReply*)), Qt::AutoConnection);
    tcpSocket->setTcp(false);
    HTTPSend(QString(tr("%1,%2,%3").arg(N_WEB_CLIENT).arg(tcpSocket->host()).arg(userName)));
    emit changeNetworkLabel("Connecting to web server...");
    connect(m_webTimer, SIGNAL(timeout()), this, SLOT(webMsg()), Qt::AutoConnection);
    webMsg();
    m_webTimer->start(5000);
  }
  else
  {
    networkDisconnect();
    QString e = tcpSocket->lastError();
    if (!e.isEmpty() && !e.isNull()) QMessageBox::critical(qobject_cast<QWidget*>(m_Parent), tr("CA Connect"), tr("%1\n\nBackup connection to web server disabled.").arg(e));
  }
}

void CANetwork::ircServerParse(IrcMessage* message)
{
    qDebug() << message->toData();
    if (message->type() == IrcMessage::Private)
    {
        IrcPrivateMessage* msg = static_cast<IrcPrivateMessage*>(message);
        QString text = msg->message();
        serverParseReceive(text, NULL);
    }
}

void CANetwork::ircClientParse(IrcMessage* message)
{
    qDebug() << message->toData();
    if (message->type() == IrcMessage::Private) {
        IrcPrivateMessage* msg = static_cast<IrcPrivateMessage*>(message);
      //  if (msg->message().startsWith(m_irc->nickName(), Qt::CaseInsensitive)) //Channel message
        if (msg->sender().name() == m_GMNick) //TODO: Add sanitisation
        {
            QString text = msg->message();//.mid(msg->message().indexOf(" "));
            clientParseReceive(text);
            //m_irc->sendCommand(IrcCommand::createMessage(m_irc->channel(), msg->sender().name() + ":" + "received " + text));
        }
    }
}

void CANetwork::clientParseReceive(QString r)
{
  QList<QString> msg = r.split(",");
  static QByteArray imgData = QByteArray();
  static QByteArray soundData = QByteArray();
  static QString chxRcv("0");
  int a = msg.value(0, QString("-1")).toInt();
  switch (a)
  {
    case N_REQUEST_USERNAME:
    {
      m_Host = msg.value(2, QString("(unknown)"));
      emit changeNetworkLabel(tr("Connected to %1").arg(m_Host.mid(0, 30)));
      clientWrite(QString::number(N_PROVIDE_USERNAME)+","+msg.value(1, QString("-1"))+","+userName.replace(',','.'));
      break;
    }
    case N_CIRCLE_DRAWN:
    {
      int p = msg.value(1, QString("-1")).toInt();
      int x = msg.value(2, QString("-1")).toInt();
      int y = msg.value(3, QString("-1")).toInt();
      QColor c(msg.value(4, QString("255")).toInt(), msg.value(5, QString("255")).toInt(), msg.value(6, QString("255")).toInt());
      int size = msg.value(7, QString("1")).toInt();
      QString letter = msg.value(8, QString(""));
      if (p >= 0 && x >= 0 && y >= 0) tabWidget->drawCircle(p, x, y, c, size, letter);
      break;
    }
    case N_BLOCK_DRAWN:
    {
      int p = msg.value(1, QString("-1")).toInt();
      int x = msg.value(2, QString("-1")).toInt();
      int y = msg.value(3, QString("-1")).toInt();
      QColor c(msg.value(4, QString("255")).toInt(), msg.value(5, QString("255")).toInt(), msg.value(6, QString("255")).toInt());
      QString letter = msg.value(7, QString(""));
      if (p >= 0 && x >= 0 && y >= 0) tabWidget->drawBlock(p, x, y, c, letter);
      break;
    }
    case N_CROSS_DRAWN:
    {
      int p = msg.value(1, QString("-1")).toInt();
      int x = msg.value(2, QString("-1")).toInt();
      int y = msg.value(3, QString("-1")).toInt();
      QColor c(msg.value(4, QString("255")).toInt(), msg.value(5, QString("255")).toInt(), msg.value(6, QString("255")).toInt());
      if (p >= 0 && x >= 0 && y >= 0) tabWidget->drawCross(p, x, y, c);
      break;
    }
    case N_TRIANGLE_DRAWN:
    {
      int p = msg.value(1, QString("-1")).toInt();
      int x = msg.value(2, QString("-1")).toInt();
      int y = msg.value(3, QString("-1")).toInt();
      QColor c(msg.value(4, QString("255")).toInt(), msg.value(5, QString("255")).toInt(), msg.value(6, QString("255")).toInt());
      int facing = msg.value(7, QString("1")).toInt();
      if (p >= 0 && x >= 0 && y >= 0) tabWidget->drawTriangle(p, x, y, c, facing);
      break;
    }
    case N_LINE_DRAWN:
    {
      int p = msg.value(1, QString("-1")).toInt();
      double x1 = msg.value(2, QString("-1")).toDouble();
      double y1 = msg.value(3, QString("-1")).toDouble();
      double x2 = msg.value(4, QString("-1")).toDouble();
      double y2 = msg.value(5, QString("-1")).toDouble();
      QColor c(msg.value(6, QString("255")).toInt(), msg.value(7, QString("255")).toInt(), msg.value(8, QString("255")).toInt());
      int w = msg.value(9, QString("1")).toInt();
      if (x1 >= 0 && y1 >= 0 && x2 >= 0 && y2 >= 0) tabWidget->drawLine(p, x1, y1, x2, y2, QPen(QBrush(c), w));
      break;
    }
    case N_CAITEM_DRAWN:
    {
      int p = msg.value(1, QString("-1")).toInt();
      int subtype = msg.value(2, QString("-1")).toInt();
      CAGraphicsItem *c = CAGraphicsItemCreator::create(subtype, new QString(msg.value(3)));
      if (c) tabWidget->drawCAItem(p,c);
      break;
    }
    case N_ITEM_REMOVED:
    {
      int p = msg.value(1, QString("-1")).toInt();
      int x = msg.value(2, QString("-1")).toInt();
      int y = msg.value(3, QString("-1")).toInt();
      if (p >= 0 && x >= 0 && y >= 0) tabWidget->removeItem(p, x, y);
      break;
    }
    case N_ITEM_REMOVED_CHECK:
    {
      int p = msg.value(1, QString("-1")).toInt();
      quint16 check = msg.value(2, QString("0")).toUInt();
      if (p >= 0 && check > 0) tabWidget->removeItem(p, check);
      break;
    }
    case N_DRAG_START:
    {
      int p = msg.value(1, QString("-1")).toInt();
      int x = msg.value(2, QString("-1")).toInt();
      int y = msg.value(3, QString("-1")).toInt();
      if (p >= 0 && x >= 0 && y >= 0) tabWidget->startDrag(p, x, y);
      break;
    }
    case N_DRAG_FINISH:
    {
      int p = msg.value(1, QString("-1")).toInt();
      int x = msg.value(2, QString("-1")).toInt();
      int y = msg.value(3, QString("-1")).toInt();
      if (p >= 0) tabWidget->finishDrag(p, x, y);
      break;
    }
    case N_TAB_ADD:
    {
      QSettings settings("ca.ini", QSettings::IniFormat);
      int w = msg.value(1, QString("20")).toInt();
      int h = msg.value(2, QString("20")).toInt();
      int b = msg.value(3, QString("20")).toInt();
      if (settings.value("grid/overrideHost", false).toBool()) b = settings.value("grid/cellSize", b).toInt();
      QColor bg(msg.value(4, QString("0")).toInt(), msg.value(5, QString("0")).toInt(), msg.value(6, QString("0")).toInt());
      QColor ln(msg.value(7, QString("255")).toInt(), msg.value(8, QString("255")).toInt(), msg.value(9, QString("255")).toInt());
      QColor co(msg.value(10, QString("255")).toInt(), msg.value(11, QString("255")).toInt(), msg.value(12, QString("0")).toInt());
      int l = msg.value(13, QString::number(Qt::DotLine)).toInt();
      if (settings.value("grid/overrideHost", false).toBool()) l = settings.value("grid/lineStyle", l).toInt();
      if (bg == Qt::black || settings.value("grid/overrideHost", false).toBool()) bg = settings.value("grid/bgColour", bg).value<QColor>();
      if (ln == Qt::white || settings.value("grid/overrideHost", false).toBool()) ln = settings.value("grid/lnColour", ln).value<QColor>();
      if (co == Qt::yellow || settings.value("grid/overrideHost", false).toBool()) co = settings.value("grid/coColour", co).value<QColor>();
      tabWidget->newGrid(w, h, b, bg, ln, co, l);
      tabWidget->setAllClient(m_Client);
      break;
    }
    case N_TAB_REMOVE:
    {
      int p = msg.value(1, QString("-1")).toInt();
      tabWidget->delGrid(p);
      break;
    }
    case N_GRID_REDRAW:
    {
      QSettings settings("ca.ini", QSettings::IniFormat);
      bool ar = settings.value("autoResize", true).toBool();

      int p = msg.value(1, QString("-1")).toInt();
      int w = msg.value(2, QString("20")).toInt();
      int h = msg.value(3, QString("20")).toInt();
      int b = msg.value(4, QString("20")).toInt();
      if (settings.value("grid/overrideHost", false).toBool()) b = settings.value("grid/cellSize", 20).toInt();
      QColor bg(msg.value(5, QString("0")).toInt(), msg.value(6, QString("0")).toInt(), msg.value(7, QString("0")).toInt());
      QColor ln(msg.value(8, QString("255")).toInt(), msg.value(9, QString("255")).toInt(), msg.value(10, QString("255")).toInt());
      QColor co(msg.value(11, QString("255")).toInt(), msg.value(12, QString("255")).toInt(), msg.value(13, QString("0")).toInt());
      int l = msg.value(14, QString::number(Qt::DotLine)).toInt();
      if (settings.value("grid/overrideHost", false).toBool()) l = settings.value("grid/lineStyle", l).toInt();
      if (bg == Qt::black || settings.value("grid/overrideHost", false).toBool()) bg = settings.value("grid/bgColour", bg).value<QColor>();
      if (ln == Qt::white || settings.value("grid/overrideHost", false).toBool()) ln = settings.value("grid/lnColour", ln).value<QColor>();
      if (co == Qt::yellow || settings.value("grid/overrideHost", false).toBool()) co = settings.value("grid/coColour", co).value<QColor>();
      CAGrid *grid = tabWidget->getGrid(p);
      tabWidget->setColumnCount(w, p);
      tabWidget->setRowCount(h, p);
      tabWidget->setBoxSize(b, p);
      grid->recalculate(w, h, grid->hMargin(), grid->vMargin(), b, ln, l, co);
      grid->draw(tabWidget->getCurrentGridScene(p));
      if (ar) tabWidget->currentResizeToFit();
      tabWidget->setBackgroundColour(bg, p);
      tabWidget->clearCurrentGrid(true, false, p);
      break;
    }
    case N_COLOUR_BG:
    {
      QSettings settings("ca.ini", QSettings::IniFormat);
      int p = msg.value(1, QString("-1")).toInt();
      QColor bg(msg.value(2, QString("0")).toInt(), msg.value(3, QString("0")).toInt(), msg.value(4, QString("0")).toInt());
      if (bg == Qt::black || settings.value("grid/overrideHost", false).toBool()) bg = settings.value("grid/bgColour", Qt::black).value<QColor>();
      tabWidget->setBackgroundColour(bg, p);
      break;
    }
    case N_COLOUR_LN:
    {
      QSettings settings("ca.ini", QSettings::IniFormat);
      int p = msg.value(1, QString("-1")).toInt();
      QColor ln(msg.value(2, QString("255")).toInt(), msg.value(3, QString("255")).toInt(), msg.value(4, QString("255")).toInt());
      if (ln == Qt::white || settings.value("grid/overrideHost", false).toBool()) ln = settings.value("grid/lnColour", Qt::white).value<QColor>();
      CAGrid *grid = tabWidget->getGrid(p);
      grid->setColour(ln);
      break;
    }
    case N_COLOUR_CO:
    {
      QSettings settings("ca.ini", QSettings::IniFormat);
      int p = msg.value(1, QString("-1")).toInt();
      QColor co(msg.value(2, QString("255")).toInt(), msg.value(3, QString("255")).toInt(), msg.value(4, QString("0")).toInt());
      if (co == Qt::yellow || settings.value("grid/overrideHost", false).toBool()) co = settings.value("grid/coColour", Qt::yellow).value<QColor>();
      CAGrid *grid = tabWidget->getGrid(p);
      grid->setCoordinateColour(co);
      break;
    }
    case N_STYLE_LN:
    {
      int p = msg.value(1, QString("-1")).toInt();
      int l = msg.value(2, QString::number(Qt::DotLine)).toInt();
      CAGrid *grid = tabWidget->getGrid(p);
      grid->setType(l);
      break;
    }
    case N_GRID_CLEAR:
    {
      int p = msg.value(1, QString("-1")).toInt();
      int total = msg.value(2, QString("0")).toInt();
      tabWidget->clearCurrentGrid((bool)total, false, p);
      break;
    }
    case N_PING:
    {
      clientWrite(QString::number(N_PONG)+","+msg.value(1, QString("-1")));
      break;
    }
    case N_IMG_WEB:
    {
      int p = msg.value(1, QString("-1")).toInt();
      QString chx = msg.value(2, "0");
      int x = msg.value(3, QString("0")).toInt();
      int y = msg.value(4, QString("0")).toInt();
      QString uri = msg.value(5, "");
      if (chx == "0" && uri.isEmpty()) tabWidget->setBackgroundByFile("", p);
      else if (!tabWidget->setBackgroundByHash(chx, p, x, y))
      {
        emit changeNetworkLabel(tr("Waiting for image from host"));
        tabWidget->setBackgroundByURI(uri, p, chx, x, y);
      }
      break;
    }
    case N_PIXMAP_BG:
    {
      int p = msg.value(1, QString("-1")).toInt();
      QString chx = msg.value(2, "0");
      int x = msg.value(3, QString("0")).toInt();
      int y = msg.value(4, QString("0")).toInt();
      if (chx == "0") tabWidget->setBackgroundByFile("", p);
      else if (chxRcv != "0" && chx == chxRcv) break;
      else if (!tabWidget->setBackgroundByHash(chx, p, x, y))
      {
        if (tcpSocket->isTcp()) clientWrite(QString::number(N_REQUEST_IMG)+","+QString::number(p)+","+chx+","+QString::number(x)+","+QString::number(y));
        else HTTPSend(QString(tr("%1,%2,%3,%4,%5").arg(N_WEB_GETIMG).arg(p).arg(chx).arg(x).arg(y)));
        emit changeNetworkLabel(tr("Waiting for image from host"));
      }
      break;
    }
    case N_RECV_IMG:
    {
      int p = msg.value(1, QString("-1")).toInt();
      QString chx = msg.value(2, QString("0"));
      int piece = msg.value(3, QString("1")).toInt();
      int total = msg.value(4, QString("1")).toInt();
      QString suffix = msg.value(5, QString("png"));
      QString fData = msg.value(6, QString(""));
      int x = msg.value(7, QString("0")).toInt();
      int y = msg.value(8, QString("0")).toInt();
      imgData += qUncompress(QByteArray::fromBase64(fData.toAscii()));
      if (piece == 1 && total > 1)
      {
        chxRcv = QString(chx);
        emit updateProgress(0, total);
      }
      emit updateProgress(piece, total);
      if (piece == total)
      {
        chxRcv = QString("0");
        QDir dir("images/");
        if (!dir.exists()) QDir().mkdir("images/");
        QString newFileName(tr("images/%1.%2").arg(epoch_s(), suffix));
        QFile *file = new QFile(newFileName);
        if (!file->open(QIODevice::WriteOnly)) break;
        file->write(imgData);
        file->close();
        tabWidget->setBackgroundByHash(chx, p);
        tabWidget->backgroundOffset(p, x, y);
        imgData = QByteArray();
        emit changeNetworkLabel(tr("Connected to %1").arg(m_Host.mid(0, 30)));
      }
      break;
    }
    case N_CANCEL_IMG:
    {
      chxRcv = QString("0");
      imgData = QByteArray();
      emit updateProgress(0, 0);
      break;
    }
    case N_SOUND:
    {
      QSettings settings("ca.ini", QSettings::IniFormat);
      if (settings.value("disableSound", false).toBool()) break;
      QString chx = msg.value(1, "0");
      if (!playSound(chx)) clientWrite(QString::number(N_REQUEST_SOUND)+","+chx);
      break;
    }
    case N_RECV_SOUND:
    {
      QString chx = msg.value(2, QString("0"));
      QString fileName = msg.value(1, QString(chx));
      int piece = msg.value(3, QString("1")).toInt();
      int total = msg.value(4, QString("1")).toInt();
      QString fData = msg.value(5, QString(""));
      soundData += qUncompress(QByteArray::fromBase64(fData.toAscii()));
      if (piece == 1 && total > 1)
      {
        emit updateProgress(0, total);
      }
      emit updateProgress(piece, total);
      if (piece == total)
      {
        QDir dir("sound/");
        if (!dir.exists()) QDir().mkdir("sound/");
        QString newFileName(tr("sound/%1").arg(fileName));
        QFile *file = new QFile(newFileName);
        if (file->exists()) file->setFileName(tr("sound/%1.%2").arg(chx, QFileInfo(file->fileName()).suffix()));
        if (!file->open(QIODevice::WriteOnly)) break;
        file->write(soundData);
        file->close();
        playSound(chx);
        soundData = QByteArray();
      }
      break;
    }
    case N_STOP_SOUND:
    {
#ifdef WINDOWS
      mciSendStringA("close file", NULL, 0,0);
#endif
      break;
    }
    case N_BG_OFFSET:
    {
      int p = msg.value(1, QString("-1")).toInt();
      int x = msg.value(2, QString("0")).toInt();
      int y = msg.value(3, QString("0")).toInt();
      tabWidget->backgroundOffset(p, x, y);
      break;
    }
    case N_TAB_RENAME:
    {
      int p = msg.value(1, QString("-1")).toInt();
      QString txt = msg.value(2, QString("Map %1").arg(p));
      if (p < 0) tabWidget->setTabText(tabWidget->currentIndex(), txt);
      else if (p < tabWidget->count()) tabWidget->setTabText(p, txt);
      break;
    }
    case N_WEB_CLIENT:
    {
      tcpSocket->setWeb(msg.value(1, 0).toLong());
      m_Host = msg.value(2, QString("(unknown)"));
      m_Client = true;
      emit clientConnected(true);
      emit changeNetworkLabel(tr("Connected to %1").arg(m_Host.mid(0, 30)));
      break;
    }
    case N_WEB_INFO:
    {
      QString info = msg.value(1, "");
      if (info.length() > 0) QMessageBox::information(qobject_cast<QWidget*>(m_Parent), "Web Server", info);
      break;
    }
    case N_WEB_ERROR:
    {
      QString error = msg.value(1, "");
      if (error.length() > 0) QMessageBox::critical(qobject_cast<QWidget*>(m_Parent), "Web Server", error);
      networkDisconnect();
      break;
    }
    case N_WEB_FAIL:
    {
      networkDisconnect();
      QString e = tcpSocket->lastError();
      if (!e.isEmpty() && !e.isNull()) QMessageBox::critical(qobject_cast<QWidget*>(m_Parent), tr("CA Connect"), tr("%1\n\nBackup connection to web server also failed.").arg(e));
      else QMessageBox::critical(qobject_cast<QWidget*>(m_Parent), tr("CA Connect"), tr("Connection to web server failed."));
      break;
    }
    case N_WEB_DISC:
    {
      m_Client = false;
      emit clientConnected(false);
      emit changeNetworkLabel("Not connected");
      tcpSocket->tcpSocket()->disconnect(this, SLOT(displayError(QAbstractSocket::SocketError)));
      tcpSocket->tcpSocket()->disconnect(this, SLOT(clientRead()));
      tcpSocket->tcpSocket()->disconnect(this, SLOT(clientConnected()));
      tcpSocket->tcpSocket()->disconnect(this, SLOT(clientDisconnected()));
      disconnect(m_HTTP, SIGNAL(finished(QNetworkReply*)), this, SLOT(HTTPClient(QNetworkReply*)));
      tcpSocket->setTcp(true);
      disconnect(m_webTimer, SIGNAL(timeout()), this, SLOT(webMsg()));
      m_webTimer->stop();
      networkDisconnect();
      break;
    }
    default:
      break;
  }
}

void CANetwork::serverParseReceive(QString r, CASock *sock)
{
  QList<QString> msg = r.split(",");
  int a = msg.value(0, QString("-1")).toInt();
  if (m_ircGM)
  {
      if (a == N_GRID_RESYNC)
      {
        resync(sock);
      }
  }
  switch (a)
  {
    case N_PROVIDE_USERNAME:
    {
      int c = msg.value(1, QString("-1")).toInt();
      QString uname = msg.value(2, QString(""));
      if (uname.isNull() || uname.isEmpty()) uname = QString(tr("User%1").arg((rand()%99)+1));
      if (c >= 0 && !uname.isNull() && !uname.isEmpty())
      {
        m_ClientNames.replace(c, uname);
        emit clientChange(m_ClientNames, clients);
      }
      break;
    }
    case N_GRID_RESYNC:
    {
      resync(sock);
      break;
    }
    case N_PONG:
    {
      break;
    }
    case N_REQUEST_IMG:
    {
      int p = msg.value(1, "-1").toInt();
      QString chx = msg.value(2, "0");
      int cx = msg.value(3, QString("0")).toInt();
      int cy = msg.value(4, QString("0")).toInt();
      QString fileName = findFileByHash(chx, "images/");
      QFile imgFile;
      if (!fileName.isEmpty()) imgFile.setFileName(fileName);
      if (imgFile.fileName().isNull() || imgFile.fileName().isEmpty()) break;
      m_Send.append(new CASendFile(this, (int)N_RECV_IMG, imgFile.fileName(), sock, p, cx, cy));
      if (!m_sendTimer->isActive()) m_sendTimer->start(1000);
      break;
    }
    case N_REQUEST_SOUND:
    {
      QString chx = msg.value(1, "0");
      QFile soundFile;
      QString filename = findFileByHash(chx, "sound/");
      if (!filename.isEmpty()) soundFile.setFileName(filename);
      if (soundFile.fileName().isNull() || soundFile.fileName().isEmpty()) break;
      soundFile.open(QIODevice::ReadOnly);
      for (int x = 0; (x*_CHUNK)<soundFile.size(); x++)
      {
        QString oData;
        QByteArray rArray = qCompress(soundFile.read((qint64)_CHUNK), 9);
        oData = rArray.toBase64();
        QString msg(tr("%1,%2,%3,%4,%5,%6").arg(QString::number(N_RECV_SOUND), QString(QFileInfo(filename).fileName()).replace(",", "."), chx, QString::number(x+1), QString::number(soundFile.size()/_CHUNK+(((float)(soundFile.size()/(float)_CHUNK)==(soundFile.size()/_CHUNK))?0:1)), oData));
        serverWriteToSocket(sock, msg);
      }
      soundFile.close();
      break;
    }
    case N_CANCEL_IMG:
    {
      if (!m_Send.isEmpty())
      {
        for (int x = m_Send.size(); x > 0; x--)
        {
          if (m_Send.at((x-1))->getSock() == sock)
          {
            m_Send.at((x-1))->cancelSend();
            delete m_Send.takeAt((x-1));
          }
        }
      }
      break;
    }
    case N_WEB_CLIENT:
    {
      qlonglong web = msg.value(1, 0).toLongLong();
      QString uname = msg.value(2, QString(""));
      if (web > 0)
      {
        if (hasWebClient(web) >= 0) break;
        clients.append(new CASock(false, NULL, web));
        emit changeNetworkLabel(tr("Connected: %1").arg(clients.size()));
        resync(clients.last());
        if (uname.isNull() || uname.isEmpty()) uname = QString(tr("User%1").arg((rand()%99)+1));
        m_ClientNames.append(uname);
        emit clientChange(m_ClientNames, clients);
      }
      break;
    }
    case N_WEB_INFO:
    {
      QString info = msg.value(1, "");
      if (info.length() > 0) QMessageBox::information(qobject_cast<QWidget*>(m_Parent), "Web Server", info);
      break;
    }
    case N_WEB_ERROR:
    {
      QString error = msg.value(1, "");
      if (error.length() > 0) QMessageBox::critical(qobject_cast<QWidget*>(m_Parent), "Web Server", error);
      if (m_webTimer->isActive())
      {
        m_webTimer->stop();
        m_webTimer->disconnect(this);
      }
      for (int x = clients.size(); x > 0; x--) if (!clients.at(x)->isTcp()) delete clients.takeAt(x);
      break;
    }
    case N_WEB_DISC:
    {
      int x = hasWebClient(sock->web());
      if (x >= 0)
      {
        clients.removeAt(x);
        m_ClientNames.removeAt(x);
        clientBlockSize.removeAt(x);
        clientCurrentReceive.removeAt(x);
        if (clients.size()==0) emit changeNetworkLabel(((tcpServer->isListening())?tr("Listening on port %1").arg(tcpServer->serverPort()):tr("Listening to web server")));
        else emit changeNetworkLabel(tr("Connected: %1").arg(clients.size()));
        emit clientChange(m_ClientNames, clients);
      }
      break;
    }
    case N_WEB_NOIMG:
    {
      QString chx = msg.value(1, "0");
      sendImage(chx);
      break;
    }
    default:
      break;
  }
}

int CANetwork::hasWebClient(qlonglong web)
{
  if (isServer())
  {
    for (int x = 0; x < clients.size(); x++)
      if (!clients.at(x)->isTcp() && clients.at(x)->web() == web) return x;
  }
  return -1;
}

void CANetwork::banUser(int d)
{
  if (d < clients.size() && clients.at(d)->isTcp())
  {
    m_Banlist.append(clients.at(d)->tcpSocket()->peerAddress());
    disconnectUser(d);
  }
}

bool CANetwork::playSound(QString chx)
{
  QString fileName = findFileByHash(chx, "sound/");
  if (!fileName.isEmpty())
  {
    if (QString::compare(QFileInfo(fileName).suffix(), "mp3", Qt::CaseInsensitive) == 0)
    {
#ifdef WINDOWS
      wchar_t *wch = NULL;
      QString("open "+fileName+" type mpegvideo alias file").toWCharArray(wch);
      mciSendString(wch, NULL, 0,0);
      mciSendStringA("play file", NULL, 0, 0);
#endif /* WIN32 */
    }
    else
    {
#ifdef WINDOWS
      QSound s(fileName, this);
      s.play();
#endif
    }
    return true;
  }
  return false;
}

void CANetwork::circleDrawn(int p, int x, int y, QColor c, int size, QString letter, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4,%5,%6,%7,%8,%9").arg(QString::number(N_CIRCLE_DRAWN),QString::number(p),  QString::number(x), QString::number(y),
      QString::number(c.red()), QString::number(c.green()), QString::number(c.blue()), QString::number(size), letter));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::blockDrawn(int p, int x, int y, QColor c, QString letter, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4,%5,%6,%7,%8").arg(QString::number(N_BLOCK_DRAWN), QString::number(p), QString::number(x), QString::number(y),
      QString::number(c.red()), QString::number(c.green()), QString::number(c.blue()), letter));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::crossDrawn(int p, int x, int y, QColor c, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4,%5,%6,%7").arg(QString::number(N_CROSS_DRAWN), QString::number(p), QString::number(x), QString::number(y),
      QString::number(c.red()), QString::number(c.green()), QString::number(c.blue())));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::triangleDrawn(int p, int x, int y, QColor c, int facing, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4,%5,%6,%7,%8").arg(QString::number(N_TRIANGLE_DRAWN), QString::number(p), QString::number(x), QString::number(y),
      QString::number(c.red()), QString::number(c.green()), QString::number(c.blue()), QString::number(facing)));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::lineDrawn(int p, double x1, double y1, double x2, double y2, QPen pen, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4,%5,%6,%7,%8,%9").arg(QString::number(N_LINE_DRAWN), QString::number(p), QString::number(x1, 'f', 6), QString::number(y1, 'f', 6), QString::number(x2, 'f', 6), QString::number(y2, 'f', 6),
      QString::number(pen.color().red()), QString::number(pen.color().green()), QString::number(pen.color().blue())));
    msg += tr(",%1").arg(QString::number(pen.width()));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::itemRemoved(int p, int x, int y)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4").arg(QString::number(N_ITEM_REMOVED), QString::number(p), QString::number(x), QString::number(y)));
    serverWrite(msg);
  }
}

void CANetwork::itemRemoved(int p, quint16 check)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4").arg(QString::number(N_ITEM_REMOVED_CHECK), QString::number(p), QString::number(check)));
    serverWrite(msg);
  }
}

void CANetwork::startDrag(int p, int x, int y)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4").arg(QString::number(N_DRAG_START), QString::number(p), QString::number(x), QString::number(y)));
    serverWrite(msg);
  }
}

void CANetwork::finishDrag(int p, int x, int y)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4").arg(QString::number(N_DRAG_FINISH), QString::number(p), QString::number(x), QString::number(y)));
    serverWrite(msg);
  }
}

void CANetwork::renameTab(QString txt, int p, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3").arg(QString::number(N_TAB_RENAME), QString::number(p), txt));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::tabAdded(int w, int h, int b, QColor bg, QColor ln, QColor co, int l, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4,%5,%6,%7,%8,%9").arg(QString::number(N_TAB_ADD), QString::number(w), QString::number(h), QString::number(b),
      QString::number(bg.red()), QString::number(bg.green()), QString::number(bg.blue()), QString::number(ln.red()), QString::number(ln.green())));
    msg += tr(",%1,%2,%3,%4,%5").arg(QString::number(ln.blue()), QString::number(co.red()), QString::number(co.green()), QString::number(co.blue()), QString::number(l));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::tabRemoved(int p, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2").arg(QString::number(N_TAB_REMOVE), QString::number(p)));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::changeBackground(CABackground *b, int p, CASock *sock)
{
  if (isServer())
  {
    bool sendToWeb = false;
    QString msg("");
    switch (b->type())
    {
      case CABackground::TypeLocal:
        msg.append(tr("%1,%2,%3,%4,%5").arg(QString::number(N_PIXMAP_BG), QString::number(p), b->hash(), QString::number(b->offset().x()), QString::number(b->offset().y())));
        for (int x = 0; x < clients.size(); x++)
          if (!clients.at(x)->isTcp()) sendToWeb = true;
        break;
      case CABackground::TypeRemote:
        msg.append(tr("%1,%2,%3,%4,%5,%6").arg(QString::number(N_IMG_WEB), QString::number(p), b->hash(), QString::number(b->offset().x()), QString::number(b->offset().y()), b->location()));
        break;
      default:
      break;
    }
    if (!msg.isNull() && !msg.isEmpty())
    {
      if (sendToWeb) HTTPSend(QString(tr("%1,%2")).arg(N_WEB_HASIMG).arg(b->hash()));
      if (sock) serverWriteToSocket(sock, msg);
      else serverWrite(msg);
    }
  }
}

void CANetwork::changeBackgroundColour(QColor bg, int p, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4,%5").arg(QString::number(N_COLOUR_BG), QString::number(p), QString::number(bg.red()), QString::number(bg.green()),
      QString::number(bg.blue())));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::changeLineStyle(int l, int p, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3").arg(QString::number(N_STYLE_LN), QString::number(p), QString::number(l)));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::changeLineColour(QColor ln, int p, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4,%5").arg(QString::number(N_COLOUR_LN), QString::number(p), QString::number(ln.red()), QString::number(ln.green()),
      QString::number(ln.blue())));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::changeCoordinateColour(QColor co, int p, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4,%5").arg(QString::number(N_COLOUR_CO), QString::number(p), QString::number(co.red()), QString::number(co.green()),
      QString::number(co.blue())));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::redrawGrid(int p, int w, int h, int b, QColor bg, QColor ln, QColor co, int l, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4,%5,%6,%7,%8,%9").arg(QString::number(N_GRID_REDRAW), QString::number(p), QString::number(w), QString::number(h),
      QString::number(b), QString::number(bg.red()), QString::number(bg.green()), QString::number(bg.blue()), QString::number(ln.red())));
    msg += tr(",%1,%2,%3,%4,%5,%6").arg(QString::number(ln.green()), QString::number(ln.blue()), QString::number(co.red()), QString::number(co.green()),
      QString::number(co.blue()), QString::number(l));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::caItemDrawn(int p, CAGraphicsItem *i, CASock *sock)
{
    if (isServer())
    {
        char *s = i->transmit();
        QString msg(QString::number(N_CAITEM_DRAWN) + tr(",") + QString::number(p) + tr(",")+tr(s));
        if (sock) serverWriteToSocket(sock, msg);
        else serverWrite(msg);
        free(s);
    }
}

void CANetwork::clearGrid(int p, bool total)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3").arg(QString::number(N_GRID_CLEAR), QString::number(p), QString::number(total)));
    serverWrite(msg);
  }
}

void CANetwork::backgroundOffset(int p, int x, int y, CASock *sock)
{
  if (isServer())
  {
    QString msg(tr("%1,%2,%3,%4").arg(QString::number(N_BG_OFFSET), QString::number(p), QString::number(x), QString::number(y)));
    if (sock) serverWriteToSocket(sock, msg);
    else serverWrite(msg);
  }
}

void CANetwork::updateDownload(qint64 r, qint64 t)
{
  if (r == t) emit changeNetworkLabel(tr("Connected to %1").arg(m_Host.mid(0, 30)));
  emit updateProgress(r, t);
}

void CANetwork::resync(CASock *sock)
{
  if (isServer())
  {
    tabRemoved(-1, sock);
    redrawGrid(0, tabWidget->getColumnCount(0), tabWidget->getRowCount(0), tabWidget->getBoxSize(0), tabWidget->getBackgroundColour(0), tabWidget->getLineColour(0), tabWidget->getCoordinateColour(0), tabWidget->getLineStyle(0), sock);
    renameTab(tabWidget->tabText(0), 0, sock);
    for (int x = 1; x < tabWidget->count(); x++)
    {
      tabAdded(tabWidget->getColumnCount(x), tabWidget->getRowCount(x), tabWidget->getBoxSize(x), tabWidget->getBackgroundColour(x), tabWidget->getLineColour(x), tabWidget->getCoordinateColour(x), tabWidget->getLineStyle(x), sock);
      renameTab(tabWidget->tabText(x), x, sock);
    }
    for (int x = 0; x < tabWidget->count(); x++)
    {
      QGridView *v = static_cast<QGridView*>(tabWidget->widget(x));
      QGridScene *s;
      if (v && (s = static_cast<QGridScene*>(v->scene())))
      {
        QPoint o = QPoint(0, 0);
        CABackground *bg = v->getBackground();
        CAGrid *g = v->getGrid();
        if (bg) o = bg->offset();
        changeBackground(bg, x, sock);
        changeLineStyle(g->type(), x, sock);
        changeLineColour(g->lineColour(), x, sock);
        QList<QGraphicsItem*>ie = s->items();
        for (int i = 0; i < ie.size(); i++)
        {
          if (ie[i]->parentItem() != 0) continue;
          switch (ie[i]->type())
          {
            case QGraphicsEllipseItem::Type:
            {
              QPoint p = s->mapPoint(ie[i]->boundingRect().topLeft().x(), ie[i]->boundingRect().topLeft().y());
              if (p.x() < 0 || p.y() < 0) break;
              QAbstractGraphicsShapeItem *it = static_cast<QAbstractGraphicsShapeItem*>(ie[i]);
              QString l("");
              QList<QGraphicsItem*> ci = it->childItems();
              if (ci.size() > 0 && ci[0]->type() == QGraphicsTextItem::Type) l = static_cast<QGraphicsTextItem*>(ci[0])->toPlainText();
              circleDrawn(x, p.x(), p.y(), it->brush().color(), it->data(0).toInt(), l, sock);
              break;
            }
            case QGraphicsRectItem::Type:
            {
              QPoint p = s->mapPoint(ie[i]->boundingRect().topLeft().x(), ie[i]->boundingRect().topLeft().y());
              if (p.x() < 0 || p.y() < 0) break;
              QAbstractGraphicsShapeItem *it = static_cast<QAbstractGraphicsShapeItem*>(ie[i]);
              QString l("");
              QList<QGraphicsItem*> ci = it->childItems();
              if (ci.size() > 0 && ci[0]->type() == QGraphicsTextItem::Type) l = static_cast<QGraphicsTextItem*>(ci[0])->toPlainText();
              blockDrawn(x, p.x(), p.y(), it->brush().color(), l, sock);
              break;
            }
            case QGraphicsPathItem::Type:
            {
              QPoint p = s->mapPoint(ie[i]->boundingRect().topLeft().x(), ie[i]->boundingRect().topLeft().y());
              if (p.x() < 0 || p.y() < 0) break;
              QAbstractGraphicsShapeItem *it = static_cast<QAbstractGraphicsShapeItem*>(ie[i]);
              crossDrawn(x, p.x(), p.y(), it->brush().color(), sock);
              break;
            }
            case QGraphicsPolygonItem::Type:
            {
              QPoint p = s->mapPoint(ie[i]->pos().x(), ie[i]->pos().y());
              if (p.x() < 0 || p.y() < 0) break;
              QAbstractGraphicsShapeItem *it = static_cast<QAbstractGraphicsShapeItem*>(ie[i]);
              triangleDrawn(x, p.x(), p.y(), it->brush().color(), it->data(0).toInt(), sock);
              break;
            }
            case CALine::Type:
            {
              CALine *it = static_cast<CALine*>(ie[i]);
              lineDrawn(x, it->startX(), it->startY(), it->endX(), it->endY(), it->pen(), sock);
              break;
            }
            case CAGraphicsItem::Type:
            {
              CAGraphicsItem *it = dynamic_cast<CAGraphicsItem*>(ie[i]);
              caItemDrawn(x, it, sock);
              break;
            }
            default:
              break;
          }
        }
      }
    }
  }
  else if (tcpSocket->tcpSocket()->isValid())
  {
    QString msg(tr("%1").arg(QString::number(N_GRID_RESYNC)));
    clientWrite(msg);
  }
  else if (m_irc && !m_ircGM)
  {
    QString msg(tr("%1").arg(QString::number(N_GRID_RESYNC)));
    clientWrite(msg);
  }
}

CASendFile::CASendFile(CANetwork *parent, int type, QString file, CASock *sock, int p, int x, int y)
{
  m_Parent = parent;
  m_Type = type;
  m_File.setFileName(file);
  m_Chx = QString("0");
  if (!m_File.fileName().isNull() && !m_File.fileName().isEmpty()) m_Chx = QString(fileChecksum(m_File.fileName()));
  m_Chunk = 0;
  m_p = p;
  m_x = x;
  m_y = y;
  m_Sock = sock;
  m_Timer = new QTimer();
  connect(m_Timer, SIGNAL(timeout()), this, SLOT(sendChunk()), Qt::AutoConnection);
  //m_Timer->connect(m_Timer, SIGNAL(timeout()), this, SLOT(sendChunk()), Qt::AutoConnection);
  //m_Timer->start(1000);
}

CASendFile::~CASendFile()
{
  stopSend();
  delete m_Timer;
}

void CASendFile::sendChunk()
{
  if (m_File.fileName().isNull() || m_File.fileName().isEmpty())
  {
    stopSend();
    return;
  }
  m_File.open(QIODevice::ReadOnly);
  if ((m_Chunk*_CHUNK)<m_File.size())
  {
    m_File.seek((qint64)(m_Chunk*_CHUNK));
    QString oData;
    QByteArray rArray = qCompress(m_File.read((qint64)_CHUNK), 9);
    oData = rArray.toBase64();
    QString msg(tr("%1,%2,%3,%4,%5,%6,%7,%8,%9").arg(QString::number(m_Type), QString::number(m_p), fileChecksum(m_File.fileName()), QString::number(++m_Chunk), QString::number(m_File.size()/_CHUNK+(((float)(m_File.size()/(float)_CHUNK)==(m_File.size()/_CHUNK))?0:1)), ((QFileInfo(m_File.fileName()).suffix().isEmpty())?"png":QFileInfo(m_File.fileName()).suffix()), oData, QString::number(m_x), QString::number(m_y)));
    m_Parent->serverWriteToSocket(m_Sock, msg);
  }
  else stopSend();
  if (m_File.isOpen()) m_File.close();
}

void CASendFile::stopSend()
{
  m_Chunk = 0;
  if (m_Timer->isActive()) m_Timer->stop();
  m_Timer->disconnect(m_Timer, SIGNAL(timeout()), this, SLOT(sendChunk()));
  if (m_File.isOpen()) m_File.close();
  m_File.setFileName("");
  m_Chx = QString("");
}

void CASendFile::cancelSend()
{
  if (m_Timer->isActive()) m_Parent->serverWriteToSocket(m_Sock, QString::number(N_CANCEL_IMG));
}

void CASendFile::startSend()
{
  if (m_Timer->isActive()) stopSend();
  m_Timer->start(1000);
}

QString CASendFile::fileName()
{
  return m_File.fileName();
}

bool CASendFile::timerActive()
{
  if (m_Timer)return m_Timer->isActive();
  return false;
}

void CANetwork::soundToClients(QString chx)
{
  if (isServer())
  {
    QString msg(tr("%1,%2").arg(QString::number(N_SOUND), (chx)));
    serverWrite(msg);
  }
}

void CANetwork::stopSoundOnClients()
{
  if (isServer())
  {
    QString msg(tr("%1").arg(QString::number(N_STOP_SOUND)));
    serverWrite(msg);
  }
}

void CANetwork::networkDisconnect()
{
  if (m_serverTimer->isActive())
  {
    m_serverTimer->stop();
    m_serverTimer->disconnect(this, SLOT(ping()));
  }
  if (m_webTimer->isActive())
  {
    HTTPSend(tr("%1").arg(N_WEB_DISC));
    webMsg();
    m_webTimer->stop();
    m_webTimer->disconnect(this);
  }
  if (tcpServer->isListening())
  {
    for (int x = 0; x < clients.size(); x++)
      if (clients.at(x)->tcpSocket()->state() == QAbstractSocket::ConnectedState) clients.at(x)->tcpSocket()->disconnectFromHost();
    tcpServer->close();
    tcpServer->disconnect(this, SLOT(serverConnectionMade()));
  }
  if (tcpSocket->tcpSocket()->isValid())
  {
    if (tcpSocket->tcpSocket()->state() == QAbstractSocket::ConnectedState) tcpSocket->tcpSocket()->disconnectFromHost();
    tcpSocket->tcpSocket()->abort();
    tcpSocket->tcpSocket()->close();
    tcpSocket->tcpSocket()->disconnect(this, SLOT(displayError(QAbstractSocket::SocketError)));
    tcpSocket->tcpSocket()->disconnect(this, SLOT(clientRead()));
    tcpSocket->tcpSocket()->disconnect(this, SLOT(clientConnected()));
    tcpSocket->tcpSocket()->disconnect(this, SLOT(clientDisconnected()));
    m_HTTP->disconnect(this);
  }
  tcpSocket->setTcp(true);
  emit changeNetworkLabel("Not connected");
}

void CANetwork::ping()
{
  if (isServer())
  {
    for (int x = 0; x < clients.size(); x++)
    {
      QString msg(tr("%1,%2").arg(QString::number(N_PING)).arg(QString::number(x)));
      serverWriteToSocket(clients.at(x), msg);
    }
  }
}

void CANetwork::checkSend()
{
  bool anyActive = false;
  if (!m_Send.isEmpty())
  {
    for (int x = m_Send.size(); x > 0; x--) if (m_Send.at((x-1))->fileName().isEmpty()) delete m_Send.takeAt((x-1));
    for (int x = 0; x < m_Send.size(); x++) if (m_Send.at(x)->timerActive()) anyActive = true;
    if (!anyActive)
    {
      for (int x = 0; x < m_Send.size(); x++)
      {
        if (!m_Send.at(x)->fileName().isEmpty())
        {
          m_Send.at(x)->startSend();
          break;
        }
      }
    }
  }
  else m_sendTimer->stop();
}

bool CANetwork::isListening()
{
  if (tcpServer && tcpServer->isListening()) return true;
  return false;
}

void CANetwork::disconnectUser(int d)
{
  if (d < clients.size())
  {
    if (clients.at(d)->isTcp()) clients.at(d)->tcpSocket()->close();
    else HTTPSend(tr("%1,%2").arg(N_WEB_DISC).arg(clients.at(d)->web()));
  }
}

CASock::CASock(bool tcp, QTcpSocket *sock, qlonglong web)
{
  m_Tcp = tcp;
  m_Sock = sock;
  if (tcp && m_Sock && m_Sock->isValid()) m_Address = m_Sock->peerAddress();
  else m_Address = QHostAddress::Null;
  m_Web = web;
  m_Host = QString("");
  m_Garbage = 0;
}

void CASock::setSock(QTcpSocket *s)
{
  if (m_Sock) delete m_Sock;
  m_Sock = s;
}

void CASock::setHost(QString h)
{
  m_Host = QString(h);
}

void CASock::setError(QString e)
{
  if (!e.isEmpty() && !e.isNull()) m_Error = QString(e);
}

QString CASock::lastError()
{
  QString ret("");
  if (!m_Error.isEmpty() && !m_Error.isNull()) ret = QString(m_Error);
  m_Error.clear();
  return ret;
}

void CASock::addGarbage(int b)
{
  m_Garbage += b;
}
