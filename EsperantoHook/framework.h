#pragma once

// Windows ヘッダーからほとんど使用されていない部分を除外する
#define WIN32_LEAN_AND_MEAN
// Windows ヘッダー ファイル
#include <windows.h>

#ifdef ESPERANTOHOOK_EXPORTS
#define ESPERANTOHOOK_API __declspec(dllexport)
#else
#define ESPERANTOHOOK_API __declspec(dllimport)
#endif

// exeから呼び出してフックを開始,停止させる関数
extern "C" ESPERANTOHOOK_API bool SetHook(HHOOK* hHook);
extern "C" ESPERANTOHOOK_API bool UnsetHook(HHOOK hHook);

// フックプロシージャ
extern "C" LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);