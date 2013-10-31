#ifndef PEEPSWINDOW_H
#define PEEPSWINDOW_H

#include <Window.h>
#include <View.h>
#include <TextControl.h>
#include <PopUpMenu.h>
#include <Button.h>
#include <Menu.h>
#include <MenuItem.h>
#include "NewTextControl.h"
#include <Messenger.h>
#include "LocaleRoster.h"

class PeopleList;
class PeepsListItem;
class PersonItem;
class PersonData;
class DataView;
class GroupList;
class GroupItem;
class IntroView;

enum
{
M_LIST_INVOKE=700,
M_SAVE,
M_GROUP_MENU,
M_GROUP_CHANGE,
M_ADD_PERSON,
M_NEW_GROUP_MENU,
M_NEW_PERSON,
M_DELETE_PERSON,
M_NEXT_ITEM,
M_PREV_ITEM,
M_MOD_DATA,
M_TOGGLE_VIEW,
M_DISABLE_DELETE,
M_ENABLE_DELETE,
M_SHOW_ACTIONS,
M_PHOTO_MODIFIED,
M_SET_LANGUAGE,
M_HANDLE_NO_SELECTION,
M_HANDLE_PERSON_SELECTION,
M_HANDLE_GROUP_SELECTION,
M_SEND_PERSONAL_EMAIL,
M_SEND_WORK_EMAIL,
M_SEND_ALT_EMAIL,
M_BROWSE_WWW_HOME,
M_BROWSE_WWW_WORK,
M_BROWSE_WWW_ALT,
M_TAB_1,
M_TAB_2,
M_TAB_3,
M_TAB_4,
M_TAB_5,
M_UNDO,
M_CUT,
M_COPY,
M_PASTE,
M_RUN_TOOL
};

status_t TrashFile(BEntry *src);
const char *GetValidName(BEntry *entry);

class PeepsWindow : public BWindow
{
public:
	PeepsWindow(BMessenger msgr);
	~PeepsWindow(void);
	bool QuitRequested(void);
	void MessageReceived(BMessage*);
	void SyncData(void);
//	void SyncData(PersonItem *item);
//	void SaveData(void);
	void SortList(void);
	status_t CreatePerson(entry_ref *ref);
	void DeletePerson(PersonData *data);
	void DeleteGroup(GroupItem *item);
	void AddPerson(const entry_ref &ref, bool highlight);
	void RemovePerson(PersonData *person);
	void SelectPerson(const char *name);
	bool SelectPerson(entry_ref ref);
	void HandleNodeMonitoring(BMessage *msg);
	bool IsPerson(entry_ref ref);
	PersonData *FindPerson(entry_ref ref);
	PersonData *FindPerson(node_ref nref);
	
	void UpdateActions(bool groupmode);
	
	void ReadLocaleSettings(void);
	void WriteLocaleSettings(void);
	
private:
	BView* left_view;
	PeopleList *fPeopleList;
	BMenu *fPeopleMenu, *fTabMenu, *fEditMenu, *fToolMenu, *fLanguageMenu;
	BMenu *fActionsMenu;
	DataView *dataview;
	IntroView *fIntroView;
	
	BMenuItem *fPastePhotoItem, *fRemovePhotoItem;
	BMenuItem *fToolPeopleMover, *fToolPeopleAtAGlance, *fToolVCardExport;
	
	BMenuItem *fEmailPersonalItem, *fEmailWorkItem, *fEmailAltItem,
				*fBrowseHomeItem, *fBrowseWorkItem, *fBrowseAltItem;
	BString fLocale;
};

extern BList gPeopleData;
extern GroupList gGroupData;
extern Locale *gCurrentLocale;

int compare_peeps(const BListItem*, const BListItem*);
bool sort_groups(BListItem*, void*);

#define TRANSLATE(x) gCurrentLocale->Translate(x).String()

#endif