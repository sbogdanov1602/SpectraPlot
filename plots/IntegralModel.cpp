#include "IntegralModel.h"

const QString TIME_COLUMN_NAME("Time");
const QString LEFT_X_COLUMN_NAME("Left X");
const QString RIGHT_X_COLUMN_NAME("Right X");
const QString LEFT_Y_COLUMN_NAME("Bottom Y");
const QString RIGHT_Y_COLUMN_NAME("Top Y");
const QString VALUE_COLUMN_NAME("2D integral");

IntegralModel::IntegralModel(QObject* parent) : QAbstractTableModel(parent)
{
}
void  IntegralModel::addNewData(int2Dresult value) {
    beginInsertRows(QModelIndex(), 0, 0);
    {
        m_Results.push_front(value);
    }
    endInsertRows();
}

int IntegralModel::rowCount(const QModelIndex& /*parent*/) const
{
    int size = m_Results.size();
    return size;
}

void IntegralModel::deleteData(QModelIndexList indexes)
{
    beginRemoveRows(QModelIndex(), 0, 0);
    {
        for (int i = indexes.size() - 1; i >= 0; i--) {
            auto index = indexes[i];
            m_Results.remove(index.row());
        }
    }
    endRemoveRows();
}

void IntegralModel::deleteAllData()
{
    if (m_Results.size() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, 0);
        {
            //sb : removeAll(...) has a very strange implementation
            for (int i = m_Results.size() - 1; i >= 0; i--) {
                m_Results.remove(i);    
            }
        }
        endRemoveRows();
    }
}

int IntegralModel::columnCount(const QModelIndex& /*parent*/) const
{
    return (VALUE_COLUMN + 1);
}

QVariant IntegralModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole) {
        int row = index.row();
        int size = m_Results.size();
        if ( 0 < size && row < size) {
            int idx = row;//size - 1 - row;
            switch (index.column()) 
            {
            case TIME_COLUMN:
            {
                return m_Results[idx].time;
                break;
            }
            case LEFT_X_COLUMN:
            {
                return QString::asprintf("%7.3f", m_Results[idx].leftX);
            }
            case RIGHT_X_COLUMN:
            {
                return QString::asprintf("%7.3f", m_Results[idx].rightX);
            }
            case LEFT_Y_COLUMN:
            {
                return QString::asprintf("%8.2f", m_Results[idx].leftY);
            }
            case RIGHT_Y_COLUMN:
            {
                return QString::asprintf("%8.2f", m_Results[idx].rightY);
            }
            case VALUE_COLUMN:
            {
                return QString::asprintf("%8.4f", m_Results[idx].value);
            }
            default:
                return QVariant();
            }
        }
        else {
            return QString("Row %1, Column %2").arg(index.row()).arg(index.column());
        }
    }

    return QVariant();
}

QVariant IntegralModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section)
        {
        case TIME_COLUMN:
        {
            return TIME_COLUMN_NAME;
        }
        case LEFT_X_COLUMN:
        {
            return LEFT_X_COLUMN_NAME;
        }
        case RIGHT_X_COLUMN:
        {
            return RIGHT_X_COLUMN_NAME;
        }
        case LEFT_Y_COLUMN:
        {
            return LEFT_Y_COLUMN_NAME;
        }
        case RIGHT_Y_COLUMN:
        {
            return RIGHT_Y_COLUMN_NAME;
        }
        case VALUE_COLUMN:
        {
            return VALUE_COLUMN_NAME;
        }
        default:
            return QVariant();
        }
    }
    return QVariant();
}
