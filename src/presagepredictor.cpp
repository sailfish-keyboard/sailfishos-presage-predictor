#include "presagepredictor.h"
#include "presageworker.h"

#include <presageException.h>
#include <stdio.h>

#include <QDBusConnection>
#include <QElapsedTimer>
#include <QMetaEnum>
#include <QMutexLocker>


PresagePredictor::PresagePredictor(QQuickItem *parent):
    QQuickItem(parent),
    m_engine(new PresagePredictorModel(this)),
    m_backspacePressed(false),
    m_shiftState(NoShift)
{
    PresageWorker *worker = new PresageWorker(this);

    worker->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::finished, worker, &QObject::deleteLater);

    // Queued connections are required since we are communicating between different threads and
    // we can emit signal in the routines that have mutex locked.
    connect(this, &PresagePredictor::_setLanguageSignal, worker, &PresageWorker::setLanguage, Qt::QueuedConnection);
    connect(this, &PresagePredictor::_predictSignal, worker, &PresageWorker::predict, Qt::QueuedConnection);
    connect(this, &PresagePredictor::_learnSignal, worker, &PresageWorker::learn, Qt::QueuedConnection);
    connect(this, &PresagePredictor::_forgetSignal, worker, &PresageWorker::forget, Qt::QueuedConnection);
    connect(worker, &PresageWorker::predictedWords, this, &PresagePredictor::onPredictedWords, Qt::QueuedConnection);
    connect(worker, &PresageWorker::languageChanged, this, &PresagePredictor::languageChanged, Qt::QueuedConnection);

    m_workerThread.start();

    // Jolla settings page will call the
    // /com/jolla/keyboard clearData method once the
    // Clear learned word is pressed
    // The notificationmanager is sniff that event
    m_clearDataNotifier = new NotificationManager(this);
    connect(m_clearDataNotifier, &NotificationManager::clearDataRequested,
            this, &PresagePredictor::clearLearnedWords);
}

PresagePredictor::~PresagePredictor()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

void PresagePredictor::reset()
{
    log("PresagePredictor::reset");

    QMutexLocker _(&m_mutex);
    m_wordBuffer.clear();
    m_contextBuffer.clear();

    predict();
}

void PresagePredictor::setContext(const QString & context)
{
    if (m_contextBuffer != context) {
        QMutexLocker _(&m_mutex);
        m_contextBuffer = context;
        predict();
    }
}

void PresagePredictor::predict()
{
    // NB! Must be called WITH the mutex locked from calling
    // method
    //
    // Cannot lock mutex since can be called from
    // methods with mutex locked already. Since we send signal
    // as queued, should not be a problem when communicating
    // with the predictor worker

    // when the user continously pressing the backspace
    // the predictions would increase lag
    if (m_backspacePressed)
        return;

    log("PresagePredictor::predict");
    log(QString("CTX : %1").arg(m_contextBuffer));
    log(QString("Word: %1").arg(m_wordBuffer));

    ++m_prediction_id;
    emit _predictSignal();
}

void PresagePredictor::onPredictedWords(QStringList predictedWords, size_t /*prediction_id*/)
{
    m_predictedWords = predictedWords;

    m_engine->reload(m_predictedWords);

    log(QString("PresagePredictor::predicted  words count: %1").arg(m_predictedWords.size()));
}

bool PresagePredictor::contextStream(size_t &id, QString &language, std::string &buffer, bool force)
{
    QMutexLocker _(&m_mutex);

    if (!force && id == m_prediction_id)
        // last prediction by presage was already done, no need for a new one
        return false;

    // request new prediction
    id = m_prediction_id;
    language = m_language;

    buffer = (m_contextBuffer + m_wordBuffer).toStdString();
    return true;
}


/**
 * @brief PresagePredictor::acceptWord
 *
 * This method is called when a non-symbol characted (space, return, etc.)
 * is pressed after the wordbuffer is non-empty.
 *
 * @param word
 */
void PresagePredictor::acceptWord(const QString &word)
{
    log(QString("PresagePredictor::acceptWord(%1);").arg(word));

    emit _learnSignal(word, m_language);

    if (m_shiftState == ShiftLatchedByWordStart ||
        m_shiftState == ShiftLockedByWordStart) {
        // if we have had prediction capitalization due the wordbuffer contents switch it off
        // since we have finished the word editing
        m_shiftState = NoShift;
        setEngineCapitalization(m_shiftState);
    }

    QMutexLocker _(&m_mutex);
    m_wordBuffer.clear();
    predict();
}

void PresagePredictor::acceptPrediction(int index)
{
    log(QString("PresagePredictor::acceptPrediction(%1)").arg(index));
    if (index < m_predictedWords.size()) {
        {
            QMutexLocker _(&m_mutex);
            m_wordBuffer.clear();
        }

        if (m_shiftState == ShiftLatchedByWordStart ||
            m_shiftState == ShiftLockedByWordStart) {
            m_shiftState = NoShift;
            setEngineCapitalization(m_shiftState);
        }
    }
}

void PresagePredictor::processSymbol(const QString &symbol, bool forceAdd)
{
    Q_UNUSED(forceAdd) // TODO consider remove it from QML too
    if (symbol.length()) {
        log(QString("PresagePredictor::processSymbol %1").arg(symbol));

        QMutexLocker _(&m_mutex);
        m_wordBuffer = m_wordBuffer.append(symbol);

        predict();
    }
}

void PresagePredictor::processBackspace()
{
    log(QString("PresagePredictor::processBackspace"));
    if (m_wordBuffer.length() > 0) {
        QMutexLocker _(&m_mutex);
        m_wordBuffer = m_wordBuffer.left(m_wordBuffer.length() - 1);

        if (m_shiftState == ShiftLockedByWordStart) {
            if (m_wordBuffer.length() == 0) {
                m_engine->setCapitalizationMode((PresagePredictorModel::NonCapital));
                m_shiftState = NoShift;
            }
        }

        predict();
        if (m_backspacePressed)
            m_backspaceCounter++;
        else
            m_backspaceCounter = 0;
        m_backspacePressed = true;
    }
}

void PresagePredictor::processKeyRelease()
{
    if (m_backspacePressed) {
        m_backspacePressed = false;
        if (m_backspaceCounter) {
            QMutexLocker _(&m_mutex);
            predict();
        }
    }
}

bool PresagePredictor::isLetter(const QString & letter) const
{
    if (letter.length() == 0)
        return false;
    return letter.at(0).isLetter();
}

PresagePredictor::ShiftState PresagePredictor::shiftStateFromWordContents(const QString &word)
{
    ShiftState ret = NoShift;
    if (word.length() && word.at(0).isUpper()) {
        // if the specific word's all characters are upper case
        // predict all capital predictions
        if (word.length() > 1) {
            bool allUpper = true;
            for (int i = 1; i<word.length(); i++) {
                if (!word.at(i).isUpper()) {
                    allUpper = false;
                    break;
                }
            }
            ret = allUpper ? ShiftLockedByWordStart : ShiftLatchedByWordStart;
        } else {
            // a single capital letter reactivated
            ret = ShiftLatchedByWordStart;
        }
    }
    return ret;
}

void PresagePredictor::reactivateWord(const QString &word)
{
    log(QString("PresagePredictor::reactivateWord(%1)").arg(word));

    m_shiftState = shiftStateFromWordContents(word);
    setEngineCapitalization(m_shiftState);

    QMutexLocker _(&m_mutex);
    m_wordBuffer = word;
    predict();
}

void PresagePredictor::setEngineCapitalization(const ShiftState shiftState)
{
    switch (shiftState) {
    case NoShift:
        m_engine->setCapitalizationMode(PresagePredictorModel::NonCapital);
        break;
    case ShiftLatchedByWordStart:
    case ShiftLatched:
        m_engine->setCapitalizationMode(PresagePredictorModel::FirstCapital);
        break;
    case ShiftLockedByWordStart:
    case ShiftLocked:
        m_engine->setCapitalizationMode(PresagePredictorModel::AllCapital);
        break;
    }
}

void PresagePredictor::setShiftState(ShiftState shiftState)
{
    qDebug() << "PresagePredictor::setShiftState(" << QMetaEnum::fromType<ShiftState>().valueToKey(shiftState) << ")";
    if (m_shiftState != shiftState) {
        if (m_shiftState == ShiftLatched && shiftState == NoShift &&
            m_wordBuffer.length() == 1  && m_wordBuffer.at(0).isUpper()) {
            // when starting a word with latched shift with capital letter
            // sink the shiftchange to noncapital and fool the model with firstcapital mode
            m_shiftState = ShiftLatchedByWordStart;
        } else {
            m_shiftState = shiftState;
        }
        setEngineCapitalization(m_shiftState);
    }
}

void PresagePredictor::startLayout(int width, int height)
{
    Q_UNUSED(width)
    Q_UNUSED(height)
}

void PresagePredictor::addLayoutButton(int x, int y, int width, int height, const QString &buttonText, const QString &buttonTextShifted)
{
    Q_UNUSED(x)
    Q_UNUSED(y)
    Q_UNUSED(width)
    Q_UNUSED(height)
    Q_UNUSED(buttonText)
    Q_UNUSED(buttonTextShifted)
}

QString PresagePredictor::language() const
{
    return m_language;
}

void PresagePredictor::setLanguage(const QString &language)
{
    m_language = language;
    emit _setLanguageSignal(language);
}

void PresagePredictor::forget(QString word)
{
    emit _forgetSignal(word, m_language);
}

PresagePredictorModel *PresagePredictor::engine() const
{
    return m_engine;
}

void PresagePredictor::log(const QString &log)
{
    qDebug() << log;
}

void PresagePredictor::clearLearnedWords()
{
    log("PresagePredictor::clear learned words");
}
