#include "IntroView.h"
#include <math.h>
#include <TranslationUtils.h>
#include "PeepsWindow.h"

IntroView::IntroView(BRect frame)
 : BView(frame,"introview",B_FOLLOW_ALL,B_WILL_DRAW)
{
	font_height fh;
	GetFontHeight(&fh);
	fFontHeight=fh.ascent+fh.descent+fh.leading;

	fNoPeople=false;
	fBitmap=BTranslationUtils::GetBitmapFile("wallpaper.jpg");
	SetHighColor(0,0,0,32);
}

IntroView::~IntroView(void)
{
	delete fBitmap;
}

void IntroView::Draw(BRect rect)
{
	if(fBitmap)
	{
		SetDrawingMode(B_OP_ALPHA);
		SetBlendingMode(B_CONSTANT_ALPHA,B_ALPHA_OVERLAY);
		DrawBitmap(fBitmap,fBitmap->Bounds(),Bounds());
	}
	
	BPoint pt(10,10+fFontHeight);
	
	SetFontSize(12);
	SetDrawingMode(B_OP_BLEND);
	
	if(fNoPeople)
	{
		DrawString(TRANSLATE("To begin, choose New from the Person menu."),pt);
	}
	else
	{
		DrawString(TRANSLATE("To begin, choose a group or person from the left pane."),pt);
	}
}

void IntroView::SetNoPeopleMsg(bool value)
{
	if(fNoPeople==value)
		return;
	
	fNoPeople=value;
	Invalidate();
}
