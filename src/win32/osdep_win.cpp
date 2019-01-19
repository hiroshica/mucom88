
//
//		OsDependent win32
//		BouKiCHi 2019
//		(OS�ˑ��̃��[�`�����܂Ƃ߂܂�)
//		onion software/onitama 2019/1
//

#include <stdio.h>
#include <windows.h>
#include "osdep_win.h"
#include "realchip.h"
#include "soundds.h"

#define USE_SCCI
#define USE_HIGH_LEVEL_COUNTER

/*-------------------------------------------------------------------------------*/

//
//		windows debug support
//
void Alertf(const char *format, ...)
{
	char textbf[4096];
	va_list args;
	va_start(args, format);
	vsprintf(textbf, format, args);
	va_end(args);
	MessageBox(NULL, textbf, "error", MB_ICONINFORMATION | MB_OK);
}

/*-------------------------------------------------------------------------------*/

OsDependentWin32::OsDependentWin32(void)
{
	snddrv = NULL;
	master_window = NULL;
	RealChipInstance = NULL;

	MuteAudio = false;
	sending = false;
	threadflag = false;

	UserTimerCallback = new TimerCallback;
	UserAudioCallback = new AudioCallback;
}

OsDependentWin32::~OsDependentWin32(void)
{
	if (UserTimerCallback) delete UserTimerCallback;
	if (UserAudioCallback) delete UserAudioCallback;
	if (snddrv != NULL) delete snddrv;
}

// COM�̏�����
bool OsDependentWin32::CoInitialize() {
	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) return false;
	return true;
}


// ���`�b�v
bool OsDependentWin32::InitRealChip() {
	realchip *rc = new realchip();
	rc->Initialize();
	if (!rc->IsRealChip()) {
		RealChipInstance = NULL;			// �������Ɏ��s������g�p���Ȃ�
		return false;
	}
	RealChipInstance = rc;
	return true;
}

void OsDependentWin32::FreeRealChip() {
	if (!RealChipInstance) return;
	realchip *rc = (realchip *)RealChipInstance;
	rc->UnInitialize();
	delete rc;
	RealChipInstance = NULL;
}


int OsDependentWin32::CheckRealChip() {
	if (!RealChipInstance) return -1;
	return 0;
}

void OsDependentWin32::ResetRealChip() {
	if (!RealChipInstance) return;
	realchip *rc = (realchip *)RealChipInstance;
	rc->Reset();
}


void OsDependentWin32::OutputRealChip(unsigned int Register, unsigned int Data) {
	if (!RealChipInstance) return;
	realchip *rc = (realchip *)RealChipInstance;

	rc->SetRegister(Register, Data);
}

void OsDependentWin32::OutputRealChipAdpcm(void *pData, int size)
{
	if (!RealChipInstance) return;
	if (CheckRealChip()) return;
	realchip *rc = (realchip *)RealChipInstance;

	rc->SendAdpcmData(pData, (DWORD)size);
}


// �^�C�}�[
bool OsDependentWin32::InitTimer() {

	timerid = 0;

	TIMECAPS caps;
	if (timeGetDevCaps(&caps, sizeof(TIMECAPS)) == TIMERR_NOERROR) {
		// �}���`���f�B�A�^�C�}�[�̃T�[�r�X���x���ő��
		HANDLE myth = GetCurrentThread();
		SetThreadPriority(myth, THREAD_PRIORITY_HIGHEST);

		timer_period = caps.wPeriodMin;
		timeBeginPeriod(timer_period);
		timerid = timeSetEvent(timer_period, caps.wPeriodMin, TimeProc, reinterpret_cast<DWORD>(this), (UINT)TIME_PERIODIC);
		if (!timerid)
		{
			timeEndPeriod(timer_period);
		}
	}
	else {
		//	���s������
		timer_period = -1;
		timerid = 0;
		MessageBox(NULL, "Unable to start timer.", "Error", 0);
	}

	return false;
}

void OsDependentWin32::FreeTimer() {

	if (timerid)
	{
		timeKillEvent(timerid);
		timeEndPeriod(timer_period);
		timerid = 0;
	}
	//	�X���b�h���~����
	StopThread();
}

static void CALLBACK TimerProc(UINT uid, UINT, DWORD user, DWORD, DWORD) {
	if (!user) return;
	OsDependent* inst = (OsDependent *)(user);
	inst->UpdateTimer();
}

void OsDependentWin32::UpdateTimer()
{
	int tick = GetElapsedTime();
	UserTimerCallback->tick = tick;
	UserTimerCallback->Run();
}

// ����
int OsDependentWin32::GetMilliseconds() {
	return timeGetTime();
}

void OsDependentWin32::Delay(int ms) {
	Sleep(ms);
}

int OsDependentWin32::StartThread(void)
{
	// �X�g���[���X���b�h���J�n����

	// �C�x���g�I�u�W�F�N�g���쐬����
	hevent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// �X���b�h�𐶐�����
	hthread = CreateThread(NULL, 0x40000, (LPTHREAD_START_ROUTINE)OsDependentWin32::vThreadFunc, (LPVOID)this, 0, &threadid);
	if (hthread == NULL) {
		return -1;
	}
	// �X���b�h�̗D�揇�ʂ�ύX����
	SetThreadPriority(hthread, THREAD_PRIORITY_TIME_CRITICAL);
	return 0;
}


DWORD WINAPI OsDependentWin32::vThreadFunc(LPVOID pParam) {
	OsDependentWin32 *pThis = (OsDependentWin32 *)pParam;
	pThis->ThreadFunc();
	return 0;
}

int OsDependentWin32::StopThread(void)
{
	// �X�g���[���X���b�h���~����
	if (threadflag == FALSE) return -1;

	// �C�x���g�I�u�W�F�N�g�j��
	threadflag = FALSE;
	SetEvent(hevent);
	CloseHandle(hevent);
	hevent = NULL;
	// �X���b�h��~��҂�
	DWORD	dActive = 0;
	GetExitCodeThread(hthread, &dActive);
	if (dActive == STILL_ACTIVE) {
		WaitForSingleObject(hthread, INFINITE);
	}
	// �X���b�h�j��
	CloseHandle(hthread);

	hthread = NULL;
	threadid = 0;

	return 0;
}


void OsDependentWin32::ResetTime()
{
#ifdef USE_HIGH_LEVEL_COUNTER
	GetSystemTimeAsFileTime((FILETIME *)&last_ft);
#else
	last_tick = timeGetTime();
#endif
}


int OsDependentWin32::GetElapsedTime()
{
#ifdef USE_HIGH_LEVEL_COUNTER

	int64_t cur_ft;		// 100�i�m�b�P�ʂ̎���
	int64_t pass_ft;
	double ms;
	GetSystemTimeAsFileTime((FILETIME *)&cur_ft);
	pass_ft = cur_ft - last_ft;
	last_ft = cur_ft;

	ms = ((double)pass_ft) * (0.0001 * TICK_FACTOR);	// 1�~���b=1024 �P�ʂɒ���
	//printf( "(%f)\n",ms );

	return (int)(ms);

#else
	int curtime;
	curtime = timeGetTime();
	pass_tick = curtime - last_tick;
	last_tick = curtime;
	return 1024 * pass_tick;
#endif
}


void OsDependentWin32::ThreadFunc() {
	// �X�g���[���X���b�h���[�v
	threadflag = true;
	while (threadflag) {
		Sleep(20);
		//if (WaitForSingleObject(hevent, 20) == WAIT_TIMEOUT) {
		//	continue;
		//}
		//StreamSend(16);
		snddrv->PrepareSend();
		snddrv->Send();
	}
}

//  TimeProc
//
void CALLBACK OsDependentWin32::TimeProc(UINT uid, UINT, DWORD user, DWORD, DWORD)
{
	OsDependentWin32* inst = reinterpret_cast<OsDependentWin32*>(user);
	if (inst){
		//SetEvent(inst->hevent);
		inst->UpdateTimer();
	}
}

// �I�[�f�B�I
bool OsDependentWin32::InitAudio(void *hwnd, int Rate, int BufferSize) {

	snddrv = new WinSoundDriver::DriverDS;
	if (hwnd) snddrv->SetHWND((HWND)hwnd);
	if (!snddrv->Init(Rate, 2, BufferSize)) return false;

	SamplePerTick = ((double)Rate / 1000);
	UpdateSamples = 0.0;

	//		��s����T�E���h�o�b�t�@������Ă���
	//
	int size = Rate * 40 / 1000 * 2;
	snddrv->GetSoundBuffer()->PrepareBuffer(size);
	snddrv->GetSoundBuffer()->UpdateBuffer(size);
	//pooltime = snddrv->GetSoundBuffer()->GetPoolSize();

	//		�^�C�}�[������
	//
	//	�X�g���[���p�X���b�h
	StartThread();

	return true;
}

void OsDependentWin32::FreeAudio() {
	if (!snddrv) return;
	WaitSendingAudio();
	delete snddrv;
	snddrv = NULL;
}

void OsDependentWin32::WaitSendingAudio() {
	for (int i = 0; i < 300 && sending; i++) Sleep(10);
}

bool OsDependentWin32::SendAudio(int ms)
{
	StreamSend(ms);
	//SetEvent(hevent);
	return true;
}


void OsDependentWin32::StreamSend(int ms)
{
	int i;
	if (!snddrv) return;

	// 0�ȊO�̓X���b�h���d�����Ă���̂ő��s���Ȃ��B
	int ret = InterlockedExchange(&sending, 1);
	if (ret != 0) return;

	int writelength;
	writelength = 0;// snddrv->PrepareSend();

	UpdateSamples = (ms * SamplePerTick * sizeof(short) * 2 );
	i = (int)UpdateSamples;
	if (i>writelength ) {
		writelength = i;
	}

	if (writelength) {
		int32 *smp;
		int size = writelength >> 2;
		smp = snddrv->GetSoundBuffer()->PrepareBuffer(size*2);

		if (!MuteAudio) {
			UserAudioCallback->mix = smp;
			UserAudioCallback->size = size;
			UserAudioCallback->Run();
		}

		snddrv->GetSoundBuffer()->UpdateBuffer(size*2);
	}
	//writelength = snddrv->PrepareSend();
	//snddrv->Send();

	// �I��
	InterlockedExchange(&sending, 0);
	return;
}



//	MUCOM�v���O�C�������p
//

int OsDependentWin32::InitPlugin(Mucom88Plugin *plg, char *filename)
{
	return 0;
}


int OsDependentWin32::ExecPluginVMCommand(int, int, int, void *, void *)
{
	return 0;
}


int OsDependentWin32::ExecPluginEditorCommand(int, int, int, void *, void *)
{
	return 0;
}


