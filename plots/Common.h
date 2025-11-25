#pragma once

const int VALUE_COLUMN = 0;
const int PEAK_HEIGHT_COLUMN = 1;
const int PEAK_POS_X_COLUMN = 2;
const int PEAK_POS_Y_COLUMN = 3;
const int RATIO_POS_COLUMN = 4;
const int DESCRIPT_COLUMN = 5;
const int LEFT_X_COLUMN = 6;
const int RIGHT_X_COLUMN = 7;
const int LEFT_Y_COLUMN = 8;
const int RIGHT_Y_COLUMN = 9;
const int TIME_COLUMN = 10;
const int DATE_COLUMN = 11;

const QString VALUE_COLUMN_NAME(QObject::tr("2D integral"));
const QString LEFT_X_COLUMN_NAME(QObject::tr("Left X"));
const QString RIGHT_X_COLUMN_NAME(QObject::tr("Right X"));
const QString LEFT_Y_COLUMN_NAME(QObject::tr("Bottom Y"));
const QString RIGHT_Y_COLUMN_NAME(QObject::tr("Top Y"));
const QString TIME_COLUMN_NAME(QObject::tr("Time"));
const QString DATE_COLUMN_NAME(QObject::tr("Date"));

const QString DESCRIPT_COLUMN_NAME(QObject::tr("Description"));
const QString RATIO_POS_COLUMN_NAME(QObject::tr("Ratio to base"));
const QString PEAK_POS_X_COLUMN_NAME(QObject::tr("Peak X position"));
const QString PEAK_POS_Y_COLUMN_NAME(QObject::tr("Peak Y position"));
const QString PEAK_HEIGHT_COLUMN_NAME(QObject::tr("Peak height"));

const QString BASE_PEAK_NAME(QObject::tr("Base peak"));

const int BASE_CURSOR_ID = 10000000;

extern int gGlobalId;

struct Result
{
public:
	int id;
	double piakHight;
	double value;
	double ratio;
	double peakPosX;
	double peakPosY;
	double leftX;
	double rightX;
	double leftY;
	double rightY;
	QString description;
	QString time;
	QString date;

	Result()
	{
		id = ++gGlobalId;
		peakPosX = 0.0;
		peakPosY = 0.0;
		piakHight = 0.0;
		ratio = 0.0;
		value = 0.0;
		leftX = 0.0;
		rightX = 0.0;
		leftY = 0.0;
		rightY = 0.0;
		time = "";
		date = "";
		description = "";
	}
	
	void copyFrom(Result& other) 
	{
		id = other.id;
		peakPosX = other.peakPosX;
		peakPosY = other.peakPosY;
		piakHight = other.piakHight;
		ratio = other.ratio;
		value = other.value;
		leftX = other.leftX;
		rightX = other.rightX;
		leftY = other.leftY;
		rightY = other.rightY;
		time = other.time;
		date = other.date;
		description = other.description;
	}
};

class  CalcResult {
public:
	CalcResult() { actual = false; value = 0.0; }
	bool actual;
	double value;
} ;
