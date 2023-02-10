#pragma once

#include "pch.h"

class CDoublePoint
{
public:
	CDoublePoint();
	CDoublePoint(double x, double y);
	CDoublePoint(const CDoublePoint& ptPosition);
	virtual ~CDoublePoint();
	CDoublePoint& CDoublePoint::operator=(const CDoublePoint& ptPosition);
	CDoublePoint& CDoublePoint::operator=(const CPoint& ptPosition);
	
public:
	double m_f64X;
	double m_f64Y;

};