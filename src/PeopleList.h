#ifndef PEOPLE_LIST_H
#define PEOPLE_LIST_H

#include <View.h>
#include <OutlineListView.h>
#include <PopUpMenu.h>

class PeepsListItem;

class PeopleList : public BOutlineListView
{
public:
	PeopleList(BRect frame, const char *name, DataView *dv);
	void SelectionChanged();

	int32 FindNextAlphabetical(char c, int32 index);
	void MessageReceived(BMessage *msg);
	void KeyDown(const char *bytes,int32 numbytes);
	void ScrollToItem(PeepsListItem *item);
	void MakeFocus(bool value=true);
	void MouseDown(BPoint pt);
	void SyncData(PersonItem *item);
	void SyncData(void);
	void SaveData(void);
	void SaveImageData(void);
	void ShowContextMenu(BPoint pt);
private:
	GroupItem *lastgroup;
	DataView *fDataView;
	bool fDeleteDisabled;
	bool fGroupSelection;
	PersonItem *lastperson;
	BPopUpMenu *fPopupMenu;
};

bool IsNameChar(char c);
int charncmp(char c1, char c2);

#endif