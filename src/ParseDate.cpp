#include "ParseDate.h"
#include <stdio.h>

// Does all the work of ParseDate plus returns it in a pretty format
status_t FormatDate(const char *source, BString &out)
{
	
	BString workstr;

	status_t status=ParseDate(source,workstr);
	
	if(status!=B_OK)
	{
		out=workstr;
		return status;
	}

	int32 month,day,year;
	sscanf(workstr.String(),"%2lu%2lu%4lu",&month,&day,&year);
	char date[32];
	sprintf(date,"%ld-%ld-%ld",month,day,year);
	out=date;
	return B_OK;
}

// Takes a source string and returns a date in the format mmddyyyy
// returns B_ERROR if unable to parse the source
// returns B_BAD_VALUE on NULL source pointers
status_t ParseDate(const char *source, BString &out)
{
	if(!source)
		return B_BAD_VALUE;
	
	status_t returnval=B_OK;

	// First task: discover the format in the form
	// number/unit/day/descriptor
	BString src(source);
	BString *tempstr;
	
	// remove any separators and make them all the same
	src.ReplaceAll(',',' ');
	src.ReplaceAll('.',' ');
	src.ReplaceAll('/',' ');
	src.ReplaceAll('-',' ');
	src.ReplaceAll('(',' ');
	src.ReplaceAll(')',' ');
	
	BList wordlist(0L);
	
	returnval=TokenizeWords(src.String(),&wordlist);
	if(returnval!=B_OK)
		return returnval;

	int32 wordcount=wordlist.CountItems();
	
	char formatarray[wordcount+1];
	for(int32 i=0;i<wordcount;i++)
	{
		tempstr=(BString*)wordlist.ItemAt(i);
		
		if(IsNumber(tempstr->String()))
			formatarray[i]='n';
		else
		if(IsDescriptor(tempstr->String()))
			formatarray[i]='d';
		else
		if(IsDay(tempstr->String()))
			formatarray[i]='w';
		else
		if(IsUnit(tempstr->String()))
			formatarray[i]='u';
		else
		if(IsMonth(tempstr->String()))
			formatarray[i]='m';
		else
			formatarray[i]='i';
	}
	formatarray[wordcount]='\0';
	
	printf("ParseDate=> Source: %s, formatted: %s, format is %s\n",source,src.String(),formatarray);
	
	// whittle down to the formats which we will handle
	
	// Supported formats:
	
	// nn -> 5/25
	// nnn	-> 5/25/2004, NOTE: UNIX format is NOT supported
	// nud -> two weeks ago
	// nudt -> two weeks from now
	// mn -> may 5
	// mnn -> may 5, 2004
	// w -> Tuesday
	// wmn -> Tuesday, May 5
	// wmnn -> Tuesday, May 5 2004
	// du -> next week
	// dw -> next Tuesday
	
	time_t timet;
	timet=time(NULL);
	
	struct tm *currenttime=localtime(&timet);
	int32 day=-1,month=-1, year=-1;
	
	if(formatarray[0]=='n')
	{
		switch(wordcount)
		{
			case 2:
			{
				// One format expected: day/month
				if(formatarray[1]!='n')
					goto formaterror;
					
				day=WordToNumber(((BString*)wordlist.ItemAt(1))->String());
				month=WordToNumber(((BString*)wordlist.ItemAt(0))->String());
				if(day==-1 || day>31 || month==-1 || month>12)
					goto formaterror;

				year=currenttime->tm_year+1900;
				break;
			}
			case 3:
			{
				// expected formats:
				// month/day/year
				// number/unit/[past|future}]

				if(formatarray[1]=='n')
				{
					if(formatarray[2]!='n')
						goto formaterror;

					month=WordToNumber(((BString*)wordlist.ItemAt(0))->String());
					day=WordToNumber(((BString*)wordlist.ItemAt(1))->String());
					year=WordToNumber(((BString*)wordlist.ItemAt(2))->String());

					if(day==-1 || day>31 || month==-1 || month>12)
						goto formaterror;
					
					if(year<50)
						year+=2000;
					else
					if(year<100)
						year+=1900;
				}
				else
				if(formatarray[1]=='u')
				{
					if(formatarray[2]!='d')
						goto formaterror;
										
					int32 unitcount=WordToNumber(((BString*)wordlist.ItemAt(0))->String());
					
					tempstr=(BString*)wordlist.ItemAt(2);
					if(tempstr->ICompare("ago",3)==0)
						unitcount*=-1;
					else
					if(tempstr->ICompare("hence",5)!=0)
						goto formaterror;
					
					year=currenttime->tm_year+1900;
					month=currenttime->tm_mon+1;
					day=currenttime->tm_mday;
					
					tempstr=(BString*)wordlist.ItemAt(1);
					ModifyDateByUnit(&month,&day,&year,unitcount,tempstr->String());
				}
				else
					goto formaterror;
				
				
					
				break;
			}
			case 4:
			{
				// Expected format:
				// number/unit/descriptor/time (named day, shortcut like 'now' or 'tomorrow')
				printf("Unsupported format: Number/Unit/Descriptor/Time\n");
				goto formaterror;
				break;
			}
			default:
			{
				out=source;
				returnval=B_ERROR;
				break;
			}
		}
	}
	else
	if(formatarray[0]=='m')
	{
	
		// mnn -> may 5, 2004
		if(wordcount==2)
		{
			// Expected format: May 5
			month=MonthToNumber(((BString*)wordlist.ItemAt(0))->String())+1;
			day=WordToNumber(((BString*)wordlist.ItemAt(1))->String());
			if(day==-1 || day>31 || month==-1 || month>12)
				goto formaterror;

			year=currenttime->tm_year+1900;
		}
		else
		if(wordcount==3)
		{
			// Expected format: May 5 2004
			month=MonthToNumber(((BString*)wordlist.ItemAt(0))->String())+1;
			day=WordToNumber(((BString*)wordlist.ItemAt(1))->String());
			year=WordToNumber(((BString*)wordlist.ItemAt(2))->String());
			
			if(day==-1 || day>31 || month==-1 || month>12)
				goto formaterror;
		}
		else
			goto formaterror;
	}
	else
	if(formatarray[0]=='w')
	{
		switch(wordcount)
		{
			case 1:
			{
				// w -> Tuesday, eventually to include holidays
				printf("Unsupported format: DayOfWeek\n");
//				break;
				goto formaterror;
			}
			case 2:
			{
				// dayofweek/month/day
				month=MonthToNumber(((BString*)wordlist.ItemAt(1))->String())+1;
				day=WordToNumber(((BString*)wordlist.ItemAt(2))->String());
				if(day==-1 || day>31 || month==-1 || month>12)
					goto formaterror;
	
				year=currenttime->tm_year+1900;
				break;
			}
			case 3:
			{
				// dayofweek/month/day/year
				month=MonthToNumber(((BString*)wordlist.ItemAt(1))->String())+1;
				day=WordToNumber(((BString*)wordlist.ItemAt(2))->String());
				year=WordToNumber(((BString*)wordlist.ItemAt(3))->String());
				
				if(day==-1 || day>31 || month==-1 || month>12)
					goto formaterror;
				break;
			}
			default:
			{
				goto formaterror;
			}
		}
	}
	else
	if(formatarray[0]=='d')
	{
		// du -> next week
		// dw -> next Tuesday
		if( ((BString*)wordlist.ItemAt(0))->ICompare("next")==0 )
		{
			tempstr=(BString*)wordlist.ItemAt(1);
			
			if(tempstr->ICompare("day")==0)
			{
				day=currenttime->tm_mday+1;
				month=currenttime->tm_mon;
				year=currenttime->tm_year+1900;
				
				if( day > DaysInMonth(month,year) )
				{
					day=1;
					
					// Do month rollover
					if(month==11)
					{
						year++;
						month=0;
					}
					else
						month++;
				}
			}
			else
			if(tempstr->ICompare("week")==0)
			{
				day=currenttime->tm_mday+7;
				month=currenttime->tm_mon;
				year=currenttime->tm_year+1900;
				
				if( day > DaysInMonth(month,year) )
				{
					// subtract number of days in month to get new date
					day-=DaysInMonth(month,year);
					
					// Do month rollover
					if(month==11)
					{
						year++;
						month=0;
					}
					else
						month++;
				}
			}
			else
			if(tempstr->ICompare("month")==0)
			{
				day=currenttime->tm_mday;
				year=currenttime->tm_year+1900;
				
				if(currenttime->tm_mon==11)
				{
					month=1;
					year++;
				}
				else
					month=currenttime->tm_mon+2;

				if(day>DaysInMonth(month,year))
					day=DaysInMonth(month,year);
				
			}
			if(tempstr->ICompare("year")==0)
			{
				day=currenttime->tm_mday;
				month=currenttime->tm_mon;
				year=currenttime->tm_year+1900+1;
			}
			else
				goto formaterror;
		}
		else
		if( ((BString*)wordlist.ItemAt(0))->ICompare("last")==0 )
		{
			tempstr=(BString*)wordlist.ItemAt(1);
			
			if(tempstr->ICompare("day")==0)
			{
				if(currenttime->tm_mday==1)
				{
					year=currenttime->tm_year+1900;
					month=currenttime->tm_mon;
					if(month==0)
					{
						month=12;
						year--;
					}
					else
						month--;
					
					day=DaysInMonth(month,year);
				}
				else
				{
					day=currenttime->tm_mday-1;
					month=currenttime->tm_mon;
					year=currenttime->tm_year+1900;
				}
			}
			else
			if(tempstr->ICompare("week")==0)
			{
				year=currenttime->tm_year+1900;
				month=currenttime->tm_mon+1;
				day=currenttime->tm_mday-7;

				if(currenttime->tm_mday<1)
				{
					if(month==1)
					{
						month=12;
						year--;
					}
					else
						month--;
					
					
					day+=DaysInMonth(month,year);
				}
			}
			else
			if(tempstr->ICompare("month")==0)
			{
				day=currenttime->tm_mday;
				year=currenttime->tm_year+1900;
				
				if(currenttime->tm_mon==0)
				{
					month=12;
					year--;
				}
				else
					month=currenttime->tm_mon;

				if(day>DaysInMonth(month,year))
					day=DaysInMonth(month,year);
				
			}
			if(tempstr->ICompare("year")==0)
			{
				day=currenttime->tm_mday;
				month=currenttime->tm_mon;
				year=currenttime->tm_year+1900-1;
			}
			else
				goto formaterror;
		}
		else
			goto formaterror;
	}
	else
	{
// I guess once in 1000 years there really _is_ a reason to use a goto. I feel like
// I need to go through a purification ritual. :D
formaterror:
		out=source;
		returnval=B_ERROR;
	}
	
	// now that we have the day, month, and year, we set the output string to the date
	char returnstring[20];
	sprintf(returnstring,"%.2ld%.2ld%.4ld",month,day,year);
	out=returnstring;
	
	for(int32 i=0; i<wordlist.CountItems(); i++)
	{
		tempstr=(BString*)wordlist.ItemAt(i);
		delete tempstr;
	}
	
	return returnval;
}

// This function takes a month, day and year and modifies it by a certain number of date
// units -- going ahead or back 5 weeks, 100 years, or 1 day.
status_t ModifyDateByUnit(int32 *month,int32 *day, int32 *year, int32 unitcount, const char *unit)
{
	// The algoritms here are based on which unit is used.
	
	if(!month || !day || !year || !unit)
		return B_ERROR;
	
	if(*month<1 || *month>12 || *day<1 || *day>31 || *year<0)
		return B_ERROR;
	
	BString unitstr(unit);
	
	if(unitstr.ICompare("year",4)==0)
	{
		// Year modification is the easiest of them all
		
		year+=unitcount;
		
		// Gotta account for leap year
		if(*month==2 && *day==29)
		{
			if(!(IsLeapYear(*year)))
				*day=28;
		}
		
		return B_OK;
	}
	else
	if(unitstr.ICompare("month",5)==0)
	{
		// Month modification is almost as easy as year modding is
		int32 count=(unitcount<0)?-unitcount:unitcount;
		int32 yearcount=0,monthcount=unitcount;
		
		if(count>11)
		{
			yearcount=int32(unitcount/12);
			monthcount=unitcount % 12;
			
			year+=yearcount;
		}
		
		*month+=monthcount;
		
		if(*month>12)
		{
			*month-=12;
			*year+=1;
		}
		else
		if(*month<1)
		{
			*month+=12;
			*year-=1;
		}

		// Gotta account for leap year
		if(*month==2 && *day==29)
		{
			if(!(IsLeapYear(*year)))
				*day=28;
		}
	}
	else
	if(unitstr.ICompare("week",4)==0)
	{
		// Changing weeks is as hard as day modification and uses the same algorithm --
		// weeks are changed into days and the day mod algorithm is performed.
		
		// Here's the algorithm:
		// 1) Convert the date to the day # in the year
		// 2) Convert the units to days and add them to the day of year
		// 3) Increment year and subtract days in year from day count until
		//		day count is less than the days in year
		// 4) Increment month and subtract days in month from day count until
		//		day count is less than the days in the current month
		// 5) Day is the remaining day count plus one (I think)
		
		int32 newyear=*year;
		int32 newmonth=1;
		int32 daycount=DayOfYear(*month,*day,*year)+ (abs(unitcount)*7);
		int32 daysinyear;
		int32 daysinmonth;

		if(unitcount>0)
		{
			daysinyear=IsLeapYear(newyear)?366:365;
			while(daycount>daysinyear)
			{
				daycount-=daysinyear;
				newyear++;
				daysinyear=IsLeapYear(newyear)?366:365;
			}
			*year=newyear;
			
			daysinmonth=DaysInMonth(newmonth,newyear);
			while(daycount<daysinmonth)
			{
				daycount-=daysinmonth;
				newmonth++;
				daysinmonth=DaysInMonth(newmonth,newyear);
			}
			*month=newmonth;
			
			*day=daycount+1;
		}
		else
		{
			daysinyear=IsLeapYear(*year-1)?366:365;
			
			// if were counting back a certain number of days without
			// changing the year
			if(daysinyear>(unitcount*-1))
			{
				daycount=unitcount+DayOfYear(*month,*day,*year);
				daysinmonth=DaysInMonth(newmonth,newyear);
				while(daycount>daysinmonth)
				{
					daycount-=daysinmonth;
					newmonth++;
					daysinmonth=DaysInMonth(newmonth,newyear);
				}
				*month=newmonth;
				
				*day=daycount;
			}
			else
			{
				// Counting back a number big enough to force going into a previous year
				
				// Count back to Jan 1 of the year
				daycount=abs(unitcount)-DayOfYear(*month,*day,*year);
				
				// count back until we have less than a year left
				while(daycount>daysinyear)
				{
					daycount-=daysinyear;
					newyear--;
					daysinyear=IsLeapYear(newyear-1)?366:365;
				}
				
				if(daycount>0)
					newyear--;
				
				*year=newyear;

				// count back by months until we have less than a month left
				newmonth=12;
				daysinmonth=DaysInMonth(newmonth,newyear);
				while(daycount>daysinmonth)
				{
					daycount-=daysinmonth;
					newmonth--;
					daysinmonth=DaysInMonth(newmonth-1,newyear);
				}
				*month=newmonth;
				
				// Count back from the 1st of the month to find the day
				if(daycount>0)
				{
					*month-=1;
					*day=DaysInMonth(*month,*year)-daycount-1;
				}
				else
					*day=1;
			}
		}
		
	}
	else
	if(unitstr.ICompare("day",3)==0)
	{
		// Here's the algorithm:
		// 1) Convert the date to the day # in the year
		// 2) Convert the units to days and add them to the day of year
		// 3) Increment year and subtract days in year from day count until
		//		day count is less than the days in year
		// 4) Increment month and subtract days in month from day count until
		//		day count is less than the days in the current month
		// 5) Day is the remaining day count plus one (I think)
		
		int32 newyear=*year;
		int32 newmonth=1;
		int32 daycount;
		int32 daysinyear;
		int32 daysinmonth;

		if(unitcount>0)
		{
			daycount=unitcount+DayOfYear(*month,*day,*year);
			daysinyear=IsLeapYear(newyear)?366:365;
			while(daycount>daysinyear)
			{
				daycount-=daysinyear;
				newyear++;
				daysinyear=IsLeapYear(newyear)?366:365;
			}
			*year=newyear;
			
			daysinmonth=DaysInMonth(newmonth,newyear);
			while(daycount>daysinmonth)
			{
				daycount-=daysinmonth;
				newmonth++;
				daysinmonth=DaysInMonth(newmonth,newyear);
			}
			*month=newmonth;
			
			*day=daycount;
		}
		else
		{
			daysinyear=IsLeapYear(*year-1)?366:365;
			
			// if were counting back a certain number of days without
			// changing the year
			if(daysinyear>(unitcount*-1))
			{
				daycount=unitcount+DayOfYear(*month,*day,*year);
				daysinmonth=DaysInMonth(newmonth,newyear);
				while(daycount>daysinmonth)
				{
					daycount-=daysinmonth;
					newmonth++;
					daysinmonth=DaysInMonth(newmonth,newyear);
				}
				*month=newmonth;
				
				*day=daycount;
			}
			else
			{
				// Counting back a number big enough to force going into a previous year
				
				// Count back to Jan 1 of the year
				daycount=abs(unitcount)-DayOfYear(*month,*day,*year);
				
				// count back until we have less than a year left
				while(daycount>daysinyear)
				{
					daycount-=daysinyear;
					newyear--;
					daysinyear=IsLeapYear(newyear-1)?366:365;
				}
				
				if(daycount>0)
					newyear--;
				
				*year=newyear;

				// count back by months until we have less than a month left
				newmonth=12;
				daysinmonth=DaysInMonth(newmonth,newyear);
				while(daycount>daysinmonth)
				{
					daycount-=daysinmonth;
					newmonth--;
					daysinmonth=DaysInMonth(newmonth-1,newyear);
				}
				*month=newmonth;
				
				// Count back from the 1st of the month to find the day
				if(daycount>0)
				{
					*month-=1;
					*day=DaysInMonth(*month,*year)-daycount-1;
				}
				else
					*day=1;
			}
		}
	}
	else
		return B_ERROR;
	
	return B_OK;
}

// Converts a month name to a number 1-12
int8 MonthToNumber(const char *string)
{
	if(!string)
		return -1;
	
	const char monthstrings[][12]={"january","february","march","april","may","june",
		"july","august","september","october","november","december"};

	BString bstring(string);
	
	for(int i=0;i<12;i++)
	{
		if(bstring.ICompare(monthstrings[i],3)==0)
			return i;
	}

	return -1;
}

// Converts a day name to a number 0-6
int8 DayToNumber(const char *string)
{
	if(!string)
		return -1;
	
	const char daystrings[][10]={"sunday","monday","tuesday","wednesday","thursday",
		"friday","saturday"};
	
	BString bstring(string);
	
	for(int i=0;i<7;i++)
	{
		if(bstring.ICompare(daystrings[i],3)==0)
			return i;
	}

	return -1;
}

// Calculates the day of the year for a given date
int16 DayOfYear(uint8 month, uint8 day, uint32 year)
{
	if(month>12 || month==0 || day>31 || day==0)
		return -1;
	
	int16 returnvalue=0;
	
	for(uint16 i=1; i<month; i++)
		returnvalue+=DaysInMonth(i,year);

	returnvalue+=day;
	
	return returnvalue;
}

// Um, I can't remember what this does.
int8 DaysInMonth(uint8 month, uint32 year)
{
	switch(month)
	{
		case 9:
		case 4:
		case 6:
		case 11:
			return 30;

		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return 31;
		
		case 2:
			return IsLeapYear(year)?29:28;
		
		default:
			return -1;
	}
}

bool IsLeapYear(uint32 year)
{
	return ( (year % 4)==0 && (year % 100)!=0 )? true:false;
}

// Verifies that the word given is the name of a day of the week
bool IsDay(const char *string)
{
	if(!string)
		return false;
	
	const char daystrings[][10]={"sunday","monday","tuesday","wednesday","thursday",
		"friday","saturday","now","yesterday","tomorrow","today"};
	
	BString bstring(string);
	
	for(int i=0;i<12;i++)
	{
		if(bstring.ICompare(daystrings[i],3)==0)
			return true;
	}

	return false;
}

// verifies that the word is a unit of time
bool IsUnit(const char *string)
{
	if(!string)
		return false;
	
	const char unitstrings[][12]={"day","week","month","year","hour","second","days",
		"weeks","months","years","hours","seconds"};

	BString bstring(string);
	
	for(int i=0;i<12;i++)
	{
		if(bstring.ICompare(unitstrings[i],4)==0)
			return true;
	}

	return false;
}

// verifies that the word is the name of a month
bool IsMonth(const char *string)
{
	if(!string)
		return false;
	
	const char monthstrings[][12]={"january","february","march","april","may","june",
		"july","august","september","october","november","december"};

	BString bstring(string);
	
	for(int i=0;i<12;i++)
	{
		if(bstring.ICompare(monthstrings[i],3)==0)
			return true;
	}

	return false;
}

// returns true if the word is one used in describing time in a relative sense, such
// as ago, before, hence, from, etc.
bool IsDescriptor(const char *string)
{
	if(!string)
		return false;
	
	const char descriptorstrings[][12]={"ago","from","after","before","this","next","hence","last"};

	BString bstring(string);
	
	for(int i=0;i<8;i++)
	{
		if(bstring.ICompare(descriptorstrings[i],4)==0)
			return true;
	}

	return false;
}
