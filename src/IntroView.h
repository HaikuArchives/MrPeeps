#ifndef INTROVIEW_H_
#define INTROVIEW_H_

#include <View.h>
#include <Font.h>
#include <Bitmap.h>

class IntroView : public BView
{
public:
	IntroView(BRect frame);
	~IntroView(void);
	void Draw(BRect rect);
	void SetNoPeopleMsg(bool value);
	bool UsesNoPeopleMsg(void) const { return fNoPeople; }
	
protected:
	float fFontHeight;
	bool fNoPeople;
	BBitmap *fBitmap;
};

#endif
