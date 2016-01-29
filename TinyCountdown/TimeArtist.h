#pragma once

#include "stdafx.h"

typedef struct _timeFormat
{
	unsigned int minLen;
	unsigned int hourLen;
} TIME_FORMAT;

BOOL DrawTime(HDC hdc, COLORREF* pColor, DWORD time, RECT* pRect, TIME_FORMAT* pFormat); 
