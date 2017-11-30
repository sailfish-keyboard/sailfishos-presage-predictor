#ifndef PRESAGEPREDICTOR_H
#define PRESAGEPREDICTOR_H

#include <QAbstractItemModel>
#include <QDBusConnection>
#include <QString>
#include <QObject>
#include <QtQuick/QQuickItem>

#include <string.h>
#include <presage.h>

#include "notificationmanager.h"


/** Callback object for presage demo application.
 *
 * We need to provide a callback class to allow presage to query the
 * application's text buffer. In a real world application, this would
 * fetch the text from whatever object stores the text composition
 * (i.e. a GUI widget in a graphical interface)
 *
 * For the purpose of this demonstration program, the callback class
 * will retrieve contextual data from a standard stringstream object.
 *
 */
class PresagePredictorCallback : public PresageCallback {
public:
    PresagePredictorCallback(std::stringstream& buffer) : m_buffer(buffer) { }

    std::string get_past_stream() const { return m_buffer.str(); }
    std::string get_future_stream() const { return empty; }

private:
    std::stringstream& m_buffer;
    const std::string empty;

};

class PresagePredictorModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum PredictionRoles {
        IndexRole = Qt::UserRole + 1,
        TextRole
    };

    PresagePredictorModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QHash<int, QByteArray> roleNames() const;
    void reload();

private:
    QHash<int, QByteArray> m_roles;
signals:
    void predictionsChanged();
};

class PresagePredictor : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(PresagePredictor)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(PresagePredictorModel* engine READ engine NOTIFY engineChanged)
public:
    PresagePredictor(QQuickItem *parent = 0);
    ~PresagePredictor();

    // interface for the Xt9 based one
    Q_INVOKABLE void reset();
    Q_INVOKABLE void setContext(const QString &context);
    Q_INVOKABLE void acceptWord(const QString &context);
    Q_INVOKABLE void acceptPrediction(int index);
    Q_INVOKABLE void processSymbol(const QString &symbol, bool forceAdd);
    Q_INVOKABLE void processBackspace();
    Q_INVOKABLE bool isLetter(const QString & letter) const;
    Q_INVOKABLE void reactivateWord(const QString & word);

    Q_INVOKABLE void startLayout(int width, int height);
    Q_INVOKABLE void addLayoutButton(int x, int y, int width, int height, const QString &buttonText, const QString &buttonTextShifted);
    Q_INVOKABLE void finishLayout() {}

    QString language() const;
    void setLanguage(const QString &language);

    PresagePredictorModel *engine() const;

    QStringList predictions() const;
    void setPredictions(const QStringList &predictions);

    void predict();

    Q_INVOKABLE void setFirstLetterCapitalized(bool firstLetterCapitalized);

private:
    PresageCallback* m_callback;
    Presage *m_presage;
    NotificationManager *m_clearDataNotifier;

    QString m_language;
    PresagePredictorModel *m_engine;
    QStringList m_predictions;
    void log(const QString &log);

    bool m_firstLetterCapitalized;

private slots:
    void clearLearnedWords();

signals:
    void languageChanged();
    void engineChanged();
};

#endif // PRESAGEPREDICTOR_H
