#include "presagepredictormodel.h"

#include <QMetaEnum>

PresagePredictorModel::PresagePredictorModel(QObject *parent) :
    QAbstractListModel(parent),
    m_shiftState(NoShift)
{
    m_roles[IndexRole] = "index";
    m_roles[TextRole] = "text";
}

QVariant PresagePredictorModel::data(const QModelIndex &index, int role) const
{
    if (role == IndexRole)
        return index.row();

    if (role == TextRole) {
        if (index.row() < m_predictedWords.size()) {
            switch (m_shiftState) {
            case NoShift:
                return m_predictedWords[index.row()];
                break;
            case ShiftLatched: {
                QString ret = m_predictedWords[index.row()];
                if (ret.length()) {
                    return ret.replace(0, 1, ret.at(0).toUpper());
                }
            } break;
            case ShiftLocked:
                return m_predictedWords[index.row()].toUpper();
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
    return m_predictedWords.size();
}

QHash<int, QByteArray> PresagePredictorModel::roleNames() const
{
    return m_roles;
}

void PresagePredictorModel::reload(const QStringList predictedWords)
{
    m_predictedWords = predictedWords;
    beginResetModel();
    endResetModel();

    beginInsertRows(QModelIndex(), 0, m_predictedWords.size());
    endInsertRows();
    emit predictionsChanged();
}

void PresagePredictorModel::setShiftState(ShiftState shiftState)
{
    qDebug() << "PresagePredictorModel::setShiftState" << QMetaEnum::fromType<ShiftState>().valueToKey(shiftState);
    if (m_shiftState != shiftState) {
        m_shiftState = shiftState;
        reload(m_predictedWords);
    }
}
