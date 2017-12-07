#include "presagepredictormodel.h"

#include <QMetaEnum>

PresagePredictorModel::PresagePredictorModel(QObject *parent) :
    QAbstractListModel(parent),
    m_capitalizationMode(NonCapital)
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
            switch (m_capitalizationMode) {
            case NonCapital:
                return m_predictedWords.at(index.row()).toLower();
                break;
            case FirstCapital: {
                QString ret = m_predictedWords.at(index.row());
                if (ret.length()) {
                    return ret.replace(0, 1, ret.at(0).toUpper());
                }
            } break;
            case AllCapital:
                return m_predictedWords.at(index.row()).toUpper();
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
    beginResetModel();
    m_predictedWords = predictedWords;
    endResetModel();
    emit predictionsChanged();
}

void PresagePredictorModel::setCapitalizationMode(CapitalizationMode capitalizationMode)
{
    qDebug() << "PresagePredictorModel::setCapitalizationMode" << capitalizationMode;
    if (m_capitalizationMode != capitalizationMode) {
        m_capitalizationMode = capitalizationMode;
        reload(m_predictedWords);
    }
}
