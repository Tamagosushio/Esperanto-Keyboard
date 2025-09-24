// EsperantoLoader.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include <windows.h>
#include <shellapi.h>
#include "resource.h"

// タスクトレイのメッセージ
#define WM_APP_NOTIFY (WM_APP + 1)
// メニュー項目のID
#define IDM_TOGGLE_CONVERSION 1001
// アプリケーションのインスタンスハンドル
HINSTANCE g_hInst;
// キーボードフックのハンドル
HHOOK g_hHook = NULL;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void AddTaskTrayIcon(HWND hWnd);
void RemoveTaskTrayIcon(HWND hWnd);
void ShowContextMenu(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
										 _In_opt_ HINSTANCE hPrevInstance,
										 _In_ LPWSTR    lpCmdLine,
										 _In_ int       nCmdShow)
{
	g_hInst = hInstance;
	// ウィンドウクラスの登録
	const wchar_t CLASS_NAME[]  = L"Esperanto Loader Window Class";
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClassExW(&wc);
	// ウィンドウの作成
	HWND hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Esperanto Loader",
		0,
		0, 0, 0, 0,
		HWND_MESSAGE,
		NULL,
		hInstance,
		NULL
	);
	if (hWnd == NULL)	{
		return 0;
	}
	if (not SetHook(&g_hHook)) {
		MessageBox(NULL, L"Malsukcesis hoka agordo.", L"Eraro", MB_ICONERROR);
		DestroyWindow(hWnd);
		return FALSE;
	}
	// タスクトレイアイコンの追加
	AddTaskTrayIcon(hWnd);
	// メッセージループ
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	// タスクトレイアイコンからのメッセージ
	case WM_APP_NOTIFY:
		// 右クリックでコンテキストメニュー表示
		if (lParam == WM_RBUTTONUP) {
			ShowContextMenu(hWnd);
		}
		break;
	// メニュー項目が選択されたとき
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 終了メニュー
		switch (wmId) {
		case IDM_TOGGLE_CONVERSION:
			EnableConversion(!IsConversionEnabled());
			break;
		case IDM_EXIT:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}
		break;
	// ウィンドウが破棄されるとき
	case WM_DESTROY:
		UnsetHook(g_hHook);
		RemoveTaskTrayIcon(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void AddTaskTrayIcon(HWND hWnd) {
	NOTIFYICONDATAW nid = {};
	nid.cbSize = sizeof(NOTIFYICONDATAW);
	nid.hWnd = hWnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_APP_NOTIFY;
	nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON2));
	wcscpy_s(nid.szTip, L"Esperanto Input");
	if (not Shell_NotifyIconW(NIM_ADD, &nid)) {
		MessageBox(hWnd, L"Malsukcesis aldono de taskopleta ikono.", L"Eraro", MB_OK | MB_ICONERROR);
	}
}

void RemoveTaskTrayIcon(HWND hWnd) {
	NOTIFYICONDATAW nid = {};
	nid.cbSize = sizeof(NOTIFYICONDATAW);
	nid.hWnd = hWnd;
	nid.uID = 1;
	Shell_NotifyIconW(NIM_DELETE, &nid);
}

void ShowContextMenu(HWND hWnd) {
	POINT pt;
	GetCursorPos(&pt);
	HMENU hMenu = CreatePopupMenu();
	bool isEnabled = IsConversionEnabled();
	UINT flags = MF_BYPOSITION | MF_STRING | (isEnabled ? MF_CHECKED : MF_UNCHECKED);
	InsertMenuW(hMenu, 0, flags, IDM_TOGGLE_CONVERSION, L"Validigas konverto (Ctrl+Shift+0)");
	InsertMenuW(hMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
	InsertMenuW(hMenu, 2, MF_BYPOSITION | MF_STRING, IDM_EXIT, L"Eliras");
	SetForegroundWindow(hWnd);
	TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
	PostMessage(hWnd, WM_NULL, 0, 0);
	DestroyMenu(hMenu);
}