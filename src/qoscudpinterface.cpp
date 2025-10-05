#include <QBuffer>
#include <QNetworkDatagram>
#include <QNetworkInterface>

#include "qoscudpinterface.h"

QOscUdpInterface::QOscUdpInterface(QObject *parent) :
		QOscInterface(parent) {
	QObject::connect(&socket, &QAbstractSocket::readyRead, this,
			&QOscUdpInterface::readReady);
	rebind();
}

QOscUdpInterface::~QOscUdpInterface() {}

QString QOscUdpInterface::getRemoteAddress() const {
	return remoteAddr.toString();
}

void QOscUdpInterface::setRemoteAddress(const QString &addr) {
	QHostAddress hostAddr(addr);

	if (hostAddr != remoteAddr) {
		remoteAddr = hostAddr;
		emit remoteAddressChanged(addr);
		updateLocalAddr();
	}
}

quint16 QOscUdpInterface::getRemotePort() const { return remotePort; }

void QOscUdpInterface::setRemotePort(quint16 p) {
	if (p != remotePort) {
		remotePort = p;
		emit remotePortChanged(p);
	}
}

QString QOscUdpInterface::getLocalAddress() const {
	return localAddr.toString();
}

quint16 QOscUdpInterface::getLocalPort() const { return localPort; }

void QOscUdpInterface::setLocalPort(quint16 p) {
	if (p != localPort) {
		localPort = p;
		emit localPortChanged(p);
		rebind();
	}
}

bool QOscUdpInterface::getIsListening() const { return isListening; }

void QOscUdpInterface::rebind() {
	if (socket.isValid()) {
		socket.disconnectFromHost();

		if (socket.state() != QAbstractSocket::UnconnectedState)
			socket.waitForDisconnected();
	}

	bool state = socket.bind(localPort);

	if (state != isListening) {
		isListening = state;
		emit isListeningChanged();
	}

	if (socket.localPort() != localPort) {
		localPort = socket.localPort();
		emit localPortChanged(localPort);
	}
}

void QOscUdpInterface::updateLocalAddr() {
	for (auto &iface : QNetworkInterface::allInterfaces()) {
		if (!iface.flags().testFlag(QNetworkInterface::IsUp))
			continue;

		for (auto &entry : iface.addressEntries()) {
			QString addr = QStringLiteral("%1/%2").arg(entry.ip().toString(),
					entry.netmask().toString());
			auto p = QHostAddress::parseSubnet(addr);

			if (remoteAddr.isInSubnet(p)) {
				setLocalAddr(entry.ip().toString());
				return;
			}
		}
	}
}

void QOscUdpInterface::setLocalAddr(const QString &addr) {
	QHostAddress hostAddr(addr);
	if (!hostAddr.isNull() && !hostAddr.isEqual(localAddr)) {
		localAddr = hostAddr;
		emit localAddressChanged(addr);
	}
}

void QOscUdpInterface::sendData(const QByteArray &data) {
	socket.writeDatagram(data, remoteAddr, remotePort);
	emit messageSent();
}

void QOscUdpInterface::readReady() {
	while (socket.hasPendingDatagrams()) {
		auto datagram = socket.receiveDatagram();
		auto data = datagram.data();

		switch (QOsc::detectType(data)) {
			case QOsc::OscMessage: {
				auto msg = QOscMessage::read(data);
				if (msg.isValid()) {
					processMessage(msg);
					emit messageReceived(msg);
				}
				break;
			}

			case QOsc::OscBundle: {
				auto bundle = QOscBundle::read(data);
				if (bundle.isValid()) {
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
