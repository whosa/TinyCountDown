#pragma once

#include "resource.h"

#define MAX_LOADSTRING 100

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void SetTopMost(__in HWND hWnd, __in BOOL bTopMost);
void MinimizeMemory();
