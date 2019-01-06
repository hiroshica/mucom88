// OsDependent 
// BouKiCHi 2019

#ifndef _OS_DEP_H_
#define _OS_DEP_H_

#include "callback.h"

class OsDependent  {
public:
	OsDependent();
	~OsDependent();

	// COM
	bool CoInitialize();

	// サウンド
	AudioCallback *UserAudioCallback;
	bool InitAudio(void *hwnd, int Rate, int BufferSize);
	void FreeAudio();
	bool SendAudio();
	void WaitSendingAudio();
	bool MuteAudio;
	
	// 実チップ
	bool InitRealChip();
	void FreeRealChip();
	void ResetRealChip();
	int CheckRealChip();
	void OutputRealChip(unsigned int Register, unsigned int Data);

	// タイマー
	TimerCallback *UserTimerCallback;
	bool InitTimer();
	void FreeTimer();
	void UpdateTimer();
	int GetPassTick();

	// 時間
	int GetMilliseconds();
	void Delay(int ms);

private:
	long SoundSending;

	unsigned int TimerId;
	int TimerPeriod;
	int LastTick;

	void *SoundInstance;
	void *RealChipInstance;
};

#endif
