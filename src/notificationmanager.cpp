#include "notificationmanager.h"

#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QSettings>
#include <QDBusInterface>
#include <QDBusPendingReply>

class NotificationManagerPrivate
{
    Q_DECLARE_PUBLIC(NotificationManager)

public:
    NotificationManagerPrivate(NotificationManager *q)
        : q_ptr(q),
          interface(nullptr),
          connected(false)
    { /*...*/ }

    NotificationManager *q_ptr;

    QDBusInterface *interface;

    bool connected;
};

NotificationManager::NotificationManager(QObject *parent)
    : QObject(parent), d_ptr(new NotificationManagerPrivate(this))
{
    Q_D(NotificationManager);
    QDBusConnection::sessionBus().registerObject("/com/jolla/keyboard", this, QDBusConnection::ExportAllSlots);

    d->interface = new QDBusInterface("org.freedesktop.DBus",
                                      "/org/freedesktop/DBus",
                                      "org.freedesktop.DBus");

    d->interface->call("AddMatch", "interface='com.jolla.keyboard',member='clearData',type='method_call',eavesdrop='true'");

    this->initialize();
}

NotificationManager::~NotificationManager()
{
    Q_D(NotificationManager);
    delete d;
}


void NotificationManager::initialize()
{
    Q_D(NotificationManager);
    bool success = false;

    if(d->interface->isValid())
    {
        success = true;
        qDebug() << "Connected to Notifications D-Bus service. ";
    }

    if(!(d->connected = success))
    {
        QTimer::singleShot(2000, this, SLOT(initialize()));
        qDebug() << "Failed to connect to Notifications D-Bus service.";
    }
}

QDBusInterface* NotificationManager::interface() const
{
    Q_D(const NotificationManager);
    return d->interface;
}

uint NotificationManager::clearData()
{
    qWarning() << "clearData";
    emit clearDataRequested();
    return 0;
}
