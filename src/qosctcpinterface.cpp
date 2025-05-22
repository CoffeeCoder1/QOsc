#include <qosctcpinterface.h>
#include <QBuffer>
#include <QNetworkDatagram>
#include <QNetworkInterface>

QOscTcpInterface::QOscTcpInterface(QObject* parent) :
    QOscInterface(parent)
{
    QObject::connect(&socket, &QAbstractSocket::readyRead,
                     this,    &QOscTcpInterface::readReady);
    QObject::connect(&socket, &QAbstractSocket::connected,
                     this,    &QOscTcpInterface::connected);
    rebind();
}

QOscTcpInterface::~QOscTcpInterface()
{

}

QString QOscTcpInterface::getRemoteAddress() const
{
    return remoteAddr.toString();
}

void QOscTcpInterface::setRemoteAddress(const QString& addr)
{
    QHostAddress hostAddr(addr);

    if(hostAddr != remoteAddr)
    {
        remoteAddr = hostAddr;
        emit remoteAddressChanged(addr);
        rebind();
    }
}

quint16 QOscTcpInterface::getRemotePort() const
{
    return remotePort;
}

void QOscTcpInterface::setRemotePort(quint16 p)
{
    if(p != remotePort)
    {
        remotePort = p;
        emit remotePortChanged(p);
        rebind();
    }
}

void QOscTcpInterface::rebind()
{
    if(socket.isValid())
    {
        socket.disconnectFromHost();

        if(socket.state() != QAbstractSocket::UnconnectedState) {
            socket.waitForDisconnected();
        }
    }

    socket.connectToHost(remoteAddr, remotePort);
}

void QOscTcpInterface::sendData(const QByteArray& data)
{
    // Find the packet size
    qint32 packetSize = data.size();
    QByteArray packetSizeArray;
    packetSizeArray.append(packetSize);

    // Create a buffer to hold the packet's data
    QBuffer b;
    b.open(QIODevice::WriteOnly);

    // Write the packet size
    b.write(QByteArray().fill(0, 4 - packetSizeArray.size()));
    b.write(packetSizeArray);

    // Write the data itself
    b.write(data);
    socket.write(b.data());
    emit messageSent();
}

void QOscTcpInterface::readReady()
{
    while(socket.bytesAvailable())
    {
        qint64 bytes = socket.read(1).at(0);
        QByteArray data = socket.read(bytes);

        switch(QOsc::detectType(data))
        {
        case QOsc::OscMessage:
        {
            auto msg = QOscMessage::read(data);
            if(msg.isValid())
            {
                processMessage(msg);
                emit messageReceived(msg);
            }
            break;
        }

        case QOsc::OscBundle:
        {
            auto bundle = QOscBundle::read(data);
            if(bundle.isValid())
            {
                processBundle(bundle);
                emit bundleReceived(bundle);
            }
            break;
        }

        default:
            break;
        }
    }
}
