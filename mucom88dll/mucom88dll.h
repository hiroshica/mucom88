// mucom88dll.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//
#include "stdafx.h"

#include "cmucom.h"
#include "wavout.h"

#define kCreateMax    (1)

#ifdef MUCOM88DLL_EXPORTS
#define DLLTEST_API __declspec(dllexport)
#else
#define DLLTEST_API __declspec(dllimport)
#endif

	DLLTEST_API INT32 __stdcall Mucom88CoreCreateVM();
	DLLTEST_API bool __stdcall Mucom88CoreRemoveVM(INT32 index = -1);

	DLLTEST_API void __stdcall Mucom88CoreInit(INT32 index = -1, INT32 option = 0);
	DLLTEST_API void __stdcall Mucom88CoreReset(INT32 index = -1, INT32 option = 0);
