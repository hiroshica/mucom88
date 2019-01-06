
//
//	mucom : OpenMucom88 Command Line Tool
//			MUCOM88 by Yuzo Koshiro Copyright 1987-2019(C) 
//			Windows version by onion software/onitama 2018/11
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cmucom.h"

//#define DEBUG_MUCOM

#define DEFAULT_OUTFILE "mucom88.mub"

#define RENDER_RATE 44100
#define RENDER_SECONDS 90

/*----------------------------------------------------------*/

static void usage1( void )
{
static 	char *p[] = {
	"usage: mucom88 [options] [filename]",
	"       -p [filename] setload PCM file",
	"       -v [filename] set load voice file",
	"       -o [filename] set output file",
	"       -c [filename] compile mucom88 MML",
	"       -i [filename] info print mucom88 MML",
	"       -e Use external ROM files",
	"       -s Use SCCI device",
	"       -k Skip PCM load",
	"       -x Record WAV file",
	"       -l [n] Set Recording lengh to n seconds ",
	NULL };
	int i;
	for (i = 0; p[i]; i++) {
		printf("%s\n", p[i]);
	}
}

void WriteWORD(unsigned char *p, unsigned short v) {
    p[0] = (v & 0xff);
    p[1] = ((v>>8) & 0xff);
}

void WriteDWORD(unsigned char *p, unsigned long v) {
    p[0] = (v & 0xff);
    p[1] = ((v>>8) & 0xff);
    p[2] = ((v>>16) & 0xff);
    p[3] = ((v>>24) & 0xff);
}


// WAVヘッダ出力
void WriteWavHeader(FILE *fp, int frequency, int bits, int channels, long samples) {
  if (!fp) return;
  unsigned char hdr[0x80];
  int bytes = (bits/8);
  long pcm_bytesize = samples * channels * bytes;

  memcpy(hdr,"RIFF", 4);
  WriteDWORD(hdr + 4, pcm_bytesize + 44);
  memcpy(hdr + 8,"WAVEfmt ", 8);
  WriteDWORD(hdr + 16, 16); // chunk length
  WriteWORD(hdr + 20, 01); // pcm id
  WriteWORD(hdr + 22, channels); // ch
  WriteDWORD(hdr + 24, frequency); // freq
  WriteDWORD(hdr + 28, frequency * channels * bytes); // bytes per sec
  WriteWORD(hdr + 32, channels * bytes); // bytes per frame
  WriteWORD(hdr + 34, bits); // bits

  memcpy(hdr + 36, "data",4);
  WriteDWORD(hdr + 40, pcm_bytesize); // pcm size

  // 先頭のヘッダを更新
  fseek(fp, 0, SEEK_SET);
  fwrite(hdr, 44, 1, fp);

  // ファイルポインタを末尾に戻す
  fseek(fp, 0, SEEK_END);
}

void RecordWave(CMucom *m, int seconds) {
	int buf[512];
	short out[512];

	int rate = RENDER_RATE;
	int bits = 16;
	int channels = 2;
	long total_samples = 0;

	FILE *fp = fopen("output.wav","wb");
	if (fp == NULL) return;

	WriteWavHeader(fp, rate, bits, channels, total_samples);

	long ms = 0;
	while(total_samples < RENDER_RATE * seconds) {
		int samples = 16;
		m->RenderAudio(buf, samples);

		for(int i=0; i < samples*2; i++) {
			int v=buf[i];
			out[i] = v > 32767 ? 32767 : (v < -32768 ? -32768 : v);
		}
		fwrite(out, samples*4, 1, fp);
		total_samples += samples;
	}

	WriteWavHeader(fp, rate, bits, channels, total_samples);
	fclose(fp);
}

/*----------------------------------------------------------*/

int main( int argc, char *argv[] )
{
	char a1,a2;
	int b,st;
	int cmpopt,ppopt;
	int scci_opt;
	char fname[1024];
	char *pcmfile;
	char *outfile;
	char *voicefile;

	//	check switch and prm

#ifdef DEBUG_MUCOM
	{
		CMucom mucom;

		mucom.Init();

		mucom.Reset(0);
		//mucom.ProcessFile("test.muc");
		//mucom.PrintInfoBuffer();

		mucom.LoadPCM();
		mucom.LoadMusic("test2.mub");
		mucom.Play(0);
		mucom.PrintInfoBuffer();

		//mucom.Reset(2);
		//mucom.LoadPCM();
		//mucom.CompileFile("sampl1.muc","test2.mub");

		puts(mucom.GetMessageBuffer());

		while (1) {
			Sleep(20);
		}


		return 0;
	}
#endif

	if (argc<2) { usage1();return -1; }

	st = 0; ppopt = 0; cmpopt = 0; scci_opt = 0;
	pcmfile = MUCOM_DEFAULT_PCMFILE;
	outfile = DEFAULT_OUTFILE;
	voicefile = NULL;
	fname[0] = 0;

	int song_length = RENDER_SECONDS;

	for (b=1;b<argc;b++) {
		a1=*argv[b];a2=tolower(*(argv[b]+1));
		if (a1!='-') {
			strcpy(fname,argv[b]);
		} else {
			switch (a2) {
			case 'p':
				pcmfile = argv[b + 1]; b++;
				ppopt = 0;
				break;
			case 'v':
				voicefile = argv[b + 1]; b++;
				break;
			case 'o':
				outfile = argv[b + 1]; b++;
				break;
			case 'l':
				song_length = atoi(argv[b + 1]); b++;
				break;
			case 'c':
				cmpopt |= 2;
				break;
			case 'e':
				cmpopt |= 1;
				break;
			case 'k':
				ppopt = 1;
				break;
			case 'i':
				cmpopt |= MUCOM_CMPOPT_INFO;
				break;
			case 's':
				scci_opt = 1;
				break;
			case 'x':
				cmpopt |= MUCOM_CMPOPT_STEP;
				break;
			default:
				st=1;break;
			}
		}
	}

	if (st) { printf("#Illegal switch selected.\n");return 1; }
	if (fname[0]==0) { printf("#No file name selected.\n");return 1; }

	//		call main
	CMucom mucom;

	if (cmpopt & 8) {
		mucom.Init(NULL,cmpopt,RENDER_RATE);
	} else
		mucom.Init();

	if (scci_opt) {
		printf("Use SCCI.\n");
		mucom.SetVMOption(MUCOM_OPTION_SCCI | MUCOM_OPTION_FMMUTE, 1);
	}

	mucom.Reset(cmpopt);
	st = 0;

	if (cmpopt & MUCOM_CMPOPT_INFO) {
		mucom.ProcessFile(fname);
		mucom.PrintInfoBuffer();
		puts(mucom.GetMessageBuffer());
		return 0;
	}

	if (cmpopt & MUCOM_CMPOPT_COMPILE) {
		if (ppopt == 0) {
			mucom.LoadPCM(pcmfile);
		}
		if (voicefile != NULL) {
			mucom.LoadFMVoice(voicefile);
		}
		if (mucom.CompileFile(fname, outfile) < 0) {
			st = 1;
		}
		// 現状はコンパイル時は再生できない
		st = 1;
	} else {
		if (mucom.LoadMusic(fname) < 0) {
			st = 1;
		}
	}
	mucom.PrintInfoBuffer();
	puts(mucom.GetMessageBuffer());

	if (st) return st;

	if (cmpopt & MUCOM_CMPOPT_COMPILE) {
		mucom.Reset(0);
		mucom.Stop();
		mucom.PlayMemory();
	} else {
		if (mucom.Play(0) < 0) return -1;
	}

	if (cmpopt & MUCOM_CMPOPT_STEP) {
		RecordWave(&mucom, song_length);
	} else {
		mucom.PlayLoop();
	}

	return 0;
}

