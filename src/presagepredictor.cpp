#include "presagepredictor.h"


#include <stdio.h>

#include <QDBusConnection>
#include <QElapsedTimer>

// globals
std::string       suggestions;
std::string       config;
std::stringstream predictBuffer;
std::stringstream contextBuffer;
std::stringstream wordBuffer;
std::vector<std::string> predictedWords;

PresagePredictor::PresagePredictor(QQuickItem *parent):
    QQuickItem(parent),
    m_engine(new PresagePredictorModel()),
    m_firstLetterCapitalized(false)
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
    contextBuffer.str("");
    contextBuffer.clear();
    wordBuffer.str("");
    wordBuffer.clear();
    predict();
}

void PresagePredictor::setContext(const QString & context)
{
    log(QString("PresagePredictor::setContext %1").arg(context));
    contextBuffer.str("");
    contextBuffer.clear();
    contextBuffer << context.toStdString();
    //predict();
}

void PresagePredictor::predict()
{
    log("PresagePredictor::predict");
    log(QString("CTX : %1").arg(QString::fromStdString(contextBuffer.str())));
    log(QString("Word: %1").arg(QString::fromStdString(wordBuffer.str())));

    predictBuffer.str("");
    predictBuffer.clear();
    predictBuffer << contextBuffer.str();
    predictBuffer << wordBuffer.str();

    predictedWords = m_presage->predict();
    log(QString("PresagePredictor::predicted  words count: %1").arg(predictedWords.size()));
    /*QString bufferString = QString::fromStdString(buffer.str());
    if (m_firstLetterCapitalized) {
        // if the written word starts with capital letter modify the predictions to match that
        for (std::vector<std::string>::iterator it = words.begin() ; it != words.end(); ++it) {
            std::transform((*it).begin(), ++(*it).begin(), (*it).begin(), ::toupper);
        }
    }*/

    m_engine->reload();
}

void PresagePredictor::acceptWord(const QString &context)
{
    log(QString("PresagePredictor::acceptWord(%1);").arg(context));
    m_presage->learn(context.toStdString());
    wordBuffer.str("");
    wordBuffer.clear();
}

void PresagePredictor::acceptPrediction(int index)
{
    log(QString("PresagePredictor::acceptPrediction(%1)").arg(index));
    if ((size_t)index < predictedWords.size()) {
        m_presage->learn(predictedWords[index]);
        wordBuffer.str("");
        wordBuffer.clear();
    }
}

void PresagePredictor::processSymbol(const QString &symbol, bool forceAdd)
{
    Q_UNUSED(forceAdd) // TODO consider remove it from QML too

    if (symbol.length()) {
        log(QString("PresagePredictor::processSymbol %1").arg(symbol));
        wordBuffer << symbol.toStdString();
        predict();
    }
}

void PresagePredictor::processBackspace()
{
    log(QString("PresagePredictor::processBackspace"));
    std::string wordBufferContents = wordBuffer.str();
    if (wordBufferContents.length() > 0) {
        wordBufferContents = wordBufferContents.substr(0, wordBufferContents.length() - 1);
        wordBuffer.str("");
        wordBuffer.clear();
        wordBuffer << wordBufferContents;
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
    wordBuffer.str("");
    wordBuffer.clear();
    wordBuffer << word.toStdString();
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
        // TODO catch exception!
        m_presage->config("Presage.Predictors.DefaultSmoothedNgramPredictor.DBFILENAME",  dbFileName.toLatin1().constData());
        emit languageChanged();
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

void PresagePredictor::setFirstLetterCapitalized(bool firstLetterCapitalized)
{
    if (m_firstLetterCapitalized != firstLetterCapitalized) {
        log(QString("PresagePredictor::setFirstLetterCapitalized %1").arg(firstLetterCapitalized));
        m_firstLetterCapitalized = firstLetterCapitalized;
        predict();
    }
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
        if ((size_t)index.row() < predictedWords.size())
            return QString::fromStdString(predictedWords[index.row()]);
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
