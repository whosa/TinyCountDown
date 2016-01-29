#include "stdafx.h"
#include <stdio.h>
#include "TimeArtist.h"

BOOL DrawLine(HDC hdc, COLORREF* pColor, int index, RECT* pRect)
{
	int w = pRect->right - pRect->left; 
	int h = pRect->bottom - pRect->top;
	int unit = min(w / 6, h / 11); 
	if (unit < 1) unit = 1; 

	RECT rect;
	switch(index)
	{
	case 0:
	case 1:
	case 5:
	case 6:
		{
			BOOL indexGt4 = index > 4; 
			rect.left = indexGt4 ? pRect->right - unit : pRect->left;
			int i = indexGt4 ? index - 5 : index; 
			rect.top = pRect->top + i * (h >> 1);
			rect.right = rect.left + unit; 
			rect.bottom = rect.top + (h >> 1); 
		}
		break;
	case 2:
	case 3:
	case 4:
		{
			rect.left = pRect->left;
			rect.right = pRect->right;
			rect.top = pRect->top + (index - 2) * ((h - unit) >> 1); 
			rect.bottom = pRect->bottom - (4 - index) * ((h - unit) >> 1); 
		}
		break; 
	default:
		rect = *pRect;
		break; 
	}

	HBRUSH brush = CreateSolidBrush(*pColor); 
	FillRect(hdc, &rect, brush); 
	DeleteObject(brush); 
	return TRUE; 
}

BOOL DrawNumber(HDC hdc, COLORREF* pColor, char number, RECT* pRect)
{
	switch(number - '0')
	{
	case 0:
		DrawLine(hdc, pColor, 0, pRect);
		DrawLine(hdc, pColor, 1, pRect);
		DrawLine(hdc, pColor, 2, pRect);
		DrawLine(hdc, pColor, 4, pRect);
		DrawLine(hdc, pColor, 5, pRect);
		DrawLine(hdc, pColor, 6, pRect);
		break; 
	case 1:
		DrawLine(hdc, pColor, 5, pRect);
		DrawLine(hdc, pColor, 6, pRect);
		break; 
	case 2:
		DrawLine(hdc, pColor, 1, pRect);
		DrawLine(hdc, pColor, 2, pRect);
		DrawLine(hdc, pColor, 3, pRect);
		DrawLine(hdc, pColor, 4, pRect);
		DrawLine(hdc, pColor, 5, pRect);
		break; 
	case 3:
		DrawLine(hdc, pColor, 2, pRect);
		DrawLine(hdc, pColor, 3, pRect);
		DrawLine(hdc, pColor, 4, pRect);
		DrawLine(hdc, pColor, 5, pRect);
		DrawLine(hdc, pColor, 6, pRect);
		break;
	case 4:
		DrawLine(hdc, pColor, 0, pRect);
		DrawLine(hdc, pColor, 3, pRect);
		DrawLine(hdc, pColor, 5, pRect);
		DrawLine(hdc, pColor, 6, pRect);
		break; 
	case 5:
		DrawLine(hdc, pColor, 0, pRect);
		DrawLine(hdc, pColor, 2, pRect);
		DrawLine(hdc, pColor, 3, pRect);
		DrawLine(hdc, pColor, 4, pRect);
		DrawLine(hdc, pColor, 6, pRect);
		break;
	case 6:
		DrawLine(hdc, pColor, 0, pRect);
		DrawLine(hdc, pColor, 1, pRect);
		DrawLine(hdc, pColor, 2, pRect);
		DrawLine(hdc, pColor, 3, pRect);
		DrawLine(hdc, pColor, 4, pRect);
		DrawLine(hdc, pColor, 6, pRect);
		break;
	case 7:
		DrawLine(hdc, pColor, 2, pRect);
		DrawLine(hdc, pColor, 5, pRect);
		DrawLine(hdc, pColor, 6, pRect);
		break;
	case 8:
		DrawLine(hdc, pColor, 0, pRect);
		DrawLine(hdc, pColor, 1, pRect);
		DrawLine(hdc, pColor, 2, pRect);
		DrawLine(hdc, pColor, 3, pRect);
		DrawLine(hdc, pColor, 4, pRect);
		DrawLine(hdc, pColor, 5, pRect);
		DrawLine(hdc, pColor, 6, pRect);
		break;
	case 9:
		DrawLine(hdc, pColor, 0, pRect);
		DrawLine(hdc, pColor, 2, pRect);
		DrawLine(hdc, pColor, 3, pRect);
		DrawLine(hdc, pColor, 4, pRect);
		DrawLine(hdc, pColor, 5, pRect);
		DrawLine(hdc, pColor, 6, pRect);
		break;
	default:
		break; 
	}
	return TRUE;
}

BOOL DrawSep(HDC hdc, COLORREF* pColor, RECT* pRect)
{
	int w = pRect->right - pRect->left; 
	int h = pRect->bottom - pRect->top;
	int unit = min(w >> 1, h / 6); 
	if (unit < 1) unit = 1; 

	RECT topRect = {
		pRect->left + ((w - unit) >> 1), 
		pRect->top + (h >> 2) - (unit >> 1), 
		pRect->right - ((w - unit) >> 1), 
		pRect->top + (h >> 2) + (unit >> 1)
	};
	RECT bottomRect = {
		pRect->left + ((w - unit) >> 1), 
		pRect->bottom - (h >> 2) - (unit >> 1), 
		pRect->right - ((w - unit) >> 1), 
		pRect->bottom - (h >> 2) + (unit >> 1)
	};

	if (topRect.right - topRect.left < 1) topRect.right = topRect.left + 1;
	if (topRect.bottom - topRect.top < 1) topRect.bottom = topRect.top + 1;
	if (bottomRect.right - bottomRect.left < 1) bottomRect.right = bottomRect.left + 1;
	if (bottomRect.bottom - bottomRect.top < 1) bottomRect.bottom = bottomRect.top + 1;

	HBRUSH brush = CreateSolidBrush(*pColor); 
	FillRect(hdc, &topRect, brush); 
	FillRect(hdc, &bottomRect, brush); 
	DeleteObject(brush); 
	return TRUE;
}

BOOL DrawTime(HDC hdc, COLORREF* pColor, DWORD time, RECT* pRect, TIME_FORMAT* pFormat)
{
	int sec = time / 1000; 
	int min = sec / 60;
	int hour = min / 60; 
	sec %= 60; 
	min %= 60; 

	struct _time {
		char szSec[3];
		char szMin[3];
		char szHour[32];
	} szTime;

	ZeroMemory(&szTime, sizeof(szTime)); 
	sprintf_s(szTime.szSec, "%02d", sec); 
	int n = 4; 

	sprintf_s(szTime.szMin, "%02d", min);
	n += 5;
	if (!pFormat->minLen)
	{
		pFormat->minLen = 2; 
	}

	if (hour > 0 || pFormat->hourLen)
	{
		sprintf_s(szTime.szHour, "%d", hour);
		n += ((int)strlen(szTime.szHour) << 1) + 1; 
		if (!pFormat->hourLen)
		{
			pFormat->hourLen = (unsigned int)strlen(szTime.szHour);
		}
	}

	int w = pRect->right - pRect->left;
	int h = pRect->bottom - pRect->top;
	int wu = w / n; 

	int margin_x = wu >> 3;
	if (margin_x < 2) margin_x = 2;
	int margin_y = h >> 4;

	int i = 0; 
	if (pFormat->hourLen)
	{
		for (unsigned int index = 0; index < strlen(szTime.szHour); index++)
		{
			RECT rect = {
				pRect->left + wu * i + margin_x, 
				pRect->top + margin_y, 
				pRect->left + wu * (i + 2) - margin_x, 
				pRect->bottom - margin_y
			}; 
			DrawNumber(hdc, pColor, szTime.szHour[index], &rect); 
			i += 2; 
		}
		RECT rect = {
			pRect->left + wu * i + margin_x, 
			pRect->top + margin_y, 
			pRect->left + wu * (i + 1) - margin_x, 
			pRect->bottom - margin_y
		}; 
		DrawSep(hdc, pColor, &rect); 
		i++; 
	}

	for (unsigned int index = 0; index < strlen(szTime.szMin); index++)
	{
		RECT rect = {
			pRect->left + wu * i + margin_x, 
			pRect->top + margin_y, 
			pRect->left + wu * (i + 2) - margin_x, 
			pRect->bottom - margin_y
		}; 
		DrawNumber(hdc, pColor, szTime.szMin[index], &rect); 
		i += 2; 
	}
	RECT rect = {
		pRect->left + wu * i + margin_x, 
		pRect->top + margin_y, 
		pRect->left + wu * (i + 1) - margin_x, 
		pRect->bottom - margin_y
	}; 
	DrawSep(hdc, pColor, &rect); 
	i++; 

	for (unsigned int index = 0; index < strlen(szTime.szSec); index++)
	{
		RECT rect = {
			pRect->left + wu * i + margin_x, 
			pRect->top + margin_y, 
			pRect->left + wu * (i + 2) - margin_x, 
			pRect->bottom - margin_y
		}; 
		DrawNumber(hdc, pColor, szTime.szSec[index], &rect); 
		i += 2; 
	}

	return TRUE;
}
