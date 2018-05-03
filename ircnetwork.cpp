#include "ircnetwork.h"
#include <IrcCommand>
#include <IrcMessage>

IrcNetwork::IrcNetwork()
{
    connect(this, SIGNAL(connected()), this, SLOT(onConnected()));
    //connect(this, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
}

QString IrcNetwork::channel() const
{
    return m_channel;
}
void IrcNetwork::setChannel(const QString& channel)
{
    m_channel = channel;
}

void IrcNetwork::onConnected()
{
    sendCommand(IrcCommand::createJoin(m_channel));
}

void IrcNetwork::onMessageReceived(IrcMessage* message)
{
    if (message->type() == IrcMessage::Private) {
        IrcPrivateMessage* msg = static_cast<IrcPrivateMessage*>(message);

        if (!msg->target().compare(nickName(), Qt::CaseInsensitive)) {
            // echo private message
            sendCommand(IrcCommand::createMessage(msg->sender().name(), msg->message()));
        }
    }
}
