#ifndef MRPEEPS_H
#define MRPEEPS_H

#include <Application.h>

class PeepsWindow;

enum
{
	M_SEND_EMAIL='msne',
	M_LAUNCH_BROWSER
};

class MrPeeps : public BApplication
{
public:
	MrPeeps(void);
	~MrPeeps(void);

	void AboutRequested(void);
	void InitFileTypes(void);
	void RefsReceived(BMessage *msg);
	void MessageReceived(BMessage *msg);
	
private:
	PeepsWindow *mainwindow;
};

extern bool gRestartApp;

#endif
