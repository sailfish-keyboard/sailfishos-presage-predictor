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

    enum ShiftState {
        NoShift,
        ShiftLocked,
        ShiftLatched
    };
    Q_ENUM(ShiftState)

    PresagePredictorModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QHash<int, QByteArray> roleNames() const;
    void reload(const QStringList predictedWords);

    void setShiftState(ShiftState shiftState);

private:
    QHash<int, QByteArray> m_roles;
    ShiftState m_shiftState;
    QStringList m_predictedWords;

signals:
    void predictionsChanged();
};
Q_DECLARE_METATYPE(PresagePredictorModel::ShiftState)
#endif // PRESAGEPREDICTORMODEL_H
