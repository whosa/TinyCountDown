#pragma once

#include "resource.h"

#define MAX_LOADSTRING 100

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void SetTopMost(__in HWND hWnd, __in BOOL bTopMost);
void MinimizeMemory();
