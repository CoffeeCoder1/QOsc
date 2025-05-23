#include <qoscinterface.h>
#include <QTimer>

QOscInterface::QOscInterface(QObject* parent) :
    QObject(parent)
{

}

QOscInterface::~QOscInterface()
{

}

void QOscInterface::send(const QOscMessage& m)
{
    if(!m.isValid())
        return;

    sendData(m.package());
}

void QOscInterface::send(const QOscBundle& b)
{
    if(!b.isValid())
        return;

    sendData(b.package());
}

void QOscInterface::connect(const QOscMethod::ptr& method)
{
    methods.append(method);
}

void QOscInterface::connect(const QString& addr, QObject* obj, const char* slot)
{
    connect(QOscMethod::ptr(new QOscSlotMethod(addr, obj, slot)));
}

void QOscInterface::disconnect()
{
    methods.clear();
}

void QOscInterface::disconnect(const QString& addr)
{
    for(auto it = methods.begin(); it != methods.end();)
    {
        if((*it)->addr == addr)
            it = methods.erase(it);
        else
            ++it;
    }
}

void QOscInterface::processMessage(const QOscMessage& msg)
{
    for(auto& m : methods)
    {
        if(msg.match(m->addr))
            m->call(msg);
    }
}

void QOscInterface::processBundle(const QOscBundle& b)
{
    auto t = b.time();

    if(t.isNow())
        executeBundle(b);
    else
    {
        qint64 ms = t.toDateTime().toMSecsSinceEpoch();
        qint64 now = QDateTime::currentMSecsSinceEpoch();

        if(ms <= now)
            executeBundle(b);
        else
        {
            auto b2 = b;
            b2.setTime(QOscValue::asap());

            ms -= now;

            QTimer::singleShot(ms, this,
            [this, b2]()
            {
                processBundle(b2);
            });
        }
    }
}

void QOscInterface::executeBundle(const QOscBundle& b)
{
    for(auto& e : b)
        processMessage(e);
}
