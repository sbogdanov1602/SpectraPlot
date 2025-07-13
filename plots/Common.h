#pragma once

const int TIME_COLUMN = 0;
const int LEFT_X_COLUMN = 1;
const int RIGHT_X_COLUMN = 2;
const int LEFT_Y_COLUMN = 3;
const int RIGHT_Y_COLUMN = 4;
const int VALUE_COLUMN = 5;

struct int2Dresult
{
	double value;
	double leftX;
	double rightX;
	double leftY;
	double rightY;
	QString time;
};

class  CalcResult {
public:
	CalcResult() { actual = false; value = 0.0; }
	bool actual;
	double value;
} ;
