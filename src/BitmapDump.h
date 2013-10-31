#ifndef BITMAPDUMP_H
#define BITMAPDUMP_H

#include <TranslatorRoster.h>

class BitmapDumper
{
public:
	BitmapDumper(void);
	~BitmapDumper(void);
	
	status_t DumpToJPEG(BBitmap *bmp, BPositionIO *dest);

private:
	BTranslatorRoster *roster;
};

#endif
