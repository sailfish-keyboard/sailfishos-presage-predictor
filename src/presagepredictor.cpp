#include "presagepredictor.h"


#include <stdio.h>

#include <QDBusConnection>
#include <QElapsedTimer>

// globals
std::string       suggestions;
std::string       config;
std::stringstream buffer;    // text buffer, a real application would
// use something a little more
// sophisticated than a stringstream
std::vector<std::string> words;

PresagePredictor::PresagePredictor(QQuickItem *parent):
    QQuickItem(parent),
    m_engine(new PresagePredictorModel()),
    m_firstLetterCapitalized(false)
{
    // magic starts here
    m_callback = new PresagePredictorCallback(buffer);
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
    buffer.str("");
    buffer.clear();
    predict();
}

void PresagePredictor::setContext(const QString & context)
{
    /*if (buffer.str().length() == 0) {
        log(QString("PresagePredictor::setContext %1 when buffer empty").arg(context));
        /*buffer.str("");
        buffer.clear();
        buffer << context.toStdString();
        predict();*/
    //}
}

void PresagePredictor::predict()
{
    log(QString("PresagePredictor::predict called -> buffer \"%1\"").arg(QString::fromStdString(buffer.str())));
    words = m_presage->predict();
    log(QString("PresagePredictor::predicted  words count: %1").arg(words.size()));
    /*QString bufferString = QString::fromStdString(buffer.str());
    if (m_firstLetterCapitalized) {
        // if the written word starts with capital letter modify the predictions to match that
        for (std::vector<std::string>::iterator it = words.begin() ; it != words.end(); ++it) {
            std::transform((*it).begin(), ++(*it).begin(), (*it).begin(), ::toupper);
        }
    }*/

    m_engine->reload();
    emit onPredictionsChanged();
}

void PresagePredictor::acceptWord(const QString &context)
{
    log(QString("PresagePredictor::acceptWord(%1);").arg(context));
    m_presage->learn(context.toStdString());
}

void PresagePredictor::acceptPrediction(int index)
{
    /*log(QString("PresagePredictor::acceptPrediction(%1)").arg(index));
    if ((size_t)index < words.size()) {
        m_presage->learn(words[index]);
    }*/
}

void PresagePredictor::processSymbol(const QString &symbol, bool forceAdd)
{
    Q_UNUSED(forceAdd) // TODO consider remove it from QML too

    if (symbol.length()) {
        log(QString("PresagePredictor::processSymbol %1").arg(symbol));
        buffer << symbol.toStdString();
        words = m_presage->predict();
        predict();
    }
}

void PresagePredictor::processBackspace()
{
    log(QString("PresagePredictor::processBackspace"));
    std::string bufferContents = buffer.str();
    if (bufferContents.length() > 0) {
        bufferContents = bufferContents.substr(0, bufferContents.length() - 1);
        buffer.str("");
        buffer.clear();
        buffer << bufferContents;
        predict();
    }
}

bool PresagePredictor::isLetter(const QString & letter) const
{
    if (letter.length() == 0)
        return false;
    return letter.at(0).isLetter();
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
    QVariant returnedValue;
    QMetaObject::invokeMethod(this, "log",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, QVariant(log)));
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
        if ((size_t)index.row() < words.size())
            return QString::fromStdString(words[index.row()]);
    }
    return QVariant();
}

int PresagePredictorModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return words.size();
}

QHash<int, QByteArray> PresagePredictorModel::roleNames() const
{
    return m_roles;
}

void PresagePredictorModel::reload()
{
    beginResetModel();
    endResetModel();

    beginInsertRows(QModelIndex(), 0, words.size());
    endInsertRows();
}
