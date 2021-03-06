﻿#include "stdafx.h"

#include "mucom88dll.h"

extern "C" {

	CMucom *m_MucomVM[kCreateMax];
	INT32 m_LatestSelectNo;

	INT32 checkindex(INT32 index) {
		if (index == -1) {
			index = m_LatestSelectNo;
		}
		return index;
	}

	INT32 __stdcall Mucom88CoreCreateVM() {
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
	bool __stdcall Mucom88CoreRemoveVM(INT32 index) {
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

	void __stdcall Mucom88CoreInit(INT32 index, INT32 option) {
		index = checkindex(index);
		m_MucomVM[index]->Init(NULL, option, 0);
	}
	void __stdcall Mucom88CoreReset(INT32 index, INT32 option) {
		index = checkindex(index);
		m_MucomVM[index]->Reset(option);
	}
}