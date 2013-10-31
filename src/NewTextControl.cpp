#include "NewTextControl.h"
#include "PeepsWindow.h"

NewTextControl::NewTextControl(BRect frame, const char *name, const char *label,
			const char *text, BMessage *message, uint32 resize,
			uint32 flags)
  : BTextControl(frame,name,label,text,message,resize,flags)
{
	TextView()->SetDoesUndo(true);

	format=F_UNFORMATTED;
}

NewTextControl::~NewTextControl(void)
{
}

void NewTextControl::MakeFocus(bool isfocus)
{
	if(isfocus)
	{
		oldtext.SetTo(Text());
	}
	else
	{
		if(oldtext!=Text())
		{
			Invoke();
			oldtext.SetTo(Text());
		}
	}
	BTextControl::MakeFocus(isfocus);
}

void NewTextControl::SetText(const char *text)
{
	oldtext.SetTo(text);
	BTextControl::SetText(text);
}

uint8 NewTextControl::Format(void) const
{
	return format;
}

void NewTextControl::SetFormat(uint8 type)
{
	// Note that alphabetical characters are allowed so that we can convert
	// 800ABCDEFG to (800) 222-3334. BTW, Hooked On Phonics worked for me! :D
	
	BString phnum(TRANSLATE("META_DISALLOWED_PHONEBOX"));
	if(phnum.Compare("META_DISALLOWED_PHONEBOX")==0)
	{
		// This locale doesn't have any disallowed characters listed for
		// phone numbers, so we will use the default.
		phnum.SetTo("!@#$%^&*_=|\\]}[{\"':;?/><");
	}
	
	// For number-only cases
	BString numeric(TRANSLATE("META_DISALLOWED_NUMERICBOX"));
	if(phnum.Compare("META_DISALLOWED_NUMERICBOX")==0)
	{
		// This locale doesn't have any disallowed characters listed for
		// numbers-only boxes, so we will use the default.
		numeric.SetTo("!@#$%^&*()_-+=QWERTYUIOP{[}]|\\ASDFGHJKL;:'\"ZXCVBNM,<.>/?"
						"qwertyuiopasdfghjklzxcvbnm");
	}
	
	int32 i=0;
	
	if(format!=type)
	{
		// first, allow all characters
		switch(format)
		{
			case F_PHONE_NUMBER:
			case F_GROUP:
			{
				for(char c=phnum.ByteAt(i); c!=0; i++,c=phnum.ByteAt(i))
					TextView()->AllowChar(c);
				break;
			}
			case F_NUMERIC:
			{
				for(char c=numeric.ByteAt(i); c!=0; i++,c=numeric.ByteAt(i))
					TextView()->AllowChar(c);
				break;
			}
			default:
			{
				break;
			}
		}
		
		i=0;
		// next, disallow the proper characters
		switch(type)
		{
			case F_PHONE_NUMBER:
			case F_GROUP:
			{
				for(char c=phnum.ByteAt(i); c!=0; i++,c=phnum.ByteAt(i))
					TextView()->DisallowChar(c);
				break;
			}
			case F_NUMERIC:
			{
				for(char c=numeric.ByteAt(i); c!=0; i++,c=numeric.ByteAt(i))
					TextView()->DisallowChar(c);
				break;
			}
			default:
			{
				break;
			}
		}
		
		format=type;
	}
}
