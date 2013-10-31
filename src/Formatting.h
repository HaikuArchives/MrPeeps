#ifndef FORMATTING_H_
#define FORMATTING_H_

#include <String.h>
#include "BStringList.h"

status_t ParseName(const char *source, BString &first, BString &last);
status_t ParsePhoneNumber(const char *source, BString &out);
status_t ParseNumber(const char *source, BString &out);

status_t TokenizeWords(const char *source, BList *stringarray);
int32 CountWords(const char *source);

int32 WordToNumber(const char *source);
status_t TokenizeGroupString(const char *source, BStringList *stringlist);

bool IsDigitsOnly(const char *string);
bool IsNumber(const char *string);

#endif
