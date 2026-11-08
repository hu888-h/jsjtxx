#include "stdafx.h"
#include "line.h"

CLine::CLine()
{
	next = NULL;
}

CLine::CLine(CPoint &p1,CPoint &p2)
{
	this->p1 = p1;
	this->p2 = p2;
	next = NULL;
}

CLine::CLine(CLine &l)
{
	p1 = l.p1;
	p2 = l.p2;
	next = l.next;
}

CPoint CLine::GetP1()
{
	return p1;
}

CPoint CLine::Getp2()
{
	return p2;
}

void CLine::setNext(CLine *n)
{
	next = n;
}

CLine *CLine::getNext()
{
	return next;
}