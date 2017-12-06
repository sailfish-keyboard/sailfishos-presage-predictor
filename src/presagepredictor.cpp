#include "presagepredictor.h"

#include <presageException.h>
#include <stdio.h>

#include <QDBusConnection>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QMetaEnum>

PresagePredictor::PresagePredictor(QQuickItem *parent):
    QQuickItem(parent),
    m_presage(nullptr),
    m_engine(new PresagePredictorModel()),
    m_backspacePressed(false)
{
    m_callback = new PresagePredictorCallback(m_predictBuffer);
    try {
        m_presage = new Presage(m_callback);
    } catch (PresageException e) {
        m_presage = nullptr;
        log("Failed to initialize presage");
        return;
    }

    m_presage->config("Presage.Selector.SUGGESTIONS", "6");
    m_presage->config("Presage.Selector.REPEAT_SUGGESTIONS", "yes");

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
    QElapsedTimer dieTimer;
    dieTimer.start();
    while (dieTimer.elapsed() < 100) {
        qApp->processEvents(QEventLoop::AllEvents, 1);
    }
    if (m_presage == nullptr)
        delete m_presage;
}

void PresagePredictor::reset()
{
    log("PresagePredictor::reset");
    m_wordBuffer.clear();
    m_contextBuffer.clear();
    m_engine->setShiftState(PresagePredictorModel::NoShift);
    predict();
}

void PresagePredictor::setContext(const QString & context)
{
    if (m_contextBuffer != context) {
        m_contextBuffer = context;
        predict();
    }
}

void PresagePredictor::predict()
{
    if (m_presage == nullptr)
        return;

    // when the user continously pressing the backspace
    // the predictions would increase lag
    if (m_backspacePressed)
        return;

    log("PresagePredictor::predict");
    log(QString("CTX : %1").arg(m_contextBuffer));
    log(QString("Word: %1").arg(m_wordBuffer));

    m_predictBuffer.str("");
    m_predictBuffer.clear();
    m_predictBuffer << m_contextBuffer.toStdString();
    m_predictBuffer << m_wordBuffer.toStdString();

    m_predictedWords = m_presage->predict();
    log(QString("PresagePredictor::predicted  words count: %1").arg(m_predictedWords.size()));
    QStringList words;
    for(std::vector<std::string>::const_iterator i = m_predictedWords.begin(); i != m_predictedWords.end(); ++i)
        words.append(QString::fromStdString(*i));
    m_engine->reload(words);
}

void PresagePredictor::acceptWord(const QString &word)
{
    if (m_presage == nullptr)
        return;
    log(QString("PresagePredictor::acceptWord(%1);").arg(word));
    m_presage->learn(word.toStdString());
    m_wordBuffer.clear();
}

void PresagePredictor::acceptPrediction(int index)
{
    if (m_presage == nullptr)
        return;
    log(QString("PresagePredictor::acceptPrediction(%1)").arg(index));
    if ((size_t)index < m_predictedWords.size()) {
        m_presage->learn(m_predictedWords[index]);
        m_wordBuffer.clear();
    }
}

void PresagePredictor::processSymbol(const QString &symbol, bool forceAdd)
{
    Q_UNUSED(forceAdd) // TODO consider remove it from QML too
    if (symbol.length()) {
        log(QString("PresagePredictor::processSymbol %1").arg(symbol));
        m_wordBuffer = m_wordBuffer.append(symbol);
        predict();
    }
}

void PresagePredictor::processBackspace()
{
    log(QString("PresagePredictor::processBackspace"));
    if (m_wordBuffer.length() > 0) {
        m_wordBuffer = m_wordBuffer.left(m_wordBuffer.length() - 1);
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
        if (m_backspaceCounter)
            predict();
    }
}

bool PresagePredictor::isLetter(const QString & letter) const
{
    if (letter.length() == 0)
        return false;
    return letter.at(0).isLetter();
}

void PresagePredictor::reactivateWord(const QString &word)
{
    log(QString("PresagePredictor::reactivateWord(%1)").arg(word));
    m_wordBuffer = word;
}

void PresagePredictor::setShiftState(int shiftState)
{
    m_engine->setShiftState((PresagePredictorModel::ShiftState)shiftState);
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
    if (m_presage == nullptr)
        return;

    log(QString("PresagePredictor::setLanguage(%1)").arg(language));
    if (m_language != language) {
        m_language = language;
        QString dbFileName = QString("/usr/share/presage/database_%1.db").arg(language.toLower());
        if (QFileInfo::exists(dbFileName)) {
            try {
                m_presage->config("Presage.Predictors.DefaultSmoothedNgramPredictor.DBFILENAME",  dbFileName.toLatin1().constData());
            } catch (PresageException e) {
                return;
            }

            emit languageChanged();
        }
    }
}

PresagePredictorModel *PresagePredictor::engine() const
{
    return m_engine;
}

QStringList PresagePredictor::predictions() const
{
    return m_predictions;
}

void PresagePredictor::setPredictions(const QStringList &predictions)
{
    m_predictions = predictions;
}

void PresagePredictor::log(const QString &log)
{
    qDebug() << log;
}

void PresagePredictor::clearLearnedWords()
{
    log("PresagePredictor::clear learned words");
}
