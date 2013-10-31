#include "defs.h"
#include "PeepsItem.h"
#include <Directory.h>
#include <OutlineListView.h>
#include <fs_attr.h>
#include <stdio.h>
#include <unistd.h>
#include "DataView.h"
#include "ListData.h"

PersonItem::PersonItem(PersonData *data)
 : PeepsListItem( (data)?data->Name():NULL, true)
{
	fData=data;
}

PersonItem::~PersonItem(void)
{
}

GroupItem::GroupItem(const char *name)
 : PeepsListItem(name, false)
{
}

status_t get_attribute(const BFile &file , const char *attribute, BString *string)
{
	//get the data from the file
	attr_info name_info;
	status_t stat=file.GetAttrInfo(attribute, &name_info);

	if(stat==B_ENTRY_NOT_FOUND)
	{
		string->SetTo("");
		return stat;
	}
	else
	if(stat==B_FILE_ERROR)
	{
		string->SetTo("");
		return stat;
	}
	
	char *tmp_name=new char[name_info.size];
	
	file.ReadAttr(attribute, B_STRING_TYPE, 0, tmp_name, name_info.size);

	if(strlen(tmp_name)>0)
		string->SetTo(tmp_name);
	else
		string->SetTo("");

	delete tmp_name;

	return stat;	
}

PeepsListItem::PeepsListItem(const char *name, bool expanded)
	: BListItem(0, expanded)
{
	fName.SetTo(name);
}

PeepsListItem::~PeepsListItem(void)
{
}

void PeepsListItem::DrawItem(BView *parent, BRect my_rect, bool complete)
{
	if ((IsSelected() || complete) && IsEnabled())
	{
		if (IsSelected())
		{
			if(parent->IsFocus())
			{
				parent->SetLowColor(0,0,0);
				parent->SetHighColor(255,255,255);
			}
			else
			{
				parent->SetLowColor(224,224,224);
				parent->SetHighColor(0,0,0);
			}
		}
		else
		{
			parent->SetLowColor(parent->ViewColor());
			parent->SetHighColor(0,0,0);
		}

		parent->FillRect(my_rect,B_SOLID_LOW);

		
//		if(parent->IsFocus() && IsSelected())
//		{
//			parent->SetHighColor(224,224,224);
//			parent->StrokeRect(my_rect);
//		}
	}
	else
	{
		parent->SetLowColor(255,255,255);
		parent->SetHighColor(0,0,0);
	}

	parent->MovePenTo(my_rect.left + 4, my_rect.bottom - 2);
	parent->DrawString(fName.String());
}

GroupParser::GroupParser(const char *string)
{
	SetTo(string);
}

GroupParser::GroupParser(void)
{
	namelist.MakeEmpty();
}

GroupParser::~GroupParser(void)
{
	MakeEmpty();
}

void GroupParser::SetTo(const char *string)
{
	MakeEmpty();
	if(!string || strlen(string)==0)
	{
		groupstring.SetTo("");
		return;
	}
	
	groupstring.SetTo(string);

	char workstr[strlen(string)+1];
	strcpy(workstr,string);
	
	strtok(workstr,",");
	
	char *token=strtok(NULL,","),*lasttoken=workstr;
	
	if(!token)
	{
		namelist.AddItem(strdup(string));
		return;
	}
	
	int32 length;
	
	while(token)
	{
		length=token-lasttoken;
		
		// eat up any initial white space
		while(lasttoken[0]=='\t' || lasttoken[0]==' ')
		{
			lasttoken++;
			length--;
		}
		namelist.AddItem(lasttoken);
		lasttoken=token;

		token=strtok(NULL,",");
	}
	
	// Now we have a NULL token, so we can just copy over the final string
	length=strlen(lasttoken);

	// eat up any initial white space
	while(lasttoken[0]=='\t' || lasttoken[0]==' ')
	{
		lasttoken++;
		length--;
	}

	namelist.AddItem(lasttoken);
}

bool GroupParser::RemoveDuplicates(void)
{
	// We'll do the nice & slow way: compare each item to the others and delete
	// any others which match
	
	bool rebuild_string=false;
	
	BString *current=NULL;
	
	if(namelist.CountItems()==1)
		return false;
	
	for(int32 i=0; i<namelist.CountItems()-1; i++)
	{
		current=namelist.ItemAt(i);

		for(int32 j=i+1; j<namelist.CountItems(); j++)
		{
			BString *item2=namelist.ItemAt(j);
			if(current->Compare(item2->String())==0)
			{
				namelist.RemoveItem(item2);
				delete item2;
				
				if(!rebuild_string)
					rebuild_string=true;
				j--;
			}
		}
	}
	
	if(rebuild_string)
	{
		groupstring="";
		
		int32 count=namelist.CountItems();
		
		for(int32 i=0; i<count; i++)
		{
			current=namelist.ItemAt(i);
			groupstring+=current->String();
			if(i<count-1)
				groupstring+=",";
		}
		SetTo(groupstring.String());
	}
	return rebuild_string;
}

void GroupParser::MakeEmpty(void)
{
	namelist.MakeEmpty(true);
}

void GroupParser::PrintToStream(void)
{
	printf("GroupParser: \n");
	for(int32 i=0; i<CountGroups();i++)
		printf("\t%s\n",(char *)namelist.ItemAt(i));
}

bool GroupParser::HasGroup(const char *name)
{
	if(!name)
		return false;
	
	return (groupstring.FindFirst(name)==B_ERROR)?false:true;
}
