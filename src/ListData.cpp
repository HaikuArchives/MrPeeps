#include <File.h>
#include <stdio.h>
#include <Path.h>
#include <TranslationUtils.h>

#include "defs.h"
#include "PeepsItem.h"
#include "ListData.h"

BitmapDumper bitmapdumper;

PersonData::PersonData(const entry_ref &ref)
{
	fFileRef=ref;
	fIsLoaded=false;
	fPhoto=NULL;
	
	// Because there is just *so* much data to process on startup, we only load what we need - 
	// the file's name and the group. Everything else is loaded on demand, but once in memory,
	// it stays in memory.
	LoadNameAndGroup(ref);
}

PersonData::~PersonData(void)
{
}

void PersonData::SyncName(void)
{
	// ensures that all attached PersonItems have their name matching current data
	for(int32 i=0;i<fItemList.CountItems();i++)
	{
		PersonItem *item=(PersonItem*)fItemList.ItemAt(i);
		item->SetName(Name());
	}
}

void PersonData::SaveToFile(void)
{
	if(!fIsLoaded)
		return;
	
	UpdateFilename();

	BFile file(&fFileRef,B_READ_WRITE);
	
	if(!fPhoto)
	{
		// If the photo was deleted, erase from file
		off_t size=file.GetSize(&size);
		
		if(size>0)
			file.SetSize(0);
	}
	
	if(fName.Length()>0)
		file.WriteAttr(PERSON_NAME, B_STRING_TYPE, 0, fName.String(), fName.Length()+1);
	
	// We don't check for length on these because it will not crash. Instead a 0-length string
	// results in a blank when displayed in Tracker, which is what we want
	
	file.WriteAttr(PERSON_TITLE, B_STRING_TYPE, 0, fTitle.String(), fTitle.Length()+1); 
	file.WriteAttr(PERSON_NICKNAME, B_STRING_TYPE, 0, fNickname.String(), fNickname.Length()+1); 
	file.WriteAttr(PERSON_EMAIL, B_STRING_TYPE, 0, fEmail.String(), fEmail.Length()+1); 
	file.WriteAttr(PERSON_URL, B_STRING_TYPE, 0, fURL.String(), fURL.Length()+1); 
	file.WriteAttr(PERSON_HOME_PHONE, B_STRING_TYPE, 0, fHomePhone.String(), 
			fHomePhone.Length()+1); 
	file.WriteAttr(PERSON_WORK_PHONE, B_STRING_TYPE, 0, fWorkPhone.String(), 
			fWorkPhone.Length()+1); 
	file.WriteAttr(PERSON_CELL_PHONE, B_STRING_TYPE, 0, fCellPhone.String(),
			fCellPhone.Length()+1); 
	file.WriteAttr(PERSON_FAX, B_STRING_TYPE, 0, fFax.String(), fFax.Length()+1); 
	file.WriteAttr(PERSON_ADDRESS, B_STRING_TYPE, 0, fAddress.String(), 
			fAddress.Length()+1); 
	file.WriteAttr(PERSON_ADDRESS2, B_STRING_TYPE, 0, fAddress2.String(),
			fAddress2.Length()+1); 
	file.WriteAttr(PERSON_CITY, B_STRING_TYPE, 0, fCity.String(), fCity.Length()+1); 
	file.WriteAttr(PERSON_STATE, B_STRING_TYPE, 0, fState.String(), fState.Length()+1); 
	file.WriteAttr(PERSON_ZIP, B_STRING_TYPE, 0, fZip.String(), fZip.Length()+1); 
	file.WriteAttr(PERSON_BIRTHDAY, B_STRING_TYPE, 0, fBirthday.String(), 
			fBirthday.Length()+1); 
	file.WriteAttr(PERSON_ANNIVERSARY, B_STRING_TYPE, 0, fAnniversary.String(),
			fAnniversary.Length()+1); 

	if(fGroup.Length()>0 && fGroup.ICompare("Ungrouped")!=0)
		file.WriteAttr(PERSON_GROUP, B_STRING_TYPE, 0, fGroup.String(), fGroup.Length()+1);
	else
		file.WriteAttr(PERSON_GROUP, B_STRING_TYPE, 0, "", 1);

	file.WriteAttr(PERSON_SPOUSE, B_STRING_TYPE, 0, fSpouse.String(), 
			fSpouse.Length()+1); 
	file.WriteAttr(PERSON_CHILDREN, B_STRING_TYPE, 0, fChildren.String(),
			fChildren.Length()+1); 
	file.WriteAttr(PERSON_ASSISTANT, B_STRING_TYPE, 0, fAssistant.String(),
			fAssistant.Length()+1); 
	file.WriteAttr(PERSON_ASSISTANT_PHONE, B_STRING_TYPE, 0, 
			fAssistantPhone.String(), fAssistantPhone.Length()+1); 
	file.WriteAttr(PERSON_MSN, B_STRING_TYPE, 0, fMSN.String(), fMSN.Length()+1); 
	file.WriteAttr(PERSON_JABBER, B_STRING_TYPE, 0, fJabber.String(), 
			fJabber.Length()+1); 
	file.WriteAttr(PERSON_ICQ, B_STRING_TYPE, 0, fICQ.String(), fICQ.Length()+1); 
	file.WriteAttr(PERSON_ICQ_UIN, B_STRING_TYPE, 0, fICQUIN.String(), 
			fICQUIN.Length()+1); 
	file.WriteAttr(PERSON_YAHOO, B_STRING_TYPE, 0, fYahoo.String(), fYahoo.Length()+1); 
	file.WriteAttr(PERSON_AIM, B_STRING_TYPE, 0, fAIM.String(), fAIM.Length()+1); 
	file.WriteAttr(PERSON_EMAIL3, B_STRING_TYPE, 0, fEmail3.String(), 
			fEmail3.Length()+1); 
	file.WriteAttr(PERSON_EMAIL4, B_STRING_TYPE, 0, fEmail4.String(), 
			fEmail4.Length()+1); 
	file.WriteAttr(PERSON_EMAIL5, B_STRING_TYPE, 0, fEmail5.String(), 
			fEmail5.Length()+1); 
	file.WriteAttr(PERSON_OTHER_URL, B_STRING_TYPE, 0, fOtherURL.String(), 
			fOtherURL.Length()+1); 
	file.WriteAttr(PERSON_NOTES, B_STRING_TYPE, 0, fNotes.String(), 
			fNotes.Length()+1); 
	file.WriteAttr(PERSON_WORK_EMAIL, B_STRING_TYPE, 0, fWorkEmail.String(), 
			fWorkEmail.Length()+1); 
	file.WriteAttr(PERSON_WORK_CELL, B_STRING_TYPE, 0, 
			fWorkCellPhone.String(), fWorkCellPhone.Length()+1); 
	file.WriteAttr(PERSON_WORK_COUNTRY, B_STRING_TYPE, 0, fWorkCountry.String(),
			fWorkCountry.Length()+1); 
	file.WriteAttr(PERSON_WORK_CITY, B_STRING_TYPE, 0, fWorkCity.String(),
			fWorkCity.Length()+1); 
	file.WriteAttr(PERSON_WORK_ADDRESS, B_STRING_TYPE, 0, fWorkAddress.String(),
			fWorkAddress.Length()+1); 
	file.WriteAttr(PERSON_WORK_URL, B_STRING_TYPE, 0, fWorkURL.String(),
			fWorkURL.Length()+1); 
	file.WriteAttr(PERSON_PAGER, B_STRING_TYPE, 0, fPager.String(), fPager.Length()+1); 
	file.WriteAttr(PERSON_WORK_FAX, B_STRING_TYPE, 0, fWorkFax.String(), 
			fWorkFax.Length()+1); 
	file.WriteAttr(PERSON_WORK_ZIP, B_STRING_TYPE, 0, fWorkZip.String(),
			fWorkZip.Length()+1); 
	file.WriteAttr(PERSON_WORK_STATE, B_STRING_TYPE, 0, fWorkState.String(),
			fWorkState.Length()+1); 
	file.WriteAttr(PERSON_WORK_ADDRESS2, B_STRING_TYPE, 0, fWorkAddress2.String(),
			fWorkAddress.Length()+1); 
	file.WriteAttr(PERSON_POSITION, B_STRING_TYPE, 0, fPosition.String(),
			fPosition.Length()+1); 
	file.WriteAttr(PERSON_COMPANY, B_STRING_TYPE, 0, fCompany.String(), 
			fCompany.Length()+1); 
}

void PersonData::SaveGroup(void)
{
	BFile file(&fFileRef,B_READ_WRITE);
	if(fGroup.Length()>0 && fGroup.ICompare("Ungrouped")!=0)
		file.WriteAttr(PERSON_GROUP, B_STRING_TYPE, 0, fGroup.String(), fGroup.Length()+1);
	else
		file.WriteAttr(PERSON_GROUP, B_STRING_TYPE, 0, "", 1);
}

void PersonData::SaveImageDataToFile(void)
{
	if(!fIsLoaded)
		return;
	
	BFile file(&fFileRef,B_READ_WRITE);
	
	if(file.InitCheck()!=B_OK)
		return;
	
	off_t filesize;
	file.GetSize(&filesize);
	
	if(filesize>0 && !fPhoto)
	{
		// Remove the image data from the file
		file.SetSize(0);
		return;
	}

	bitmapdumper.DumpToJPEG(fPhoto,&file);
}

void PersonData::LoadNameAndGroup(const entry_ref &ref)
{
	if(fIsLoaded)
		LoadFromFile(ref);
	
	BFile file(&ref, B_READ_ONLY);
	get_attribute(file, PERSON_NAME,&fName);
	get_attribute(file, PERSON_GROUP,&fGroup);
	file.GetNodeRef(&fNodeRef);
	fFileName=ref.name;
}

void PersonData::LoadFromFile(const entry_ref &ref)
{
	BFile file(&ref, B_READ_ONLY);
	
	if(file.InitCheck()!=B_OK)
		return;
	
	off_t filesize;
	file.GetSize(&filesize);
	if(filesize>0)
		fPhoto=BTranslationUtils::GetBitmap(&ref);
	
	get_attribute(file, PERSON_NAME,&fName);
	get_attribute(file, PERSON_TITLE,&fTitle);
	get_attribute(file, PERSON_NICKNAME,&fNickname);
	get_attribute(file, PERSON_EMAIL,&fEmail);
	get_attribute(file, PERSON_URL,&fURL);
	get_attribute(file, PERSON_HOME_PHONE,&fHomePhone);
	get_attribute(file, PERSON_WORK_PHONE,&fWorkPhone);
	get_attribute(file, PERSON_CELL_PHONE,&fCellPhone);
	get_attribute(file, PERSON_FAX,&fFax);
	get_attribute(file, PERSON_ADDRESS,&fAddress);
	get_attribute(file, PERSON_ADDRESS2,&fAddress2);
	get_attribute(file, PERSON_CITY,&fCity);
	get_attribute(file, PERSON_STATE,&fState);
	get_attribute(file, PERSON_ZIP,&fZip);
	get_attribute(file, PERSON_BIRTHDAY,&fBirthday);
	get_attribute(file, PERSON_ANNIVERSARY,&fAnniversary);
	get_attribute(file, PERSON_GROUP,&fGroup);
	get_attribute(file, PERSON_SPOUSE,&fSpouse);
	get_attribute(file, PERSON_CHILDREN,&fChildren);
	get_attribute(file, PERSON_ASSISTANT,&fAssistant);
	get_attribute(file, PERSON_ASSISTANT_PHONE,&fAssistantPhone);
	get_attribute(file, PERSON_MSN,&fMSN);
	get_attribute(file, PERSON_JABBER,&fJabber);
	get_attribute(file, PERSON_ICQ,&fICQ);
	get_attribute(file, PERSON_ICQ_UIN,&fICQUIN);
	get_attribute(file, PERSON_YAHOO,&fYahoo);
	get_attribute(file, PERSON_AIM,&fAIM);
	get_attribute(file, PERSON_EMAIL3,&fEmail3);
	get_attribute(file, PERSON_EMAIL4,&fEmail4);
	get_attribute(file, PERSON_EMAIL5,&fEmail5);
	get_attribute(file, PERSON_OTHER_URL,&fOtherURL);
	get_attribute(file, PERSON_NOTES,&fNotes);
	get_attribute(file, PERSON_WORK_EMAIL,&fWorkEmail);
	get_attribute(file, PERSON_WORK_CELL,&fWorkCellPhone);
	get_attribute(file, PERSON_WORK_COUNTRY,&fWorkCountry);
	get_attribute(file, PERSON_WORK_CITY,&fWorkCity);
	get_attribute(file, PERSON_WORK_ADDRESS,&fWorkAddress);
	get_attribute(file, PERSON_WORK_URL,&fWorkURL);
	get_attribute(file, PERSON_PAGER,&fPager);
	get_attribute(file, PERSON_WORK_FAX,&fWorkFax);
	get_attribute(file, PERSON_WORK_ZIP,&fWorkZip);
	get_attribute(file, PERSON_WORK_STATE,&fWorkState);
	get_attribute(file, PERSON_WORK_ADDRESS2,&fWorkAddress2);
	get_attribute(file, PERSON_POSITION,&fPosition);
	get_attribute(file, PERSON_COMPANY,&fCompany);
	fFileName=ref.name;
	fIsLoaded=true;
}
	
PersonItem *PersonData::CreateInstance(const char *groupname)
{
	PersonItem *item=new PersonItem(this);
	item->SetGroup(groupname);
	fItemList.AddItem(item);
	return item;
}

void PersonData::DestroyInstance(PersonItem *item)
{
	fItemList.RemoveItem(item);
	delete item;
}

bool PersonData::HasInstance(PersonItem *item)
{
	return fItemList.HasItem(item);
}

void PersonData::UpdateFilename(void)
{
	//rename our file if it is needed
	if(fName==fFileName)
		return;
	
	fFileName=fName;
	GetValidPersonName(&fFileName);
	
	BEntry toentry(&fFileRef);
	toentry.Rename(fFileName.String());
	toentry.GetRef(&fFileRef);
}

void PersonData::PrintToStream(void)
{
	printf("PersonData : %s\n"
		"\tE-Mail: %s\n"
		"\tHome Phone: %s\n"
		"\tWork Phone: %s\n"
		"\tAddress: %s\n"
		"\tCity: %s\n"
		"\tState: %s\n"
		"\tZip: %s\n"
		"\tGroup: %s\n", 
		fName.String(),fEmail.String(),fHomePhone.String(),
		fWorkPhone.String(),fAddress.String(),fCity.String(),
		fState.String(),fZip.String(),fGroup.String());
}

void PersonData::RenameGroup(const char *from, const char *to)
{
	// This function only concerns the object's data and is called when
	// changing the name of one of the groups to which it belongs -- this
	// updates the string obtained by Group() without obliterating the other
	// group names
	
	if(!from || !to)
	{
		printf("ERROR: Attempted PersonData group rename using NULL value\n");
		return;
	}
	
	if(fGroup.FindFirst(to)!=B_ERROR)
	{
		for(int32 i=0; i<fGroupList.CountItems(); i++)
		{
			GroupItem *item=(GroupItem*)fGroupList.ItemAt(i);
			if(strcmp(item->Name(),to)==0)
			{
				RemoveFromGroup(item);
				fGroup.RemoveFirst(from);
				return;
			}
		}
	}
	
	fGroup.ReplaceFirst(from,to);

	BFile file(&fFileRef,B_READ_WRITE);
	if(fGroup.Length()>0 && fGroup.ICompare("Ungrouped")!=0)
		file.WriteAttr(PERSON_GROUP, B_STRING_TYPE, 0, fGroup.String(), fGroup.Length()+1);
	else
		file.WriteAttr(PERSON_GROUP, B_STRING_TYPE, 0, "", 1);
}

PersonItem *PersonData::InstanceForGroup(const char *name)
{
	if(!name)
		return NULL;
		
	for(int32 i=0; i<fItemList.CountItems(); i++)
	{
		PersonItem *item=(PersonItem*)fItemList.ItemAt(i);
		if(strcmp(item->Group(),name)==0)
			return item;
	}
	return NULL;
}

void PersonData::AddToGroup(GroupItem *group)
{
	if(fGroupList.HasItem(group))
		return;
	
	fGroupList.AddItem(group);
	if(fGroupList.CountItems()>1)
	{
		fGroup+=",";
		fGroup+=group->Name();
	}
	else
		fGroup=group->Name();
}

void PersonData::RemoveFromGroup(GroupItem *group)
{
	fGroupList.RemoveItem(group);
	
	if(fGroupList.CountItems()==0)
		fGroup="";
	else
	{
		fGroup.RemoveFirst(group->Name());
		if(fGroup.ByteAt(0L)==',')
		{
			fGroup.RemoveFirst(",");
			return;
		}
		else
		if(fGroup.ByteAt(fGroup.CountChars()-1)==',')
		{
			fGroup.RemoveLast(",");
		}
		else
		if(fGroup.FindFirst(",,")!=B_ERROR)
		{
			fGroup.ReplaceFirst(",,",",");
		}
	}
}

void PersonData::SetPhoto(BBitmap *bmp)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	if(fPhoto)
		delete fPhoto;
	
	if(bmp && bmp->IsValid())
		fPhoto=bmp;
	else
		fPhoto=NULL;
}

void PersonData::SetName(const char *newname)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fName.SetTo(newname);
}

void PersonData::SetTitle(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fTitle.SetTo(value);
}

void PersonData::SetNickname(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fNickname.SetTo(value);
}

void PersonData::SetEmail(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fEmail.SetTo(value);
}

void PersonData::SetURL(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fURL.SetTo(value);
}

void PersonData::SetHomePhone(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fHomePhone.SetTo(value);
}

void PersonData::SetWorkPhone(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fWorkPhone.SetTo(value);
}

void PersonData::SetCellPhone(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fCellPhone.SetTo(value);
}

void PersonData::SetFax(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fFax.SetTo(value);
}

void PersonData::SetAddress(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fAddress.SetTo(value);
}

void PersonData::SetAddress2(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fAddress2.SetTo(value);
}

void PersonData::SetCity(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fCity.SetTo(value);
}

void PersonData::SetState(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fState.SetTo(value);
}

void PersonData::SetZip(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fZip.SetTo(value);
}

void PersonData::SetBirthday(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fBirthday.SetTo(value);
}

void PersonData::SetAnniversary(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fAnniversary.SetTo(value);
}

void PersonData::SetGroup(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fGroup.SetTo(value);
}

void PersonData::SetSpouse(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fSpouse.SetTo(value);
}

void PersonData::SetChildren(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fChildren.SetTo(value);
}

void PersonData::SetAssistant(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fAssistant.SetTo(value);
}

void PersonData::SetAssistantPhone(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fAssistantPhone.SetTo(value);
}

void PersonData::SetMSN(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fMSN.SetTo(value);
}

void PersonData::SetJabber(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fJabber.SetTo(value);
}

void PersonData::SetICQ(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fICQ.SetTo(value);
}

void PersonData::SetICQUIN(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fICQUIN.SetTo(value);
}

void PersonData::SetYahoo(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fYahoo.SetTo(value);
}

void PersonData::SetAIM(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fAIM.SetTo(value);
}

void PersonData::SetEmail3(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fEmail3.SetTo(value);
}

void PersonData::SetEmail4(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fEmail4.SetTo(value);
}

void PersonData::SetEmail5(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fEmail5.SetTo(value);
}

void PersonData::SetOtherURL(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fOtherURL.SetTo(value);
}

void PersonData::SetNotes(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fNotes.SetTo(value);
}

void PersonData::SetWorkEmail(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fWorkEmail.SetTo(value);
}

void PersonData::SetWorkCellPhone(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fWorkCellPhone.SetTo(value);
}

void PersonData::SetWorkCountry(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fWorkCountry.SetTo(value);
}

void PersonData::SetWorkCity(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fWorkCity.SetTo(value);
}

void PersonData::SetWorkAddress(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fWorkAddress.SetTo(value);
}

void PersonData::SetWorkURL(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fWorkURL.SetTo(value);
}

void PersonData::SetPager(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fPager.SetTo(value);
}

void PersonData::SetWorkFax(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fWorkFax.SetTo(value);
}

void PersonData::SetWorkZip(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fWorkZip.SetTo(value);
}

void PersonData::SetWorkState(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fWorkState.SetTo(value);
}

void PersonData::SetWorkAddress2(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fWorkAddress2.SetTo(value);
}

void PersonData::SetPosition(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fPosition.SetTo(value);
}

void PersonData::SetCompany(const char *value)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	fCompany.SetTo(value);
}

BBitmap *PersonData::Photo(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fPhoto;
}

const char *PersonData::Name(void)
{
//	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fName.String();
}

const char *PersonData::Title(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fTitle.String();
}

const char *PersonData::Nickname(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fNickname.String();
}

const char *PersonData::Email(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fEmail.String();
}

const char *PersonData::URL(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fURL.String();
}

const char *PersonData::HomePhone(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fHomePhone.String();
}

const char *PersonData::WorkPhone(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fWorkPhone.String();
}

const char *PersonData::CellPhone(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fCellPhone.String();
}

const char *PersonData::Fax(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fFax.String();
}

const char *PersonData::Address(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fAddress.String();
}

const char *PersonData::Address2(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fAddress2.String();
}

const char *PersonData::City(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fCity.String();
}

const char *PersonData::State(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fState.String();
}

const char *PersonData::Zip(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fZip.String();
}

const char *PersonData::Birthday(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fBirthday.String();
}

const char *PersonData::Anniversary(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fAnniversary.String();
}

const char *PersonData::Group(void)
{
//	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fGroup.String();
}

const char *PersonData::Spouse(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fSpouse.String();
}

const char *PersonData::Children(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fChildren.String();
}

const char *PersonData::Assistant(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fAssistant.String();
}

const char *PersonData::AssistantPhone(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fAssistantPhone.String();
}

const char *PersonData::MSN(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fMSN.String();
}

const char *PersonData::Jabber(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fJabber.String();
}

const char *PersonData::ICQ(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fICQ.String();
}

const char *PersonData::ICQUIN(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fICQUIN.String();
}

const char *PersonData::Yahoo(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fYahoo.String();
}

const char *PersonData::AIM(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fAIM.String();
}

const char *PersonData::Email3(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fEmail3.String();
}

const char *PersonData::Email4(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fEmail4.String();
}

const char *PersonData::Email5(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fEmail5.String();
}

const char *PersonData::OtherURL(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fOtherURL.String();
}

const char *PersonData::Notes(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fNotes.String();
}

const char *PersonData::WorkEmail(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fWorkEmail.String();
}

const char *PersonData::WorkCellPhone(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fWorkCellPhone.String();
}

const char *PersonData::WorkCountry(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fWorkCountry.String();
}

const char *PersonData::WorkCity(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fWorkCity.String();
}

const char *PersonData::WorkAddress(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fWorkAddress.String();
}

const char *PersonData::WorkURL(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fWorkURL.String();
}

const char *PersonData::Pager(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fPager.String();
}

const char *PersonData::WorkFax(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fWorkFax.String();
}

const char *PersonData::WorkZip(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fWorkZip.String();
}

const char *PersonData::WorkState(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fWorkState.String();
}

const char *PersonData::WorkAddress2(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fWorkAddress2.String();
}

const char *PersonData::Position(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fPosition.String();
}

const char *PersonData::Company(void)
{
	if(!fIsLoaded) LoadFromFile(fFileRef);
	return fCompany.String();
}


GroupData::GroupData(const char *name)
{
	fName.SetTo(name);
	
	fGroupItem=new GroupItem(name);
	fGroupItem->fData=this;
}

GroupData::~GroupData(void)
{
	delete fGroupItem;
}

GroupItem *GroupData::GetInstance(void)
{
	return fGroupItem;
}

bool GroupData::HasPerson(PersonData *person)
{
	for(int32 i=0; i<fPersonList.CountItems(); i++)
	{
		PersonData *pdata=(PersonData*)fPersonList.ItemAt(i);
		if(pdata==person)
			return true;
	}
	return false;
}

void GroupData::AddPerson(PersonData *person)
{
	// Return if item already belongs to the group or is invalid
	if(!person || HasPerson(person))
		return;
	
	fPersonList.AddItem(person);

	// We probably won't have a case where the person item says it
	// belongs to the group even though the group item has no record of
	// it, but we'll handle the case anyway.
	if(person->BelongsToGroup(fGroupItem))
		return;

	person->AddToGroup(fGroupItem);
}

void GroupData::RemovePerson(PersonData *person)
{
	if(!person)
		return;
	
	if(person->BelongsToGroup(fGroupItem))
	{
		fPersonList.RemoveItem(person);
		person->RemoveFromGroup(fGroupItem);
	}
}

GroupList::GroupList(void)
{
	fGroupList.MakeEmpty();
}

GroupList::~GroupList(void)
{
	for(int32 i=0; i<fGroupList.CountItems();i++)
	{
		GroupData *data=(GroupData*)fGroupList.ItemAt(i);
		delete data;
	}
}

GroupData *GroupList::AddGroup(const char *name)
{
	if(!name)
		return NULL;
	
	GroupData *gdata=FindGroup(name);

	if(gdata)
		return gdata;

	gdata=new GroupData(name);
	fGroupList.AddItem(gdata);
	return gdata;
}

void GroupList::RemoveGroup(const char *name)
{
	if(!name)
		return;
	
	GroupData *gdata=FindGroup(name);
	if(gdata)
	{
		fGroupList.RemoveItem(gdata);
		delete gdata;
	}
}

bool GroupList::HasGroup(const char *name)
{
	return FindGroup(name)?true:false;
}

GroupData *GroupList::FindGroup(const char *name)
{
	if(!name)
		return NULL;
	
	GroupData *gdata;
	for(int32 i=0; i<fGroupList.CountItems(); i++)
	{
		gdata=(GroupData*)fGroupList.ItemAt(i);
		if(strcmp(name,gdata->Name())==0)
			return gdata;
	}
	return NULL;
}

GroupItem *GroupList::ItemForGroup(const char *name)
{
	if(!name)
		return NULL;
	
	GroupData *gdata=FindGroup(name);

	return (gdata)?gdata->fGroupItem:NULL;
}

GroupItem *GroupList::ItemForGroup(GroupData *data)
{
	if(!data)
		return NULL;
		
	return data->fGroupItem;
}

void GetValidPersonName(BString *requested)
{
	// Given the requested name of a person, this function checks to see if it exists
	// and if it does, modifies the name such that appending the name to the path will
	// result in a non-existant filename.
	
	if(!requested)
		return;
	
	BString workpath(requested->String());
	workpath.Prepend("/boot/home/people/");

	BEntry entry(workpath.String());

	char newpath[B_PATH_NAME_LENGTH];
	int32 attempt=1;

	if(entry.Exists())
	{
		do
		{
			attempt++;
			sprintf(newpath, "/boot/home/people/%s %ld", requested->String(), attempt);
			entry.SetTo(newpath);

		} while (entry.Exists());

		sprintf(newpath, "%s %ld", requested->String(), attempt);
		requested->SetTo(newpath);
	}
}
