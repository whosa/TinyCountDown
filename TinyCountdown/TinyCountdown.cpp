#include "stdafx.h"
#include "TinyCountdown.h"
#include "MessageProcess.h"
#include "Commctrl.h"

#define WND_WIDTH	100
#define WND_HEIGHT	50
#define SCR_GAP_X	20
#define SCR_GAP_Y	35

HINSTANCE hInst;								
TCHAR szTitle[MAX_LOADSTRING];					
TCHAR szWindowClass[MAX_LOADSTRING];			
HWND hHelpDlg;

void SetTopMost(__in HWND hWnd, __in BOOL bTopMost)
{
	::SetWindowPos(hWnd, bTopMost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED | SWP_NOACTIVATE); 
}

void MinimizeMemory()
{
	HANDLE hProcess = ::GetCurrentProcess(); 
	::SetProcessWorkingSetSize(hProcess, -1, -1); 
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TINYCOUNTDOWN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TINYCOUNTDOWN));
	MinimizeMemory();

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!IsWindow(hHelpDlg) || !IsDialogMessage(hHelpDlg, &msg))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	return (int) msg.wParam;
}




ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TINYCOUNTDOWN));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) ::GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance;

   int scr_x = GetSystemMetrics(SM_CXFULLSCREEN);
   int scr_y = GetSystemMetrics(SM_CYFULLSCREEN);

   hWnd = CreateWindow(szWindowClass, szTitle, WS_THICKFRAME | WS_SYSMENU | WS_POPUP,
      scr_x - SCR_GAP_X - WND_WIDTH, SCR_GAP_Y, WND_WIDTH, WND_HEIGHT, NULL, NULL, hInstance, NULL);

   hHelpDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_HELP), hWnd, (DLGPROC)HelpDlgProc);

   if (!hWnd)
   {
      return FALSE;
   }

   SetTopMost(hWnd, TRUE);
   SetWindowLongW(hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
