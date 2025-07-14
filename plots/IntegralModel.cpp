#include "IntegralModel.h"
#include <qdir.h>
#include <qfiledialog.h>

const QString VALUE_COLUMN_NAME(QObject::tr("2D integral"));
const QString LEFT_X_COLUMN_NAME(QObject::tr("Left X"));
const QString RIGHT_X_COLUMN_NAME(QObject::tr("Right X"));
const QString LEFT_Y_COLUMN_NAME(QObject::tr("Bottom Y"));
const QString RIGHT_Y_COLUMN_NAME(QObject::tr("Top Y"));
const QString TIME_COLUMN_NAME(QObject::tr("Time"));
const QString DATE_COLUMN_NAME(QObject::tr("Date"));

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

void IntegralModel::saveToCsvFile()
{
    if (m_Results.size() > 0)
    {
        QString str = "";

        auto curDir = QDir::current();
        QString filter("*.csv");
        auto filePath = QFileDialog::getSaveFileName(nullptr, tr("Save to file"),
            curDir.dirName(), "*.csv", &filter, QFileDialog::Option::ReadOnly);

        if (!filePath.isEmpty())
        {
            QFile file(filePath);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                file.close();
            }
            else {
                str = VALUE_COLUMN_NAME + "," + LEFT_X_COLUMN_NAME + "," + RIGHT_X_COLUMN_NAME + "," + LEFT_Y_COLUMN_NAME;
                str += "," + RIGHT_Y_COLUMN_NAME + "," + TIME_COLUMN_NAME + "," + DATE_COLUMN_NAME + "\n";

                QTextStream out(&file);
                out << str;

                for (auto it = m_Results.begin(); it != m_Results.end(); it++)
                {
                    str = QString::asprintf("%8.4f,%7.3f,%7.3f,%8.2f,%8.2f"
                        , it->value, it->leftX, it->rightX, it->leftY, it->rightY);
                    str += "," + it->time + "," + it->date + "\n";
                    out << str;
                }

                file.close();
            }
        }
    }
}

int IntegralModel::columnCount(const QModelIndex& /*parent*/) const
{
    return (DATE_COLUMN + 1);
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
            case VALUE_COLUMN:
            {
                return QString::asprintf("%8.4f", m_Results[idx].value);
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
            case TIME_COLUMN:
            {
                return m_Results[idx].time;
                break;
            }
            case DATE_COLUMN:
            {
                return m_Results[idx].date;
                break;
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
        case VALUE_COLUMN:
        {
            return VALUE_COLUMN_NAME;
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
        case TIME_COLUMN:
        {
            return TIME_COLUMN_NAME;
        }
        case DATE_COLUMN:
        {
            return DATE_COLUMN_NAME;
        }
        default:
            return QVariant();
        }
    }
    return QVariant();
}
