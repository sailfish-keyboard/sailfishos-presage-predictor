#ifndef PRESAGEPREDICTORMODEL_H
#define PRESAGEPREDICTORMODEL_H

#include <QAbstractListModel>
#include <QVariant>

#include "presagepredictor.h"

class PresagePredictor; // forward declaration

class PresagePredictorModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum PredictionRoles {
        IndexRole = Qt::UserRole + 1,
        TextRole
    };

    enum CapitalizationMode {
        NonCapital,
        FirstCapital,
        AllCapital
    };

    PresagePredictorModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QHash<int, QByteArray> roleNames() const;
    void reload(const QStringList predictedWords);

    void setCapitalizationMode(CapitalizationMode capitalizationMode);

private:
    QHash<int, QByteArray> m_roles;
    CapitalizationMode m_capitalizationMode;
    QStringList m_predictedWords;

signals:
    void predictionsChanged();
};

#endif // PRESAGEPREDICTORMODEL_H
