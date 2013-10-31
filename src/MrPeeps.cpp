#include <Alert.h>
#include <Entry.h>
#include <Directory.h>
#include <File.h>
#include <Message.h>
#include <Bitmap.h>
#include <String.h>
#include <Roster.h>
#include <Mime.h>
#include <stdlib.h>
#include "PersonIcons.h"
#include "MrPeeps.h"
#include "defs.h"
#include "AboutWindow.h"
#include "PeepsWindow.h"

bool gRestartApp=false;

int main(int argc, char** argv)
{
	// this guarantees a restart only when requested
	gRestartApp=false;
	
	MrPeeps* mrpeeps = new MrPeeps();
	mrpeeps->Run();
	
	if(gRestartApp)
	{
		app_info info;
		mrpeeps->GetAppInfo(&info);
		be_roster->Launch(&info.ref);
	}
	
	delete mrpeeps;
	return(0);
}


MrPeeps::MrPeeps(void)
 :BApplication(APP_SIGNATURE)
{
	gCurrentLocale=locale_roster->LocaleAt(0);
	
	InitFileTypes();
	
	AboutWindow *ab=new AboutWindow(ABOUT_STARTUP);
	ab->Show();

	mainwindow = new PeepsWindow(ab->GetStatusMessenger());
	mainwindow->Show();

	ab->PostMessage(B_QUIT_REQUESTED);
}

MrPeeps::~MrPeeps(void)
{
}

void MrPeeps::AboutRequested(void)
{
	AboutWindow *ab;
	if(modifiers() & (B_LEFT_COMMAND_KEY | B_LEFT_SHIFT_KEY |
		 			B_LEFT_CONTROL_KEY | B_LEFT_OPTION_KEY))
		 ab=new AboutWindow(ABOUT_OK2);
	else
		 ab=new AboutWindow();
	ab->Show();
}

void MrPeeps::RefsReceived(BMessage *msg)
{
	// We need to put this wait loop into the function so that when the app
	// is started it will wait for the main window to appear.
	while(CountWindows()!=1)
		snooze(1000);
	
	// Find our window
	BWindow *win=WindowAt(0);
	
	int32 i=1;
	while(win)
	{
		if(strcmp(win->Title(),"Mr. Peeps!")==0)
		{
			win->PostMessage(msg);
			return;
		}
		win=WindowAt(i++);
	}

	if(!win)
	{
		AboutWindow *ab=new AboutWindow(ABOUT_STARTUP);
		ab->Show();
	
		mainwindow = new PeepsWindow(ab->GetStatusMessenger());
		mainwindow->Show();
	
		ab->PostMessage(B_QUIT_REQUESTED);
		mainwindow->PostMessage(msg);
	}
}

// This handles the filetypes being installed with all the extra attributes over the
// regular R5 types
void MrPeeps::InitFileTypes(void)
{
	BEntry entry("/boot/home/config/settings/MrPeeps");
	BFile file;
	
	// Check for settings directory
	if(!entry.Exists())
		create_directory("/boot/home/config/settings/MrPeeps",0777);

	entry.SetTo("/boot/home/config/settings/MrPeeps/MrPeeps");
	if(!entry.Exists())
	{
		file.SetTo("/boot/home/config/settings/MrPeeps/MrPeeps",B_READ_WRITE|B_CREATE_FILE);
		file.Unset();
	}
	
	
	entry.SetTo("/boot/home/config/settings/MrPeeps/filetypes_installed");
	if(!entry.Exists())
	{
		BFile file("/boot/home/config/settings/MrPeeps/filetypes_installed",B_CREATE_FILE);
		BMimeType mime;
		BString string;
		BMessage msg,info;
		uint8 installtype=2;
		int32 index=0;
		BBitmap	large_icon(BRect(0, 0, B_LARGE_ICON-1, B_LARGE_ICON-1), B_COLOR_8_BIT);
		BBitmap	mini_icon(BRect(0, 0, B_MINI_ICON-1, B_MINI_ICON-1), B_COLOR_8_BIT);
		
		// install person mime type
		mime.SetType(PERSON_FILE_TYPE);
		if(mime.IsInstalled()) 
		{
			if (mime.GetAttrInfo(&info) == B_OK)
			{
				while (info.FindString("attr:name", index++, &string) == B_OK)
				{
					if (string.Compare(PERSON_EMAIL5)==0)
						installtype=1;
					
					if (string.Compare(PERSON_BIRTHDAY)==0)
					{
						installtype=0;
						break;
					}
				}
				if (installtype>0)
					mime.Delete();
			}
		}
		if(installtype>0)
		{
			mime.Install();
			large_icon.SetBits(kLargePersonIcon, large_icon.BitsLength(), 0, B_COLOR_8_BIT);
			mini_icon.SetBits(kSmallPersonIcon, mini_icon.BitsLength(), 0, B_COLOR_8_BIT);
			mime.SetShortDescription("Person");
			mime.SetLongDescription("Information about a person");
			mime.SetIcon(&large_icon, B_LARGE_ICON);
			mime.SetIcon(&mini_icon, B_MINI_ICON);
			
			BAlert *alert=new BAlert("Mr. Peeps!", "Would you like to make Mr. Peeps! the "
			"default application for People files? If not sure, choose 'No'.","Yes","No");

			if(alert->Go()==0)
				mime.SetPreferredApp(APP_SIGNATURE);
			else
				mime.SetPreferredApp("application/x-vnd.Be-PEPL");
	
			// General Person data
			msg.AddString("attr:public_name", "First Name"); 
			msg.AddString("attr:name", PERSON_FIRSTNAME); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
			
			msg.AddString("attr:public_name", "Last Name");
			msg.AddString("attr:name", PERSON_LASTNAME); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Nickname"); 
			msg.AddString("attr:name", PERSON_NICKNAME); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Title"); 
			msg.AddString("attr:name", PERSON_TITLE); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Group"); 
			msg.AddString("attr:name", PERSON_GROUP); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
	
			// Personal data
			msg.AddString("attr:public_name", "Home Address"); 
			msg.AddString("attr:name", PERSON_ADDRESS); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Home Address(2)"); 
			msg.AddString("attr:name", PERSON_ADDRESS2); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "City"); 
			msg.AddString("attr:name", PERSON_CITY); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 90); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "State"); 
			msg.AddString("attr:name", PERSON_STATE); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 50); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Postal Code"); 
			msg.AddString("attr:name", PERSON_ZIP); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 50); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Country"); 
			msg.AddString("attr:name", PERSON_COUNTRY); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Home Phone"); 
			msg.AddString("attr:name", PERSON_HOME_PHONE); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 90); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			
			msg.AddString("attr:public_name", "Home Fax"); 
			msg.AddString("attr:name", PERSON_FAX); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 90); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false);
			 
			msg.AddString("attr:public_name", "Cell phone"); 
			msg.AddString("attr:name", PERSON_CELL_PHONE);
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 90); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "E-mail"); 
			msg.AddString("attr:name", PERSON_EMAIL);
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Web Page"); 
			msg.AddString("attr:name", PERSON_URL); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			// Work data
			msg.AddString("attr:public_name", "Company"); 
			msg.AddString("attr:name", PERSON_COMPANY); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Work Address"); 
			msg.AddString("attr:name", PERSON_WORK_ADDRESS); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Work Address (2)"); 
			msg.AddString("attr:name", PERSON_WORK_ADDRESS2); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Work City"); 
			msg.AddString("attr:name", PERSON_WORK_CITY); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 90); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Work State"); 
			msg.AddString("attr:name", PERSON_WORK_STATE); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 50); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Work Postal Code"); 
			msg.AddString("attr:name", PERSON_WORK_ZIP); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 50); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Work Country"); 
			msg.AddString("attr:name", PERSON_WORK_COUNTRY); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Work Phone"); 
			msg.AddString("attr:name", PERSON_WORK_PHONE); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 90); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			
			msg.AddString("attr:public_name", "Work Fax"); 
			msg.AddString("attr:name", PERSON_WORK_FAX); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 90); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false);
			 
			msg.AddString("attr:public_name", "Work Cell Phone"); 
			msg.AddString("attr:name", PERSON_WORK_CELL);
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 90); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Work E-mail"); 
			msg.AddString("attr:name", PERSON_WORK_EMAIL);
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Work Web Address"); 
			msg.AddString("attr:name", PERSON_WORK_URL); 
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Pager"); 
			msg.AddString("attr:name", PERSON_PAGER);
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 90); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
	
			// Internet stuff
			msg.AddString("attr:public_name", "E-mail 3"); 
			msg.AddString("attr:name", PERSON_EMAIL3);
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "E-mail 4"); 
			msg.AddString("attr:name", PERSON_EMAIL4);
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
			
			msg.AddString("attr:public_name", "E-mail 5"); 
			msg.AddString("attr:name", PERSON_EMAIL4);
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
		
			msg.AddString("attr:public_name", "ICQ number"); 
			msg.AddString("attr:name", PERSON_ICQ);
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "AIM number"); 
			msg.AddString("attr:name", PERSON_AIM);
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Jabber name"); 
			msg.AddString("attr:name", PERSON_JABBER);
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Yahoo messenger"); 
			msg.AddString("attr:name", PERSON_YAHOO);
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
	
			msg.AddString("attr:public_name", "Other Web Address"); 
			msg.AddString("attr:name", PERSON_OTHER_URL);
			msg.AddInt32("attr:type", B_STRING_TYPE); 
			msg.AddBool("attr:viewable", true); 
			msg.AddBool("attr:editable", true); 
			msg.AddInt32("attr:width", 120); 
			msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
			msg.AddBool("attr:extra", false); 
			
			if(installtype==2)
			{
				// Attributes used only by Mr. Peeps!
				msg.AddString("attr:public_name", "Birthday"); 
				msg.AddString("attr:name", PERSON_BIRTHDAY); 
				msg.AddInt32("attr:type", B_STRING_TYPE); 
				msg.AddBool("attr:viewable", true); 
				msg.AddBool("attr:editable", true); 
				msg.AddInt32("attr:width", 50); 
				msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
				msg.AddBool("attr:extra", false); 
	
				msg.AddString("attr:public_name", "Anniversary"); 
				msg.AddString("attr:name", PERSON_ANNIVERSARY); 
				msg.AddInt32("attr:type", B_STRING_TYPE); 
				msg.AddBool("attr:viewable", true); 
				msg.AddBool("attr:editable", true); 
				msg.AddInt32("attr:width", 50); 
				msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
				msg.AddBool("attr:extra", false); 
	
				msg.AddString("attr:public_name", "Spouse"); 
				msg.AddString("attr:name", PERSON_SPOUSE); 
				msg.AddInt32("attr:type", B_STRING_TYPE); 
				msg.AddBool("attr:viewable", true); 
				msg.AddBool("attr:editable", true); 
				msg.AddInt32("attr:width", 50); 
				msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
				msg.AddBool("attr:extra", false); 
	
				msg.AddString("attr:public_name", "Children"); 
				msg.AddString("attr:name", PERSON_CHILDREN); 
				msg.AddInt32("attr:type", B_STRING_TYPE); 
				msg.AddBool("attr:viewable", true); 
				msg.AddBool("attr:editable", true); 
				msg.AddInt32("attr:width", 50); 
				msg.AddInt32("attr:alignment", B_ALIGN_LEFT); 
				msg.AddBool("attr:extra", false); 
			}
			
			mime.SetAttrInfo(&msg);
		}
	}
}

void MrPeeps::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case M_SEND_EMAIL:
		{
			BString address;
			if(msg->FindString("address",&address)!=B_OK)
				break;
			
			BString cmdstring("/boot/beos/apps/BeMail mailto:");
			cmdstring+=address;
			cmdstring+=" &";
			system(cmdstring.String());
			break;
		}
		case M_LAUNCH_BROWSER:
		{
			BString address;
			if(msg->FindString("address",&address)!=B_OK)
				break;
			
			BString cmdstring("/boot/beos/apps/NetPositive ");
			cmdstring+=address;
			cmdstring+=" &";
			system(cmdstring.String());
			break;
		}
		default:
		{
			BApplication::MessageReceived(msg);
		}
	}
}
