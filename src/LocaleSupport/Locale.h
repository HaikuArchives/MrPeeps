#ifndef LOCALE_H
#define LOCALE_H

#include <string>
#include <map>
#include <String.h>

typedef std::map<BString,BString> LocaleEntryList;

class Locale
{
public:
	Locale(const char *name);
	Locale(void);
	
	virtual ~Locale(void);
	
	void Initialize(void);
	bool IsInitialized(void) const { return fInit; }
	
	const char *Name(void) { return fName.String(); }
	
	BString Translate(const char *instring);
	
private:
	LocaleEntryList fEntryList;
	bool fInit;
	BString fName;
};

#endif
