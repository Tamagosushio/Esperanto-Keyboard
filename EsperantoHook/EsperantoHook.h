// 以下の ifdef ブロックは、DLL からのエクスポートを容易にするマクロを作成するための
// 一般的な方法です。この DLL 内のすべてのファイルは、コマンド ラインで定義された ESPERANTOHOOK_EXPORTS
// シンボルを使用してコンパイルされます。このシンボルは、この DLL を使用するプロジェクトでは定義できません。
// ソースファイルがこのファイルを含んでいる他のプロジェクトは、
// ESPERANTOHOOK_API 関数を DLL からインポートされたと見なすのに対し、この DLL は、このマクロで定義された
// シンボルをエクスポートされたと見なします。
#ifdef ESPERANTOHOOK_EXPORTS
#define ESPERANTOHOOK_API __declspec(dllexport)
#else
#define ESPERANTOHOOK_API __declspec(dllimport)
#endif

// このクラスは dll からエクスポートされました
class ESPERANTOHOOK_API CEsperantoHook {
public:
	CEsperantoHook(void);
	// TODO: メソッドをここに追加します。
};

extern ESPERANTOHOOK_API int nEsperantoHook;

ESPERANTOHOOK_API int fnEsperantoHook(void);
