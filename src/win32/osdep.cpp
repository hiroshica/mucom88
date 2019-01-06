// OsDependent 
// BouKiCHi 2019

#include <stdio.h>
#include "realchip.h"
#include "soundds.h"
#include "osdep.h"
#include <windows.h>

// タイマー用
static void CALLBACK TimerProc(UINT, UINT, DWORD, DWORD, DWORD);

// コンストラクタ
OsDependent::OsDependent() {
	SoundInstance = NULL;
	RealChipInstance = NULL;
	MuteAudio = false;
	UserTimerCallback = new TimerCallback;
	UserAudioCallback = new AudioCallback;
	SoundSending = 0;
}
OsDependent::~OsDependent() {}


bool OsDependent::CoInitialize() {
	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) return false;
	return true;
}

// オーディオ
bool OsDependent::InitAudio(void *hwnd, int Rate, int BufferSize) {
	WinSoundDriver::DriverDS *snddrv = new WinSoundDriver::DriverDS;
	if (hwnd) snddrv->SetHWND((HWND)hwnd);
	if (!snddrv->Init(Rate, 2, BufferSize)) return false;

	int size = Rate * 40 / 1000 * 2;
	snddrv->GetSoundBuffer()->PrepareBuffer(size);
	snddrv->GetSoundBuffer()->UpdateBuffer(size);
	SoundInstance = snddrv;
	return true;
}

void OsDependent::FreeAudio() {
	if (!SoundInstance) return;
	WinSoundDriver::DriverDS *snddrv = (WinSoundDriver::DriverDS *)SoundInstance;
	WaitSendingAudio();
	delete snddrv;
	SoundInstance = NULL;
}

void OsDependent::WaitSendingAudio() {
	for (int i = 0; i < 300 && SoundSending == 1; i++) Sleep(10);
}

bool OsDependent::SendAudio() {
	if (!SoundInstance) return false;
	WinSoundDriver::DriverDS *snddrv = (WinSoundDriver::DriverDS *)SoundInstance;

	// 0以外はスレッドが重複しているので続行しない。
	int ret = InterlockedExchange(&SoundSending, 1);
	if (ret != 0) return false;

	int writelength;
	writelength = snddrv->PrepareSend();
	if (writelength) {
		int32 *smp;
		int size = writelength >> 2;
		smp = snddrv->GetSoundBuffer()->PrepareBuffer(size * 2);

		if (!MuteAudio) {
			UserAudioCallback->mix = smp;
			UserAudioCallback->size = size;
			UserAudioCallback->Run();
		}

		snddrv->GetSoundBuffer()->UpdateBuffer(size * 2);
	}
	snddrv->Send();

	// 終了
	InterlockedExchange(&SoundSending, 0);
	return true;
}

// 実チップ
bool OsDependent::InitRealChip() {
	realchip *rc = new realchip();
	rc->Initialize();
	if (!rc->IsRealChip()) return false;
	RealChipInstance = rc;
}

void OsDependent::FreeRealChip() {
	if (!RealChipInstance) return;
	realchip *rc = (realchip *)RealChipInstance;
	rc->UnInitialize();
	delete rc;
	rc = NULL;
}


int OsDependent::CheckRealChip() {
	if (!RealChipInstance) return -1;
	return 0;
}

void OsDependent::ResetRealChip() {
	if (!RealChipInstance) return;
	realchip *rc = (realchip *)RealChipInstance;
	rc->Reset();
}


void OsDependent::OutputRealChip(unsigned int Register, unsigned int Data) {
	if (!RealChipInstance) return;
	realchip *rc = (realchip *)RealChipInstance;

	rc->SetRegister(Register, Data);
}


// タイマー
bool OsDependent::InitTimer() {
	TIMECAPS caps;
	TimerId = 0;
	if (timeGetDevCaps(&caps, sizeof(TIMECAPS)) == TIMERR_NOERROR){
		// マルチメディアタイマーのサービス精度を最大に
		HANDLE myth = GetCurrentThread();
		SetThreadPriority(myth, THREAD_PRIORITY_HIGHEST);

		TimerPeriod = caps.wPeriodMin;
		timeBeginPeriod(TimerPeriod);
		TimerId = timeSetEvent(TimerPeriod, caps.wPeriodMin, TimerProc, reinterpret_cast<DWORD>(this), (UINT)TIME_PERIODIC);
		LastTick = GetMilliseconds();
		return true;
	}

	//	失敗した時
	TimerPeriod = -1;
	TimerId = 0;
	return false;
}

void OsDependent::FreeTimer() {
	if (!TimerId) return;

	timeKillEvent(TimerId);
	timeEndPeriod(TimerPeriod);
	TimerId = 0;
}

static void CALLBACK TimerProc(UINT uid, UINT, DWORD user, DWORD, DWORD) {
	if (!user) return;
	OsDependent* inst = (OsDependent *)(user);
	inst->UpdateTimer();
}

void OsDependent::UpdateTimer() {
	int CurrentTime = GetMilliseconds();
	int tick = CurrentTime - LastTick;
	LastTick = CurrentTime;
	UserTimerCallback->tick = tick;
	UserTimerCallback->Run();
}

// 時間
int OsDependent::GetMilliseconds() {
	return timeGetTime();
}

void OsDependent::Delay(int ms) {
	Sleep(ms);
}
