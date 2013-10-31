#include <ScrollView.h>
#include "DataView.h"
#include "PeepsWindow.h"
#include "PeepsItem.h"
#include "ListData.h"
#include <stdio.h>
#include <Clipboard.h>

#ifndef FIELD_DIVIDE
#define FIELD_DIVIDE	90
#define VERTICAL_DIVIDE	30
#endif

DataView::DataView(BRect frame)
 :BView(frame,"dataview",B_FOLLOW_ALL,B_WILL_DRAW)
{	
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	// vars used to handle incidents where there are two text fields in the same line
	float leftend=(Bounds().Width()/2)-10;
	float rightstart=(Bounds().Width()/2)+5;
	
	BRect con_location(15, 25,Bounds().right-15,75);
	
	// Create the pane which edits group names
	groupview=new BView(Bounds(),"GroupView",B_FOLLOW_LEFT|B_FOLLOW_TOP,B_WILL_DRAW);
	groupview->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(groupview);
	
	fGroupEdit=new NewTextControl(con_location, "GroupName",TRANSLATE("Group Name"),0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fGroupEdit->SetDivider(StringWidth(TRANSLATE("Group Name"))+5);
	groupview->AddChild(fGroupEdit);
	
	
	// Begin work on the main pane which displays all the different information fields
	
	personview=new BTabView(Bounds(),"personview",B_WIDTH_FROM_LABEL,B_FOLLOW_ALL,B_WILL_DRAW);
	AddChild(personview);
	
	// Main Tab
	BView *mainview=new BView(Bounds(),TRANSLATE("Main"),B_FOLLOW_ALL,B_WILL_DRAW);
	mainview->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	con_location.Set(15, 10,Bounds().right/3,VERTICAL_DIVIDE*3);
	
	//'global' BRect for placement
	fBitmapView=new BitmapView(con_location,"photoview",new BMessage(M_PHOTO_MODIFIED),NULL);
	fBitmapView->SetBitmapRemovable(true);
	mainview->AddChild(fBitmapView);
	
	con_location=Bounds();
	con_location.left=fBitmapView->Frame().right+10;
	con_location.right-=15;
	con_location.top+=10;
	con_location.bottom-=75;
	
	// this value will be used to make the labels of certain fields in the object
	// right-aligned with each other
	float labelwidth;
	
	// This array will be used to cache values for StringWidth() calls because they are
	// rather expensive. The values will be overwritten after each alignment group is calculated.
	float strwidth[5];
	
	// Main tab	
	
	// Full name and nickname are aligned, but not title
	
	strwidth[0]=StringWidth(TRANSLATE("Full Name"))+5;
	strwidth[1]=StringWidth(TRANSLATE("Nickname"))+5;
	labelwidth=MAX(strwidth[0],strwidth[1]);
	
	fName=new NewTextControl(con_location, "Name", TRANSLATE("Full Name"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fName->SetAlignment(B_ALIGN_LEFT, B_ALIGN_LEFT);
	fName->SetDivider(labelwidth);
	mainview->AddChild(fName);

	con_location.top += VERTICAL_DIVIDE;
	fNickname=new NewTextControl(con_location, "Nickname", TRANSLATE("Nickname"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fNickname->SetAlignment(B_ALIGN_LEFT, B_ALIGN_LEFT);
	fNickname->SetDivider(labelwidth);
	mainview->AddChild(fNickname);

	con_location.top += VERTICAL_DIVIDE;
	fTitle=new NewTextControl(con_location, "Title", TRANSLATE("Title"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fTitle->SetAlignment(B_ALIGN_LEFT, B_ALIGN_LEFT);
	fTitle->SetDivider(StringWidth(TRANSLATE("Title"))+5);
	mainview->AddChild(fTitle);
	
	
	// align  birthday, spouse, children, and group, but not anniversary
	
	strwidth[0]=StringWidth(TRANSLATE("Birthday"));
	strwidth[1]=StringWidth(TRANSLATE("Spouse"));
	strwidth[2]=StringWidth(TRANSLATE("Children"));
	strwidth[3]=StringWidth(TRANSLATE("Group"));
	labelwidth=MAX(strwidth[0],strwidth[1]);
	labelwidth=MAX(labelwidth,strwidth[2]);
	labelwidth=MAX(labelwidth,strwidth[3])+5;
	
	con_location.top += VERTICAL_DIVIDE;
	con_location.left=15;
	fBirthday=new NewTextControl(BRect(con_location.left, con_location.top, leftend, con_location.bottom),
		"Birthday", TRANSLATE("Birthday"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fBirthday->SetDivider(labelwidth);
	mainview->AddChild(fBirthday);

	fAnniversary=new NewTextControl(BRect(rightstart, con_location.top, con_location.right, con_location.bottom),
		"Anniversary", TRANSLATE("Anniversary"), 0, new BMessage(M_MOD_DATA),	B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fAnniversary->SetDivider(StringWidth(TRANSLATE("Anniversary"))+5);
	mainview->AddChild(fAnniversary);
	
	con_location.top += VERTICAL_DIVIDE;
	fSpouse=new NewTextControl(con_location, "Spouse", TRANSLATE("Spouse"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fSpouse->SetDivider(labelwidth);
	mainview->AddChild(fSpouse);

	con_location.top += VERTICAL_DIVIDE;
	fChildren=new NewTextControl(con_location, "Children", TRANSLATE("Children"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fChildren->SetDivider(labelwidth);
	mainview->AddChild(fChildren);

	// groups menu
	con_location.top += VERTICAL_DIVIDE;
	groupmenu=new BPopUpMenu(TRANSLATE("Group"));
	groupfield=new BMenuField(BRect(80,236,220,265), "G", "", groupmenu);
	groupfield->SetDivider(0);
	groupfield->SetEnabled(true);
	mainview->AddChild(groupfield);
	
	groupfield->Hide();
	
	//now for the groups entry field
	fGroup=new NewTextControl(con_location, "Group Field", TRANSLATE("Group"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fGroup->SetDivider(labelwidth);
	fGroup->SetFormat(F_GROUP);
	mainview->AddChild(fGroup);

	personview->AddTab(mainview);

	// Personal Tab
	BView *homeview=new BView(Bounds(),TRANSLATE("Personal"),B_FOLLOW_ALL,B_WILL_DRAW);
	homeview->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	con_location=Bounds();
	con_location.left+=15;
	con_location.right-=15;
	con_location.top+=10;
	con_location.bottom-=75;
	
	strwidth[0]=StringWidth(TRANSLATE("Address"));
	strwidth[1]=StringWidth(TRANSLATE("Address (2)"));
	labelwidth=MAX(strwidth[0],strwidth[1])+5;
	
	fAddress=new NewTextControl(con_location, "Address", TRANSLATE("Address"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fAddress->SetDivider(labelwidth);
	homeview->AddChild(fAddress);

	con_location.top += VERTICAL_DIVIDE;
	fAddress2=new NewTextControl(con_location, "Address (2)", TRANSLATE("Address (2)"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fAddress2->SetDivider(labelwidth);
	homeview->AddChild(fAddress2);

	strwidth[0]=StringWidth(TRANSLATE("City"));
	strwidth[1]=StringWidth(TRANSLATE("State"));
	labelwidth=MAX(strwidth[0],strwidth[1])+5;
	
	con_location.top += VERTICAL_DIVIDE;
	fCity=new NewTextControl(con_location, "City", TRANSLATE("City"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fCity->SetDivider(labelwidth);
	homeview->AddChild(fCity);

	con_location.top += VERTICAL_DIVIDE;
	fState=new NewTextControl(BRect(con_location.left, con_location.top, leftend, con_location.bottom),
		"State", TRANSLATE("State"), 0, new BMessage(M_MOD_DATA));
	fState->SetDivider(labelwidth);
	homeview->AddChild(fState);
	
	fZip=new NewTextControl(BRect(rightstart, con_location.top, con_location.right, con_location.bottom),
		"Zip", TRANSLATE("Zip"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fZip->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	fZip->SetDivider(StringWidth(TRANSLATE("Zip"))+5);
	homeview->AddChild(fZip);

	strwidth[0]=StringWidth(TRANSLATE("Home Phone"));
	strwidth[1]=StringWidth(TRANSLATE("Cell Phone"));
	labelwidth=MAX(strwidth[0],strwidth[1])+5;
	
	con_location.top += VERTICAL_DIVIDE;
	fHomePhone=new NewTextControl(con_location, "Home Phone", TRANSLATE("Home Phone"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fHomePhone->SetDivider(labelwidth);
	fHomePhone->SetFormat(F_PHONE_NUMBER);
	homeview->AddChild(fHomePhone);
	
	con_location.top += VERTICAL_DIVIDE;
	fCellPhone=new NewTextControl(con_location, "Cell Phone", TRANSLATE("Cell Phone"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fCellPhone->SetDivider(labelwidth);
	fCellPhone->SetFormat(F_PHONE_NUMBER);
	homeview->AddChild(fCellPhone);
	
	strwidth[0]=StringWidth(TRANSLATE("Fax"));
	strwidth[1]=StringWidth(TRANSLATE("E-Mail"));
	labelwidth=MAX(strwidth[0],strwidth[1])+5;
	
	con_location.top += VERTICAL_DIVIDE;
	fFax=new NewTextControl(con_location, "Fax", TRANSLATE("Fax"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fFax->SetDivider(labelwidth);
	fFax->SetFormat(F_PHONE_NUMBER);
	homeview->AddChild(fFax);
	
	con_location.top += VERTICAL_DIVIDE;
	fEmail=new NewTextControl(con_location, "E-Mail", TRANSLATE("E-Mail"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fEmail->SetDivider(labelwidth);
	homeview->AddChild(fEmail);

	strwidth[0]=StringWidth(TRANSLATE("Alternate E-Mail"));
	strwidth[1]=StringWidth(TRANSLATE("Web Address"));
	strwidth[2]=StringWidth(TRANSLATE("Alt. Web Address"));
	labelwidth=MAX(strwidth[0],strwidth[1]);
	labelwidth=MAX(labelwidth,strwidth[2])+5;
	
	con_location.top += VERTICAL_DIVIDE;
	fEmail3=new NewTextControl(con_location, "Alternate E-Mail", TRANSLATE("Alternate E-Mail"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fEmail3->SetDivider(labelwidth);
	homeview->AddChild(fEmail3);

	con_location.top += VERTICAL_DIVIDE;
	fURL=new NewTextControl(con_location, "Web Address", TRANSLATE("Web Address"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fURL->SetDivider(labelwidth);
	homeview->AddChild(fURL);

	con_location.top += VERTICAL_DIVIDE;
	fOtherURL=new NewTextControl(con_location, "Alt. Web Address", TRANSLATE("Alt. Web Address"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fOtherURL->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	fOtherURL->SetDivider(labelwidth);
	homeview->AddChild(fOtherURL);

	personview->AddTab(homeview);

	// Work tab
	BView *workview=new BView(Bounds(),TRANSLATE("Work"),B_FOLLOW_ALL,B_WILL_DRAW);
	workview->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	con_location=Bounds();
	con_location.left+=15;
	con_location.right-=15;
	con_location.top+=10;
	con_location.bottom-=75;

	strwidth[0]=StringWidth(TRANSLATE("Position"));
	strwidth[1]=StringWidth(TRANSLATE("Company"));
	strwidth[2]=StringWidth(TRANSLATE("Address"));
	strwidth[3]=StringWidth(TRANSLATE("Address (2)"));
	labelwidth=MAX(strwidth[0],strwidth[1]);
	labelwidth=MAX(labelwidth,strwidth[2]);
	labelwidth=MAX(labelwidth,strwidth[3])+5;
	
	fPosition=new NewTextControl(con_location, "Position", TRANSLATE("Position"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fPosition->SetDivider(labelwidth);
	workview->AddChild(fPosition);

	con_location.top += VERTICAL_DIVIDE;
	fCompany=new NewTextControl(con_location, "Company", TRANSLATE("Company"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fCompany->SetDivider(labelwidth);
	workview->AddChild(fCompany);

	con_location.top += VERTICAL_DIVIDE;
	fWorkAddress=new NewTextControl(con_location, "Address", TRANSLATE("Address"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fWorkAddress->SetDivider(labelwidth);
	workview->AddChild(fWorkAddress);

	con_location.top += VERTICAL_DIVIDE;
	fWorkAddress2=new NewTextControl(con_location, "Address (2)", TRANSLATE("Address (2)"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fWorkAddress2->SetDivider(labelwidth);
	workview->AddChild(fWorkAddress2);
	
	con_location.top += VERTICAL_DIVIDE;
	fWorkCity=new NewTextControl(con_location, "City", TRANSLATE("City"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	// set divider later to align with fax
	workview->AddChild(fWorkCity);

	con_location.top += VERTICAL_DIVIDE;
	fWorkState=new NewTextControl(BRect(con_location.left, con_location.top, leftend, con_location.bottom),
		"State", TRANSLATE("State"), 0, new BMessage(M_MOD_DATA));
	// set divider later to align with fax
	workview->AddChild(fWorkState);
	
	fWorkZip=new NewTextControl(BRect(rightstart, con_location.top, con_location.right, con_location.bottom),
		"Zip", TRANSLATE("Zip"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	workview->AddChild(fWorkZip);

	con_location.top += VERTICAL_DIVIDE;
	fWorkPhone=new NewTextControl(BRect(con_location.left, con_location.top, leftend, con_location.bottom),
		"Work Phone", TRANSLATE("Work Phone"), 0, new BMessage(M_MOD_DATA));
	fWorkPhone->SetDivider(StringWidth(TRANSLATE("Work Phone"))+5);
	fWorkPhone->SetFormat(F_PHONE_NUMBER);
	workview->AddChild(fWorkPhone);

	fWorkCellPhone=new NewTextControl(BRect(rightstart, con_location.top, con_location.right, con_location.bottom),
		"Cell Phone", TRANSLATE("Cell Phone"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fWorkCellPhone->SetDivider(StringWidth(TRANSLATE("Cell Phone"))+5);
	fWorkCellPhone->SetFormat(F_PHONE_NUMBER);
	workview->AddChild(fWorkCellPhone);

	con_location.top += VERTICAL_DIVIDE;
	fWorkFax=new NewTextControl(BRect(con_location.left, con_location.top, leftend, con_location.bottom),
		"Fax", TRANSLATE("Fax"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	// set divider in just a moment
	fWorkFax->SetFormat(F_PHONE_NUMBER);
	workview->AddChild(fWorkFax);
	
	fPager=new NewTextControl(BRect(rightstart, con_location.top, con_location.right, con_location.bottom),
		"Pager", TRANSLATE("Pager"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fPager->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	fPager->SetFormat(F_PHONE_NUMBER);
	workview->AddChild(fPager);
	
	strwidth[0]=StringWidth(TRANSLATE("Zip"));
	strwidth[1]=StringWidth(TRANSLATE("Pager"));
	labelwidth=MAX(strwidth[0],strwidth[1])+5;
	
	fWorkZip->SetDivider(labelwidth);
	fPager->SetDivider(labelwidth);

	con_location.top += VERTICAL_DIVIDE;
	fWorkEmail=new NewTextControl(con_location, "E-Mail", TRANSLATE("E-Mail"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	workview->AddChild(fWorkEmail);
	
	strwidth[0]=StringWidth(TRANSLATE("City"));
	strwidth[1]=StringWidth(TRANSLATE("State"));
	strwidth[2]=StringWidth(TRANSLATE("Fax"));
	strwidth[3]=StringWidth(TRANSLATE("E-Mail"));
	labelwidth=MAX(strwidth[0],strwidth[1]);
	labelwidth=MAX(labelwidth,strwidth[2]);
	labelwidth=MAX(labelwidth,strwidth[3])+5;
	
	fWorkCity->SetDivider(labelwidth);
	fWorkState->SetDivider(labelwidth);
	fWorkFax->SetDivider(labelwidth);
	fWorkEmail->SetDivider(labelwidth);
	
	strwidth[0]=StringWidth(TRANSLATE("Web Address"));
	strwidth[1]=StringWidth(TRANSLATE("Assistant Phone"));
	labelwidth=MAX(strwidth[0],strwidth[1])+5;
	
	con_location.top += VERTICAL_DIVIDE;
	fWorkURL=new NewTextControl(con_location, "Web Address", TRANSLATE("Web Address"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fWorkURL->SetDivider(labelwidth);
	workview->AddChild(fWorkURL);
	
	con_location.top += VERTICAL_DIVIDE;
	fAssistant=new NewTextControl(con_location, "Assistant", TRANSLATE("Assistant"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fAssistant->SetDivider(StringWidth(TRANSLATE("Assistant"))+5);
	workview->AddChild(fAssistant);
	
	con_location.top += VERTICAL_DIVIDE;
	fAssistantPhone=new NewTextControl(con_location, "Assistant Phone", TRANSLATE("Assistant Phone"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fAssistantPhone->SetDivider(labelwidth);
	fAssistantPhone->SetFormat(F_PHONE_NUMBER);
	workview->AddChild(fAssistantPhone);
	
	personview->AddTab(workview);
	
	
	// Instant Message Tab
	
	BView *internetview=new BView(Bounds(),TRANSLATE("Instant Message"),B_FOLLOW_ALL,B_WILL_DRAW);
	internetview->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	con_location=Bounds();
	con_location.left+=15;
	con_location.right-=15;
	con_location.top+=10;
	con_location.bottom-=75;

	strwidth[0]=StringWidth(TRANSLATE("MSN Passport"));
	strwidth[1]=StringWidth(TRANSLATE("Yahoo! Messenger"));
	labelwidth=MAX(strwidth[0],strwidth[1])+5;
	
	fMSN=new NewTextControl(con_location, "MSN Passport", TRANSLATE("MSN Passport"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fMSN->SetDivider(labelwidth);
	internetview->AddChild(fMSN);
	
	con_location.top += VERTICAL_DIVIDE;
	fYahoo=new NewTextControl(con_location, "Yahoo! Messenger", TRANSLATE("Yahoo! Messenger"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fYahoo->SetDivider(labelwidth);
	internetview->AddChild(fYahoo);
	
	strwidth[0]=StringWidth(TRANSLATE("AOL"));
	strwidth[1]=StringWidth(TRANSLATE("ICQ"));
	strwidth[2]=StringWidth(TRANSLATE("Jabber"));
	labelwidth=MAX(strwidth[0],strwidth[1]);
	labelwidth=MAX(labelwidth,strwidth[2])+5;
	
	con_location.top += VERTICAL_DIVIDE;
	fAIM=new NewTextControl(con_location, "AOL", TRANSLATE("AOL"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fAIM->SetDivider(labelwidth);
	internetview->AddChild(fAIM);
	
	con_location.top += VERTICAL_DIVIDE;
	fICQ=new NewTextControl(con_location, "ICQ", TRANSLATE("ICQ"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fICQ->SetDivider(labelwidth);
	internetview->AddChild(fICQ);
	
	con_location.top += VERTICAL_DIVIDE;
	fJabber=new NewTextControl(con_location, "Jabber", TRANSLATE("Jabber"), 0, new BMessage(M_MOD_DATA),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fJabber->SetDivider(labelwidth);
	internetview->AddChild(fJabber);
	
	personview->AddTab(internetview);
	
	// Notes tab
	con_location.Set(5,5,Bounds().right-B_V_SCROLL_BAR_WIDTH-1, Bounds().bottom-B_H_SCROLL_BAR_HEIGHT-11);
	fNotes=new BTextView(con_location,"notesview",Bounds().InsetByCopy(10,10),B_FOLLOW_ALL, B_WILL_DRAW | B_NAVIGABLE);
	
	BScrollView *notesv=new BScrollView(TRANSLATE("Notes"),fNotes,B_FOLLOW_ALL,B_WILL_DRAW,false,true);
	
	personview->AddTab(notesv);
	
	groupview->Hide();
	personview->Hide();
}

DataView::~DataView(void)
{
	for(int32 i=0;i<widthlist.CountItems();i++)
		delete (widthstruct*)widthlist.ItemAt(i);
}

// Sets display fields to values in the data
void DataView::SetData(PersonData *data)
{
	if(!data)
		return;
	
	SetName(data->Name());
	SetTitle(data->Title());
	SetNickname(data->Nickname());
	SetEmail(data->Email());
	SetURL(data->URL());
	SetHomePhone(data->HomePhone());
	SetWorkPhone(data->WorkPhone());
	SetCellPhone(data->CellPhone());
	SetFax(data->Fax());
	SetAddress(data->Address());
	SetAddress2(data->Address2());
	SetCity(data->City());
	SetState(data->State());
	SetZip(data->Zip());
	SetBirthday(data->Birthday());
	SetAnniversary(data->Anniversary());
	SetGroup(data->Group());;
	SetSpouse(data->Spouse());
	SetChildren(data->Children());
	SetAssistant(data->Assistant());
	SetAssistantPhone(data->AssistantPhone());
	SetMSN(data->MSN());
	SetJabber(data->Jabber());
	SetICQ(data->ICQ());
	SetYahoo(data->Yahoo());
	SetAIM(data->AIM());
	SetEmail3(data->Email3());
	SetOtherURL(data->OtherURL());
	SetNotes(data->Notes());
	SetWorkEmail(data->WorkEmail());
	SetWorkCellPhone(data->WorkCellPhone());
	SetWorkCity(data->WorkCity());
	SetWorkAddress(data->WorkAddress());
	SetWorkURL(data->WorkURL());
	SetPager(data->Pager());
	SetWorkFax(data->WorkFax());
	SetWorkZip(data->WorkZip());
	SetWorkState(data->WorkState());
	SetWorkAddress2(data->WorkAddress2());
	SetPosition(data->Position());
	SetCompany(data->Company());
	SetPhoto(data->Photo());
}

// Places values in the passed PersonData from what are in
// the DataView's display fields
void DataView::GetData(PersonData *data)
{
	if(!data)
		return;
	
	data->SetName(fName->Text());
	data->SetTitle(fTitle->Text());
	data->SetNickname(fNickname->Text());
	data->SetEmail(fEmail->Text());
	data->SetURL(fURL->Text());
	data->SetHomePhone(fHomePhone->Text());
	data->SetWorkPhone(fWorkPhone->Text());
	data->SetCellPhone(fCellPhone->Text());
	data->SetFax(fFax->Text());
	data->SetAddress(fAddress->Text());
	data->SetAddress2(fAddress2->Text());
	data->SetCity(fCity->Text());
	data->SetState(fState->Text());
	data->SetZip(fZip->Text());
	data->SetBirthday(fBirthday->Text());
	data->SetAnniversary(fAnniversary->Text());
	data->SetGroup(fGroup->Text());;
	data->SetSpouse(fSpouse->Text());
	data->SetChildren(fChildren->Text());
	data->SetAssistant(fAssistant->Text());
	data->SetAssistantPhone(fAssistantPhone->Text());
	data->SetMSN(fMSN->Text());
	data->SetJabber(fJabber->Text());
	data->SetICQ(fICQ->Text());
	data->SetYahoo(fYahoo->Text());
	data->SetAIM(fAIM->Text());
	data->SetEmail3(fEmail3->Text());
	data->SetOtherURL(fOtherURL->Text());
	data->SetNotes(fNotes->Text());
	data->SetWorkEmail(fWorkEmail->Text());
	data->SetWorkCellPhone(fWorkCellPhone->Text());
	data->SetWorkCity(fWorkCity->Text());
	data->SetWorkAddress(fWorkAddress->Text());
	data->SetWorkURL(fWorkURL->Text());
	data->SetPager(fPager->Text());
	data->SetWorkFax(fWorkFax->Text());
	data->SetWorkZip(fWorkZip->Text());
	data->SetWorkState(fWorkState->Text());
	data->SetWorkAddress2(fWorkAddress2->Text());
	data->SetPosition(fPosition->Text());
	data->SetCompany(fCompany->Text());
	data->SetPhoto(fBitmapView->GetBitmap());
}

void DataView::ShowTab(uint8 index)
{
	if(personview->Selection()!=index)
		personview->Select(index);
}

void DataView::SetFirstFieldFocus(void)
{
	BView *view;
	
	switch(personview->Selection())
	{
		case 0:
			view=fName;
			break;
		case 1:
			view=fAddress;
			break;
		case 2:
			view=fPosition;
			break;
		case 3:
			view=fMSN;
			break;
		case 4:
			view=fNotes;
			break;
		default:
			return;
	}
	view->MakeFocus(true);
}

BTextView *DataView::GetFocusTextView(void)
{
	// this is a royal pain
	if(fName->TextView()->IsFocus()) return fName->TextView();
	if(fTitle->TextView()->IsFocus()) return fTitle->TextView();
	if(fNickname->TextView()->IsFocus()) return fNickname->TextView();
	if(fEmail->TextView()->IsFocus()) return fEmail->TextView();
	if(fURL->TextView()->IsFocus()) return fURL->TextView();
	if(fHomePhone->TextView()->IsFocus()) return fHomePhone->TextView();
	if(fWorkPhone->TextView()->IsFocus()) return fWorkPhone->TextView();
	if(fCellPhone->TextView()->IsFocus()) return fCellPhone->TextView();
	if(fFax->TextView()->IsFocus()) return fFax->TextView();
	if(fAddress->TextView()->IsFocus()) return fAddress->TextView();
	if(fAddress2->TextView()->IsFocus()) return fAddress2->TextView();
	if(fCity->TextView()->IsFocus()) return fCity->TextView();
	if(fState->TextView()->IsFocus()) return fState->TextView();
	if(fZip->TextView()->IsFocus()) return fZip->TextView();
	if(fBirthday->TextView()->IsFocus()) return fBirthday->TextView();
	if(fAnniversary->TextView()->IsFocus()) return fAnniversary->TextView();
	if(fGroup->TextView()->IsFocus()) return fGroup->TextView();
	if(fSpouse->TextView()->IsFocus()) return fSpouse->TextView();
	if(fChildren->TextView()->IsFocus()) return fChildren->TextView();
	if(fAssistant->TextView()->IsFocus()) return fAssistant->TextView();
	if(fAssistantPhone->TextView()->IsFocus()) return fAssistantPhone->TextView();
	if(fMSN->TextView()->IsFocus()) return fMSN->TextView();
	if(fJabber->TextView()->IsFocus()) return fJabber->TextView();
	if(fICQ->TextView()->IsFocus()) return fICQ->TextView();
	if(fYahoo->TextView()->IsFocus()) return fYahoo->TextView();
	if(fAIM->TextView()->IsFocus()) return fAIM->TextView();
	if(fEmail3->TextView()->IsFocus()) return fEmail3->TextView();
	if(fOtherURL->TextView()->IsFocus()) return fOtherURL->TextView();
	if(fWorkEmail->TextView()->IsFocus()) return fWorkEmail->TextView();
	if(fWorkCellPhone->TextView()->IsFocus()) return fWorkCellPhone->TextView();
	if(fWorkCity->TextView()->IsFocus()) return fWorkCity->TextView();
	if(fWorkAddress->TextView()->IsFocus()) return fWorkAddress->TextView();
	if(fWorkURL->TextView()->IsFocus()) return fWorkURL->TextView();
	if(fPager->TextView()->IsFocus()) return fPager->TextView();
	if(fWorkFax->TextView()->IsFocus()) return fWorkFax->TextView();
	if(fWorkZip->TextView()->IsFocus()) return fWorkZip->TextView();
	if(fWorkState->TextView()->IsFocus()) return fWorkState->TextView();
	if(fWorkAddress2->TextView()->IsFocus()) return fWorkAddress2->TextView();
	if(fPosition->TextView()->IsFocus()) return fPosition->TextView();
	if(fCompany->TextView()->IsFocus()) return fCompany->TextView();

	if(fNotes->IsFocus()) return fNotes;
	
	return NULL;
}

void DataView::CutField(void)
{
	BTextView *currentfield=GetFocusTextView();
	if(currentfield)
		currentfield->Cut(be_clipboard);
}

void DataView::CopyField(void)
{
	BTextView *currentfield=GetFocusTextView();
	if(currentfield)
		currentfield->Copy(be_clipboard);
}

void DataView::PasteField(void)
{
	BTextView *currentfield=GetFocusTextView();
	if(currentfield)
		currentfield->Paste(be_clipboard);
}

void DataView::UndoField(void)
{
	BTextView *currentfield=GetFocusTextView();
	if(currentfield)
		currentfield->Undo(be_clipboard);
}

const char * DataView::Group(void) const
{
	if(IsPersonMode())
		return fGroup->Text();
	return fGroupEdit->Text();
}

void DataView::SetGroup(const char *data)
{
	fGroup->SetText(data);
	fGroupEdit->SetText(data);
	AddGroup(data);
	BMenuItem *item=groupmenu->FindMarked();
	if(item)
		item->SetMarked(false);

	if( (strcmp(data,"")==0) || (strcmp(data,TRANSLATE("Ungrouped"))==0) )
		item=groupmenu->FindItem(TRANSLATE("Ungrouped"));
	else
		item=groupmenu->FindItem(data);
	item->SetMarked(true);
}

void DataView::AddGroup(const char *data)
{
	BMenuItem *item,*current;
	if( (strcmp(data,"")==0) || (strcmp(data,TRANSLATE("Ungrouped"))==0) )
	{
		item=groupmenu->FindItem(TRANSLATE("Ungrouped"));
		if(item)
			return;

		item=new BMenuItem(TRANSLATE("Ungrouped"), new BMessage(M_GROUP_MENU));
	}
	else
	{
		item=groupmenu->FindItem(data);
		if(item)
			return;
		item=new BMenuItem(data, new BMessage(M_GROUP_MENU));
	}
	
	// See if there would be a collision between the group edit box and the selection menu
	widthstruct *ws=new widthstruct;
	ws->width=StringWidth(data);
	ws->string=data;
	
	if( (groupfield->Frame().left+ws->width+25) > fGroup->Frame().left)
	{
		float x=fGroup->Frame().left-(groupfield->Frame().left+ws->width)-25;
		if(-x>groupfield->Frame().left)
			groupfield->MoveBy(-groupfield->Frame().left,0);
		else
			groupfield->MoveBy(x,0);
	}

	// Find the appropriate spot for our group so that it's all nice and sorted. :)
	for(int32 i=0;i<groupmenu->CountItems();i++)
	{
		current=groupmenu->ItemAt(i);
		if(current && (strcmp(current->Label(),item->Label())>0))
		{
			groupmenu->AddItem(item,i);
			return;
		}
	}
	groupmenu->AddItem(item);
}

void DataView::RemoveGroup(const char *data)
{
	BMenuItem *item=groupmenu->FindItem(data);
	if(item)
	{
		groupmenu->RemoveItem(item);
		delete item;
		for(int32 i=0;i<widthlist.CountItems();i++)
		{
			widthstruct *ws=(widthstruct*)widthlist.ItemAt(i);
			if(ws && ws->string.Compare(data)==0)
			{
				widthlist.RemoveItem(i);
				delete ws;
				break;
			}
		}
	}

	float maxwidth=0;
	for(int32 i=0;i<widthlist.CountItems();i++)
	{
		widthstruct *ws=(widthstruct*)widthlist.ItemAt(i);
		if(ws && ws->width>maxwidth)
			maxwidth=ws->width;
	}
	if( fGroup->Frame().left-(groupfield->Frame().left+maxwidth) > 100)
		groupfield->MoveBy( fGroup->Frame().left-(groupfield->Frame().left+maxwidth)-100,0);
}

bool DataView::HasGroup(const char *data)
{
	if( (strcmp(data,"")==0) || (strcmp(data,TRANSLATE("Ungrouped"))==0) )
		return ( (groupmenu->FindItem(TRANSLATE("Ungrouped")))?true:false);
	return ( (groupmenu->FindItem(data))?true:false);
}

void DataView::SetGroupMode(void)
{
	while(!personview->IsHidden())
		personview->Hide();
	
	while(groupview->IsHidden())
		groupview->Show();
}

void DataView::SetPersonMode(void)
{
	while(!groupview->IsHidden())
		groupview->Hide();

	while(personview->IsHidden())
		personview->Show();
}

void DataView::SetAddressVisible(const bool &show)
{
	if(show && fAddress->IsHidden())
		fAddress->Show();
	else
		if(!show && !fAddress->IsHidden())
			fAddress->Hide();
}

void DataView::SetCityVisible(const bool &show)
{
	if(show && fCity->IsHidden())
		fCity->Show();
	else
		if(!show && !fCity->IsHidden())
			fCity->Hide();
}

void DataView::SetStateVisible(const bool &show)
{
	if(show && fState->IsHidden())
		fState->Show();
	else
		if(!show && !fState->IsHidden())
			fState->Hide();
}

void DataView::SetZIPVisible(const bool &show)
{
	if(show && fZip->IsHidden())
		fZip->Show();
	else
		if(!show && !fZip->IsHidden())
			fZip->Hide();
}

void DataView::SetHPhoneVisible(const bool &show)
{
	if(show && fHomePhone->IsHidden())
		fHomePhone->Show();
	else
		if(!show && !fHomePhone->IsHidden())
			fHomePhone->Hide();
}

void DataView::SetWPhoneVisible(const bool &show)
{
	if(show && fWorkPhone->IsHidden())
		fWorkPhone->Show();
	else
		if(!show && !fWorkPhone->IsHidden())
			fWorkPhone->Hide();
}

void DataView::SetEmailVisible(const bool &show)
{
	if(show && fEmail->IsHidden())
		fEmail->Show();
	else
		if(!show && !fEmail->IsHidden())
			fEmail->Hide();
}

void DataView::SetGroupVisible(const bool &show)
{
	if(show && fGroup->IsHidden() && fGroupEdit->IsHidden())
	{
		fGroup->Show();
		fGroupEdit->Show();
	}
	else
		if(!show && !fGroup->IsHidden() && !fGroupEdit->IsHidden())
		{
			fGroup->Hide();
			fGroupEdit->Hide();
		}
}

