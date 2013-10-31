#include <Application.h>
#include "MrPeeps.h"
#include <Window.h>
#include <stdio.h>
#include "DataView.h"
#include "PeepsWindow.h"
#include "defs.h"
#include "ListData.h"
#include "PeepsItem.h"
#include "PeopleList.h"
#include "Formatting.h"
#include <stdlib.h>
#include <String.h>

PeopleList::PeopleList(BRect frame, const char *name, DataView *dv)
	: BOutlineListView(frame, name, B_SINGLE_SELECTION_LIST,B_FOLLOW_TOP_BOTTOM)
{
	lastgroup=NULL;
	lastperson=NULL;
	fDataView=dv;
	fDeleteDisabled=false;
	fGroupSelection=false;
	fPopupMenu=NULL;
}

void PeopleList::SyncData(void)
{
	SyncData(NULL);
}

void PeopleList::SyncData(PersonItem *item)
{
	// Transfers current data in the textboxes to the PersonData and updates all the other instances
	// in other groups which belong to the PersonData instance in question.
	
	PersonData *data;
	PersonItem *currentitem;
	BString workstr;
	GroupData *ogdata,*ngdata;
	GroupParser ogp, ngp;
	
	if(!item)
	{
		int32 selection=CurrentSelection();
		if(selection<0)
		{
			// no selection, so we don't need to do anything
			return;
		}
		
		PeepsListItem *peepsitem=(PeepsListItem*)ItemAt(selection);
		
		// This, believe it or not, does happen. Hackish code, I know. :D
		if(peepsitem->IsGroup())
		{
			peepsitem->SetName(fDataView->Group());
			Draw(Bounds());
			return;
		}
		currentitem=(PersonItem*)peepsitem;
	}
	else
		currentitem=item;
	
	data=currentitem->GetData();
	if(!data)
	{
		debugger("NULL GetData() call in PeepsWindow::SyncData()\n");
		return;
	}
	
	if(strcmp(data->Name(),fDataView->Name())!=0)
	{
		if(strlen(fDataView->Name())<1)
			fDataView->SetName(TRANSLATE("Unnamed Person"));
		
		data->SetName(fDataView->Name());
		data->SyncName();
		SortItemsUnder(Superitem(currentitem),true,compare_peeps);
		Select(IndexOf(currentitem));
	}
	
	if(fDataView->IsPersonMode())
	{
		if(strlen(fDataView->Name())<1)
			fDataView->SetName(TRANSLATE("Unnamed Person"));
		
		data->SetName(fDataView->Name());
		data->SetTitle(fDataView->Title());
		data->SetNickname(fDataView->Nickname());
		data->SetEmail(fDataView->Email());
		data->SetURL(fDataView->URL());
		if(strcmp(data->HomePhone(),fDataView->HomePhone())!=0)
		{
			if(ParsePhoneNumber(fDataView->HomePhone(),workstr)==B_OK)
			{
				data->SetHomePhone(workstr.String());
				fDataView->SetHomePhone(workstr.String());
			}
			else
				data->SetHomePhone(fDataView->HomePhone());
		}
		if(strcmp(data->WorkPhone(),fDataView->WorkPhone())!=0)
		{
			if(ParsePhoneNumber(fDataView->WorkPhone(),workstr)==B_OK)
			{
				data->SetWorkPhone(workstr.String());
				fDataView->SetWorkPhone(workstr.String());
			}
			else
				data->SetWorkPhone(fDataView->WorkPhone());
		}
		if(strcmp(data->CellPhone(),fDataView->CellPhone())!=0)
		{
			if(ParsePhoneNumber(fDataView->CellPhone(),workstr)==B_OK)
			{
				data->SetCellPhone(workstr.String());
				fDataView->SetCellPhone(workstr.String());
			}
			else
				data->SetCellPhone(fDataView->CellPhone());
		}
		if(strcmp(data->Fax(),fDataView->Fax())!=0)
		{
			if(ParsePhoneNumber(fDataView->Fax(),workstr)==B_OK)
			{
				data->SetFax(workstr.String());
				fDataView->SetFax(workstr.String());
			}
			else
				data->SetFax(fDataView->Fax());
		}
		data->SetAddress(fDataView->Address());
		data->SetAddress2(fDataView->Address2());
		data->SetCity(fDataView->City());
		data->SetState(fDataView->State());
		data->SetZip(fDataView->Zip());
		data->SetBirthday(fDataView->Birthday());
		data->SetAnniversary(fDataView->Anniversary());
		data->SetSpouse(fDataView->Spouse());
		data->SetChildren(fDataView->Children());
		data->SetAssistant(fDataView->Assistant());
		data->SetAssistantPhone(fDataView->AssistantPhone());
		data->SetMSN(fDataView->MSN());
		data->SetJabber(fDataView->Jabber());
		data->SetICQ(fDataView->ICQ());
		data->SetYahoo(fDataView->Yahoo());
		data->SetAIM(fDataView->AIM());
		data->SetEmail3(fDataView->Email3());
		data->SetOtherURL(fDataView->OtherURL());
		data->SetNotes(fDataView->Notes());
		data->SetWorkEmail(fDataView->WorkEmail());
		if(strcmp(data->WorkCellPhone(),fDataView->WorkCellPhone())!=0)
		{
			if(ParsePhoneNumber(fDataView->WorkCellPhone(),workstr)==B_OK)
			{
				data->SetWorkCellPhone(workstr.String());
				fDataView->SetWorkCellPhone(workstr.String());
			}
			else
				data->SetWorkCellPhone(fDataView->WorkCellPhone());
		}
		data->SetWorkCity(fDataView->WorkCity());
		data->SetWorkAddress(fDataView->WorkAddress());
		data->SetWorkURL(fDataView->WorkURL());
		if(strcmp(data->Pager(),fDataView->Pager())!=0)
		{
			if(ParsePhoneNumber(fDataView->Pager(),workstr)==B_OK)
			{
				data->SetPager(workstr.String());
				fDataView->SetPager(workstr.String());
			}
			else
				data->SetPager(fDataView->Pager());
		}
		if(strcmp(data->WorkFax(),fDataView->WorkFax())!=0)
		{
			if(ParsePhoneNumber(fDataView->WorkFax(),workstr)==B_OK)
			{
				data->SetWorkFax(workstr.String());
				fDataView->SetWorkFax(workstr.String());
			}
			else
				data->SetWorkFax(fDataView->WorkFax());
		}
		data->SetWorkZip(fDataView->WorkZip());
		data->SetWorkState(fDataView->WorkState());
		data->SetWorkAddress2(fDataView->WorkAddress2());
		data->SetPosition(fDataView->Position());
		data->SetCompany(fDataView->Company());

		if(strcmp(data->Group(),fDataView->Group())!=0)
		{
			// Do some prep to ensure that the data is OK to work with
			// According to the BeOS standard, groups must be delimited by a comma with no space
			// following it, so we'll remove comma-space combinations along with double-commas, and 
			// leading and trailing spaces of each group.
			
			workstr=fDataView->Group();
			if(workstr.CountChars()==0)
				workstr=TRANSLATE("Ungrouped");
			
			while(workstr.FindFirst(",,")!=B_ERROR && workstr.CountChars()>0)
				workstr.ReplaceAll(",,",",");
			while(workstr.FindFirst(", ")!=B_ERROR && workstr.CountChars()>0)
				workstr.ReplaceAll(", ",",");
			while(workstr.FindFirst(" ,")!=B_ERROR && workstr.CountChars()>0)
				workstr.ReplaceAll(" ,",",");
			while(workstr.FindFirst(" ")==0 && workstr.CountChars()>0)
				workstr.RemoveFirst(" ");
			while(workstr.FindLast(",")==workstr.CountChars()-1 && workstr.CountChars()>0)
				workstr.RemoveLast(",");
			
			if(workstr.CountChars()==0)
				workstr=TRANSLATE("Ungrouped");
			
			ogp.SetTo(data->Group());
			ngp.SetTo(fDataView->Group());
			
			if(ngp.RemoveDuplicates())
				workstr=ngp.GroupString();
			
			fDataView->SetGroup(workstr.String());
			
			// Weed out the easy conditions - renaming a group in a file with only one group,
			// adding a group to an ungrouped item, or ungrouping a file
			
			if(ogp.CountGroups()==0)
				ogp.SetTo(TRANSLATE("Ungrouped"));
			if(ngp.CountGroups()==0)
				ngp.SetTo(TRANSLATE("Ungrouped"));
			
			if(ogp.CountGroups()==1 && ngp.CountGroups()==1)
			{
				// Changing groups with only one group -- most common case
				ngdata=gGroupData.FindGroup(ngp.GroupAt(0));
				ogdata=gGroupData.FindGroup(ogp.GroupAt(0));
				
				if(!ngdata)
				{
					// moving group existing group to a new group
					ngdata=gGroupData.AddGroup(ngp.GroupAt(0));
					AddItem(ngdata->GetInstance());
				}
				
				ogdata->RemovePerson(data);
				ngdata->AddPerson(data);
				PersonItem *pitem=data->InstanceForGroup(ogp.GroupAt(0));
				if(pitem)
				{
					// This is the place that'll be a big pain in the butt. We have all the data
					// we need to move the item from the old group to the new group. The trick is
					// to do it in such a way that it'll be easy to reselect the item.
					
					pitem->SetGroup(ngp.GroupAt(0));
		
					int32 newindex=FullListIndexOf(ngdata->GetInstance())+1;
					MoveItem(FullListIndexOf(pitem),newindex);
					Expand(ngdata->GetInstance());
		
					FullListSortItems(compare_peeps);
					DoForEach(sort_groups, (void*)this);
		
					newindex=IndexOf(pitem);
					Select(newindex);
				}
				
				if(ogdata->GetInstance()->IsExpanded())
					Collapse(ogdata->GetInstance());
		
				if(ogdata->CountPeople()==0)
				{
					// Remove empty group
					RemoveItem(ogdata->GetInstance());
					gGroupData.RemoveGroup(ogdata->Name());
				}
			}
			else
			{
				// Changing multiple groups
				
//				bool find_another_instance=false;
				
				// 1) Parse the old group string and remove any groups not found in the new one
				for(int32 i=0; i<ogp.CountGroups(); i++)
				{
					if(!ngp.HasGroup(ogp.GroupAt(i)))
					{
						ogdata=gGroupData.FindGroup(ogp.GroupAt(i));
						
						PersonItem *remove=data->InstanceForGroup(ogdata->Name());
						
						// If we delete the current selection, we will simply do nothing. The former
						// behavior of selecting the same data in another group was both
						// distracting, disorienting, and annoying
//						if(remove==currentitem)
//							find_another_instance=true;
						
						// Do the removal of the person from the group and the person's item
						// from the list. Note that we do not actually require to call
						// data->RemoveFromGroup() because the GroupData class does this for us
						// in RemovePerson().
						ogdata->RemovePerson(data);
						RemoveItem(remove);
						data->DestroyInstance(remove);
						
						// Remove any groups made empty
						if(ogdata->CountPeople()==0)
						{
							RemoveItem(ogdata->GetInstance());
							gGroupData.RemoveGroup(ogdata->Name());
						}
						else
						{
							Collapse(ogdata->GetInstance());
						}
					}
				}
				
				// 2) Add any newly-created groups
				for(int32 i=0;i<ngp.CountGroups(); i++)
				{
					if(!ogp.HasGroup(ngp.GroupAt(i)))
					{
						ngdata=gGroupData.AddGroup(ngp.GroupAt(i));
						if(!HasItem(ngdata->GetInstance()))
							AddItem(ngdata->GetInstance());
						data->AddToGroup(ngdata->GetInstance());
						AddUnder(data->CreateInstance(ngdata->Name()),ngdata->GetInstance());
					}
				}
				
				FullListSortItems(compare_peeps);
				DoForEach(sort_groups, (void*) this);
		
/*				if(find_another_instance)
				{
					// currentitem is invalid. Find another instance for the data being edited
					currentitem=data->InstanceAt(0L);
				}
				currentgitem=(GroupItem*)Superitem(currentitem);
				if(!currentgitem->IsExpanded())
					Expand(currentgitem);
				Select(IndexOf(currentitem));
*/
			}
			data->SetGroup(fDataView->Group());
		} // end if dataview->IsPersonMode
	}
	else
	{
		// dataview is in Group Mode
		
	}
	
	// By not calling Invalidate(), we avoid some really annoying flicker problems
	Draw(Bounds());
}


void PeopleList::SaveData(void)
{
	// utility function for saving person data

	// first we find and call save on the selected object
	int selection=CurrentSelection(0);
	if(selection>=0)
	{
		PeepsListItem *selected_item=(PeepsListItem*)ItemAt(selection);
		
		if(!selected_item->IsGroup())
		{
#ifndef DATA_READ_ONLY
			PersonData *data=((PersonItem*)selected_item)->GetData();
			if(data)
				data->SaveToFile();
#endif
		}
		else
		{
			// Group item. Check for changes, and if there were any, change all subitems
			if(strcmp(fDataView->Group(),selected_item->Name())==0)
				return;
			
			// Check to see if the new group name exists
			GroupItem *ogitem=(GroupItem*)selected_item;
			GroupData *ngdata=gGroupData.FindGroup(fDataView->Group());
			if(ngdata)
			{
				// group exists. move all items to new (existing) group
				
				GroupItem *ngitem=ngdata->GetInstance();
				
				PersonItem *pitem=(PersonItem*)ItemUnderAt(selected_item,true,0);
				while(pitem)
				{
					PersonData *pdata=pitem->GetData();

					if(pdata->BelongsToGroup(ngitem))
					{
						// the person's data already belongs to the new group, so just delete the
						// item instance
						pdata->RemoveFromGroup(ogitem);
						pdata->DestroyInstance(pitem);
						continue;
					}
					
					int32 newindex=FullListIndexOf(ngdata->GetInstance())+1;
					pdata->RemoveFromGroup(ogitem);
					pdata->AddToGroup(ngitem);
					pdata->SaveGroup();
					MoveItem(FullListIndexOf(pitem),newindex);
					Expand(ngitem);
					
					pitem=(PersonItem*)ItemUnderAt(selected_item,true,0);
				}
				
				RemoveItem(ogitem);
				gGroupData.RemoveGroup(ogitem->Name());
								
				FullListSortItems(compare_peeps);
				DoForEach(sort_groups, (void*)this);
				Select(IndexOf(ngitem));

			}
			else
			{
				// new group doesn't already exist. Rename the group attribute in all subitems
				int32 i=0;
				PersonItem *pitem=(PersonItem*)ItemUnderAt(selected_item,true,i++);
				while(pitem)
				{
					PersonData *data=pitem->GetData();
					data->RenameGroup(selected_item->Name(),fDataView->Group());
					pitem=(PersonItem*)ItemUnderAt(selected_item,true,i++);
				}
				selected_item->SetName(fDataView->Group());
				((GroupItem*)selected_item)->GetData()->SetName(fDataView->Group());
				((PeepsWindow*)Window())->SortList();
				Select(IndexOf(selected_item));
			}
		}
	}
}

void PeopleList::SaveImageData(void)
{
	// We only receive this call when the data in DataView has changed and we need to update it
	
	int selection=CurrentSelection(0);
	if(selection>=0)
	{
		PeepsListItem *selected_item=(PeepsListItem*)ItemAt(selection);
		if(!selected_item->IsGroup())
		{
			PersonData *data=((PersonItem*)selected_item)->GetData();
			if(data)
			{
				data->SetPhoto(fDataView->Photo());
				data->SaveImageDataToFile();
			}
		}
	}
}

void PeopleList::SelectionChanged(void)
{
	// This is necessary because the code below will cause a hang if any changes to the 
	// selection are made in the main window's constructor
	if(Window()->IsHidden())
		return;
	
	PeepsWindow *window=(PeepsWindow*)Window();
	
	int selected_index=CurrentSelection();
		
	if (selected_index>=0)
	{
		PeepsListItem *currentitem=(PeepsListItem*)ItemAt(selected_index);
		
		// update the textboxes to reflect the change to the new "current" person data
		if(currentitem)
		{
			if(fDataView->IsHidden())
				fDataView->Show();

			if(!currentitem->IsGroup())
			{
				lastperson=(PersonItem*)currentitem;
				
				fDataView->SetPersonMode();
				fDataView->SetData(((PersonItem*)currentitem)->GetData());

				if(fDeleteDisabled)
				{
					window->PostMessage(M_ENABLE_DELETE);
					fDeleteDisabled=false;
				}
				
				if(fGroupSelection)
				{
					window->PostMessage(M_HANDLE_PERSON_SELECTION);
					fGroupSelection=false;
				}
				else
					window->UpdateActions(false);
			}
			else
			{
				fDataView->SetGroupMode();
				fDataView->SetGroup(currentitem->Name());

				if(BString(TRANSLATE("Ungrouped")).ICompare(currentitem->Name())==0)
				{
					window->PostMessage(M_DISABLE_DELETE);
					fDeleteDisabled=true;
				}
				else
				if(fDeleteDisabled)
				{
					window->PostMessage(M_ENABLE_DELETE);
					fDeleteDisabled=false;
				}

				if(!fGroupSelection)
				{
					window->PostMessage(M_HANDLE_GROUP_SELECTION);
					fGroupSelection=true;
				}
			}
		}
	}
	else 
	{
		// otherwise hide both for a blank screen
		if(!fDataView->IsHidden())
			fDataView->Hide();
	}
}

void PeopleList::MouseDown(BPoint pt)
{
	// Auto
	int32 index=IndexOf(pt);
	if(index<0)
		return;

	int32 selection=CurrentSelection();
	if(selection>=0)
	{
		PeepsListItem *pitem=(PeepsListItem*)ItemAt(selection);
		if(pitem && !pitem->IsGroup())
		{
			SyncData((PersonItem*)pitem);
			SaveData();
		}
	}
	
	BListItem *item=(BListItem*)ItemAt(index);
	if(!item)
		return;
	GroupItem *pitem=dynamic_cast<GroupItem*>(item);
	if(pitem)
	{
		if(lastgroup!=pitem)
		{
			// we have a group item, so expand the thing
			Expand(pitem);
			Select(index);
			if(lastgroup)
				Collapse(lastgroup);
			lastgroup=pitem;
		}
		else
		{
			BListItem *lastitem=ItemAt(CurrentSelection());
			Select(index);
			
			if(pitem->IsExpanded() && Superitem(lastitem)!=pitem)
				Collapse(pitem);
			else
				Expand(pitem);
			lastgroup=pitem;
		}
		
	}
	else
	{
		// we have a person item, so just select the thing
		GroupItem *currentgroup=(GroupItem*)Superitem(item);
		if(currentgroup && currentgroup!=lastgroup)
		{
			if(lastgroup)
				Collapse(lastgroup);
			lastgroup=currentgroup;
		}
		Select(index);

		BPoint temppoint;
		uint32 buttons;
		GetMouse(&temppoint,&buttons);
		
		pt.x-=5;
		pt.y-=5;
		
		if(buttons == B_SECONDARY_MOUSE_BUTTON)
			ShowContextMenu(ConvertToScreen(pt));
	}
	MakeFocus(true);

}


void PeopleList::KeyDown(const char *bytes,int32 numbytes)
{
	if(numbytes==1)
	{
		if(IsNameChar(bytes[0]))
		{
			Select(FindNextAlphabetical(bytes[0],CurrentSelection()));
			ScrollToSelection();
		}

		switch(bytes[0])
		{
			// contract item or move up to parent item if contracted
			case B_LEFT_ARROW:
			{
				PeepsListItem *item=(PeepsListItem*)ItemAt(CurrentSelection());
				if(!item)
					break;
					
				if(!Superitem(item))
				{
					if(item->IsExpanded())
						Collapse(item);
				}
				else
				{
					PeepsListItem *parent=(PeepsListItem*)Superitem(item);
					if(parent)
						Select(IndexOf(parent));

					ScrollToItem(parent);
				}
				break;
			}
			// expand item or move to first child item if expanded and one exists
			case B_RIGHT_ARROW:
			{
				PeepsListItem *item=(PeepsListItem*)ItemAt(CurrentSelection());
				if(!item)
					break;
					
				if(item->IsExpanded() || Superitem(item))
				{
					int32 index=FullListIndexOf(item);
					Select(index+1);
					
					ScrollToItem(item);

				}
				else
					Expand(item);
				break;
			}
			case B_UP_ARROW:
			case B_DOWN_ARROW:
			case B_HOME:
			case B_END:
			{
				BOutlineListView::KeyDown(bytes, numbytes);
				ScrollToItem((PeepsListItem*)ItemAt(CurrentSelection()));
				break;
			}
			case B_DELETE:
			{
				Window()->PostMessage(M_DELETE_PERSON);
				break;
			}
			// Toggle expansion on these keys
			case B_SPACE:
			case B_ENTER:
			{
				PeepsListItem *item=(PeepsListItem*)ItemAt(CurrentSelection());
				if(!item)
					break;
					
				if(item->IsExpanded())
					Collapse(item);
				else
					Expand(item);
				break;
			}
			default:
				BOutlineListView::KeyDown(bytes,numbytes);
		}
	}
	else
	{
		// Call the inherited version for everything that we don't specifically support
		BOutlineListView::KeyDown(bytes,numbytes);
	}
}


void PeopleList::ScrollToItem(PeepsListItem *item)
{
	// Scroll the item to the center of the view for maximum visibility

	// Get the visible region's size
	BRect bounds=Parent()->Bounds();
	BRect labelframe(ItemFrame(IndexOf(item)));
	float scrolly;

	if(bounds.Contains(labelframe))
		return;

	scrolly=labelframe.bottom-((bounds.Height()-labelframe.Height())/2);

	ScrollTo(BPoint(0,scrolly));
}

void PeopleList::MakeFocus(bool value)
{
	if(value)
		SaveData();

	BOutlineListView::MakeFocus(value);
	InvalidateItem(CurrentSelection());
}

void PeopleList::ShowContextMenu(BPoint pt)
{
	// Context menu functionality:
	// Send email to {pick a valid menu item}
	// Go to website {work, personal, other}
	// Get map of address
	
	// Construct the context menu based on what fields are not empty
	
	fPopupMenu=new BPopUpMenu(TRANSLATE("Actions"),false,false);

	BMenuItem *item;
	BMessage *msg;

	// Submenu for sending email to the selected contact	
	bool email=(strlen(fDataView->Email())>0);
	bool workemail=(strlen(fDataView->WorkEmail())>0);
	bool email3=(strlen(fDataView->Email3())>0);

	BMenu *emailmenu=new BMenu(TRANSLATE("Send E-Mail to"));
	
	if(email)
	{
		msg=new BMessage(M_SEND_EMAIL);
		msg->AddString("address",fDataView->Email());
		item=new BMenuItem(TRANSLATE("Personal Address"),msg);
		emailmenu->AddItem(item);
	}
	else
	{
		item=new BMenuItem(TRANSLATE("Personal Address"),new BMessage('dmmy'));
		emailmenu->AddItem(item);
		item->SetEnabled(false);
	}
	
	if(workemail)
	{
		msg=new BMessage(M_SEND_EMAIL);
		msg->AddString("address",fDataView->WorkEmail());
		emailmenu->AddItem(new BMenuItem(TRANSLATE("Work Address"),msg));
	}
	else
	{
		item=new BMenuItem(TRANSLATE("Work Address"),new BMessage('dmmy'));
		emailmenu->AddItem(item);
		item->SetEnabled(false);
	}

	if(email3)
	{
		msg=new BMessage(M_SEND_EMAIL);
		msg->AddString("address",fDataView->Email3());
		emailmenu->AddItem(new BMenuItem(TRANSLATE("Alternate Address"),msg));
	}
	else
	{
		item=new BMenuItem(TRANSLATE("Alternate Address"),new BMessage('dmmy'));
		emailmenu->AddItem(item);
		item->SetEnabled(false);
	}

	if(!email && !workemail && !email3)
		emailmenu->SetEnabled(false);

	emailmenu->SetTargetForItems(be_app);
	fPopupMenu->AddItem(emailmenu);
	
	// Submenu for browser launch
	bool url=(strlen(fDataView->URL())>0);
	bool workurl=(strlen(fDataView->WorkURL())>0);
	bool otherurl=(strlen(fDataView->OtherURL())>0);

	BMenu *browsermenu=new BMenu(TRANSLATE("Go to Web Page"));
	
	if(url)
	{
		msg=new BMessage(M_LAUNCH_BROWSER);
		msg->AddString("address",fDataView->URL());
		item=new BMenuItem(TRANSLATE("Personal Web Address"),msg);
		browsermenu->AddItem(item);
	}
	else
	{
		item=new BMenuItem(TRANSLATE("Personal Web Address"),new BMessage('dmmy'));
		browsermenu->AddItem(item);
		item->SetEnabled(false);
	}
	
	if(workurl)
	{
		msg=new BMessage(M_LAUNCH_BROWSER);
		msg->AddString("address",fDataView->WorkURL());
		browsermenu->AddItem(new BMenuItem(TRANSLATE("Work Web Address"),msg));
	}
	else
	{
		item=new BMenuItem(TRANSLATE("Work Web Address"),new BMessage('dmmy'));
		browsermenu->AddItem(item);
		item->SetEnabled(false);
	}

	if(otherurl)
	{
		msg=new BMessage(M_LAUNCH_BROWSER);
		msg->AddString("address",fDataView->OtherURL());
		browsermenu->AddItem(new BMenuItem(TRANSLATE("Alternate Web Address"),msg));
	}
	else
	{
		item=new BMenuItem(TRANSLATE("Alternate Web Address"),new BMessage('dmmy'));
		browsermenu->AddItem(item);
		item->SetEnabled(false);
	}

	if(!url && !workurl && !otherurl)
		browsermenu->SetEnabled(false);

	browsermenu->SetTargetForItems(be_app);
	fPopupMenu->AddItem(browsermenu);
	
	
	fPopupMenu->SetTargetForItems(this);
	fPopupMenu->Go(pt,true,true);
	
	delete fPopupMenu;
}

void PeopleList::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case B_SIMPLE_DATA:
		{
			Window()->PostMessage(msg);
			break;
		}
		default:
		{
			BOutlineListView::MessageReceived(msg);
			break;
		}
	}
}

int32 PeopleList::FindNextAlphabetical(char c, int32 index)
{
	PeepsListItem *fileitem;
	char name[255];
	if(index==-1)
		index=0;
	
	if(!IsNameChar(c))
		return -1;

	int32 i=index+1;
	while(i!=index)
	{
		if(i==CountItems())
			i=0;
		fileitem=(PeepsListItem*)ItemAt(i);
		if(fileitem)
		{
			strcpy(name,fileitem->Name());
			if(charncmp(name[0],c)==0)
				return i;
		}
		i++;
		if(i==CountItems())
			i=0;
	}
	return -1;
}

bool IsNameChar(char c)
{
	const char validstring[]="1234567890-_~.,+=!@#$%^&[]{}";
	if( (c>64 && c<91) || (c>96 && c<123))
		return true;
	int validlen=strlen(validstring);
	for(int i=0;i<validlen;i++)
	{
		if(c==validstring[i])
			return true;
	}
	return false;
}

int charncmp(char c1, char c2)
{
	if(c1>96 && c1<123)
		c1-=32;
	if(c2>96 && c2<123)
		c2-=32;

	if(c1<c2)
		return -1;
	else
		if(c2<c1)
			return 1;
	return 0;
}
