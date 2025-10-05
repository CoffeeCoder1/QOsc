#ifndef QOSCUDPINTERFACE_H
#define QOSCUDPINTERFACE_H

#include "qoscinterface.h"
#include <QUdpSocket>

class QOSC_EXPORT QOscUdpInterface :  public QOscInterface
{
	Q_OBJECT
	
	Q_PROPERTY(QString remoteAddr READ getRemoteAddress WRITE setRemoteAddress NOTIFY remoteAddressChanged)
	Q_PROPERTY(quint16 remotePort READ getRemotePort WRITE setRemotePort NOTIFY remotePortChanged)
	Q_PROPERTY(QString localAddr READ getLocalAddress NOTIFY localAddressChanged)
	Q_PROPERTY(quint16 localPort READ getLocalPort WRITE setLocalPort NOTIFY localPortChanged)
	Q_PROPERTY(bool isListening READ getIsListening NOTIFY isListeningChanged)
	
public:
	QOscUdpInterface(QObject* parent = nullptr);
	~QOscUdpInterface() override;
	
	QString getRemoteAddress() const;
	void setRemoteAddress(const QString& addr);
	
	quint16 getRemotePort() const;
	void setRemotePort(quint16 p);
	
	QString getLocalAddress() const;
	
	quint16 getLocalPort() const;
	void setLocalPort(quint16 p);
	
	bool getIsListening() const;
	
signals:
	void remoteAddressChanged(const QString& addr);
	void remotePortChanged(quint16 port);
	
	void localAddressChanged(const QString& addr);
	void localPortChanged(quint16 port);
	
	void isListeningChanged();

protected:
	void sendData(const QByteArray& data) override;
	
private:
	void rebind();
	void updateLocalAddr();
	void setLocalAddr(const QString& addr);

	QHostAddress remoteAddr = QHostAddress("127.0.0.1");
	quint16      remotePort = 0;
	QHostAddress localAddr  = QHostAddress("127.0.0.1");
	quint16      localPort  = 0;
	QUdpSocket   socket;

	bool isListening = false;
	
private slots:
	void readReady();
};

#endif // QOSCUDPINTERFACE_H


