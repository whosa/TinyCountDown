#include "stdafx.h"
#include "MessageProcess.h"
#include "Resource.h"
#include "TinyCountdown.h"
#include "Countdown.h"
#include <tchar.h>

#define MAX_TIME_LEN	9
#define HELP_BK_COLOR	0x00e0ffe0

// 全局变量
extern HINSTANCE hInst;					// 当前窗体
extern TCHAR szTitle[];					// 标题栏文本
extern TCHAR szWindowClass[];			// 主窗体类名
extern HWND hHelpDlg;

Countdown cd; 
BOOL bShowHelp = TRUE;

void SetTime(HWND hWnd)
{
	UINT state = cd.getState();
	if (!(state & CDS_PAUSED))
	{
		cd.pauseCountdown();
	}
	if (DialogBox(hInst, MAKEINTRESOURCE(IDD_TIME_INPUT), hWnd, TimeInputProc) == IDOK)
	{
		cd.resetCountdown(hWnd); 
		::InvalidateRect(hWnd, NULL, TRUE); 
	}
	else if (!(state & CDS_PAUSED))
	{
		cd.pauseCountdown();
	}
	MinimizeMemory();
}

void PauseStartCountdown(HWND hWnd)
{
	UINT state = cd.getState();
	switch(state)
	{
	case CDS_STOPPED:
		cd.resetCountdown(hWnd);
		::InvalidateRect(hWnd, NULL, TRUE); 
		break;
	case CDS_WAITING:
		cd.startCountdown(hWnd);
		break;
	default:
		cd.pauseCountdown();
		break;
	}
}

void ShowHelpDialog(HWND hWnd, BOOL bShow)
{
	BOOL bHelpExists = IsWindowVisible(hHelpDlg); 
	if (!bShow && bHelpExists)
	{
		ShowWindow(hHelpDlg, SW_HIDE);
		MinimizeMemory();
	}
	else if (bShow && !bHelpExists)
	{
		//hHelpDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_HELP), hWnd, (DLGPROC)HelpDlgProc);
		//MoveHelpDialog(hWnd, hHelpDlg);
		ShowWindow(hHelpDlg, SW_SHOWNOACTIVATE);
		//SetFocus(hWnd);
	}
}

void MoveHelpDialog(HWND hWnd, HWND hDlg)
{
	if (IsWindow(hDlg))
	{
		RECT dlgRect;
		GetClientRect(hDlg, &dlgRect); 
		RECT wndRect;
		GetWindowRect(hWnd, &wndRect);
		int h = GetSystemMetrics(SM_CYFULLSCREEN); 
		if (wndRect.top + dlgRect.bottom + 1 > h)
		{
			dlgRect.top = wndRect.top - dlgRect.bottom - 1;
		}
		else
		{
			dlgRect.top += wndRect.bottom + 1;
		}
		dlgRect.left += (wndRect.right + wndRect.left - dlgRect.right) >> 1; 
		MoveWindow(hDlg, dlgRect.left, dlgRect.top, dlgRect.right, dlgRect.bottom, FALSE);
	}
}

//  WM_COMMAND	- 菜单处理
//  WM_PAINT	- 窗体绘制
//  WM_DESTROY	- 退出
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	static POINT preMousePos; 
	static RECT preWindowPos;
	static TRACKMOUSEEVENT* ptme = NULL;
	static DWORD clickTime = 0; 
	static BOOL bActivateClick = FALSE;

	switch (message)
	{
	case WM_MOUSEACTIVATE:
		bActivateClick = (HIWORD(lParam) == WM_LBUTTONDOWN);
		break;
	case WM_ACTIVATE:
		::InvalidateRect(hWnd, NULL, TRUE); 
		MinimizeMemory();
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			MinimizeMemory();
			break;
		case IDM_EXIT:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_TIMER:
		cd.TimerProc(hWnd);
		::InvalidateRect(hWnd, NULL, TRUE); 
		break;
	case WM_ERASEBKGND:
		return cd.draw(hWnd, (HDC)wParam);
		break;
	case WM_CLOSE:
		if (cd.getState() & CDS_STARTED)
		{
			if (MessageBox(hWnd, _T("Countdown is ongoing. Are you sure you want to exit?"), _T("Question"), MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				DestroyWindow(hWnd);
			}
			else
			{
				MinimizeMemory();
			}
		}
		else
		{
			DestroyWindow(hWnd);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_RBUTTONUP:
		SetTime(hWnd);
		break;
	case WM_LBUTTONDOWN:
		{
			SetCapture(hWnd); 
			preMousePos.x = GET_X_LPARAM(lParam); 
			preMousePos.y = GET_Y_LPARAM(lParam);
			ClientToScreen(hWnd, &preMousePos);
			GetWindowRect(hWnd, &preWindowPos); 
		}
		break;
	case WM_LBUTTONUP:
		{
			RECT r;
			GetWindowRect(hWnd, &r);
			if (!memcmp(&preWindowPos, &r, sizeof(RECT)) && !bActivateClick)
			{
				PauseStartCountdown(hWnd);
			}
			bActivateClick = FALSE;
			ReleaseCapture();
		}
		break;
	case WM_MOUSEMOVE:
		{
			if (!ptme)
			{
				ptme = new TRACKMOUSEEVENT;
				ptme->cbSize		= sizeof(TRACKMOUSEEVENT);
				ptme->dwFlags		= TME_LEAVE;
				ptme->hwndTrack		= hWnd;
				ptme->dwHoverTime	= HOVER_DEFAULT;
				TrackMouseEvent(ptme); 
			}
			if (GetForegroundWindow() == hWnd)
			{
				ShowHelpDialog(hWnd, bShowHelp);
			}
			if (GetCapture() == hWnd)
			{
				POINT p;
				p.x = GET_X_LPARAM(lParam); 
				p.y = GET_Y_LPARAM(lParam);
				ClientToScreen(hWnd, &p);
				int dx = p.x - preMousePos.x; 
				int dy = p.y - preMousePos.y; 
				MoveWindow(hWnd, 
					preWindowPos.left + dx, 
					preWindowPos.top + dy, 
					preWindowPos.right - preWindowPos.left, 
					preWindowPos.bottom - preWindowPos.top, 
					TRUE); 
				MoveHelpDialog(hWnd, hHelpDlg);
			}
			if (!(cd.getState() & CDS_STARTED))
			{
				::InvalidateRect(hWnd, NULL, TRUE); 
			}
		}
		break;
	case WM_MOUSELEAVE:
		ShowHelpDialog(hWnd, FALSE);
		delete ptme;
		ptme = NULL;
		break;
	case WM_MBUTTONUP:
		bShowHelp = !bShowHelp;
		ShowHelpDialog(hWnd, bShowHelp); 
		break;
	case WM_MOUSEWHEEL:
		{
			short delta = HIWORD(wParam);
			delta >>= (LOWORD(wParam) & MK_CONTROL) ? 3 : 6;
			RECT rect = {0};
			GetWindowRect(hWnd, &rect);
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			if (x > rect.left && x < rect.right && y > rect.top && y < rect.bottom)
			{
				int w = rect.right - rect.left;
				int h = rect.bottom - rect.top;
				int dx = (w > h ? delta * w / h : delta);
				int dy = (w > h ? delta : delta * h / w);
				int dw = (w > h ? (delta << 1) * w / h : (delta << 1));
				int dh = (w > h ? (delta << 1) : (delta << 1) * h / w);
				if (w <= 50 && delta < 0)
				{
					dx = dw = 0;
				}
				if (h <= 24 && delta < 0)
				{
					dy = dh = 0;
				}
				if (dx != 0 && dy != 0)
				{
					MoveWindow(hWnd, 
						rect.left - dx, 
						rect.top - dy,
						w + dw,
						h + dh, 
						TRUE);
					MoveHelpDialog(hWnd, hHelpDlg);
				}
			}
		}
		break;
	case WM_SIZE:
		{
			unsigned short w = LOWORD(lParam);
			unsigned short h = HIWORD(lParam);
			if (w < 5 && h < 3)
			{
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			}
			else
			{
				MoveHelpDialog(hWnd, hHelpDlg);
			}
		}
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_RETURN:
			SetTime(hWnd);
			break;
		case VK_SPACE:
			PauseStartCountdown(hWnd);
			break;
		case VK_BACK:
			cd.resetCountdown(hWnd);
			::InvalidateRect(hWnd, NULL, TRUE); 
			break;
		case VK_HOME:
			SetTopMost(hWnd, TRUE); 
			break;
		case VK_END:
			SetTopMost(hWnd, FALSE);
			break;
		case VK_F1:
			bShowHelp = !bShowHelp;
			ShowHelpDialog(hWnd, bShowHelp); 
			break;
		case 'M':
			{
				static BOOL bMax = FALSE;
				bMax = !bMax;
				::ShowWindow(hWnd, bMax ? SW_MAXIMIZE : SW_RESTORE); 
			}
			break;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK TimeInputProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
			HWND hEdit = GetDlgItem(hDlg, IDC_EDIT_TIME);
			Edit_LimitText(hEdit, MAX_TIME_LEN);

			int sec = cd.getTotalTime();
			int hour = sec / 3600;
			int min = sec / 60;
			sec %= 60;
			min %= 60;
			TCHAR time[MAX_TIME_LEN + 1];
			_stprintf_s(time, _T("%d:%02d:%02d"), hour, min, sec); 
			Edit_SetText(hEdit, time); 
			Edit_SetSel(hEdit, 0, MAX_TIME_LEN + 1);

			// Move the dialog to the center of parent window.
			HWND hWnd = GetParent(hDlg);
			RECT wndRect;
			GetWindowRect(hWnd, &wndRect);
			RECT dlgRect;
			GetWindowRect(hDlg, &dlgRect);

			dlgRect.right -= dlgRect.left;
			dlgRect.bottom -= dlgRect.top;
			dlgRect.left = ((wndRect.left + wndRect.right) >> 1);
			dlgRect.top = ((wndRect.top + wndRect.bottom) >> 1);
			dlgRect.left -= (dlgRect.right >> 1);
			dlgRect.top -= (dlgRect.bottom >> 1);
			MoveWindow(hDlg, dlgRect.left, dlgRect.top, dlgRect.right, dlgRect.bottom, FALSE); 

			if (GetDlgCtrlID((HWND) wParam) != IDC_EDIT_TIME)
			{
				SetFocus(hEdit); 
				return FALSE;
			}
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				int time[3] = {0, -1, -1};
				HWND hEdit = GetDlgItem(hDlg, IDC_EDIT_TIME);
				TCHAR editText[MAX_TIME_LEN + 1];
				int len = Edit_GetText(hEdit, editText, MAX_TIME_LEN + 1);
				for (int i = 0, j = 0; i < len; i++)
				{
					if (editText[i] != ':')
					{
						time[j] *= 10;
						time[j] += (editText[i] - '0');
					}
					else
					{
						time[++j] = 0;
					}
				}
				UINT sec = time[0];
				for (int i = 1; i < 3; i++)
				{
					if (time[i] >= 0)
					{
						sec *= 60;
						sec += time[i];
					}
				}
				cd.setTotalTime(sec);
			}
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		case IDC_EDIT_TIME:
			{
				switch(HIWORD(wParam))
				{
				case EN_UPDATE:
					{
						static BOOL changing = FALSE;
						if (!changing)
						{
							HWND hEdit = GetDlgItem(hDlg, IDC_EDIT_TIME); 
							TCHAR editText[MAX_TIME_LEN + 1];
							TCHAR finalText[MAX_TIME_LEN + 1];
							DWORD sel = Edit_GetSel(hEdit);
							int len = Edit_GetText(hEdit, editText, MAX_TIME_LEN + 1);
							ZeroMemory(finalText, sizeof(finalText));
							int colonCount = 0;
							for (int i = 0, j = 0; i < len; i++)
							{
								switch(editText[i])
								{
								case '.':
								case ';':
								case '\'':
								case '\"':
								case ',':
								case '`':
								case '-':
								case '+':
								case '_':
								case '=':
									editText[i] = ':';
									break;
								}

								if ((editText[i] >= '0' && editText[i] <= '9') || (editText[i] == ':' && colonCount < 2))
								{
									finalText[j++] = editText[i]; 
									if (editText[i] == ':') colonCount++;
								}
							}
							changing = TRUE;
							Edit_SetText(hEdit, finalText);
							Edit_SetSel(hEdit, LOWORD(sel), HIWORD(sel));
							changing = FALSE;
						}
					}
					break;
				}
			}
			break;
		}
		break;

	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK HelpDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH bkBrush = NULL; 
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
			SetWindowLong(hDlg, GWL_EXSTYLE, WS_EX_LAYERED); 
			SetLayeredWindowAttributes(hDlg, NULL, 215, LWA_ALPHA);
			bkBrush = CreateSolidBrush(HELP_BK_COLOR); 
			HWND hWnd = GetParent(hDlg); 
			MoveHelpDialog(hWnd, hDlg);
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			DestroyWindow(hDlg);
			return (INT_PTR)TRUE;
			break;
		}
		break;

	case WM_RBUTTONDOWN:
	case WM_HELP:
		DestroyWindow(hDlg);
		break;

	case WM_ACTIVATE:
		SetActiveWindow(GetParent(hDlg));
		break;

	case WM_CTLCOLORSTATIC:
		{
			HDC hdc = (HDC) wParam;
			SetBkColor(hdc, HELP_BK_COLOR);
		}
	case WM_CTLCOLORDLG:
		return (INT_PTR)bkBrush;
		break;

	case WM_DESTROY:
		if (bkBrush)
		{
			DeleteObject(bkBrush); 
			bkBrush = NULL;
		}
		MinimizeMemory();
		break;

	}
	return (INT_PTR)FALSE;
}

// 僶乕僕儑儞忣曬儃僢僋僗偺儊僢僙乕僕 僴儞僪儔偱偡丅
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
