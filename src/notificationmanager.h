#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QtDBus/QDBusContext>

#include <QDBusInterface>
#include <QDBusPendingCallWatcher>

typedef QHash<QString, QString> QStringHash;

class NotificationManager : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.jolla.keyboard")

    Q_PROPERTY(QDBusInterface* interface READ interface)
public:
    explicit NotificationManager(QObject *parent = 0);
            ~NotificationManager();

    QDBusInterface* interface() const;

Q_SIGNALS:
    void clearDataRequested();

public Q_SLOTS:
    uint clearData();

protected Q_SLOTS:
    void initialize();

private:
    class NotificationManagerPrivate *d_ptr;

    Q_DISABLE_COPY(NotificationManager)
    Q_DECLARE_PRIVATE(NotificationManager)
};

#endif // NOTIFICATIONMANAGER_H
