#include "StdAfx.h"
#include "Countdown.h"
#include "TimeArtist.h"
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")

#define DARK(x)		(x * 4 / 5)
#define ALMOST_ZERO	100

Countdown::Countdown()
{
	this->m_normalBkColor = RGB(0, 0, 0);
	this->m_warningBkColor = RGB(255 , 255, 0);
	this->m_stoppedBkColor = RGB(255, 63, 63); 
	this->m_pausedBkColor = RGB(63, 63, 63);
	this->m_currBkColor = this->m_normalBkColor;

	this->m_normalForeColor = RGB(63, 214, 63);
	this->m_pausedForeColor = RGB(190, 190, 190); 
	this->m_warningForeColor = RGB(0, 0, 0);
	this->m_stoppedForeColor = RGB(63, 63, 0); 
	this->m_currForeColor = this->m_normalForeColor;

	this->setTotalTime(25 * 60);				// default: 15 min.
	this->m_displayTime = this->m_totalMilliSec;
	this->m_cdState = CDS_WAITING;
}

Countdown::~Countdown()
{
}

void Countdown::setTotalTime(UINT sec)
{
	this->m_totalMilliSec = sec * 1000;
	ZeroMemory(&this->m_tFormat, sizeof(TIME_FORMAT));
}

UINT Countdown::getTotalTime()
{
	return this->m_totalMilliSec / 1000;
}

void Countdown::chooseColor()
{
	switch(this->m_cdState)
	{
	case CDS_STOPPED:
		this->m_currBkColor = this->m_stoppedBkColor; 
		this->m_currForeColor = this->m_stoppedForeColor;
		break;
	case CDS_PAUSED | CDS_STARTED:
	case CDS_WAITING:
		this->m_currBkColor = this->m_pausedBkColor; 
		this->m_currForeColor = this->m_pausedForeColor; 
		break;
	case CDS_STARTED:
		if (this->m_displayTime <= (WARN_TIME >> 2) + ALMOST_ZERO)
		{
			DWORD ms = (this->m_displayTime - ALMOST_ZERO) % 1000; 
			this->m_currBkColor = ms % 250 < 200 ? this->m_normalBkColor : this->m_warningBkColor; 
			this->m_currForeColor = ms % 250 < 200 ? this->m_normalForeColor : this->m_warningForeColor;
		}
		else if (this->m_displayTime <= (WARN_TIME >> 1) + ALMOST_ZERO)
		{
			DWORD ms = (this->m_displayTime - ALMOST_ZERO) % 1000; 
			this->m_currBkColor = ms % 500 < 400 ? this->m_normalBkColor : this->m_warningBkColor; 
			this->m_currForeColor = ms % 500 < 400 ? this->m_normalForeColor : this->m_warningForeColor;
		}
		else if (this->m_displayTime <= WARN_TIME + ALMOST_ZERO)
		{
			DWORD ms = (this->m_displayTime - ALMOST_ZERO) % 1000; 
			this->m_currBkColor = ms < 800 ? this->m_normalBkColor : this->m_warningBkColor; 
			this->m_currForeColor = ms < 800 ? this->m_normalForeColor : this->m_warningForeColor;
		}
		else
		{
			this->m_currBkColor = this->m_normalBkColor;
			this->m_currForeColor = this->m_normalForeColor;
		}
		break;
	default:
		break;
	}
}

BOOL Countdown::draw(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint(hWnd, &ps);

	BOOL ret = this->draw(hWnd, hdc);

	::EndPaint(hWnd, &ps);
	return ret;
}

BOOL Countdown::draw(HWND hWnd, HDC hdc)
{
	RECT rect; 
	GetClientRect(hWnd, &rect); 
	SIZE size = {rect.right - rect.left, rect.bottom - rect.top}; 
	HDC hMemDc = ::CreateCompatibleDC(hdc); 
	HBITMAP hbm = ::CreateCompatibleBitmap(hdc, size.cx, size.cy); 
	::SelectObject(hMemDc, hbm); 

	this->chooseColor();

	if (hWnd != GetForegroundWindow())
	{
		this->m_currBkColor = RGB(DARK(GetRValue(this->m_currBkColor)), DARK(GetGValue(this->m_currBkColor)), DARK(GetBValue(this->m_currBkColor))); 
		this->m_currForeColor = RGB(DARK(GetRValue(this->m_currForeColor)), DARK(GetGValue(this->m_currForeColor)), DARK(GetBValue(this->m_currForeColor))); 
	}

	HBRUSH brush = NULL;
	HBRUSH bkBrush = ::CreateSolidBrush(this->m_currBkColor);
	HBRUSH oldBrush = (HBRUSH)::SelectObject(hMemDc, bkBrush); 
	FillRect(hMemDc, &rect, bkBrush); 
	RECT r = {
		rect.left + 5, 
		rect.top + 5,
		rect.right - 3,
		rect.bottom - 5
	}; 

	DrawTime(hMemDc, &this->m_currForeColor, this->m_displayTime + 900, &r, &m_tFormat);

	// Draw the progress line.
	if (this->m_displayTime > ALMOST_ZERO)
	{
		rect.top = rect.bottom - 3;
		int w = rect.right - rect.left;
		rect.left += w * (this->m_totalMilliSec - this->m_displayTime) / this->m_totalMilliSec;
		brush = ::CreateSolidBrush(this->m_currForeColor); 
		FillRect(hMemDc, &rect, brush); 
	}

	::BitBlt(hdc, 0, 0, size.cx, size.cy, hMemDc, 0, 0, SRCCOPY); 

	if (brush)
	{
		::DeleteObject(brush); 
	}
	::DeleteObject(bkBrush); 
	::DeleteObject(hbm);
	::DeleteDC(hMemDc); 
	return TRUE;
}

BOOL Countdown::drawBackground(HWND hWnd, HDC hdc)
{
	RECT rect; 
	GetClientRect(hWnd, &rect); 
	SIZE size = {rect.right - rect.left, rect.bottom - rect.top}; 
	this->chooseColor();

	if (hWnd != GetForegroundWindow())
	{
		this->m_currBkColor = RGB(DARK(GetRValue(this->m_currBkColor)), DARK(GetGValue(this->m_currBkColor)), DARK(GetBValue(this->m_currBkColor))); 
		this->m_currForeColor = RGB(DARK(GetRValue(this->m_currForeColor)), DARK(GetGValue(this->m_currForeColor)), DARK(GetBValue(this->m_currForeColor))); 
	}

	HBRUSH bkBrush = ::CreateSolidBrush(this->m_currBkColor);
	FillRect(hdc, &rect, bkBrush); 

	::DeleteObject(bkBrush); 
	return TRUE;
}

BOOL Countdown::startCountdown(HWND hwnd)
{
	if (this->m_cdState & CDS_STARTED)
	{
		this->resetCountdown(hwnd); 
	}

	this->m_startTime = GetTickCount();
	if (SetTimer(hwnd, IDT_COUNTDOWN, ELAPSE, NULL))
	{
		this->m_cdState = CDS_STARTED;
		PlaySound(TEXT("start.wav"), NULL, SND_FILENAME | SND_ASYNC);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Countdown::pauseCountdown()
{
	if (this->m_cdState & CDS_STARTED)
	{
		if (this->m_cdState & CDS_PAUSED)
		{
			this->m_cdState &= (~CDS_PAUSED);
		}
		else
		{
			this->m_cdState |= CDS_PAUSED;
		}
	}
	return this->m_cdState & CDS_PAUSED;
}

BOOL Countdown::resetCountdown(HWND hwnd)
{
	BOOL ret = TRUE; 
	if (this->m_cdState != CDS_STOPPED)
	{
		this->stopCountdown(hwnd); 
	}
	this->m_cdState = CDS_WAITING;
	this->m_displayTime = this->m_totalMilliSec; 

	return ret; 
}

BOOL Countdown::stopCountdown(HWND hwnd)
{
	BOOL ret = TRUE; 
	if (this->m_cdState & CDS_STARTED)
	{
		if (ret = KillTimer(hwnd, IDT_COUNTDOWN))
		{
			this->m_cdState = CDS_STOPPED;
		}
	}

	return ret; 
}

void Countdown::TimerProc(HWND hwnd)
{
	if (this->m_cdState == CDS_STARTED)
	{
		DWORD dt = GetTickCount() - this->m_startTime; 
		this->m_displayTime = this->m_totalMilliSec - dt; 
		if (this->m_displayTime <= ALMOST_ZERO)
		{
			this->stopCountdown(hwnd); 
			PlaySound(TEXT("end.wav"), NULL, SND_FILENAME | SND_ASYNC);
		}
	}
	else if (this->m_cdState & CDS_PAUSED)
	{
		DWORD dt = this->m_totalMilliSec - this->m_displayTime; 
		this->m_startTime = GetTickCount() - dt; 
	}
}

UINT Countdown::getState()
{
	return this->m_cdState;
}