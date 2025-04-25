#ifndef QOSCINTERFACE_H
#define QOSCINTERFACE_H

#include "qoscmethod.h"
#include "qoscmessage.h"
#include "qoscbundle.h"
#include <QHostAddress>

class QOSC_EXPORT QOscInterface :  public QObject
{
    Q_OBJECT

public:
    QOscInterface(QObject* parent = nullptr);
    ~QOscInterface() override;

    template<class T>
    void send(const QString& pattern, const T& arg)
    {
        QOscMessage msg(pattern, arg);
        send(msg);
    }

    void connect(const QString& addr, QObject* obj, const char* slot);

    template<class Func>
    void connect(const QString& addr, Func f)
    {
            connect(QOscMethod::ptr(new QOscLambdaMethod<Func>(addr, f)));
    }

    void disconnect();
    void disconnect(const QString& addr);

public slots:
    void send(const QOscMessage& m);
    void send(const QOscBundle& b);

signals:
    void messageReceived(const QOscMessage& msg);
    void bundleReceived(const QOscBundle& bundle);

    void messageSent();

protected:
    void processMessage(const QOscMessage& msg);

    void processBundle(const QOscBundle& b);
    void executeBundle(const QOscBundle& b);

    virtual void sendData(const QByteArray& data) = 0;

private:
    void connect(const QOscMethod::ptr& method);

    QList<QOscMethod::ptr> methods;
};

#endif // QOSCINTERFACE_H
