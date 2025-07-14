#pragma once
#include <qtableview.h>
class IntegralTableView : public QTableView
{
public:
	IntegralTableView(QWidget* parent);
	
private:
	void deleteSelected();
	void deleteAll();
	void saveToCsvFile();
};

