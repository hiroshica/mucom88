//	$Id: file.cpp,v 1.6 1999/12/28 11:14:05 cisc Exp $

#include <stdio.h>
#include <string.h>


#include "headers.h"
#include "file.h"

// ---------------------------------------------------------------------------
//	�\�z/����
// ---------------------------------------------------------------------------

FileIO::FileIO()
{
    fp = NULL;
    flags = 0;
}

FileIO::FileIO(const char* filename, uint flg)
{
    fp = NULL;
    flags = 0;
    Open(filename, flg);
}

FileIO::~FileIO()
{
    Close();
}

// ---------------------------------------------------------------------------
//	�t�@�C�����J��
// ---------------------------------------------------------------------------

bool FileIO::Open(const char* filename, uint flg)
{
    Close();

    strncpy(path, filename, _MAXPATH);

    fp = fopen(filename, "rb");
    if (!fp)
        return false;
    
    SetLogicalOrigin(0);

    return true;
}

// ---------------------------------------------------------------------------
//	�t�@�C�����Ȃ��ꍇ�͍쐬
// ---------------------------------------------------------------------------

bool FileIO::CreateNew(const char* filename)
{
    Close();

    strncpy(path, filename, _MAXPATH);

    fp = fopen(filename, "wb");
    if (!fp)
        return false;
    
    SetLogicalOrigin(0);

    return !!(flags & open);
}

// ---------------------------------------------------------------------------
//	�t�@�C������蒼��
// ---------------------------------------------------------------------------

bool FileIO::Reopen(uint flg)
{
    return false;
}

// ---------------------------------------------------------------------------
//	�t�@�C�������
// ---------------------------------------------------------------------------

void FileIO::Close()
{
    if (fp)
        fclose(fp);
    
    fp = NULL;
    
}

// ---------------------------------------------------------------------------
//	�t�@�C���k�̓ǂݏo��
// ---------------------------------------------------------------------------

int32 FileIO::Read(void* dest, int32 size)
{
    if (!fp)
        return -1;
    
    int len = (int)fread(dest, 1, size, fp);
    return len;
}

// ---------------------------------------------------------------------------
//	�t�@�C���ւ̏����o��
// ---------------------------------------------------------------------------

int32 FileIO::Write(const void* dest, int32 size)
{
    if (!fp)
        return -1;
    
    int len = (int)fwrite(dest, 1, size, fp);
    return len;
}

// ---------------------------------------------------------------------------
//	�t�@�C�����V�[�N
// ---------------------------------------------------------------------------

int32 FileIO::Seek(int32 pos, SeekMethod method)
{
	int32 retcode = -1;
    if (!fp)
        return retcode;
    
    switch (method)
    {
    case begin:	
        retcode = fseek(fp, pos, SEEK_SET);
        break;
    case current:
        retcode = fseek(fp, pos, SEEK_CUR);
        break;
    case end:		
        retcode = fseek(fp, pos, SEEK_END);
        break;
    default:
		break;
	}

    return retcode;
}

// ---------------------------------------------------------------------------
//	�t�@�C���̈ʒu�𓾂�
// ---------------------------------------------------------------------------

int32 FileIO::Tellp()
{
    if (!fp)
        return -1;
    
    return (int)ftell(fp);
}

// ---------------------------------------------------------------------------
//	���݂̈ʒu���t�@�C���̏I�[�Ƃ���
// ---------------------------------------------------------------------------

bool FileIO::SetEndOfFile()
{
    return false;
}