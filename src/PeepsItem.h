#ifndef PEOPLE_ITEM_H
#define PEOPLE_ITEM_H

#include <String.h>
#include "BStringList.h"
#include <File.h>
#include <TextControl.h>
#include <MenuItem.h>
#include <ListItem.h>
#include <Entry.h>

class PersonItem;
class GroupItem;
class GroupData;
class PersonData;

status_t get_attribute(const BFile &file , const char *attribute, BString *string);
int32 CountCharInstances(const char c, const char *string);

// used as a superclass for group and person items
class PeepsListItem : public BListItem
{
public:
	PeepsListItem(const char *name, bool expanded=true);
	virtual ~PeepsListItem();
	void DrawItem(BView *parent, BRect my_rect, bool complete = false);
	const char *Name(void) const { return fName.String(); }
	void SetName(const char *newname) { fName.SetTo(newname); }
	virtual bool IsGroup(void) const { return false; }
protected:
		BString fName;		
};

class GroupItem : public PeepsListItem
{
public:
	GroupItem(const char *name);
	bool IsGroup(void) const { return true; }
	GroupData *GetData(void) const { return fData; }
protected:
	friend class GroupData;
	GroupData *fData;
};

class PersonItem : public PeepsListItem
{
public:
	PersonItem(PersonData *data);
	~PersonItem();
	
	PersonData *GetData(void) const { return fData; }
	void SetData(PersonData *data) { fData=data; }
	const char *Group(void) const { return fGroup.String(); }
	void SetGroup(const char *group) { fGroup.SetTo(group); }
protected:
	PersonData *fData;
	BString fGroup;
};

class GroupParser
{
public:
	GroupParser(const char *string);
	GroupParser(void);
	~GroupParser(void);
	
	int32 CountGroups(void) { return namelist.CountItems(); }
	void SetTo(const char *string);
	void MakeEmpty(void);
	const char *GroupAt(int32 index) { return namelist.ItemAt(index)->String(); }
	bool HasGroup(const char *name);
	void PrintToStream(void);
	bool RemoveDuplicates(void);
	const char *GroupString(void) { return groupstring.String(); }
protected:
	BString groupstring;
	BStringList namelist;
};

#endif
