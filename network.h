#ifndef NETWORK_H
#define NETWORK_H

#include <QString>
#include <QPointer>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QColor>
#include <QPen>
#include "ircnetwork.h"
#include "qgridtab.h"
#include <IrcMessage>
#include "connectdialog.h"


enum { N_PING = 1, N_PONG,
       N_REQUEST_USERNAME = 100, N_PROVIDE_USERNAME,
       N_CIRCLE_DRAWN = 200, N_BLOCK_DRAWN, N_CROSS_DRAWN, N_TRIANGLE_DRAWN, N_ITEM_REMOVED, N_DRAG_START, N_DRAG_FINISH, N_TAB_ADD, N_TAB_REMOVE, N_GRID_REDRAW, N_GRID_CLEAR,
       N_GRID_RESYNC, N_COLOUR_BG, N_COLOUR_LN, N_COLOUR_CO, N_PIXMAP_BG, N_REQUEST_IMG, N_RECV_IMG, N_SOUND, N_REQUEST_SOUND, N_RECV_SOUND, N_STOP_SOUND, N_LINE_DRAWN,
       N_ITEM_REMOVED_CHECK, N_CAITEM_DRAWN, N_BG_OFFSET, N_STYLE_LN, N_TAB_RENAME, N_CANCEL_IMG, N_IMG_WEB,
       N_WEB_SERVER = 300, N_WEB_CLIENT, N_WEB_MSG, N_WEB_INFO, N_WEB_ERROR, N_WEB_SEND, N_WEB_SENDALL, N_WEB_FAIL, N_WEB_DISC, N_WEB_HASIMG, N_WEB_NOIMG, N_WEB_GETIMG };

class CABackground;
class CASendFile;
class QGridTab;
class CAGraphicsItem;
class CASock;

class CANetwork : public QObject
{
  Q_OBJECT
public:
  CANetwork(QGridTab *t, QObject* p);
  ~CANetwork();
  void processConnectDialog(QString u, QString a, quint16 p, int r, bool w);
  void processConnectDialog(ConnectDialog *cd);
  void cancelDownload();
  bool client() {return m_Client;}
  int numClients();
  void networkDisconnect();
  QList<QString> clientNames() {return m_ClientNames;}
  QList<CASock*> clientList() {return clients;}
  bool isListening();
  void HTTPSend(QString m);
  void serverWrite(QString msg);
  void clientWrite(QString msg);
  void serverWriteToUser(QString u, QString msg);
  void serverWriteToSocket(CASock *sock, QString msg);
  void sendImage(QString chx);

private:
  bool m_Client;
  QPointer<QObject> m_Parent;
  QGridTab *tabWidget;
  QTimer *m_serverTimer;
  QTimer *m_webTimer;
  QTimer *m_sendTimer;
  QList<CASendFile*> m_Send;
  QPointer<QTcpServer> tcpServer;
  CASock* tcpSocket;
  QList<CASock*> clients;
  QList<CASock*> oldClients;
  QList<QString> m_ClientNames;
  QString userName;
  QList<quint16> clientBlockSize;
  QList<QString> clientCurrentReceive;
  QQueue<QByteArray> msgQueue;
  quint16 blockSize;
  QString currentReceive;
  QString m_Host;
  QStringList webBuffer;
  QNetworkAccessManager *m_HTTP;
  bool isServer();
  bool playSound(QString chx);
  void clientParseReceive(QString r);
  void serverParseReceive(QString r, CASock *sock);
  int hasWebClient(qlonglong web);
  QList<QHostAddress> m_Banlist;
  void banUser(int d);
  bool useIRC;
  bool m_ircGM;

  IrcNetwork *m_irc;
  QString m_GMNick;

signals:
  void clientChange(QList<QString> clientNames, QList<CASock*> clients);
  void changeNetworkLabel(QString txt);
  void clientConnected(bool c);
  void updateProgress(qint64 r, qint64 t);

private slots:
  void displayError(QAbstractSocket::SocketError socketError);
  void serverConnectionMade();
  void clientRead();
  void serverRead();
  void clientConnected();
  void clientDisconnected();
  void serverClientDisconnected();
  void HTTPServer(QNetworkReply *reply);
  void HTTPClient(QNetworkReply *reply);
  void webMsg();

  void ircClientParse(IrcMessage* message);

  void ircServerParse(IrcMessage* message);

public slots:
  void circleDrawn(int p, int x, int y, QColor c, int size, QString letter, CASock *sock = NULL);
  void blockDrawn(int p, int x, int y, QColor c, QString letter, CASock *sock = NULL);
  void crossDrawn(int p, int x, int y, QColor c, CASock *sock = NULL);
  void triangleDrawn(int p, int x, int y, QColor c, int facing, CASock *sock = NULL);
  void lineDrawn(int p, double x1, double y1, double x2, double y2, QPen pen, CASock *sock = NULL);
  void itemRemoved(int p, int x, int y);
  void itemRemoved(int p, quint16 check);
  void startDrag(int p, int x, int y);
  void finishDrag(int p, int x, int y);
  void renameTab(QString txt, int p = -1, CASock *sock = NULL);
  void tabAdded(int w = 20, int h = 20, int b = 20, QColor bg = Qt::black, QColor ln = Qt::white, QColor co = Qt::yellow, int l = Qt::DotLine, CASock *sock = NULL);
  void tabRemoved(int p, CASock *sock = NULL);
  void changeBackground(CABackground *b, int p = -1, CASock *sock = NULL);
  void changeBackgroundColour(QColor bg, int p = -1, CASock *sock = NULL);
  void changeLineColour(QColor ln, int p = -1, CASock *sock = NULL);
  void changeLineStyle(int l, int p = -1, CASock *sock = NULL);
  void changeCoordinateColour(QColor co, int p = -1, CASock *sock = NULL);
  void redrawGrid(int p = -1, int w = 20, int h = 20, int b = 20, QColor bg = Qt::black, QColor ln = Qt::white, QColor co = Qt::yellow, int l = Qt::DotLine, CASock *sock = NULL);
  void clearGrid(int p, bool total = false);
  void resync(CASock *sock = NULL);
  void ping();
  void checkSend();
  void disconnectUser(int d);
  void soundToClients(QString chx);
  void stopSoundOnClients();
  void caItemDrawn(int p, CAGraphicsItem * i, CASock *sock = NULL);
  void backgroundOffset(int p, int x, int y, CASock *sock = NULL);
  void updateDownload(qint64 r, qint64 t);
};

class CASendFile : public QObject
{
  Q_OBJECT

public:
  CASendFile(CANetwork *parent, int type, QString file, CASock *sock, int p = -1, int x = 0, int y = 0);
  ~CASendFile();
  bool timerActive();
  void cancelSend();
  void startSend();
  QString fileName();
  CASock *getSock() {return m_Sock;}

private:
  int m_Type;
  QFile m_File;
  QString m_Chx;
  QTimer *m_Timer;
  CASock *m_Sock;
  CANetwork *m_Parent;
  int m_Chunk;
  int m_p;
  int m_x;
  int m_y;
  void stopSend();
  //void serverWriteToSocket(QString msg);

private slots:
  void sendChunk();
};

class CASock
{
public:
  CASock(bool tcp, QTcpSocket *sock = NULL, qlonglong web = 0);
  bool isTcp() {return m_Tcp;}
  void setSock(QTcpSocket *s);
  QTcpSocket *tcpSocket() {return m_Sock;}
  qlonglong web() {return m_Web;}
  QString host() {return m_Host;}
  void setHost(QString h);
  void setWeb(qlonglong w) {m_Web = w;}
  void setTcp(bool t) {m_Tcp = t;}
  void setError(QString e);
  QString lastError();
  void addGarbage(int b);
  qlonglong garbage() {return m_Garbage;}
  QHostAddress address() {return m_Address;}

private:
  bool m_Tcp;
  QTcpSocket *m_Sock;
  qlonglong m_Web;
  QString m_Host;
  QString m_Error;
  qlonglong m_Garbage;
  QHostAddress m_Address;
};

#endif // NETWORK_H
