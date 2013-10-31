#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <Button.h>
#include <Messenger.h>
#include <Font.h>
#include <StatusBar.h>

enum
{
	ABOUT_STARTUP=0,
	ABOUT_OK,
	ABOUT_OK2=2
};

enum
{
	M_RESET_STATUS='mrst',
	M_SET_STATUS='msst',
	M_UPDATE_STATUS='mups'
};

class AboutView : public BView
{
public:
	AboutView(BRect frame, uint8 aboutflags);
	~AboutView(void);
	void AttachedToWindow(void);
	void Draw(BRect update);
	void MessageReceived(BMessage *msg);

	BBitmap *fLogo;
	BFont fFont;
	BButton *fOK;
	BStatusBar *fStatus;
	
	char version[64];
	float versionwidth, writtenwidth, namewidth, basedonwidth;
	float versionheight, writtenheight, nameheight, basedonheight;
	float textstart;
		
	uint8 fAboutFlags;
	int32 fEntryCount;
};

class AboutWindow : public BWindow
{
public:
	AboutWindow(uint8 aboutflags=ABOUT_OK);
	~AboutWindow(void);
	virtual bool QuitRequested(void);
	
	BMessenger GetStatusMessenger(void) { return BMessenger(aboutview); }
	AboutView *aboutview;
};

#endif