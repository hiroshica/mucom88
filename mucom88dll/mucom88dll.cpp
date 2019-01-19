// mucom88dll.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"

#include "mucom88dll.h"


CMucom *Mucom88Core::m_MucomVM[kCreateMax];
INT32 Mucom88Core::m_LatestSelectNo;


	INT32 Mucom88Core::CreateVM() {
		for (INT32 index = 0; index < kCreateMax; ++index) {
			if (m_MucomVM[index] == NULL) {
				m_MucomVM[index] = new CMucom();
				m_LatestSelectNo = index;
				return index;
			}
		}
		return -1;
	}


	// index -1 = 最後に選択していたVM
	bool Mucom88Core::RemoveVM(INT32 index) {
		if (index == -1) {
			for (index = kCreateMax; index >= 0; --index) {
				if (m_MucomVM[index] != NULL) {
					delete m_MucomVM[index];
					m_MucomVM[index] = NULL;
					return true;
				}
			}
		}
		else {
			if (m_MucomVM[index] != NULL) {
				delete m_MucomVM[index];
				m_MucomVM[index] = NULL;
				return true;
			}
		}
		return false;
	}
	INT32 Mucom88Core::checkindex(INT32 index) {
		if (index == -1) {
			index = m_LatestSelectNo;
		}
		return index;
	}

	void Mucom88Core::Init(INT32 index, INT32 option) {
		index = checkindex(index);
		m_MucomVM[index]->Init(NULL, option, 0);
	}
	void Mucom88Core::Reset(INT32 index, INT32 option) {
		index = checkindex(index);
		m_MucomVM[index]->Reset(option);
	}
