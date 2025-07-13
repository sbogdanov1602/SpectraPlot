#include "IntegralTableView.h"
#include <IntegralModel.h>

IntegralTableView::IntegralTableView(QWidget* parent) :QTableView(parent)
{
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::MultiSelection);

    QAction* deleteSelectedAction = new QAction(tr("Delete selected"));
    QAction* deleteAllAction = new QAction(tr("Delete all"));
    setContextMenuPolicy(Qt::ActionsContextMenu);
	addAction(deleteSelectedAction);
    addAction(deleteAllAction);
    deleteSelectedAction->setEnabled(true);
    deleteAllAction->setEnabled(true);

    QObject::connect(deleteSelectedAction, &QAction::triggered, this, qOverload<>(&IntegralTableView::deleteSelected));
    QObject::connect(deleteAllAction, &QAction::triggered, this, qOverload<>(&IntegralTableView::deleteAll));
}

void IntegralTableView::deleteSelected()
{
    QItemSelectionModel* selectionModel = this->selectionModel();
    if (!selectionModel->hasSelection())
        return;

    if (selectionModel->selectedRows().empty())
        return;

    QModelIndexList indexes = selectionModel->selectedRows();
    ((IntegralModel*)model())->deleteData(indexes);
}

void IntegralTableView::deleteAll()
{
    ((IntegralModel*)model())->deleteAllData();

}
