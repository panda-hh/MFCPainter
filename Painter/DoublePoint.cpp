#pragma once

#include "pch.h"
#include "DoublePoint.h"


CDoublePoint::CDoublePoint()
{
	m_f64X = 0;
	m_f64Y = 0;
}
CDoublePoint::CDoublePoint(double x, double y)
{
	m_f64X = x;
	m_f64Y = y;

}
CDoublePoint::CDoublePoint(const CDoublePoint& ptPosition)
{
	this->m_f64X = ptPosition.m_f64X;
	this->m_f64Y = ptPosition.m_f64Y;
}

CDoublePoint::~CDoublePoint()
{
}

CDoublePoint& CDoublePoint::operator=(const CDoublePoint& ptPosition)
{
	if(this != &ptPosition)
	{
		this->m_f64X = ptPosition.m_f64X;
		this->m_f64Y = ptPosition.m_f64Y;
	}

	return *this;
}

CDoublePoint& CDoublePoint::operator=(const CPoint& ptPosition)
{
	m_f64X = ptPosition.x;
	m_f64Y = ptPosition.y;
	return *this;
}
