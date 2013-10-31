#ifndef LIST_DATA
#define LIST_DATA

#include <Entry.h>
#include <List.h>
#include <String.h>
#include "BStringList.h"
#include <Bitmap.h>
#include "BitmapDump.h"

class PeepsListItem;
class GroupItem;
class PersonItem;

// This class is used to hold all the data for a person because a person may
// end up having multiple instances in the list
class PersonData
{
public:
	PersonData(const entry_ref &ref);
	~PersonData(void);
	
	void SaveToFile(void);
	void SaveImageDataToFile(void);
	void SaveGroup(void);
	void LoadNameAndGroup(const entry_ref &ref);
	void LoadFromFile(const entry_ref &ref);
	inline void LoadFromFile(void) { LoadFromFile(fFileRef); }
	void UpdateFilename(void);
	void PrintToStream(void);

	int32 CountGroups(void) { return fGroupList.CountItems(); }
	void AddToGroup(GroupItem *group);
	void RemoveFromGroup(GroupItem *group);
	bool BelongsToGroup(GroupItem *group) const { return fGroupList.HasItem(group); }
	GroupItem *GroupAt(int32 index) { return (GroupItem*)fGroupList.ItemAt(index); }
	void RenameGroup(const char *from, const char *to);

	PersonItem *CreateInstance(const char *groupname);
	void DestroyInstance(PersonItem *item);
	bool HasInstance(PersonItem *item);
	int32 CountInstances(void) { return fItemList.CountItems(); }
	PersonItem *InstanceAt(int32 index) { return (PersonItem*)fItemList.ItemAt(index); }
	PersonItem *InstanceForGroup(const char *name);

	void SyncName(void);
	
	// Get and Set data methods. This was a pain to type... :P
	void SetPhoto(BBitmap *bmp);
	void SetName(const char *newname);
	void SetTitle(const char *value);
	void SetNickname(const char *value);
	void SetEmail(const char *value);
	void SetURL(const char *value);
	void SetHomePhone(const char *value);
	void SetWorkPhone(const char *value);
	void SetCellPhone(const char *value);
	void SetFax(const char *value);
	void SetAddress(const char *value);
	void SetAddress2(const char *value);
	void SetCity(const char *value);
	void SetState(const char *value);
	void SetZip(const char *value);
	void SetBirthday(const char *value);
	void SetAnniversary(const char *value);
	void SetGroup(const char *value);
	void SetSpouse(const char *value);
	void SetChildren(const char *value);
	void SetAssistant(const char *value);
	void SetAssistantPhone(const char *value);
	void SetMSN(const char *value);
	void SetJabber(const char *value);
	void SetICQ(const char *value);
	void SetICQUIN(const char *value);
	void SetYahoo(const char *value);
	void SetAIM(const char *value);
	void SetEmail3(const char *value);
	void SetEmail4(const char *value);
	void SetEmail5(const char *value);
	void SetOtherURL(const char *value);
	void SetNotes(const char *value);
	void SetWorkEmail(const char *value);
	void SetWorkCellPhone(const char *value);
	void SetWorkCountry(const char *value);
	void SetWorkCity(const char *value);
	void SetWorkAddress(const char *value);
	void SetWorkURL(const char *value);
	void SetPager(const char *value);
	void SetWorkFax(const char *value);
	void SetWorkZip(const char *value);
	void SetWorkState(const char *value);
	void SetWorkAddress2(const char *value);
	void SetPosition(const char *value);
	void SetCompany(const char *value);
	
	BBitmap *Photo(void);
	const char *Name(void);
	const char *Title(void);
	const char *Nickname(void);
	const char *Email(void);
	const char *URL(void);
	const char *HomePhone(void);
	const char *WorkPhone(void);
	const char *CellPhone(void);
	const char *Fax(void);
	const char *Address(void);
	const char *Address2(void);
	const char *City(void);
	const char *State(void);
	const char *Zip(void);
	const char *Birthday(void);
	const char *Anniversary(void);
	const char *Group(void);
	const char *Spouse(void);
	const char *Children(void);
	const char *Assistant(void);
	const char *AssistantPhone(void);
	const char *MSN(void);
	const char *Jabber(void);
	const char *ICQ(void);
	const char *ICQUIN(void);
	const char *Yahoo(void);
	const char *AIM(void);
	const char *Email3(void);
	const char *Email4(void);
	const char *Email5(void);
	const char *OtherURL(void);
	const char *Notes(void);
	const char *WorkEmail(void);
	const char *WorkCellPhone(void);
	const char *WorkCountry(void);
	const char *WorkCity(void);
	const char *WorkAddress(void);
	const char *WorkURL(void);
	const char *Pager(void);
	const char *WorkFax(void);
	const char *WorkZip(void);
	const char *WorkState(void);
	const char *WorkAddress2(void);
	const char *Position(void);
	const char *Company(void);

	entry_ref FileRef(void) const { return fFileRef; }
	node_ref NodeRef(void) const { return fNodeRef; }

protected:	
	BString fName, fTitle, fNickname,fEmail,fURL,fHomePhone,fWorkPhone,fCellPhone,
			fFax,fAddress,fAddress2,fCity,fState,fZip,fBirthday,fAnniversary,fGroup,
			fSpouse,fChildren,fAssistant,fAssistantPhone,fMSN,fJabber,fICQ,fICQUIN,
			fYahoo,fAIM,fEmail3,fEmail4,fEmail5,fOtherURL,fNotes,fWorkEmail,
			fWorkCellPhone,fWorkCountry,fWorkCity,fWorkAddress,fWorkURL,fPager,
			fWorkFax,fWorkZip,fWorkState,fWorkAddress2,fPosition,fCompany, fFileName;
	
	BBitmap *fPhoto;
	
	// list which holds pointers to a GroupItem for each group to which the person
	// belongs
	BList fGroupList;
	
	// Holds pointers to instances of PersonItem which utilize this data
	BList fItemList;
		
	entry_ref fFileRef;
	node_ref fNodeRef;
	
	// flag which, if false, means that only the name and group has been loaded
	bool fIsLoaded;
};


class GroupData
{
public:
	GroupData(const char *name);
	~GroupData(void);

	bool HasPerson(PersonData *person);
	void AddPerson(PersonData *person);
	void RemovePerson(PersonData *person);
	int32 CountPeople(void) { return fPersonList.CountItems(); }
	PersonData *PersonAt(int32 index) { return (PersonData*)fPersonList.ItemAt(index); }
	
	GroupItem *GetInstance(void);
	
	const char *Name(void) { return fName.String(); }
	void SetName(const char *name) { fName.SetTo(name); }
protected:
	friend class GroupList;
	GroupItem *fGroupItem;
	BList fPersonList;
	BString fName;
};

class GroupList
{
public:
	GroupList(void);
	~GroupList(void);
	
	GroupData *AddGroup(const char *name);
	void RemoveGroup(const char *name);
	bool HasGroup(const char *name);

	GroupData *FindGroup(const char *name);

	GroupItem *ItemForGroup(const char *name);
	GroupItem *ItemForGroup(GroupData *data);
	int32 CountGroups(void) { return fGroupList.CountItems(); }
	
protected:
	BList fGroupList;
};

extern BitmapDumper bitmapdumper;

void GetValidPersonName(BString *requested);

#endif
