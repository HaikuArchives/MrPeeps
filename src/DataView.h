#ifndef DATAVIEW_H
#define DATAVIEW_H

#include <TabView.h>
#include <TextControl.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <String.h>
#include "NewTextControl.h"
#include "BitmapView.h"

class PersonData;
class BitmapView;

class DataView : public BView
{
public:
	DataView(BRect frame);
	~DataView(void);
	
	void SetData(PersonData *data);
	void GetData(PersonData *data);
	void ShowTab(uint8 index);
	void SetFirstFieldFocus(void);
	BTextView *GetFocusTextView(void);

	void SetName(const char *newname) { fName->SetText(newname); }
	void SetTitle(const char *value) { fTitle->SetText(value); }
	void SetNickname(const char *value) { fNickname->SetText(value); }
	void SetEmail(const char *value) { fEmail->SetText(value); }
	void SetURL(const char *value) { fURL->SetText(value); }
	void SetHomePhone(const char *value) { fHomePhone->SetText(value); }
	void SetWorkPhone(const char *value) { fWorkPhone->SetText(value); }
	void SetCellPhone(const char *value) { fCellPhone->SetText(value); }
	void SetFax(const char *value) { fFax->SetText(value); }
	void SetAddress(const char *value) { fAddress->SetText(value); }
	void SetAddress2(const char *value) { fAddress2->SetText(value); }
	void SetCity(const char *value) { fCity->SetText(value); }
	void SetState(const char *value) { fState->SetText(value); }
	void SetZip(const char *value) { fZip->SetText(value); }
	void SetBirthday(const char *value) { fBirthday->SetText(value); }
	void SetAnniversary(const char *value) { fAnniversary->SetText(value); }
	void SetGroup(const char *value);
	void SetSpouse(const char *value) { fSpouse->SetText(value); }
	void SetChildren(const char *value) { fChildren->SetText(value); }
	void SetAssistant(const char *value) { fAssistant->SetText(value); }
	void SetAssistantPhone(const char *value) { fAssistantPhone->SetText(value); }
	void SetMSN(const char *value) { fMSN->SetText(value); }
	void SetJabber(const char *value) { fJabber->SetText(value); }
	void SetICQ(const char *value) { fICQ->SetText(value); }
	void SetYahoo(const char *value) { fYahoo->SetText(value); }
	void SetAIM(const char *value) { fAIM->SetText(value); }
	void SetEmail3(const char *value) { fEmail3->SetText(value); }
	void SetOtherURL(const char *value) { fOtherURL->SetText(value); }
	void SetNotes(const char *value) { fNotes->SetText(value); }
	void SetWorkEmail(const char *value) { fWorkEmail->SetText(value); }
	void SetWorkCellPhone(const char *value) { fWorkCellPhone->SetText(value); }
	void SetWorkCity(const char *value) { fWorkCity->SetText(value); }
	void SetWorkAddress(const char *value) { fWorkAddress->SetText(value); }
	void SetWorkURL(const char *value) { fWorkURL->SetText(value); }
	void SetPager(const char *value) { fPager->SetText(value); }
	void SetWorkFax(const char *value) { fWorkFax->SetText(value); }
	void SetWorkZip(const char *value) { fWorkZip->SetText(value); }
	void SetWorkState(const char *value) { fWorkState->SetText(value); }
	void SetWorkAddress2(const char *value) { fWorkAddress2->SetText(value); }
	void SetPosition(const char *value) { fPosition->SetText(value); }
	void SetCompany(const char *value) { fCompany->SetText(value); }
	void SetPhoto(BBitmap *value) { fBitmapView->SetBitmap(value); }

	const char *Name(void) const { return fName->Text(); }
	const char *Title(void) const { return fTitle->Text(); }
	const char *Nickname(void) const { return fNickname->Text(); }
	const char *Email(void) const { return fEmail->Text(); }
	const char *URL(void) const { return fURL->Text(); }
	const char *HomePhone(void) const { return fHomePhone->Text(); }
	const char *WorkPhone(void) const { return fWorkPhone->Text(); }
	const char *CellPhone(void) const { return fCellPhone->Text(); }
	const char *Fax(void) const { return fFax->Text(); }
	const char *Address(void) const { return fAddress->Text(); }
	const char *Address2(void) const { return fAddress2->Text(); }
	const char *City(void) const { return fCity->Text(); }
	const char *State(void) const { return fState->Text(); }
	const char *Zip(void) const { return fZip->Text(); }
	const char *Birthday(void) const { return fBirthday->Text(); }
	const char *Anniversary(void) const { return fAnniversary->Text(); }
	const char *Group(void) const;
	const char *Spouse(void) const { return fSpouse->Text(); }
	const char *Children(void) const { return fChildren->Text(); }
	const char *Assistant(void) const { return fAssistant->Text(); }
	const char *AssistantPhone(void) const { return fAssistantPhone->Text(); }
	const char *MSN(void) const { return fMSN->Text(); }
	const char *Jabber(void) const { return fJabber->Text(); }
	const char *ICQ(void) const { return fICQ->Text(); }
	const char *Yahoo(void) const { return fYahoo->Text(); }
	const char *AIM(void) const { return fAIM->Text(); }
	const char *Email3(void) const { return fEmail3->Text(); }
	const char *OtherURL(void) const { return fOtherURL->Text(); }
	const char *Notes(void) const { return fNotes->Text(); }
	const char *WorkEmail(void) const { return fWorkEmail->Text(); }
	const char *WorkCellPhone(void) const { return fWorkCellPhone->Text(); }
	const char *WorkCity(void) const { return fWorkCity->Text(); }
	const char *WorkAddress(void) const { return fWorkAddress->Text(); }
	const char *WorkURL(void) const { return fWorkURL->Text(); }
	const char *Pager(void) const { return fPager->Text(); }
	const char *WorkFax(void) const { return fWorkFax->Text(); }
	const char *WorkZip(void) const { return fWorkZip->Text(); }
	const char *WorkState(void) const { return fWorkState->Text(); }
	const char *WorkAddress2(void) const { return fWorkAddress2->Text(); }
	const char *Position(void) const { return fPosition->Text(); }
	const char *Company(void) const { return fCompany->Text(); }
	BBitmap *Photo(void) const { return fBitmapView->GetBitmap(); }
	
	void AddGroup(const char *data);
	void RemoveGroup(const char *data);
	bool HasGroup(const char *data);
	
	void SetGroupMode(void);
	void SetPersonMode(void);
	bool IsPersonMode(void) const { return fGroupEdit->IsHidden(); }

	void ResetFocus(void) { fName->MakeFocus(true); }

	void SetAddressVisible(const bool &show);
	void SetCityVisible(const bool &show);
	void SetStateVisible(const bool &show);
	void SetZIPVisible(const bool &show);
	void SetHPhoneVisible(const bool &show);
	void SetWPhoneVisible(const bool &show);
	void SetEmailVisible(const bool &show);
	void SetGroupVisible(const bool &show);
	
	void CutField(void);
	void CopyField(void);
	void PasteField(void);
	void UndoField(void);

private:
	typedef struct
	{
		BString string;
		float width;
	} widthstruct;
	
	BView *groupview;
	NewTextControl *fGroupEdit;	
	
	BTabView *personview;
	NewTextControl *fName, *fTitle, *fNickname,*fEmail,*fURL,*fHomePhone,*fWorkPhone,*fCellPhone,
			*fFax,*fAddress,*fAddress2,*fCity,*fState,*fZip,*fBirthday,*fAnniversary,*fGroup,
			*fSpouse,*fChildren,*fAssistant,*fAssistantPhone,*fMSN,*fJabber,*fICQ,
			*fYahoo,*fAIM,*fEmail3,*fOtherURL,*fWorkEmail,
			*fWorkCellPhone,*fWorkCity,*fWorkAddress,*fWorkURL,*fPager,
			*fWorkFax,*fWorkZip,*fWorkState,*fWorkAddress2,*fPosition,*fCompany;
	
	BTextView *fNotes;
	BitmapView *fBitmapView;
	
	BMenuField *groupfield;
	BPopUpMenu *groupmenu;
	BList widthlist;
};

#endif