
//
//		Configure for MUCOM88
//
#ifndef __mucom88config_h
#define __mucom88config_h

#define VERSION "1.7b"
#define MAJORVER 1
#define MINORVER 7

//
//		���t���O:�ȉ��̃��x���̓R���p�C���I�v�V�����Őݒ肳��܂�
//
//#define MUCOM88WIN		// Windows(WIN32) version flag
//#define MUCOM88LINUX		// Unix/Linux version flag
//#define MUCOM88IOS		// iOS version flag
//#define MUCOM88NDK		// android NDK version flag

//		�����R�[�h�t���O:�ȉ��̃��x���͎����I�ɐݒ肳��܂�
//
//#define MUCOM88UTF8		// UTF8�g�p�t���O

#ifdef MUCOM88WIN
#else
#define MUCOM88UTF8
#endif

#endif
