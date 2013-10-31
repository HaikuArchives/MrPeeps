#include <Application.h>
#include <MenuField.h>
#include <MenuBar.h>
#include <ScrollView.h>
#include <Directory.h>
#include <Entry.h>
#include <Path.h>
#include <String.h>
#include <Roster.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Query.h>
#include <fs_attr.h>
#include <stdlib.h>
#include <stdio.h>
#include <OS.h>
#include <NodeInfo.h>
#include <VolumeRoster.h>
#include <Volume.h>
#include <Alert.h>
#include <NodeMonitor.h>

#include "MrPeeps.h"
#include "PeepsWindow.h"
#include "PeepsItem.h"
#include "PeopleList.h"
#include "DataView.h"
#include "ListData.h"
#include "defs.h"
#include "AboutWindow.h"
#include "IntroView.h"

//geometry
#ifndef FIELD_DIVIDE
#define FIELD_DIVIDE	90
#define VERTICAL_DIVIDE	30
#endif

#define PEOPLE_FOLDER "/boot/home/people/"

BList gPeopleData;
GroupList gGroupData;
Locale *gCurrentLocale;

#define M_REMOVE_IMAGE 'mrmi'
#define M_PASTE_IMAGE 'mpsi'

PeepsWindow::PeepsWindow(BMessenger target)
	: BWindow(BRect(50,50,600,470), "Mr. Peeps!", B_DOCUMENT_WINDOW, B_NOT_ZOOMABLE | B_NOT_H_RESIZABLE)
{
	float wmin,wmax,hmin,hmax;
	
	GetSizeLimits(&wmin,&wmax,&hmin,&hmax);
	wmin=500;
	hmin=400;
	
	SetSizeLimits(wmin,wmax,hmin,hmax);
		
	ReadLocaleSettings();
	if(fLocale.CountChars()>0)
	{
		locale_roster->SetLocale(fLocale.String());
		gCurrentLocale=locale_roster->GetLocale();
	}
	
	BRect r(Bounds());
	BMessage *msg;
	
	r.bottom=20;
	BMenuBar *mb=new BMenuBar(r,"menubar");
	AddChild(mb);
	
	fPeopleMenu=new BMenu(TRANSLATE("Person"));
	fPeopleMenu->AddItem(new BMenuItem(TRANSLATE("New"),new BMessage(M_ADD_PERSON),'N'));
	fPeopleMenu->AddSeparatorItem();
	fPeopleMenu->AddItem(new BMenuItem(TRANSLATE("Previous"),new BMessage(M_PREV_ITEM),B_UP_ARROW));
	fPeopleMenu->AddItem(new BMenuItem(TRANSLATE("Next"),new BMessage(M_NEXT_ITEM),B_DOWN_ARROW));
	fPeopleMenu->AddSeparatorItem();
	
	fActionsMenu=new BMenu(TRANSLATE("Actions"));
		
	// Submenu for sending e-mail
	BMenu *emailmenu=new BMenu(TRANSLATE("Send E-Mail to"));
	
	fEmailPersonalItem=new BMenuItem(TRANSLATE("Personal Address"),new BMessage(M_SEND_PERSONAL_EMAIL));
	emailmenu->AddItem(fEmailPersonalItem);
	
	fEmailWorkItem=new BMenuItem(TRANSLATE("Work Address"),new BMessage(M_SEND_WORK_EMAIL));
	emailmenu->AddItem(fEmailWorkItem);
	
	fEmailAltItem=new BMenuItem(TRANSLATE("Alternate Address"),new BMessage(M_SEND_ALT_EMAIL));
	emailmenu->AddItem(fEmailAltItem);
	fActionsMenu->AddItem(emailmenu);
	
	// Submenu for sending e-mail
	BMenu *browsermenu=new BMenu(TRANSLATE("Go to Web Page"));
	
	fBrowseHomeItem=new BMenuItem(TRANSLATE("Personal Web Address"),new BMessage(M_BROWSE_WWW_HOME));
	browsermenu->AddItem(fBrowseHomeItem);
	
	fBrowseWorkItem=new BMenuItem(TRANSLATE("Work Web Address"),new BMessage(M_BROWSE_WWW_WORK));
	browsermenu->AddItem(fBrowseWorkItem);
	
	fBrowseAltItem=new BMenuItem(TRANSLATE("Alternate Web Address"),new BMessage(M_BROWSE_WWW_ALT));
	browsermenu->AddItem(fBrowseAltItem);
	fActionsMenu->AddItem(browsermenu);
	
	fActionsMenu->AddSeparatorItem();
	fPastePhotoItem=new BMenuItem(TRANSLATE("Paste Photo from Clipboard"),new BMessage(M_PASTE_IMAGE));
	fActionsMenu->AddItem(fPastePhotoItem);
	fRemovePhotoItem=new BMenuItem(TRANSLATE("Remove Photo"),new BMessage(M_REMOVE_IMAGE));
	fActionsMenu->AddItem(fRemovePhotoItem);
	fActionsMenu->AddSeparatorItem();
	fActionsMenu->AddItem(new BMenuItem(TRANSLATE("Move To Trash"),new BMessage(M_DELETE_PERSON),'T'));
	fPeopleMenu->AddItem(fActionsMenu);
	fActionsMenu->SetEnabled(false);
	
	
	fPeopleMenu->AddSeparatorItem();
	fPeopleMenu->AddItem(new BMenuItem(TRANSLATE("About..."),new BMessage(B_ABOUT_REQUESTED)));
	mb->AddItem(fPeopleMenu);

	fEditMenu=new BMenu(TRANSLATE("Edit"));
	fEditMenu->AddItem(new BMenuItem(TRANSLATE("Undo"),new BMessage(M_UNDO),'Z'));
	fEditMenu->AddSeparatorItem();
	fEditMenu->AddItem(new BMenuItem(TRANSLATE("Cut"),new BMessage(M_CUT),'X'));
	fEditMenu->AddItem(new BMenuItem(TRANSLATE("Copy"),new BMessage(M_COPY),'C'));
	fEditMenu->AddItem(new BMenuItem(TRANSLATE("Paste"),new BMessage(M_PASTE),'V'));
	mb->AddItem(fEditMenu);
	fEditMenu->SetEnabled(false);
	
	fTabMenu=new BMenu(TRANSLATE("Tab"));
	fTabMenu->AddItem(new BMenuItem(TRANSLATE("Main"),new BMessage(M_TAB_1),'1'));
	fTabMenu->AddItem(new BMenuItem(TRANSLATE("Personal"),new BMessage(M_TAB_2),'2'));
	fTabMenu->AddItem(new BMenuItem(TRANSLATE("Work"),new BMessage(M_TAB_3),'3'));
	fTabMenu->AddItem(new BMenuItem(TRANSLATE("Instant Message"),new BMessage(M_TAB_4),'4'));
	fTabMenu->AddItem(new BMenuItem(TRANSLATE("Notes"),new BMessage(M_TAB_5),'5'));
	mb->AddItem(fTabMenu);
	fTabMenu->SetEnabled(false);
	
	BMessage *menumsg;
	BEntry toolentry;
	
	// Note that I don't just simply have a way to iterate over whatever's in a folder
	// because I want to control what tools are officially supported by Mr. Peeps!
	// It's not that I don't want anyone helping -- I just want quality control with
	// tool usability.
	
	fToolMenu=new BMenu(TRANSLATE("Tools"));
	
	toolentry.SetTo("/boot/home/config/settings/MrPeeps/PeopleMover");
	if(toolentry.Exists())
	{
		menumsg=new BMessage(M_RUN_TOOL);
		menumsg->AddString("signature","application/x-vnd.wgp-PeopleMover");
		fToolPeopleMover=new BMenuItem(TRANSLATE("People Mover"),menumsg);
		fToolMenu->AddItem(fToolPeopleMover);
	}
	else
		fToolPeopleMover=NULL;
	
	toolentry.SetTo("/boot/home/config/settings/MrPeeps/PersonAtAGlance");
	if(toolentry.Exists())
	{
		menumsg=new BMessage(M_RUN_TOOL);
		menumsg->AddString("signature","application/x-vnd.wgp-PersonAtAGlance");
		fToolPeopleAtAGlance=new BMenuItem(TRANSLATE("Person at a Glance"),menumsg);
		fToolPeopleAtAGlance->SetEnabled(false);
		fToolMenu->AddItem(fToolPeopleAtAGlance);
	}
	else
		fToolPeopleAtAGlance=NULL;
	
	toolentry.SetTo("/boot/home/config/settings/MrPeeps/VCardExport");
	if(toolentry.Exists())
	{
		menumsg=new BMessage(M_RUN_TOOL);
		menumsg->AddString("signature","application/x-vnd.wgp-VCardExport");
		fToolVCardExport=new BMenuItem(TRANSLATE("Export Person to VCard"),menumsg);
		fToolVCardExport->SetEnabled(false);
		fToolMenu->AddItem(fToolVCardExport);
	}
	else
		fToolVCardExport=NULL;
	
	if(fToolMenu->CountItems()>0)
	{
		// PeopleMover does not require a selection, so if it is installed, allow the
		// user to select it.
		if(!fToolPeopleMover)
			fToolMenu->SetEnabled(false);
		mb->AddItem(fToolMenu);
	}
	else
	{
		delete fToolMenu;
		fToolMenu=NULL;
	}
	
	if(locale_roster->CountLocales()>1)
	{
		fLanguageMenu=new BMenu(TRANSLATE("Language"));
		fLanguageMenu->SetRadioMode(true);
		for(int32 i=0; i<locale_roster->CountLocales(); i++)
		{
			Locale *locale=locale_roster->LocaleAt(i);
			BMessage *langmsg=new BMessage(M_SET_LANGUAGE);
			langmsg->AddInt32("index",i);
			fLanguageMenu->AddItem(new BMenuItem(locale->Name(),langmsg));
		}
		mb->AddItem(fLanguageMenu);
		
		BMenuItem *markeditem=fLanguageMenu->FindItem(fLocale.String());
		if(markeditem)
			markeditem->SetMarked(true);
	}
	else
		fLanguageMenu=NULL;
	
	// set up left frame
	r.top+=mb->Bounds().bottom+1;
	r.bottom=Bounds().bottom;
	r.right=200;
	left_view=new BView(r, "left_view", B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW);
	AddChild(left_view);
	
	// Check to see that all of the tabs will fit
	float tabwidth=left_view->StringWidth(TRANSLATE("Main"))+
		left_view->StringWidth(TRANSLATE("Personal"))+
		left_view->StringWidth(TRANSLATE("Work"))+
		left_view->StringWidth(TRANSLATE("Instant Message"))+
		left_view->StringWidth(TRANSLATE("Notes"))+120;
	
	r.left=201;

	if(tabwidth+r.left>Bounds().right)
		ResizeTo(tabwidth+r.left, Bounds().Height());
	
	r.right=Bounds().right;
	
	fIntroView=new IntroView(r);
	AddChild(fIntroView);
	
	
	dataview=new DataView(fIntroView->Bounds());
	fIntroView->AddChild(dataview);
	dataview->Hide();
	
	
	// create list for left frame
	BRect tmp_rect=left_view->Bounds();
	tmp_rect.right -= B_V_SCROLL_BAR_WIDTH;
	fPeopleList=new PeopleList(tmp_rect, "fPeopleList",dataview);
	
	// put scrolled list in left_view
	BScrollView *my_scroll=new BScrollView("scroll_people", fPeopleList,
			B_FOLLOW_TOP_BOTTOM, 0, false, true);
	left_view->AddChild(my_scroll);


	// open directory of people
	BDirectory people_dir(PEOPLE_FOLDER);
	
	msg=new BMessage(M_SET_STATUS);
	int32 refcount=people_dir.CountEntries();
	msg->AddInt32("refcount",refcount);
	target.SendMessage(msg);
	
	if(refcount==0)
		fIntroView->SetNoPeopleMsg(true);
	
	// our variables for instantiation and garbage collection later
	entry_ref tmp_ref;
	BFile tmp_file;
	BString filename;
		
	while (people_dir.GetNextRef(&tmp_ref)==B_OK)
	{
		// variables for use here
		tmp_file.SetTo(&tmp_ref, B_READ_WRITE);
		
		if(tmp_file.ReadAttrString(PERSON_NAME,&filename)==B_OK)
			AddPerson(tmp_ref, false);
		
		target.SendMessage(new BMessage(M_UPDATE_STATUS));
	}

	SortList();
	fPeopleList->MakeFocus(true);

	node_ref nref;
	people_dir.GetNodeRef(&nref);
	watch_node(&nref, B_WATCH_NAME | B_WATCH_ATTR | B_WATCH_DIRECTORY,this);
}

PeepsWindow::~PeepsWindow(void)
{
	PersonData *data;
	for(int32 i=0; i<gPeopleData.CountItems();i++)
	{
		data=(PersonData*)gPeopleData.ItemAt(i);
		delete data;
	}
}

bool PeepsWindow::QuitRequested()
{
	fPeopleList->SyncData();
	fPeopleList->SaveData();
	be_app->PostMessage(B_QUIT_REQUESTED);
	return(true);
}

void PeepsWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case B_ABOUT_REQUESTED:
		{
			be_app->PostMessage(msg);
			break;
		}
		case M_RUN_TOOL:
		{
			BString sig;
			if(msg->FindString("signature",&sig)==B_OK)
			{
				if(strcmp(sig.String(),"application/x-vnd.wgp-PeopleMover")==0)
				{
					be_roster->Launch(sig.String());
					break;
				}
				
				// Get the filename for the currently-selected person
				int32 selection=fPeopleList->CurrentSelection();
				if(selection<0)
					break;
				
				PeepsListItem *peepsitem=(PeepsListItem*)fPeopleList->ItemAt(selection);
				if(!peepsitem || peepsitem->IsGroup())
					break;
				
				PersonItem *personitem=(PersonItem*)peepsitem;
				PersonData *persondata=personitem->GetData();
				
				if(!persondata)
					break;
				
				entry_ref ref=persondata->FileRef();
				BPath path(&ref);
				
				char *launchv[1];
				launchv[0]=new char[255];
				
				sprintf(launchv[0],"%s",path.Path());
				
				be_roster->Launch(sig.String(),1,launchv,NULL);
				
				delete [] launchv[0];
			}
			break;
		}
		case M_SHOW_ACTIONS:
		{
			uint32 buttons;
			BPoint pt;
			
			fPeopleList->GetMouse(&pt,&buttons,false);
			pt.x=(pt.x>5)?pt.x-5:0;
			pt.y=(pt.y>5)?pt.y-5:0;
			fPeopleList->ShowContextMenu(fPeopleList->ConvertToScreen(pt));
			
			break;
		}
		case M_BITMAP_REMOVED:
		case M_PHOTO_MODIFIED:
		{
			fPeopleList->SaveImageData();
			break;
		}
		case B_REFS_RECEIVED:
		case B_SIMPLE_DATA:
		{
			entry_ref ref;
			if(msg->FindRef("refs",0,&ref)==B_OK)
			{
				if(!IsPerson(ref))
				{
					// Might be an image dropped on the photo view. find out which child is the
					// target
					BPoint pt;
					if(msg->FindPoint("_drop_offset",&pt)!=B_OK)
						break;
					
					BView *target=FindView(pt);
					if(!target || strcmp(target->Name(),"photoview")!=0)
						break;
					
					// This will set the image, if possible
					target->MessageReceived(msg);
					
					// Save the image data to a real file and attach the Person attributes
					// to it
					fPeopleList->SaveImageData();
				}
				
				// Offer to move/copy file if not in People directory
				BDirectory dir,peopledir("/boot/home/people");
				BEntry entry(&ref);
				entry.GetParent(&dir);
				if(dir!=peopledir)
				{
					BString refname(ref.name);
					BString errstr=TRANSLATE("%s is not currently stored in the People folder. Mr. Peeps! will not"
						" see it on startup unless it is stored there. Would you like to move it"
						" there?");
					char errmsg[errstr.CountChars() - 2 + refname.CountChars() + 1];
					sprintf(errmsg,errstr.String(),refname.String());
					
					BAlert *alert=new BAlert("Mr. Peeps!",errmsg,TRANSLATE("Move"),TRANSLATE("Don't Move"));
					if(alert->Go()==0)
					{
						entry.MoveTo(&peopledir);
						entry.GetRef(&ref);
					}
				}
				if(!SelectPerson(ref))
					AddPerson(ref,true);
			}
			break;
		}
		case M_PREV_ITEM:
		{
			if(fPeopleList->CountItems()==0)
				break;
				
			int32 selection=fPeopleList->CurrentSelection();

			if(!fPeopleList->IsFocus())
			{
				fPeopleList->MakeFocus(true);
				PeepsListItem *pitem=(PeepsListItem*)fPeopleList->ItemAt(selection);
				if(!pitem->IsGroup())
				{
					fPeopleList->SyncData((PersonItem*)pitem);
//					fPeopleList->SaveData();
				}
				fPeopleList->SaveData();
			}

			if(selection>=0)
			{
				if(selection>0)
					fPeopleList->Select(selection-1);
			}
			else
			{
				fPeopleList->Select(0L);
			}
			break;
		}
		case M_NEXT_ITEM:
		{
			if(fPeopleList->CountItems()==0)
				break;
				
			int32 selection=fPeopleList->CurrentSelection();

			if(!fPeopleList->IsFocus())
			{
				fPeopleList->MakeFocus(true);
				PeepsListItem *pitem=(PeepsListItem*)fPeopleList->ItemAt(selection);
				if(!pitem->IsGroup())
				{
					fPeopleList->SyncData((PersonItem*)pitem);
//					fPeopleList->SaveData();
				}
				fPeopleList->SaveData();
			}
			
			if(selection==fPeopleList->CountItems()-1 || selection<0)
				fPeopleList->Select(fPeopleList->CountItems()-1);
			else
				fPeopleList->Select(selection+1);
			
			break;
		}
		case M_MOD_DATA:
		case M_SAVE:
		{
			fPeopleList->SyncData(NULL);
			fPeopleList->SaveData();
			break;
		}
		case M_ADD_PERSON:
		{
			entry_ref ref;
			
			status_t stat=CreatePerson(&ref);
			if(stat==B_FILE_EXISTS)
			{
				for(int32 i=0;i<gPeopleData.CountItems(); i++)
				{
					PersonData *pdata=(PersonData*)gPeopleData.ItemAt(i);
					
					if(BString(TRANSLATE("New Person")).ICompare(pdata->Name())==0)
					{
						int32 selection=fPeopleList->CurrentSelection();
						if(selection>=0)
						{
							PeepsListItem *peepsitem=(PeepsListItem*)fPeopleList->ItemAt(selection);
							if(peepsitem && !peepsitem->IsGroup())
								fPeopleList->Collapse(fPeopleList->Superitem(peepsitem));
						}
						PersonItem *pitem=pdata->InstanceAt(0);
						GroupItem *gitem=(GroupItem*)fPeopleList->Superitem(pitem);
						if(!gitem->IsExpanded())
							fPeopleList->Expand(gitem);
						fPeopleList->Select(fPeopleList->IndexOf(pitem));
						fPeopleList->ScrollToSelection();
						break;
					}
				}
			}
			else
			{
				// Add the current group to the New Person's file so that we end up staying
				// in the current group.
				if(stat==B_OK)
				{
					int32 selection=fPeopleList->CurrentSelection();
					BString groupstr(TRANSLATE("Ungrouped"));

					if(selection>=0)
					{
						PeepsListItem *currentitem=(PeepsListItem*)fPeopleList->ItemAt(selection);
						if(currentitem->IsGroup())
							groupstr=currentitem->Name();
						else
							groupstr=((PersonItem*)currentitem)->Group();
					}
					PersonData *pdata=new PersonData(ref);
					pdata->SetGroup(groupstr.String());
					pdata->SetName(TRANSLATE("New Person"));
					pdata->SaveToFile();
					delete pdata;
					AddPerson(ref,true);
					SortList();
					SelectPerson(TRANSLATE("New Person"));
					dataview->SetFirstFieldFocus();
				}
			}
			break;
		}
		case M_DELETE_PERSON:
		{
			int32 selection=fPeopleList->CurrentSelection();
			if(selection<0)
				break;
			PeepsListItem *item=(PeepsListItem*)fPeopleList->ItemAt(selection);
			if(item->IsGroup())
				DeleteGroup((GroupItem*)item);
			else
			{
				DeletePerson(((PersonItem*)item)->GetData());
			}
			break;
		}
		case M_DISABLE_DELETE:
		{
			BMenuItem *item=fPeopleMenu->FindItem(TRANSLATE("Move To Trash"));
			if(item)
				item->SetEnabled(false);
			break;
		}	
		case M_ENABLE_DELETE:
		{
			BMenuItem *item=fPeopleMenu->FindItem(TRANSLATE("Move To Trash"));
			if(item)
				item->SetEnabled(true);
			break;
		}	
		case M_SET_LANGUAGE:
		{
			// Restart MrPeeps!
			gRestartApp=true;
			
			int32 language;
			if(msg->FindInt32("index",&language)!=B_OK)
				break;
			
			BMenuItem *item = fLanguageMenu->ItemAt(language);
			if(!item)
				break;
			
			fLocale = item->Label();
			WriteLocaleSettings();
			
			be_app->PostMessage(B_QUIT_REQUESTED);
			break;
		}
		case M_HANDLE_NO_SELECTION:
		{
			// This is the initial case. No selection means quite a few different
			// menu items are disabled.
			fTabMenu->SetEnabled(false);
			fEditMenu->SetEnabled(false);
			
			if(fToolPeopleAtAGlance)
				fToolPeopleAtAGlance->SetEnabled(false);
			
			if(fToolVCardExport)
				fToolVCardExport->SetEnabled(false);
			
			UpdateActions(true);
			break;
		}	
		case M_HANDLE_GROUP_SELECTION:
		{
			// This is the initial case. No selection means quite a few different
			// menu items are disabled.
			fTabMenu->SetEnabled(false);
			fEditMenu->SetEnabled(true);
			
			if(fToolPeopleAtAGlance)
				fToolPeopleAtAGlance->SetEnabled(false);
			
			if(fToolVCardExport)
				fToolVCardExport->SetEnabled(false);
			
			UpdateActions(true);
			break;
		}	
		case M_HANDLE_PERSON_SELECTION:
		{
			// This is the initial case. No selection means quite a few different
			// menu items are disabled.
			fTabMenu->SetEnabled(true);
			fEditMenu->SetEnabled(true);
			
			if(fToolPeopleAtAGlance)
				fToolPeopleAtAGlance->SetEnabled(true);
			
			if(fToolVCardExport)
				fToolVCardExport->SetEnabled(true);
			
			UpdateActions(false);
			break;
		}	
		case M_SEND_PERSONAL_EMAIL:
		{
			if(dataview->Email())
			{
				BMessage *emailmsg=new BMessage(M_SEND_EMAIL);
				emailmsg->AddString("address",dataview->Email());
				be_app->PostMessage(emailmsg);
			}
			break;
		}
		case M_SEND_WORK_EMAIL:
		{
			if(dataview->WorkEmail())
			{
				BMessage *emailmsg=new BMessage(M_SEND_EMAIL);
				emailmsg->AddString("address",dataview->WorkEmail());
				be_app->PostMessage(emailmsg);
			}
			break;
		}
		case M_SEND_ALT_EMAIL:
		{
			if(dataview->Email3())
			{
				BMessage *emailmsg=new BMessage(M_SEND_EMAIL);
				emailmsg->AddString("address",dataview->Email3());
				be_app->PostMessage(emailmsg);
			}
			break;
		}
		case M_BROWSE_WWW_HOME:
		{
			if(dataview->URL())
			{
				BMessage *wwwmsg=new BMessage(M_LAUNCH_BROWSER);
				wwwmsg->AddString("address",dataview->URL());
				be_app->PostMessage(wwwmsg);
			}
			break;
		}
		case M_BROWSE_WWW_WORK:
		{
			if(dataview->WorkURL())
			{
				BMessage *wwwmsg=new BMessage(M_LAUNCH_BROWSER);
				wwwmsg->AddString("address",dataview->WorkURL());
				be_app->PostMessage(wwwmsg);
			}
			break;
		}
		case M_BROWSE_WWW_ALT:
		{
			if(dataview->OtherURL())
			{
				BMessage *wwwmsg=new BMessage(M_LAUNCH_BROWSER);
				wwwmsg->AddString("address",dataview->OtherURL());
				be_app->PostMessage(wwwmsg);
			}
			break;
		}
		case M_TAB_1:
		{
			fPeopleList->SyncData();
			fPeopleList->SaveData();
			dataview->ShowTab(0);
			if(!fPeopleList->IsFocus())
				dataview->SetFirstFieldFocus();
			break;
		}
		case M_TAB_2:
		{
			fPeopleList->SyncData();
			fPeopleList->SaveData();
			dataview->ShowTab(1);
			if(!fPeopleList->IsFocus())
				dataview->SetFirstFieldFocus();
			break;
		}
		case M_TAB_3:
		{
			fPeopleList->SyncData();
			fPeopleList->SaveData();
			dataview->ShowTab(2);
			if(!fPeopleList->IsFocus())
				dataview->SetFirstFieldFocus();
			break;
		}
		case M_TAB_4:
		{
			fPeopleList->SyncData();
			fPeopleList->SaveData();
			dataview->ShowTab(3);
			if(!fPeopleList->IsFocus())
				dataview->SetFirstFieldFocus();
			break;
		}
		case M_TAB_5:
		{
			fPeopleList->SyncData();
			fPeopleList->SaveData();
			dataview->ShowTab(4);
			if(!fPeopleList->IsFocus())
				dataview->SetFirstFieldFocus();
			break;
		}
		case M_UNDO:
		{
			dataview->UndoField();
			break;
		}
		case M_CUT:
		{
			dataview->CutField();
			break;
		}
		case M_COPY:
		{
			dataview->CopyField();
			break;
		}
		case M_PASTE:
		{
			dataview->PasteField();
			break;
		}
		case B_NODE_MONITOR:
		{
			HandleNodeMonitoring(msg);
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}
status_t PeepsWindow::CreatePerson(entry_ref *ref)
{
	if(!ref)
		return B_BAD_VALUE;
	
	// Creates a file called "New Person" in the People directory. If one already exists, then
	// we find it in the people list and show it
	
	BFile file;
	BString filename="/boot/home/people/";
	filename+=TRANSLATE("New Person");
	status_t status=file.SetTo("/boot/home/people/New Person", B_READ_WRITE | B_CREATE_FILE | 
			B_FAIL_IF_EXISTS);
			
	if(status==B_OK)
	{
		BNodeInfo ninfo(&file);
		ninfo.SetType(PERSON_FILE_TYPE);
		
		file.Unset();
		get_ref_for_path(filename.String(),ref);
		
		return B_OK;
	}
	
	switch(status)
	{
		case B_BAD_VALUE:
			printf("B_BAD_VALUE\n"); break;
		case B_ENTRY_NOT_FOUND: 
			printf("B_ENTRY_NOT_FOUND\n"); break;
		case B_FILE_EXISTS:
			return B_FILE_EXISTS;
		case B_PERMISSION_DENIED:
			printf("B_PERMISSION_DENIED"); break;
		case B_NO_MEMORY:
			printf("B_NO_MEMORY\n"); break;
		default:
			printf("Some other error in PeepsWindow::CreatePerson\n"); break;
	}
	return status;
}

void PeepsWindow::DeletePerson(PersonData *data)
{
	if(!data)
		return;
	
	PeepsListItem *previtem=NULL, *nextitem=NULL;
	
	for(int32 i=0; i<data->CountGroups();i++)
	{
		PersonItem *pitem=data->InstanceAt(i);
		GroupItem *gitem=(GroupItem*)fPeopleList->Superitem(pitem);

		if(pitem->IsSelected())
		{
			int32 index=fPeopleList->IndexOf(pitem);
			if(index>1)
			{
				previtem=(PeepsListItem*)fPeopleList->ItemAt(index-1);
//				nextitem=(PeepsListItem*)fPeopleList->ItemAt(index+1);
			}
			else
			{
				// First person in the entire list. Check to see if this is the only item in
				// the group. If it is, then select the next item in the list, if it exists.
				if(fPeopleList->CountItemsUnder(gitem,true)==1)
					nextitem=(PeepsListItem*)fPeopleList->ItemAt(index+1);
				else
					previtem=(PeepsListItem*)fPeopleList->ItemAt(index-1);
			}
		}
		fPeopleList->RemoveItem(pitem);
		data->DestroyInstance(pitem);
		if(fPeopleList->CountItemsUnder(gitem,true)==0)
		{
			fPeopleList->RemoveItem(gitem);
			gGroupData.RemoveGroup(gitem->Name());
		}
	}
	entry_ref personref=data->FileRef();
	BEntry entry(&personref);
	if(previtem)
		fPeopleList->Select(fPeopleList->IndexOf(previtem));
	else
	if(nextitem)
		fPeopleList->Select(fPeopleList->IndexOf(nextitem));
	else
		fPeopleList->Select(0L);
#ifndef DATA_READ_ONLY
	TrashFile(&entry);
#endif

	gPeopleData.RemoveItem(data);
	delete data;
}

void PeepsWindow::AddPerson(const entry_ref &ref, bool highlight)
{
	// Check to see if it's actually a Person file
	if(!IsPerson(ref))
		return;
	
	GroupData *gdata;
	
	// Add person to data list
	PersonData *pdata=new PersonData(ref);
	gPeopleData.AddItem(pdata);
	
	// add group(s) and an item for each group
	
	if(strlen(pdata->Group())==0)
	{
		// Ungrouped case
		gdata=gGroupData.AddGroup(TRANSLATE("Ungrouped"));
		if(!fPeopleList->HasItem(gdata->GetInstance()))
			fPeopleList->AddItem(gdata->GetInstance());
		
		gdata->AddPerson(pdata);
		fPeopleList->AddUnder(pdata->CreateInstance(TRANSLATE("Ungrouped")),gdata->GetInstance());
		return;
	}
	
	GroupParser gp(pdata->Group());
	
	for(int32 i=0; i<gp.CountGroups(); i++)
	{
		gdata=gGroupData.AddGroup(gp.GroupAt(i));
		gdata->AddPerson(pdata);
		if(!fPeopleList->HasItem(gdata->GetInstance()))
			fPeopleList->AddItem(gdata->GetInstance());
		fPeopleList->AddUnder(pdata->CreateInstance(gp.GroupAt(i)),gdata->GetInstance());
	}
	if(highlight)
	{
		SelectPerson(pdata->Name());
	}
}

void PeepsWindow::RemovePerson(PersonData *person)
{
	if(!person)
		return;

	// TODO: Implement PeepsWindow::RemovePerson
	
	debugger("PeepsWindow::RemovePerson() unimplemented");
}

void PeepsWindow::SelectPerson(const char *name)
{
	if(!name)
		return;

	int32 currentselection=fPeopleList->CurrentSelection();
	
	for(int32 i=0; i<fPeopleList->FullListCountItems(); i++)
	{
		PeepsListItem *item=(PeepsListItem*)fPeopleList->FullListItemAt(i);
		if(strcmp(name,item->Name())==0 && !item->IsGroup())
		{
			GroupItem *oldgroup=(GroupItem*)fPeopleList->FullListItemAt(currentselection);
			GroupItem *newgroup=(GroupItem*)fPeopleList->Superitem(item);
			if(oldgroup!=newgroup && oldgroup)
				fPeopleList->Collapse(oldgroup);
			if(!newgroup->IsExpanded())
				fPeopleList->Expand(newgroup);
			fPeopleList->Select(fPeopleList->IndexOf(item));
		}
	}
}

bool PeepsWindow::SelectPerson(entry_ref ref)
{
	int32 currentselection=fPeopleList->CurrentSelection();
	
	for(int32 i=0; i<fPeopleList->FullListCountItems(); i++)
	{
		PeepsListItem *item=(PeepsListItem*)fPeopleList->FullListItemAt(i);
		if(!item->IsGroup())
		{
			PersonItem *pitem=(PersonItem*)item;
			if(ref==pitem->GetData()->FileRef())
			{
				GroupItem *oldgroup=(GroupItem*)fPeopleList->FullListItemAt(currentselection);
				GroupItem *newgroup=(GroupItem*)fPeopleList->Superitem(item);
				if(oldgroup!=newgroup && oldgroup)
					fPeopleList->Collapse(oldgroup);
				if(!newgroup->IsExpanded())
					fPeopleList->Expand(newgroup);
				fPeopleList->Select(fPeopleList->IndexOf(item));
				return true;
			}
		}
	}
	return false;
}

void PeepsWindow::DeleteGroup(GroupItem *item)
{
	// We are going to delete the group. All items will be made ungrouped. Because
	// it cannot be undone, we will ask the user if it's ok to delete the group
	
	BString alertmsg=TRANSLATE("Are you sure you want to delete this group?");
	alertmsg+="\n\n";
	alertmsg+=TRANSLATE("All items will be removed from this group, but the items themselves will not"
		" be deleted. Once done, this cannot be undone.");
	BAlert *alert=new BAlert("Mr. Peeps!", alertmsg.String(), TRANSLATE("Delete Group"), 
		TRANSLATE("Cancel"));
	
	if(alert->Go()==1)
		return;

	if(!item)
		return;
	
	if(!item->IsExpanded())
		fPeopleList->Expand(item);

	// First, ensure that we have a group for all these items to go to
	
	GroupData 	*oldgdata=item->GetData(),
				*ungrouped=gGroupData.FindGroup(TRANSLATE("Ungrouped"));
	
	if(!ungrouped)
	{
		ungrouped=gGroupData.AddGroup(TRANSLATE("Ungrouped"));
		fPeopleList->AddItem(ungrouped->GetInstance());
	}

	if(!ungrouped->GetInstance()->IsExpanded())
		fPeopleList->Expand(ungrouped->GetInstance());

	int32 groupindex=fPeopleList->FullListIndexOf(ungrouped->GetInstance())+1;
	
	// Now, we iterate through each person in the group to remove the group name
	// from their group list strings
	for(int32 i=0; i<oldgdata->CountPeople(); i++)
	{
		PersonData *pdata=oldgdata->PersonAt(i);

		if(pdata->CountGroups()==1)
		{
			pdata->AddToGroup(ungrouped->GetInstance());
			fPeopleList->MoveItem(fPeopleList->FullListIndexOf(pdata->InstanceAt(0)),groupindex);
		}
		else
		{
			PersonItem *instance=pdata->InstanceForGroup(item->Name());
			fPeopleList->RemoveItem(instance);
			pdata->DestroyInstance(instance);
		}
		pdata->RemoveFromGroup((GroupItem*)item);
	}
	
	fPeopleList->RemoveItem((GroupItem*)item);
	gGroupData.RemoveGroup(item->Name());
	fPeopleList->SortItemsUnder(ungrouped->GetInstance(),true,compare_peeps);
	fPeopleList->Select(fPeopleList->IndexOf(ungrouped->GetInstance()));
}

void PeepsWindow::SortList(void)
{
	fPeopleList->FullListSortItems(compare_peeps);
	fPeopleList->DoForEach(sort_groups, (void*) fPeopleList);

}

bool PeepsWindow::IsPerson(entry_ref ref)
{
	BNode node(&ref);
	BNodeInfo nodeinfo(&node);
	char type[255];
	
	nodeinfo.GetType(type);
	
	if(strcmp(type,PERSON_FILE_TYPE)==0)
		return true;

	return false;
}

void PeepsWindow::HandleNodeMonitoring(BMessage *msg)
{
/*	int32 opcode;
	
	if(msg->FindInt32("opcode",&opcode)!=B_OK)
		return;
	
//debugger("");
	switch(opcode)
	{
		case B_ENTRY_CREATED:
		{
			entry_ref ref;
			BString name;

			msg->FindInt32("device",&ref.device);
			msg->FindInt64("directory",&ref.directory);
			msg->FindString("name",&name);
			ref.set_name(name.String());
			
			if(!FindPerson(ref))
				AddPerson(ref,false);
			break;
		}
		case B_ENTRY_REMOVED:
		{
			// find the former entry in the list data
			entry_ref ref;
			msg->FindInt32("device",&ref.device);
			msg->FindInt64("directory",&ref.directory);

			bool entry_found=false;
			PersonData *pdata=NULL;
			
			for(int32 i=0;i<gPeopleData.CountItems();i++)
			{
				entry_ref tempref;
				pdata=(PersonData*)gPeopleData.ItemAt(i);
				if(!pdata)
					continue;
				tempref=pdata->FileRef();
				if( (ref.directory==tempref.directory) && (ref.device==tempref.device) )
				{
					entry_found=true;
					ref=tempref;
				}
			}
			
			// if entry_found is true, then ref points to the old file and pdata holds
			// the data for the former file
			if(entry_found)
				RemovePerson(pdata);
			
			break;
		}
		case B_ENTRY_MOVED:
		{
			BEntry entry("/boot/home/people");
			entry_ref people_ref;
			entry.GetRef(&people_ref);
			entry.Unset();
			
			entry_ref tempref;
			BString name;
			
			msg->FindString("name",&name);
			tempref.set_name(name.String());
			msg->FindInt32("device",&tempref.device);
			msg->FindInt64("from directory",&tempref.directory);
			
			// A file was moved from the People directory. Treat as ENTRY_REMOVED
			if(tempref.directory==people_ref.directory)
			{
				bool entry_found=false;
				PersonData *pdata=NULL;
				
				for(int32 i=0;i<gPeopleData.CountItems();i++)
				{
					entry_ref currentref;
					pdata=(PersonData*)gPeopleData.ItemAt(i);
					if(!pdata)
						continue;
					currentref=pdata->FileRef();
					if( (currentref.directory==tempref.directory) && (currentref.device==tempref.device) )
					{
						entry_found=true;
						currentref=tempref;
					}
				}
				
				// if entry_found is true, then ref points to the old file and pdata holds
				// the data for the former file
				if(entry_found)
					RemovePerson(pdata);
				break;
			}
			
			// A file was moved to the People directory. Treat as ENTRY_CREATED
			msg->FindInt64("to directory",&tempref.directory);
			if(tempref.directory==people_ref.directory)
			{
				entry_ref ref;
				BString name;
	
				msg->FindInt32("device",&ref.device);
				msg->FindInt64("directory",&ref.directory);
				msg->FindString("name",&name);
				ref.set_name(name.String());
				
				if(!FindPerson(ref))
					AddPerson(ref,false);
			}
			break;
		}
		case B_ATTR_CHANGED:
		{
			node_ref nref;
			msg->FindInt32("device",&nref.device);
			msg->FindInt64("node",&nref.node);

			bool entry_found=false;
			PersonData *pdata=NULL;
			
			for(int32 i=0;i<gPeopleData.CountItems();i++)
			{
				node_ref tempnref;
				pdata=(PersonData*)gPeopleData.ItemAt(i);
				if(!pdata)
					continue;
				tempnref=pdata->NodeRef();
				if( (nref.node==tempnref.node) && (nref.device==tempnref.device) )
				{
					entry_found=true;
					nref=tempnref;
				}
			}
			
			// if entry_found is true, then ref points to the old file and pdata holds
			// the data for the former file
			if(entry_found)
			{
			debugger("TODO: Implement B_ATTR_CHANGED");

				// 1) Create a new PersonData item which is a copy of the one in memory.
				// 2) Check to see if the updated person is the current selection. If so,
				//		copy data to fDataView. call SyncData(), and return.
				// 3) Compare the copy to the one in the PeopleData list.
				// 4) Perform the same type of operations as in PeopleList::SyncData for
				// modifying groups and names. Copy over the rest of the data.
			}
			break;
		}
		default:
		{
			break;
		}
	}
*/
}

PersonData *PeepsWindow::FindPerson(entry_ref ref)
{
	PersonData *pdata=NULL;
	for(int32 i=0; i<gPeopleData.CountItems(); i++)
	{
		pdata=(PersonData*)gPeopleData.ItemAtFast(i);
		if(pdata->FileRef()==ref)
			return pdata;
	}
	return NULL;
}

PersonData *PeepsWindow::FindPerson(node_ref nref)
{
	PersonData *pdata=NULL;
	for(int32 i=0; i<gPeopleData.CountItems(); i++)
	{
		pdata=(PersonData*)gPeopleData.ItemAtFast(i);
		if(pdata->NodeRef()==nref)
			return pdata;
	}
	return NULL;
}

void PeepsWindow::UpdateActions(bool groupmode)
{
	if(groupmode)
	{
		fActionsMenu->SetEnabled(false);
		return;
	}
	
	fActionsMenu->SetEnabled(true);
	PersonItem *item=(PersonItem*)fPeopleList->ItemAt(fPeopleList->CurrentSelection());
	
	if(item)
	{
		PersonData *data=item->GetData();
		
		BString str=data->Email();
		fEmailPersonalItem->SetEnabled(str.CountChars()>0);
		str=data->WorkEmail();
		fEmailWorkItem->SetEnabled(str.CountChars()>0);
		str=data->Email3();
		fEmailAltItem->SetEnabled(str.CountChars()>0);
		
		str=data->URL();
		fBrowseHomeItem->SetEnabled(str.CountChars()>0);
		str=data->WorkURL();
		fBrowseWorkItem->SetEnabled(str.CountChars()>0);
		str=data->OtherURL();
		fBrowseAltItem->SetEnabled(str.CountChars()>0);
		
		if(data->Photo())
			fRemovePhotoItem->SetEnabled(true);
		else
			fRemovePhotoItem->SetEnabled(false);
	}
	
}

void PeepsWindow::ReadLocaleSettings(void)
{
	BFile file("/boot/home/config/settings/MrPeeps/CurrentLocale", B_READ_ONLY);
	
	if(file.InitCheck()!=B_OK)
		return;
	
	file.Seek(0,SEEK_END);
	off_t size = file.Position();
	file.Seek(0,SEEK_SET);
	
	char name[size];
	file.Read(name,size);
	file.Unset();
	
	fLocale=name;
}

void PeepsWindow::WriteLocaleSettings(void)
{
	BFile file("/boot/home/config/settings/MrPeeps/CurrentLocale",
				B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	
	if(file.InitCheck()!=B_OK)
		return;
	
	file.Seek(0,SEEK_SET);
	file.Write(fLocale.String(),fLocale.Length()+1);
	file.Unset();
}

bool sort_groups(BListItem *this_group, void *void_list)
{
	PeopleList *my_list=(PeopleList*)void_list;
	if (my_list->Superitem(this_group) == NULL)
		my_list->SortItemsUnder(this_group, 0, compare_peeps);

	return false;
}

int compare_peeps(const BListItem *item_one, const BListItem *item_two)
{
	PeepsListItem *item_three=(PeepsListItem *)item_one;
	PeepsListItem *item_four=(PeepsListItem *)item_two;
	BString one=item_three->Name();
	BString two=item_four->Name();
	
	return one.ICompare(two);
}

status_t TrashFile(BEntry *src)
{
	// Move a file to the Trash. If the name exists, rename the entry before moving
	
	if(!src)
		return B_ERROR;
	
	status_t status;
	BPath path;
	BEntry dest;
	BString pathstr("/boot/home/Desktop/Trash/");
	BDirectory dir(pathstr.String());
	
	char newname[B_FILE_NAME_LENGTH];
	src->GetName(newname);
	pathstr+=newname;
	
	dest.SetTo(pathstr.String());
	GetValidName(&dest);
	dest.GetPath(&path);
	
	BPath srcpath;
	src->GetPath(&srcpath);
	BNode node(srcpath.Path());
	if(node.InitCheck()==B_OK)
	{
		node.WriteAttr("_trk/original_path",B_STRING_TYPE,0,
				srcpath.Path(),strlen(srcpath.Path())+1);
	}
	
	status=src->MoveTo(&dir,path.Path(),false);
	
	if(status==B_CROSS_DEVICE_LINK)
	{
		BPath srcpath;
		src->GetPath(&srcpath);
		BString command("mv "),srcstring(srcpath.Path()),deststring(path.Path());
		srcstring.CharacterEscape(" ",'\\');
		deststring.CharacterEscape(" ",'\\');
		command+=srcstring;
		command+=" ";
		command+=deststring;
		system(command.String());
	}
	return status;

}

const char *GetValidName(BEntry *entry)
{
	// given a particular location, this will (1) check to see if said entry
	// exists and if it does, generates a filename which will work complete with
	// the full path. The entry is also set to this new, valid path
	
	BPath path;
	entry->GetPath(&path);

	if(entry->Exists())
	{
		// separate into path and leaf
		char leafbase[B_FILE_NAME_LENGTH];
		char newpath[B_PATH_NAME_LENGTH];
		strcpy(leafbase, path.Leaf());
		path.GetParent(&path);
		
		int32 attempt=1;
		
		do
		{
			if(attempt>1)
				sprintf(newpath, "%s/%s %s %ld", path.Path(),leafbase, TRANSLATE("copy"), attempt);
			else
				sprintf(newpath, "%s/%s %s", path.Path(),leafbase,TRANSLATE("copy"));

			entry->SetTo(newpath);

			attempt++;
		} while (entry->Exists());
		
		return newpath;
	}
	
	return path.Path();
}
