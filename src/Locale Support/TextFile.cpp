#include "TextFile.h"
#include <stdio.h>
#include <OS.h>
#include <string.h>

TextFile::TextFile(const char *path, uint32 openmode)
 : BFile(path,openmode)
{
	fReadBuffer=new char[4096];
	fReadBufferSize=4096;
	
	if(InitCheck()==B_OK)
	{
		GetSize(&fBufferSize);
		fBuffer=new char[fBufferSize+1];
		
		Read(fBuffer,fBufferSize);
		Seek(0,SEEK_SET);
		
		fBuffer[fBufferSize]=0;
	}
	else
	{
		fBuffer=NULL;
		fBufferSize=0;
	}
}

TextFile::~TextFile(void)
{
	delete [] fBuffer;
	delete [] fReadBuffer;
}

const char *TextFile::ReadLine(void)
{
	off_t pos=Position();
	
	char *c=fBuffer;
	c+=sizeof(char) * pos;
	
	char *eol=strchr(c,'\n');
	
	if(!eol)
	{
		// This means that there are no more linefeeds before the the
		// end of the file
		eol=strchr(c,'\0');
		if(!eol)
			return NULL;
	}
	
	int32 length=eol-c;
	
	if(length>fReadBufferSize)
	{
		fReadBufferSize=length;
		delete fReadBuffer;
		fReadBuffer=new char[fReadBufferSize];
	}
	
	strncpy(fReadBuffer,c,length);
	fReadBuffer[length]=0;
	
	Seek(length+1, SEEK_CUR);
	
	// do we need to add a \0?
	return fReadBuffer;
}
