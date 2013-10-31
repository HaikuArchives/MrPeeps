#ifndef PARSEDATE_H
#define PARSEDATE_H

#include "Formatting.h"

status_t ParseDate(const char *source, BString &out);
status_t FormatDate(const char *source, BString &out);

int8 MonthToNumber(const char *string);
int8 DayToNumber(const char *string);
int16 DayOfYear(uint8 month, uint8 day, uint32 year);
int8 DaysInMonth(uint8 month,uint32 year);

bool IsLeapYear(uint32 year);
bool IsDay(const char *string);
bool IsUnit(const char *string);
bool IsMonth(const char *string);
bool IsDescriptor(const char *string);

status_t ModifyDateByUnit(int32 *month,int32 *day, int32 *year, int32 unitcount, const char *unit);

#endif
