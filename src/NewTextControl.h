#ifndef DATACONTROL_H_
#define DATACONTROL_H_

#include <TextControl.h>
#include <String.h>

enum
{
	F_PHONE_NUMBER=0,
	F_GROUP,
	F_NUMERIC,
	F_UNFORMATTED=255
};

class NewTextControl : public BTextControl
{
public:
	NewTextControl(BRect frame, const char *name, const char *label,
			const char *text, BMessage *message,
			uint32 resize=B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags=B_WILL_DRAW | B_NAVIGABLE);
	virtual ~NewTextControl(void);
	virtual void MakeFocus(bool isfocus=true);
	virtual void SetText(const char *text);
	uint8 Format(void) const;
	void SetFormat(uint8 type);
protected:
	BString oldtext;
	uint8 format;
};

#endif
