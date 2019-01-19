//
//		Sound Buffer Class
//		(����MIX�p�̃T�E���h�o�b�t�@)
//		(32bitPCM�f�[�^��������A32/16bit�œn���܂�)
//			onion software/onitama 2018/11
//

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "soundbuf.h"

inline int Limit(int v, int max, int min)
{
	return v > max ? max : (v < min ? min : v);
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

SoundBuf::SoundBuf( void )
{
	m_size = 0;
	m_buffer = NULL;
}


SoundBuf::~SoundBuf( void )
{
	if ( m_buffer ) {
		delete m_buffer;
	}
}


void SoundBuf::Reset(int size)
{
	m_size = size;
	m_buffer = new int32[size + SNDBUF_LIMIT];
	m_curptr = 0;
	m_endptr = size;
	m_readptr = 0;
	m_wtick = 0;
	m_rtick = 0;
	//printf( "#Alloc sound buffer %d.\n",m_size );
}


void SoundBuf::PrepareReadBuffer(void)
{
}


int SoundBuf::GetBuffer16(void *dst, int size)
{
	int i;
	int16 *p;
	int32 *src;
	//printf("#Get Buffer16(%d) %d.\n", size>>1, m_readptr);
	if (m_endptr == 0) return -1;

	if ((m_rtick + size) > m_wtick) {
		//printf("#Buffer not enough tick(%d) end(%d).\n", m_rtick, m_wtick);
		return -1;
	}

	p = (int16 *)dst;
	src = m_buffer + m_readptr;
	for (i = 0; i < size; i++){
		if (m_readptr >= m_endptr) { m_readptr = 0; src = m_buffer; }
		*p++ = Limit(*src++, 32767, -32768);
		m_readptr++;
	}
	m_rtick += size;
	return 0;
}


int SoundBuf::GetBuffer32(void *dst, int size)
{
	int i;
	int32 *p;
	int32 *src;
	if (m_endptr == 0) return -1;
	p = (int32 *)dst;
	src = m_buffer + m_readptr;
	for (i = 0; i < size; i++){
		if (m_readptr >= m_endptr) { m_readptr = 0; src = m_buffer; }
		*p++ = *src++;
		m_readptr++;
	}
	m_rtick += size;
	return 0;
}

int32 *SoundBuf::PrepareBuffer(int size)
{
	//		size����int32�o�b�t�@������
	int sz;
	int32 *p;
	sz = size; if (sz > SNDBUF_LIMIT) sz = SNDBUF_LIMIT;
	p = m_buffer + m_curptr;
	memset(p, 0, sizeof(int32)*sz);
	//printf("#Prepare buffer(%d) %d.\n", m_wtick - m_rtick, sz);
	return p;
}

void SoundBuf::UpdateBuffer(int size)
{
	//		size����int32�o�b�t�@���m�肳����
	int sz;
	sz = size; if (sz > SNDBUF_LIMIT) sz = SNDBUF_LIMIT;
	m_curptr += sz;
	if (m_curptr >= m_size) {
		//printf("#Recycle buffer(%d).\n", size);
		m_endptr = m_curptr;
		m_curptr = 0;
	}
	m_wtick += sz;
}


