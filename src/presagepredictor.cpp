#include "presagepredictor.h"

#include <stdio.h>

#include <QDBusConnection>
#include <QElapsedTimer>
#include <QFileInfo>

// globals
std::string       suggestions;
std::string       config;
std::stringstream predictBuffer;

std::vector<std::string> predictedWords;

PresagePredictor::PresagePredictor(QQuickItem *parent):
    QQuickItem(parent),
    m_engine(new PresagePredictorModel()),
    m_backspacePressed(false)
{
    // magic starts here
    m_callback = new PresagePredictorCallback(predictBuffer);
    m_presage = new Presage(m_callback, config);

    m_presage->config("Presage.Selector.SUGGESTIONS", "6");
    m_presage->config("Presage.Selector.REPEAT_SUGGESTIONS", "yes");

    // Jolla settings page will call the
    // /com/jolla/keyboard clearData method once the
    // Clear learned word is pressed
    // The notificationmanager is sniff that event
    m_clearDataNotifier = new NotificationManager(this);
    connect(m_clearDataNotifier, &NotificationManager::clearDataRequested,
            this, &PresagePredictor::clearLearnedWords);

    m_engine->setCapitalizationMode(PresagePredictorModel::NonCapitalized);
}

PresagePredictor::~PresagePredictor()
{
    QElapsedTimer dieTimer;
    dieTimer.start();
    while (dieTimer.elapsed() < 100) {
        qApp->processEvents(QEventLoop::AllEvents, 1);
    }
    delete m_presage;
}

void PresagePredictor::reset()
{
    log("PresagePredictor::reset");
    m_wordBuffer.clear();
    m_contextBuffer.clear();
    m_engine->setCapitalizationMode(PresagePredictorModel::NonCapitalized);
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
    // when the user continously pressing the backspace
    // the predictions would increase lag
    if (m_backspacePressed)
        return;

    log("PresagePredictor::predict");
    log(QString("CTX : %1").arg(m_contextBuffer));
    log(QString("Word: %1").arg(m_wordBuffer));

    predictBuffer.str("");
    predictBuffer.clear();
    predictBuffer << m_contextBuffer.toStdString();
    predictBuffer << m_wordBuffer.toStdString();

    predictedWords = m_presage->predict();
    log(QString("PresagePredictor::predicted  words count: %1").arg(predictedWords.size()));
    m_engine->reload();
}

void PresagePredictor::acceptWord(const QString &word)
{
    log(QString("PresagePredictor::acceptWord(%1);").arg(word));
    m_presage->learn(word.toStdString());
    m_wordBuffer.clear();
    m_engine->setCapitalizationMode(PresagePredictorModel::NonCapitalized);
}

void PresagePredictor::acceptPrediction(int index)
{
    log(QString("PresagePredictor::acceptPrediction(%1)").arg(index));
    if ((size_t)index < predictedWords.size()) {
        m_presage->learn(predictedWords[index]);
        m_wordBuffer.clear();
        m_engine->setCapitalizationMode(PresagePredictorModel::NonCapitalized);
    }
}

void PresagePredictor::processSymbol(const QString &symbol, bool forceAdd)
{
    Q_UNUSED(forceAdd) // TODO consider remove it from QML too
    if (symbol.length()) {
        log(QString("PresagePredictor::processSymbol %1").arg(symbol));
        if (m_wordBuffer.length() == 0) {
            if (symbol.at(0).isUpper()) {
                m_engine->setCapitalizationMode(PresagePredictorModel::FirstLetterCapitalized);
            }
        }
        m_wordBuffer = m_wordBuffer.append(symbol);
        predict();
    }
}

void PresagePredictor::processBackspace()
{
    log(QString("PresagePredictor::processBackspace"));
    if (m_wordBuffer.length() > 0) {
        m_wordBuffer = m_wordBuffer.left(m_wordBuffer.length() - 1);
        if (m_wordBuffer.length() == 0) {
            // we have just erased a whole word switch back to NonCapitalized mode
            // TODO it might be useful to analyze the context's last nonwhitespace character
            // if it is a sentence end (.?!)
            // it might be better to push it down to the Presage
            m_engine->setCapitalizationMode(PresagePredictorModel::NonCapitalized);
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

void PresagePredictor::reactivateWord(const QString &word)
{
    log(QString("PresagePredictor::reactivateWord(%1)").arg(word));
    m_wordBuffer = word;

    if (m_wordBuffer.length() && m_wordBuffer.at(0).isUpper()) {
        m_engine->setCapitalizationMode(PresagePredictorModel::FirstLetterCapitalized);
    } else {
        m_engine->setCapitalizationMode(PresagePredictorModel::NonCapitalized);
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
    log(QString("PresagePredictor::setLanguage(%1)").arg(language));
    if (m_language != language) {
        m_language = language;
        QString dbFileName = QString("/usr/share/presage/database_%1.db").arg(language.toLower());
        if (QFileInfo::exists(dbFileName)) {
            try {
                m_presage->config("Presage.Predictors.DefaultSmoothedNgramPredictor.DBFILENAME",  dbFileName.toLatin1().constData());
            } catch (PresageException) {

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


PresagePredictorModel::PresagePredictorModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_roles[IndexRole] = "index";
    m_roles[TextRole] = "text";
}

QVariant PresagePredictorModel::data(const QModelIndex &index, int role) const
{
    if (role == IndexRole)
        return index.row();
    if (role == TextRole) {
        if ((size_t)index.row() < predictedWords.size()) {
            switch (m_capitalizationMode) {
            case NonCapitalized:
                return QString::fromStdString(predictedWords[index.row()]);
                break;
            case FirstLetterCapitalized: {
                QString ret = QString::fromStdString(predictedWords[index.row()]);
                if (ret.length()) {
                    return ret.replace(0, 1, ret.at(0).toUpper());
                }
            } break;
            case AllLettersCapitalized:
                return QString::fromStdString(predictedWords[index.row()]).toUpper();
                break;
            }
        }
    }
    return QVariant();
}

int PresagePredictorModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return predictedWords.size();
}

QHash<int, QByteArray> PresagePredictorModel::roleNames() const
{
    return m_roles;
}

void PresagePredictorModel::reload()
{
    beginResetModel();
    endResetModel();

    beginInsertRows(QModelIndex(), 0, predictedWords.size());
    endInsertRows();
    emit predictionsChanged();
}

PresagePredictorModel::CapitalizationMode PresagePredictorModel::capitalizationMode() const
{
    return m_capitalizationMode;
}

void PresagePredictorModel::setCapitalizationMode(const CapitalizationMode &capitalizationMode)
{
    if (m_capitalizationMode != capitalizationMode) {
        m_capitalizationMode = capitalizationMode;
        reload();
    }
}
