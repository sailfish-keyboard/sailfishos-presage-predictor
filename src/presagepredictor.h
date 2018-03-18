#ifndef PRESAGEPREDICTOR_H
#define PRESAGEPREDICTOR_H

#include <QAbstractItemModel>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QThread>

#include <QtQuick/QQuickItem>

#include <string>

#include "presagepredictormodel.h"

class PresagePredictorModel; // forward declaration


/// \brief Presage predictor interface connecting requests with the predictor
///
/// The object of this class links the GUI with Presage working thread by
/// forwarding perdiction requests and replies between the prediction engine
/// and GUI. For that, Presage worker thread is created in the constructor as
/// well as presage prediction engine wrapper with the wrapper (PresageWorker)
/// moved into the working thread. Later communication between PresageWorker
/// and this class object is done through queued signals. For each prediction
/// request, language change, learn request, a signal is emitted by this object
/// and received by PresageWorker. Response from PresageWorker is received by
/// other queued signal. This allows to handle predictions in asynchrounous manner
/// and should allow to provide predictions without noticable lag for the user
/// during interaction with the keyboard.
///
/// To ensure that the most relevant prediction is provided by the prediction engine,
/// this object holds the latest context that is asked for by the prediction
/// engine before finding the prediction. This allows the predictor to catch up with
/// fast input by skipping the prediction requests that are not up-to-date anymore.
/// For that, PresagePredictor keeps request id (m_prediction_id) and provides
/// an interface to ask for the latest prediction context (contextStream). Since
/// the context is asked for in the other thread, one has to protect changes/requests for
/// the involved variables by mutex. In addition, predict() method emitting signal
/// requesting the prediction and updating m_prediction_id, has to be called by the method
/// which keeps the mutex locked (no internal locking is done in predict() to avoid
/// using recursive mutexes).
///
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

    /// \brief Instruct presage to forget a word from the predictors with active learning
    ///
    /// Presage is actively learning new words and, if committed, typos will be remembered
    /// as well. This method allows to remove a specified word from user databases. Note that
    /// it will not remove words that are used by predictors which are working without learning.
    /// This is assuming that read-only databases are system-provided
    Q_INVOKABLE void forget(QString word);

    QString language() const;
    void setLanguage(const QString &language);

    /// \brief Provide current context for prediction
    ///
    /// The conext is provided if there is a newer prediction requested than the
    /// prediction done by PresageWorker the last time (as indicated by input value
    /// of id). Fills id, language, and buffer with the new values if the prediction
    /// id requested. Context can be requested regardless of whether the prediction is
    /// expected by setting force to true. Such enforcing is used when prediction can
    /// be changed due to the changes in parameters which are not followed by id directly
    /// (forgotten words in the databases, for example). Whether new prediction is
    /// needed is indicated by the return value (set to true if prediction is requested or
    /// forced).
    ///
    /// \param id On input, the last prediction provided by the caller. On output, set id to the requested one
    /// \param language On output: Language for the requested prediction
    /// \param buffer On output: Context for the prediction (past stream)
    /// \param force If true, return context even if the prediction is not expected for given id
    /// \return true if new request is asked for; false if there is no new request
    bool contextStream(size_t &id, QString &language, std::string &buffer, bool force);

    PresagePredictorModel *engine() const;

protected:

    void predict(); ///< Request new prediction. NB! Call only with the locked mutex

    ShiftState shiftStateFromWordContents(const QString &word);

    void setEngineCapitalization(const ShiftState shiftState);
    void log(const QString &log);


private:
    QThread m_workerThread;
    QMutex m_mutex;              ///< Protect access from worker and GUI threads

    PresagePredictorModel *m_engine;

    QString m_language;
    size_t m_prediction_id{0}; ///< Incremented for each prediction asked from presage

    QStringList m_predictedWords;
    QString m_contextBuffer;
    QString m_wordBuffer;
    bool m_backspacePressed;
    int m_backspaceCounter;
    ShiftState m_shiftState;

private slots:
    void onPredictedWords(QStringList predictedWords, size_t prediction_id);

signals:
    void languageChanged();
    void engineChanged();

    // signals used to communicate with the worker thread
    // don't use for external communication, used internally
    void _predictSignal();                              ///< ask worker for prediction
    void _setLanguageSignal(QString language);          ///< propagate language change to worker
    void _forgetSignal(QString word, QString language); ///< forget a given word
};

Q_DECLARE_METATYPE(PresagePredictor::ShiftState)

#endif // PRESAGEPREDICTOR_H
