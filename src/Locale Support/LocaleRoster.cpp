#include "LocaleRoster.h"
#include <Directory.h>
#include <Entry.h>

static LocaleRoster sLocaleRoster;
LocaleRoster *locale_roster=&sLocaleRoster;

LocaleRoster::LocaleRoster(void)
{
	BString str("/boot/home/config/settings/MrPeeps/locale/");
	BDirectory dir("/boot/home/config/settings/MrPeeps/locale/");
	
	if(dir.InitCheck()!=B_OK)
		create_directory("/boot/home/config/settings/MrPeeps/locale/",0777);
	
	fDefaultLocale = new Locale();
	fLocaleList.AddItem(fDefaultLocale);
	
	entry_ref ref;
	while(dir.GetNextRef(&ref)==B_OK)
	{
		Locale *loc=new Locale(ref.name);
		fLocaleList.AddItem(loc);
	}
}

LocaleRoster::~LocaleRoster(void)
{
	for(int32 i=0; i<fLocaleList.CountItems(); i++)
	{
		Locale *loc=(Locale*)fLocaleList.ItemAt(i);
		delete loc;
	}
	fLocaleList.MakeEmpty();
}
	
int32 LocaleRoster::CountLocales(void) const
{
	return fLocaleList.CountItems();
}

Locale *LocaleRoster::LocaleAt(const int32 &index)
{
	return (Locale*)fLocaleList.ItemAt(index);
}
	
void LocaleRoster::SetLocale(const int32 &index)
{
	Locale *locale = (Locale *) fLocaleList.ItemAt(index);
	
	SetLocale(locale);
}

void LocaleRoster::SetLocale(Locale *locale)
{
	if(locale)
	{
		if(!locale->IsInitialized())
			locale->Initialize();
		
		fCurrentLocale = locale;
		
		return;
	}
	
	fCurrentLocale = fDefaultLocale;
}

bool LocaleRoster::SetLocale(const char *name)
{
	if(!name)
		return false;
	
	for(int32 i=0; i<fLocaleList.CountItems(); i++)
	{
		Locale *locale = (Locale*)fLocaleList.ItemAt(i);
		if(locale && strcmp(locale->Name(),name)==0)
		{
			SetLocale(locale);
			return true;
		}
	}
	return false;
}
