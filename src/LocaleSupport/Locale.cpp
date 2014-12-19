#include <stdio.h>
#include <string>
#include <string.h>
#include <OS.h>
#include "TextFile.h"
#include "Locale.h"

/*
	This class is probably the complete wrong way to go about adding
	translations, but it shouldn't be too hard to rip out if it needs
	to be rewritten or replaced later.
*/

Locale::Locale(const char *name)
{
	fInit=false;
	fName=name;
}

Locale::Locale(void)
{
	fInit=false;
}

Locale::~Locale(void)
{
}

void Locale::Initialize(void)
{
	// Here we load the dictionary file for the app
	
	fInit=true;
	
	if(fName.CountChars()<1)
	{
		fName="English";
		return;
	}
	
	BString str("/boot/home/config/settings/MrPeeps/locale/");
	str+=fName;
	
	// Read in the dictionary file into a BString
	TextFile file(str.String(),B_READ_ONLY);
	
	// At this point, we'll read in all the data about the translation
	
	// Now read in the translation strings
	bool nextline=false;
	
	while(1)
	{
		BString line=file.ReadLine(),key="",value="";
		
		if(line.CountChars()==0)
			break;
		
		int32 i=0;
		
		for(i=0;i<line.CountChars();i++)
		{
			if(line[i]=='\t')
				break;
			
			// lines starting with ; or # are comments
			if(i==0)
			{
				if( line[i]=='#' || line[i]==';')
				{
					nextline=true;
					break;
				}
				else
				{
					key+=line[i];
				}
			}
			else
			{
				key+=line[i];
			}
		}
		
		if(nextline)
		{
			nextline=false;
			continue;
		}

		i++;
		while(line[i])
		{
			value+=line[i];
			i++;
		}
		
		fEntryList[key.String()]=value.String();
	}
	
	return;
}

BString Locale::Translate(const char *instring)
{
	if(!instring)
		return NULL;
	
	if(!fInit)
	{
		Initialize();
		if(!fInit)
		{
			printf("Locale %s has bad Initialize() function\n",Name());
			return instring;
		}
	}
	
	BString str=fEntryList[instring];
	if(str.CountChars()>0)
		return str;
	
	return BString(instring);
}

