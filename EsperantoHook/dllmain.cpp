#pragma once

// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h" 
#include "framework.h"
#include <Windows.h>
#include <optional>

// DLLのインスタンスハンドル
static HMODULE g_hModule = NULL;
// 直前に入力されたキーコード
static std::optional<DWORD> g_prevKeyCode = 0;
// 1文字目はShiftキーが押されているか
static bool g_isFirstKeyShifted = false;
// 変換が有効化か
static bool g_isConversionEnabled = true;

constexpr DWORD SWITCH_ENABLE_VIRTUAL_KEYCODE = 0x30; // '0'キー

// 変換ルール
struct ConversionRule {
	DWORD firstKey;
	DWORD secondKey;
	wchar_t lowerChar;
	wchar_t upperChar;
};

ConversionRule g_conversionRules[] = {
	{ 0x43, 0x58, L'ĉ', L'Ĉ' },
	{ 0x47, 0x58, L'ĝ', L'Ĝ' },
	{ 0x48, 0x58, L'ĥ', L'Ĥ' },
	{ 0x4A, 0x58, L'ĵ', L'Ĵ' },
	{ 0x53, 0x58, L'ŝ', L'Ŝ' },
	{ 0x55, 0x58, L'ŭ', L'Ŭ' },
};

bool SetHook(HHOOK* hHook) {
	*hHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, g_hModule, 0);
	return *hHook != NULL;
}

bool UnsetHook(HHOOK hHook) {
	if (hHook) {
		return UnhookWindowsHookEx(hHook);
	}
	return false;
}

void EnableConversion(bool bEnable) {
	g_isConversionEnabled = bEnable;
}

bool IsConversionEnabled(void) {
	return g_isConversionEnabled;
}

// キーボードフックプロシージャ
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	// キー入力イベントが発生し、それがキーが押されたイベントである場合
	if (nCode == HC_ACTION and wParam == WM_KEYDOWN) {
		KBDLLHOOKSTRUCT* pkbhs = (KBDLLHOOKSTRUCT*)lParam;
		DWORD currentKeyCode = pkbhs->vkCode;
		// Ctrl + Shift + '0' で変換の有効/無効を切り替え
		if (currentKeyCode == SWITCH_ENABLE_VIRTUAL_KEYCODE and (GetAsyncKeyState(VK_CONTROL) & 0x8000) and (GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
			g_isConversionEnabled = !g_isConversionEnabled; // 状態を反転
			return 1; // ホットキーの入力をOSに伝えない
		}
		// 変換が無効化されている場合は状態をリセットして何もしない
		if (not g_isConversionEnabled) {
			g_prevKeyCode.reset();
			g_isFirstKeyShifted = false;
			return CallNextHookEx(NULL, nCode, wParam, lParam);
		}
		if (g_prevKeyCode.has_value()) {
			// 直前に入力されたキーコードと現在のキーコードを比較して変換ルールを適用
			for (const auto& rule : g_conversionRules) {
				if (g_prevKeyCode.value() == rule.firstKey and currentKeyCode == rule.secondKey) {
					// 変換ルールに一致した場合、直前のキー入力を削除し、変換後の文字を入力
					INPUT inputs[2] = {};
					ZeroMemory(inputs, sizeof(inputs));
					// 直前のキー入力を削除（バックスペース）
					inputs[0].type = INPUT_KEYBOARD;
					inputs[0].ki.wVk = VK_BACK;
					// 変換後の文字を入力
					wchar_t charToSend = g_isFirstKeyShifted ? rule.upperChar : rule.lowerChar;
					inputs[1].type = INPUT_KEYBOARD;
					inputs[1].ki.wScan = charToSend;
					inputs[1].ki.dwFlags = KEYEVENTF_UNICODE;
					SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
					// 状態リセット
					g_prevKeyCode.reset();
					g_isFirstKeyShifted = false;
					// 変換が行われたため、現在のキー入力は無視
					return 1;
				}
			}
		}
		bool isPotentialFirstKey = false;
		for (const auto& rule : g_conversionRules) {
			if(currentKeyCode == rule.firstKey) {
				isPotentialFirstKey = true;
				break;
			}
		}
		if (isPotentialFirstKey) {
			g_prevKeyCode = currentKeyCode;
			g_isFirstKeyShifted = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0) ^ ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
		} else {
			switch (currentKeyCode) {
			case VK_SHIFT:
			case VK_LSHIFT:
			case VK_RSHIFT:
			case VK_CONTROL:
			case VK_LCONTROL:
			case VK_RCONTROL:
			case VK_MENU: // Altキー
			case VK_LMENU:
			case VK_RMENU:
			case VK_LWIN:
			case VK_RWIN:
				// 修飾キーの場合は何もしない (状態を維持)
				break;
			default:
				// それ以外のキーの場合は状態をリセット
				g_prevKeyCode.reset();
				g_isFirstKeyShifted = false;
				break;
			}
		}
	}
	// 次のフックプロシージャに処理を渡す
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hModule = hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

