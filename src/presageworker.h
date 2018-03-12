#ifndef PRESAGEWORKER_H
#define PRESAGEWORKER_H

#include "presagepredictor.h"

#include <QObject>

#include <QStringList>

#include <string>

#include <memory>
#include <vector>
#include <sstream>

class Presage;
class PresageCallback;

class PresageWorker : public QObject
{
    Q_OBJECT
public:
    explicit PresageWorker(PresagePredictor *pmain);
    ~PresageWorker();

signals:
    void predictedWords(QStringList, size_t);
    void languageChanged();

public slots:
    void setLanguage(const QString &language);
    void predict();
    void learn(QString text, QString language);
    void forget(QString word, QString language);

private:
    std::unique_ptr<PresageCallback> m_callback;
    std::unique_ptr<Presage> m_presage;
    PresagePredictor        *m_predictor;

    bool m_presageInitialized;
    bool m_forcePrediction{false};
    size_t m_last_id{0};
    QString m_language;
    std::shared_ptr<std::string> m_buffer;
};

#endif // PRESAGEWORKER_H
