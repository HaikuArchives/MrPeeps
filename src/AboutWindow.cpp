#include <Application.h>
#include <AppFileInfo.h>
#include <Roster.h>
#include <String.h>
#include <stdio.h>
#include <Screen.h>
#include <TranslationUtils.h>
#include "AboutWindow.h"
#include "PeepsWindow.h"


AboutWindow::AboutWindow(uint8 aboutflags)
 : BWindow(BRect(100,100,500,400),"Mr. Peeps!", (aboutflags==0)?B_NO_BORDER_WINDOW_LOOK:B_MODAL_WINDOW_LOOK,
 	B_MODAL_APP_WINDOW_FEEL,
 	B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	BScreen screen;
	BRect screenrect(screen.Frame());
	
	aboutview=new AboutView(Bounds(), aboutflags);
	AddChild(aboutview);
	
	MoveTo( (screenrect.Width()-Frame().Width())/2, (screenrect.Height()-Frame().Height())/2 );
}

AboutWindow::~AboutWindow(void)
{
	
}

bool AboutWindow::QuitRequested(void)
{
	// This is implemented only for development purposes
//	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

AboutView::AboutView(BRect frame, uint8 aboutflags)
 : BView (frame, "AboutView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetViewColor(126,126,190);
	
	fAboutFlags=aboutflags;

	if(fAboutFlags & ABOUT_OK2)
		fLogo=BTranslationUtils::GetBitmap('JPEG',"peepslogo2.jpg");
	else
		fLogo=BTranslationUtils::GetBitmap('JPEG',"peepslogo.jpg");
	
	textstart=(fLogo)?fLogo->Bounds().Width()+10:10;
	
	app_info ai;
	version_info vi;
	be_app->GetAppInfo(&ai);
	BFile file(&ai.ref,B_READ_ONLY);
	BAppFileInfo appinfo(&file);
	appinfo.GetVersionInfo(&vi,B_APP_VERSION_KIND);

	BString variety;
	switch(vi.variety)
	{
		case 0:
			variety=TRANSLATE("Development");
			break;
		case 1:
			variety=TRANSLATE("Alpha");
			break;
		case 2:
			variety=TRANSLATE("Beta");
			break;
		case 3:
			variety=TRANSLATE("Gamma");
			break;
		case 4:
			variety=TRANSLATE("Golden Master");
			break;
		default:
			variety=TRANSLATE("Final");
			break;
	}
	
	if(variety!="Final")
	{
		sprintf(version,"%s %lu.%lu.%lu %s %lu",TRANSLATE("Version"),vi.major,vi.middle,vi.minor,
			variety.String(),vi.internal);
	}
	else
	{
		sprintf(version,"%s %lu.%lu.%lu",TRANSLATE("Version"),vi.major,vi.middle,vi.minor);
	}
	font_height height;

	fFont=*be_bold_font;
	fFont.SetSize(18.0);
	fFont.GetHeight(&height);
	namewidth=fFont.StringWidth("Mr. Peeps!");
	nameheight=height.ascent+height.descent+height.leading;

	fFont=*be_plain_font;
	fFont.SetSize(15.0);
	fFont.GetHeight(&height);
	
	versionwidth=fFont.StringWidth(version);
	versionheight=height.ascent+height.descent+height.leading;	

	fFont.SetSize(12.0);
	fFont.GetHeight(&height);

	writtenwidth=fFont.StringWidth(TRANSLATE("Written by DarkWyrm"));
	writtenheight=height.ascent+height.descent+height.leading;

	basedonwidth=fFont.StringWidth(TRANSLATE("Based on Peeps! v.10 by Justin Bishop"));
	basedonheight=height.ascent+height.descent+height.leading;	

	
	if(fAboutFlags!=ABOUT_STARTUP)
	{
		fOK=new BButton(BRect(0,0,100,100),"OKbutton",TRANSLATE("OK"),new BMessage(B_QUIT_REQUESTED));
		fOK->ResizeToPreferred();
		fOK->MoveTo(Bounds().Width()-fOK->Bounds().Width()-10,
			Bounds().Height()-fOK->Bounds().Height()-10);
		fOK->SetResizingMode(B_FOLLOW_NONE);
		AddChild(fOK);
		fOK->MakeDefault(true);
		fStatus=NULL;
	}
	else
	{
		BRect statrect(Bounds());
		fStatus=new BStatusBar(statrect,"statusbar",TRANSLATE("Reading entries"));
		fStatus->ResizeToPreferred();
		if(fStatus->Bounds().Width()!=Bounds().Width())
			fStatus->ResizeTo(Bounds().Width(),fStatus->Bounds().Height());
		fStatus->SetBarColor(ViewColor());
		fStatus->SetResizingMode(B_FOLLOW_NONE);
		AddChild(fStatus);
		fOK=NULL;
	}
}

AboutView::~AboutView(void)
{
}

void AboutView::AttachedToWindow(void)
{
	float maxwidth=MAX(versionwidth,namewidth);
	maxwidth=MAX(writtenwidth,maxwidth);
	maxwidth=MAX(basedonwidth,maxwidth);

	if(fLogo)
	{
		if(fAboutFlags!=ABOUT_STARTUP)
			Window()->ResizeTo(fLogo->Bounds().Width()+maxwidth+20,fLogo->Bounds().Height()+10);	
		else
			Window()->ResizeTo(fLogo->Bounds().Width()+maxwidth+20,fLogo->Bounds().Height()+
				fStatus->Bounds().Height()+10);	
	}

	if(fOK)
		fOK->MoveTo(Bounds().Width()-fOK->Bounds().Width()-10,
			Bounds().Height()-fOK->Bounds().Height()-10);
	if(fStatus)
		fStatus->MoveTo(0,Bounds().Height()-fStatus->Bounds().Height());
}

void AboutView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case M_SET_STATUS:
		{
			if(fStatus)
			{	
				int32 refcount;
				if(msg->FindInt32("refcount",&refcount)==B_OK)
				{
					fStatus->Reset();
					fStatus->SetMaxValue(refcount);
					fEntryCount=refcount;
					
					char string[64];
					sprintf(string,TRANSLATE("Reading %ld entries"),refcount);
					fStatus->SetText(string);
				}
			}
			break;
		}
		case M_RESET_STATUS:
		{
			if(fStatus)
			{	
				BString str(TRANSLATE("Reading Entries"));
				str+="...";
				fStatus->SetText(str.String());
				fStatus->Reset();
				fStatus->SetMaxValue(fEntryCount);
			}
			break;
		}
		case M_UPDATE_STATUS:
		{
			if(fStatus)
			{
				char string[64];
				sprintf(string,TRANSLATE("Reading %ld entries"),fEntryCount);
				fStatus->Update(1,string);
			}
			break;
		}
		default:
			BView::MessageReceived(msg);
	}
}

void AboutView::Draw(BRect update)
{
	if(fLogo)
		DrawBitmap(fLogo, BPoint(0,0));
	
	SetLowColor(ViewColor());
	
	BPoint point(textstart,nameheight+10);

	fFont=*be_bold_font;
	fFont.SetSize(18);
	SetFont(&fFont);
	
	
	if(fAboutFlags & ABOUT_OK2)
	{
		DrawString("Mrs. Peeps!",point);
	
		fFont=*be_plain_font;
		fFont.SetSize(15);
		SetFont(&fFont);
	
		point.y+=versionheight;
		DrawString("The version that got away...",point);
	}
	else
	{
		DrawString("Mr. Peeps!",point);
	
		fFont=*be_plain_font;
		fFont.SetSize(15);
		SetFont(&fFont);
	
		point.y+=versionheight;
		DrawString(version,point);
	
		fFont.SetSize(12);
		SetFont(&fFont);
	
		point.y+=writtenheight*2;
		DrawString(TRANSLATE("Written by DarkWyrm"),point);
	
		point.y+=basedonheight;
		DrawString(TRANSLATE("Based on Peeps! v.10 by Justin Bishop"),point);
	}
}
