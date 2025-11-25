#pragma once
#include <qabstractitemmodel.h>
#include "Common.h"
#include <qmutex.h>

class IntegralModel :  public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit IntegralModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void addNewData(Result value);
    void deleteData(QModelIndexList indexes);
    void deleteAllData();
    void saveToCsvFile();
    QModelIndex findById(int id);
private:
    QList<Result> m_Results;
};

