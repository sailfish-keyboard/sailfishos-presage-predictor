#include "presagepredictor.h"

#include <presageException.h>
#include <stdio.h>

#include <QDBusConnection>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QMetaEnum>
#include <QStandardPaths>


PresagePredictor::PresagePredictor(QQuickItem *parent):
    QQuickItem(parent),
    m_callback(nullptr),
    m_presage(nullptr),
    m_engine(new PresagePredictorModel(this)),
    m_backspacePressed(false),
    m_shiftState(NoShift),
    m_presageInitialized(false)
{
    m_callback = new PresagePredictorCallback(m_predictBuffer);
    try {
        m_presage = new Presage(m_callback);
    } catch (PresageException e) {
        m_presage = nullptr;
        log(QString("Failed to initialize presage: ") + e.what());
        return;
    }

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
    if (m_presage != nullptr)
        delete m_presage;
    if (m_callback != nullptr)
        delete m_callback;
}

void PresagePredictor::reset()
{
    log("PresagePredictor::reset");
    m_wordBuffer.clear();
    m_contextBuffer.clear();
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
    if (m_presage == nullptr || !m_presageInitialized)
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
    if (m_presage == nullptr)
        return;
    log(QString("PresagePredictor::acceptWord(%1);").arg(word));
    m_presage->learn(word.toStdString());
    if (m_shiftState == ShiftLatchedByWordStart ||
        m_shiftState == ShiftLockedByWordStart) {
        // if we have had prediction capitalization due the wordbuffer contents switch it off
        // since we have finished the word editing
        m_shiftState = NoShift;
        setEngineCapitalization(m_shiftState);
    }
    m_wordBuffer.clear();
    predict();
}

void PresagePredictor::acceptPrediction(int index)
{
    if (m_presage == nullptr)
        return;
    log(QString("PresagePredictor::acceptPrediction(%1)").arg(index));
    if ((size_t)index < m_predictedWords.size()) {
        m_wordBuffer.clear();

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
        m_wordBuffer = m_wordBuffer.append(symbol);
        predict();
    }
}

void PresagePredictor::processBackspace()
{
    log(QString("PresagePredictor::processBackspace"));
    if (m_wordBuffer.length() > 0) {
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
                qDebug() << e.what();
                return;
            }

            QString userdb =
                QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
                QString("/presage/lm_%1.db").arg(language.toLower());
            m_presage->config("Presage.Predictors.UserSmoothedNgramPredictor.DBFILENAME",
                              userdb.toLatin1().constData());
            m_presageInitialized = true;
            emit languageChanged();
        } else {
            m_presageInitialized = false;
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
