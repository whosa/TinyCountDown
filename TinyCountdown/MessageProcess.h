#pragma once

#include "stdafx.h"

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	TimeInputProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	HelpDlgProc(HWND, UINT, WPARAM, LPARAM);
void MoveHelpDialog(HWND hWnd, HWND hDlg);
