
//
//	cmucomvm.cpp structures
//
#ifndef __mucomvm_h
#define __mucomvm_h

#include <stdio.h>
#include "Z80/Z80.h"
#include "fmgen/opna.h"

#include "mucom88config.h"
#include "osdep.h"
#include "membuf.h"

//#define DEBUGZ80_TRACE

enum {
	VMFLAG_NONE, VMFLAG_EXEC, VMFLAG_HALT
};

#define VM_OPTION_FMMUTE 1
#define VM_OPTION_SCCI 2
#define VM_OPTION_FASTFW 4
#define VM_OPTION_STEP 8

#define VMBANK_MAX 4
#define VMBANK_SIZE 0x4000

#define OPNACH_MAX 16
#define OPNACH_FM 0
#define OPNACH_PSG 6
#define OPNACH_RHYTHM 9
#define OPNACH_ADPCM 10
#define OPNAREG_MAX 0x200

class mucomvm : public Z80 {
public:
	mucomvm();
	~mucomvm();

	//		Z80コントロール
	void InitSoundSystem(int Rate);
	void SetOption(int option);
	int GetOption(void) { return m_option; }
	void SetPC(uint16_t adr);
	void CallAndHalt(uint16_t adr);
	int CallAndHalt2(uint16_t adr, uint8_t code);
	int CallAndHaltWithA(uint16_t adr, uint8_t areg);
	int ExecUntilHalt(int times = 0x10000);

	//		仮想マシンコントロール
	void Reset(void);
	void ResetFM(void);
	int LoadPcm(const char *fname, int maxpcm = 32);
	int LoadPcmFromMem(const char *buf, int sz, int maxpcm = 32);
	int LoadMem(const char *fname, int adr, int size);
	int SendMem(const unsigned char *src, int adr, int size);
	int SaveMem(const char *fname,int adr, int size);
	int SaveMemExpand(const char *fname, int adr, int size, char *header, int hedsize, char *footer, int footsize, char *pcm, int pcmsize);
	char *LoadAlloc(const char *fname, int *sizeout);
	void SetVolume(int fmvol, int ssgvol);
	void SetFastFW(int value);
	void SkipPlay(int count);
	void PlayLoop(void);

	//		仮想マシンステータス
	int GetFlag(void) { return m_flag; }
	int Peek(uint16_t adr);
	int Peekw(uint16_t adr);
	void Poke(uint16_t adr, uint8_t data);
	void Pokew(uint16_t adr, uint16_t data);
	void PeekToStr(char *out, uint16_t adr, uint16_t length);
	void BackupMem(uint8_t *mem_bak);
	void RestoreMem(uint8_t *mem_bak);

	void ResetTimer(void);
	void StartINT3(void);
	void StopINT3(void);
	void SetWindow(void *window) { master_window = window; }
	void SetIntCount(int value) { time_intcount = value; }
	int GetIntCount(void) { return time_intcount; }
	int GetMasterCount(void) { return time_master; }
	int GetPassTick(void) { return pass_tick; }

	//		YM2608ステータス
	void SetChMuteAll(bool sw);
	void SetChMute(int ch, bool sw);
	bool GetChMute(int ch);
	int GetChStatus(int ch);

	//		デバッグ用
	void Msgf(const char *format, ...);
	char *GetMessageBuffer(void) { return membuf->GetBuffer(); }
	void DumpBin(uint16_t adr, uint16_t length);
	int DeviceCheck(void);
	int GetMessageId() { return msgid; }

	//		ADPCM用
	int ConvertWAVtoADPCMFile(const char *fname, const char *sname);

	//		BANK切り替え
	void ClearBank(void);
	void ChangeBank(int bank);

	//		CHDATA用
	void InitChData(int chmax, int chsize);
	void SetChDataAddress(int ch, int adr);
	uint8_t *GetChData(int ch);
	uint8_t GetChWork(int index);
	void ProcessChData(void);

	//		オーディオ書き込み
	void RenderAudio(void *mix, int size);
	void AudioCallback(void *mix, int size);
	void UpdateTime(int tick);
	void UpdateCallback(int tick);

private:
	//		Z80
	int32_t load(uint16_t adr);
	void store(uint16_t adr, uint8_t data);
	int32_t input(uint16_t adr);
	void output(uint16_t adr, uint8_t data);
	void Halt(void);

	//		メモリ(64K)
	int m_flag;
	int m_option;						// 動作オプション
	int m_fastfw;						// 早送りカウント
	int m_fmvol, m_ssgvol;				// ボリューム
	int bankmode;						// BANK(3=MAIN/012=BGR)
	uint8_t mem[0x10000];				// メインメモリ
	uint8_t vram[VMBANK_MAX][0x4000];	// GVRAM(3:退避/012=BRG)

	//		音源
	FM::OPNA *opn;

	int sound_reg_select;
	int sound_reg_select2;
	void FMOutData(int data);
	void FMOutData2(int data);
	int FMInData();
	int FMInData2();

	//		OPNA情報スタック
	int Rate;
	uint8_t chmute[OPNACH_MAX];
	uint8_t chstat[OPNACH_MAX];
	uint8_t regmap[OPNAREG_MAX];

	//		CH情報スタック
	int channel_max, channel_size;
	uint16_t pchadr[64];
	uint8_t *pchdata;
	uint8_t pchwork[16];

	//		割り込みタイマー関連
	int time_master;
	int time_scount;
	int time_intcount;
	int pass_tick;
	int last_tick;

	bool playflag;
	bool busyflag;
	bool int3flag;
	int predelay;
	int int3mask;
	int msgid;

	void checkThreadBusy(void);

	//		メッセージバッファ
	CMemBuf *membuf;

	//		OS依存部分
	OsDependent *osd;
	void *master_window;

};



#endif
