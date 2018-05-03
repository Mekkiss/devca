#ifndef IRCNETWORK_H
#define IRCNETWORK_H
#include <IrcSession>

class IrcNetwork : public IrcSession
{
    Q_OBJECT
    Q_PROPERTY(QString channel READ channel WRITE setChannel)
public:
    IrcNetwork();
    QString channel() const;
    void setChannel(const QString& channel);
private slots:
    void onConnected();
    void onMessageReceived(IrcMessage* message);

private:
    QString m_channel;
};

#endif // IRCNETWORK_H
