// OsDependent 
// BouKiCHi 2019

#ifndef _OS_DEP_WIN_H_
#define _OS_DEP_WIN_H_

#include <stdint.h>
#include "../osdep.h"
#include "soundds.h"
#include "realchip.h"

class mucomvm;

class OsDependentWin32 : public OsDependent {
public:
	OsDependentWin32();
	~OsDependentWin32();

	// COM�̏�����(�ŏ��̂P��̂�)
	bool CoInitialize();

	// �T�E���h
	bool InitAudio(void *hwnd, int Rate, int BufferSize);
	void FreeAudio();
	bool SendAudio(int ms);
	void WaitSendingAudio();

	// ���`�b�v
	bool InitRealChip();
	void FreeRealChip();
	void ResetRealChip();
	int CheckRealChip();
	void OutputRealChip(unsigned int Register, unsigned int Data);
	void OutputRealChipAdpcm(void *pData, int size);

	// �^�C�}�[
	bool InitTimer();
	void FreeTimer();
	void UpdateTimer();
	void ResetTime();
	int GetElapsedTime();

	// ����
	int GetMilliseconds();
	void Delay(int ms);

	// �v���O�C���g��
	int InitPlugin(Mucom88Plugin *plg, char *filename);
	int ExecPluginVMCommand( int, int, int, void *, void *);
	int ExecPluginEditorCommand( int, int, int, void *, void *);

private:
	//		�e��VM�C���X�^���X
	mucomvm *vm;

	//		�T�E���h�Đ�
	WinSoundDriver::DriverDS *snddrv;
	HWND master_window;

	//		�^�C�}�[
	static void CALLBACK TimeProc(UINT, UINT, DWORD, DWORD, DWORD);
	static DWORD WINAPI vThreadFunc(LPVOID pParam);

	int StartThread(void);
	int StopThread(void);
	void StreamSend(int ms);
	void ThreadFunc(void);

	UINT timer_period;
	UINT timerid;
	double SamplePerTick;
	double UpdateSamples;

	bool threadflag;

	int64_t last_ft;

	HANDLE hevent;
	HANDLE hthread;
	DWORD threadid;
	LONG sending;

	//		���`�b�v�Ή�
	realchip *RealChipInstance;

};


#endif
