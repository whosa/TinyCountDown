#pragma once
#include "TimeArtist.h"

#define CDS_STOPPED	0
#define CDS_STARTED	1
#define CDS_PAUSED	(1 << 1)
#define CDS_WAITING	(1 << 2)

#define IDT_COUNTDOWN 0x80
#define ELAPSE		10

#define WARN_TIME	10000

class Countdown
{
public:
	Countdown();
	virtual ~Countdown();
	BOOL draw(HWND hwnd);
	BOOL draw(HWND hwnd, HDC hdc);
	BOOL drawBackground(HWND hwnd, HDC hdc);

	BOOL startCountdown(HWND hwnd);
	BOOL pauseCountdown();
	BOOL resetCountdown(HWND hwnd);
	BOOL stopCountdown(HWND hwnd); 

	UINT getState();
	void setTotalTime(UINT sec);
	UINT getTotalTime(); 

	void TimerProc(HWND hwnd); 
protected:
	COLORREF m_normalBkColor;
	COLORREF m_warningBkColor;
	COLORREF m_stoppedBkColor;
	COLORREF m_pausedBkColor;
	COLORREF m_currBkColor;
	COLORREF m_normalForeColor;
	COLORREF m_warningForeColor;
	COLORREF m_stoppedForeColor;
	COLORREF m_pausedForeColor;
	COLORREF m_currForeColor;

	DWORD m_totalMilliSec;
	DWORD m_startTime;
	DWORD m_displayTime;

	TIME_FORMAT m_tFormat;

	UINT m_cdState;

	void chooseColor();
private:
};
