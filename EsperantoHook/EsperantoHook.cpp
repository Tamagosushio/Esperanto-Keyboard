// EsperantoHook.cpp : DLL 用にエクスポートされる関数を定義します。
//

#include "pch.h"
#include "framework.h"
#include "EsperantoHook.h"


// これは、エクスポートされた変数の例です
ESPERANTOHOOK_API int nEsperantoHook=0;

// これは、エクスポートされた関数の例です。
ESPERANTOHOOK_API int fnEsperantoHook(void)
{
    return 0;
}

// これは、エクスポートされたクラスのコンストラクターです。
CEsperantoHook::CEsperantoHook()
{
    return;
}
