#include "presageworker.h"

#include <QtGlobal>
#include <QFileInfo>
#include <QStandardPaths>

#include <string>
#include <sstream>
#include <presage.h>


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
    PresagePredictorCallback(std::shared_ptr<std::string> buffer): m_buffer(buffer) { }

    std::string get_past_stream() const { return *m_buffer; }
    std::string get_future_stream() const { return empty; }

public:
    std::shared_ptr<std::string> m_buffer;
    const std::string empty;
};


///////////////////////////////////
/// Implementation of PresageWorker
PresageWorker::PresageWorker(PresagePredictor *pmain) :
    QObject(),
    m_predictor(pmain)
{
    m_buffer.reset(new std::string);
    m_callback.reset(new PresagePredictorCallback(m_buffer));
    if (!m_callback)
        return;

    try {
        m_presage.reset(new Presage(m_callback.get()));
    } catch (PresageException e) {
        m_presage.reset();
        qCritical() << QString("Failed to initialize presage: ") + e.what();
        return;
    }
}

PresageWorker::~PresageWorker()
{
}

void PresageWorker::predict()
{
    predict_impl();
}

void PresageWorker::predict_impl(bool force_prediction)
{
    if (!m_presage || !m_presageInitialized)
        return;

    QString lang = m_language;
    if (m_predictor->contextStream(m_last_id, lang, *m_buffer, force_prediction)) {
        setLanguage(lang);

        // stream was updated
        std::vector< std::string > std_words = m_presage->predict();
        QStringList words;
        for (const auto &a: std_words)
            words.append(QString::fromStdString(a));

        emit predictedWords(words, m_last_id);
    }
}

void PresageWorker::setLanguage(const QString &language)
{
    if (!m_presage)
        return;

    if (m_language != language) {
        m_language = language;
        qDebug() << "Language: " << m_language;
        QString dbFileName = QString("/usr/share/presage/database_%1").arg(m_language);
        if (QFileInfo::exists(dbFileName)) {
            try {
                m_presage->config("Presage.Predictors.DefaultSmoothedNgramTriePredictor.DBFILENAME",  dbFileName.toLatin1().constData());
            } catch (PresageException e) {
                qCritical() << e.what();
                return;
            }

            QString userdb =
                QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
                QString("/presage/lm_%1.db").arg(m_language);
            m_presage->config("Presage.Predictors.UserSmoothedNgramPredictor.DBFILENAME",
                              userdb.toLatin1().constData());
            m_presageInitialized = true;

            // Hunspell support. Set even if the files are absent - hunspell will just be returning nothing
            QString hunspell_base = "/usr/share/hunspell/" + m_language;
            m_presage->config("Presage.Predictors.DefaultHunspellPredictor.DICTIONARYBASE",
                              hunspell_base.toLatin1().constData());

            emit languageChanged();
        } else {
            m_presageInitialized = false;
        }
    }
}

void PresageWorker::learn(QString text, QString language)
{
    // learn only if the language if the same as the current one
    if (m_language == language) {
        m_presage->learn(text.toStdString());
    }
}

void PresageWorker::forget(QString word, QString language)
{
    qDebug() << "Requesting to forget: " << word << " lang: " << language << " current_lang: " << m_language;
    // forget only if the language if the same as the current one
    if (m_language == language) {
        m_presage->forget(word.toStdString());
        predict_impl(true);
    }
}
