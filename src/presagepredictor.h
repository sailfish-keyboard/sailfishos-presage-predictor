#ifndef PRESAGEPREDICTOR_H
#define PRESAGEPREDICTOR_H

#include <QAbstractItemModel>
#include <QDBusConnection>
#include <QString>
#include <QObject>
#include <QtQuick/QQuickItem>

#include <string.h>
#include <sstream>
#include <presage.h>

#include "notificationmanager.h"
#include "presagepredictormodel.h"

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

class PresagePredictorModel; // forward declaration

class PresagePredictor : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(PresagePredictor)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(PresagePredictorModel* engine READ engine NOTIFY engineChanged)
public:
    enum ShiftState {
        NoShift,
        ShiftLocked,
        ShiftLatched,
        ShiftLockedByWordStart,
        ShiftLatchedByWordStart
    };
    Q_ENUM(ShiftState)

    PresagePredictor(QQuickItem *parent = 0);
    ~PresagePredictor();

    // interface for the Xt9 based one
    Q_INVOKABLE void reset();
    Q_INVOKABLE void setContext(const QString &context);
    Q_INVOKABLE void acceptWord(const QString &word);
    Q_INVOKABLE void acceptPrediction(int index);
    Q_INVOKABLE void processSymbol(const QString &symbol, bool forceAdd);
    Q_INVOKABLE void processBackspace();
    Q_INVOKABLE void processKeyRelease();
    Q_INVOKABLE bool isLetter(const QString & letter) const;
    Q_INVOKABLE void reactivateWord(const QString & word);
    Q_INVOKABLE void setShiftState(ShiftState shiftState);

    Q_INVOKABLE void startLayout(int width, int height);
    Q_INVOKABLE void addLayoutButton(int x, int y, int width, int height, const QString &buttonText, const QString &buttonTextShifted);
    Q_INVOKABLE void finishLayout() {}

    QString language() const;
    void setLanguage(const QString &language);

    PresagePredictorModel *engine() const;

    QStringList predictions() const;
    void setPredictions(const QStringList &predictions);

    void predict();

    ShiftState shiftStateFromWordContents(const QString &word);


private:
    PresageCallback* m_callback;
    Presage *m_presage;
    NotificationManager *m_clearDataNotifier;

    QString m_language;
    PresagePredictorModel *m_engine;
    QStringList m_predictions;
    void log(const QString &log);
    QString m_contextBuffer;
    QString m_wordBuffer;
    bool m_backspacePressed;
    int m_backspaceCounter;
    ShiftState m_shiftState;

    std::stringstream m_predictBuffer;
    std::vector<std::string> m_predictedWords;
    void setEngineCapitalization(const ShiftState shiftState);

    bool m_presageInitialized;

private slots:
    void clearLearnedWords();

signals:
    void languageChanged();
    void engineChanged();
};
Q_DECLARE_METATYPE(PresagePredictor::ShiftState)
#endif // PRESAGEPREDICTOR_H
