#ifndef LOCALE_ROSTER_H
#define LOCALE_ROSTER_H

#include <List.h>
#include "Locale.h"

class LocaleRoster
{
public:
	LocaleRoster(void);
	~LocaleRoster(void);
	
	int32 CountLocales(void) const;
	Locale *LocaleAt(const int32 &index);
	
	void SetLocale(const int32 &index);
	void SetLocale(Locale *locale);
	bool SetLocale(const char *name);
	Locale *GetLocale(void) { return fCurrentLocale; }

private:
	BList fLocaleList;
	Locale *fCurrentLocale;
	Locale *fDefaultLocale;
};

extern LocaleRoster *locale_roster;

#endif
