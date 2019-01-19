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


namespace Mucom88_DLL {
	class Mucom88Core {
		CMucom *m_MucomVM[kCreateMax];
		INT32 m_LatestSelectNo;

		DLLTEST_API INT32 CreateVM();   // 返り値 -1 = error : 0 > インデックス番号
		// index -1 = 最後に選択していたVM
		DLLTEST_API bool RemoveVM(INT32 index = -1);
		DLLTEST_API void Init(INT32 index = -1, INT32 option = 0);
		DLLTEST_API void Reset(INT32 index = -1, INT32 option = 0);


		INT32 checkindex(INT32 index);

	};
}