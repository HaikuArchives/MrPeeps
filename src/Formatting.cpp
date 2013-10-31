#include <List.h>
#include <OS.h>
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include "Formatting.h"

#define USEC_IN_WEEK 60480000000ULL
#define USEC_IN_DAY 8640000000ULL
#define USEC_IN_HOUR 360000000UL
#define USEC_IN_MINUTE 60000000UL
#define USEC_IN_SECOND 1000000UL

// Separates a name into first and last. If only one name is given, it is assumed to 
// be the first name. If a person has a middle initial or middle name given, it is
// tacked onto the end of the first name. Recognition of the Mexican (and Spanish) names
// like 'de Jesus' or 'de Maria' are also attempted.

// Returns B_ERROR if unable to parse the name or there are no words in the string
// Returns B_BAD_VALUE if given a NULL source pointer
status_t ParseName(const char *source, BString &first, BString &last)
{
	if(!source)
		return B_BAD_VALUE;
	
	int32 wordcount=CountWords(source);
	
	if(wordcount<1)
		return B_ERROR;
	
	BString src(source);
	
	bool invertname=false;
	
	if(src.FindFirst(",")!=B_ERROR)
	{
		invertname=true;
		src.ReplaceFirst(',',' ');
	}
	
	BList namelist(0L);
	
	status_t status=TokenizeWords(source,&namelist);

	if(status!=B_OK)
		return status;

	first="";
	last="";
	
	BString *tempstr=NULL;

	switch(wordcount)
	{
		case 2:
		{
			uint8 firstindex,lastindex;
			if(invertname)
			{
				firstindex=1;
				lastindex=0;
			}
			else
			{
				firstindex=0;
				lastindex=1;
			}
			first=*((BString*)namelist.ItemAt(firstindex));
			last=*((BString*)namelist.ItemAt(lastindex));
			break;
		}
		case 1:
		{
			first=*((BString*)namelist.ItemAt(0));
			break;
		}
		default:
		{
			// check for something similar to 'de Jesus' or 'de Maria'
			tempstr=(BString*)namelist.ItemAt(wordcount-2);
			
			if(tempstr->ICompare("de")==0 || tempstr->ICompare("van")==0)
			{
				last=*tempstr;
				last+=" ";
				last+=*((BString*)namelist.ItemAt(wordcount-1));
			}
			else
				last=*((BString*)namelist.ItemAt(wordcount-1));
			
			first+=*((BString*)namelist.ItemAt(0));
			for(int32 i=1; i<wordcount-2; i++)
			{
				first+=" ";
				first+=*((BString*)namelist.ItemAt(i));
			}

			
			break;
		}
	}	
	
	for(int32 i=0; i<namelist.CountItems(); i++)
	{
		tempstr=(BString*)namelist.ItemAt(i);
		delete tempstr;
	}
	return B_OK;
}

status_t ParsePhoneNumber(const char *source, BString &out)
{
	// If we were handling international numbers, this could get nasty.
	// Luckily, because format recognition is locale-specific, we can get a little lazy. :)
	// Formats to handle:
	// (xxx)xxx-xxxx
	// xxx-xxx-xxxx
	// xxxxxxxxxx
	// xxx-xxxx
	// xxxxxxx
	// xxx.xxx.xxxx
	// xxx.xxxx
	
	// output format is (xxx) xxx-xxxx
	
	if(!source)
		return B_BAD_VALUE;
	
	// Automatically deem unrecognized if we have separators which
	// are not parentheses, dashes, dots, or spaces. If anything is left after removing
	// the allowed characters, we suddenly have an unrecognized format. This would
	// include international numbers which have a + at the beginning.
	BString temp(source);
	temp.RemoveSet("0123456789()-. QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm");
	if(temp.CountChars()!=0)
	{
		out=source;
		return B_OK;
	}
	
	BString src(source);

	// if there are letters in this phone number, convert them to numbers
	src.ReplaceSet("abcABC","2");
	src.ReplaceSet("defDEF","3");
	src.ReplaceSet("ghiGHI","4");
	src.ReplaceSet("jklJKL","5");
	src.ReplaceSet("mnoMNO","6");
	src.ReplaceSet("pqrsPQRS","7");
	src.ReplaceSet("tuvTUV","8");
	src.ReplaceSet("wxyzWXYZ","9");
	
	// remove all separators before we start counting words
	src.RemoveSet("()-. \t");
	
	int32 wordcount=CountWords(src.String());
	
	if(wordcount!=1)
		return B_ERROR;
	
	out="";
	
	// handle xxxxxxx or xxxxxxxxxx. Anything else is automatically unrecognized

	// check lengths
	int32 length=src.CountChars();
	if(length!=7 && length!=10)
		out=source;
	else
	{
		out=src;
		if(length==7)
			out.Insert("-",3);
		else
		{
			out.Insert("(",0);
			out.Insert(") ",4);
			out.Insert("-",9);
		}
	}
	return B_OK;
}

// Counts the number of words based on separation by tabs or spaces
// Returns the word count or B_ERROR on NULL strings
int32 CountWords(const char *source)
{
	if(!source)
		return B_ERROR;
	
	int32 length=strlen(source);
	int32 wordcount=0;
	
	char lastchar,c=source[0];
	
	if( c!=' ' && c!='\t' && length==1)
	{
		return 1;
	}
	
	for(int32 i=1; i<length; i++)
	{
		c=source[i];
		
		if(c!=' ' && c!='\t')
			continue;
		
		// We got this far, so it must be either a space or a tab
		
		// Check to see that if we have consecutive whitespaces we don't have extra
		// words
		lastchar=source[i-1];
		if(lastchar==' ' || lastchar=='\t')
			continue;

		wordcount++;
	}

	if(c!=' ' && c!='\t')
		wordcount++;
	
	return wordcount;
}

// Tokenizes a string by whitespace into a list of BStrings
// Returns B_ERROR if unable to parse the name or there are no words in the string
// Returns B_BAD_VALUE if given a NULL pointer or the list is not empty
status_t TokenizeWords(const char *source, BList *stringarray)
{
	if(!source || !stringarray || !stringarray->IsEmpty())
		return B_BAD_VALUE;
	
	// convert all tabs to spaces and eliminate consecutive spaces so that we can 
	// easily use strtok() 
	BString bstring(source);
	bstring.ReplaceAll('\t',' ');
	bstring.ReplaceAll("  "," ");

	char *workstr=new char[strlen(source)+1];
	strcpy(workstr,bstring.String());
	strtok(workstr," ");
	
	char *token=strtok(NULL," "),*lasttoken=workstr;
	
	if(!token)
	{
		delete workstr;
		stringarray->AddItem(new BString(bstring));
		return B_OK;
	}
	
	int32 length;
	BString *newword;
	
	while(token)
	{
		length=token-lasttoken;
		
		newword=new BString(lasttoken,length+1);
		lasttoken=token;
		stringarray->AddItem(newword);

		token=strtok(NULL," ");
	}
	
	length=strlen(lasttoken);
	newword=new BString(lasttoken,length+1);
	lasttoken=token;
	stringarray->AddItem(newword);
	
	
	delete [] workstr;

	return B_OK;
}

// Takes a source string and returns a number string ready to go through atoi()
// Note that only integers are supported. Decimal points are treated as separators
// as used in non-US countries.
// returns B_ERROR if unable to parse the source
// returns B_BAD_VALUE on NULL source pointers
status_t ParseNumber(const char *source, BString &out)
{
	if(!source)
		return B_BAD_VALUE;
	
	BString src(source);
	BString *tempstr;
	uint32 returnvalue=0;
	
	if(IsDigitsOnly(src.String()))
	{
		out=src;
		return B_OK;
	}
	
	// remove any separators and make them all the same. We can afford to strip out
	// these because we're dealing with converting words into numbers.
	src.ReplaceAll(',',' ');
	src.ReplaceAll('.',' ');
	src.ReplaceAll('/',' ');
	src.ReplaceAll('-',' ');
	src.ReplaceAll('(',' ');
	src.ReplaceAll(')',' ');
	
	// We only go up to 999,999,999 so this shouldn't be *too* bad...
	BList wordlist(0L);
	TokenizeWords(src.String(),&wordlist);
	int32 wordcount=wordlist.CountItems();
	
	// useful things to know
	
	// million will always be the 2nd or 3rd word 
	// thousand is always no more than 5 words following million (if it exists)
	// numbers less than 1000 take no more than 4 words
	// thousand and million always follow a number less than 1000 -- act like multipliers
	// no more than 2 words can precede hundred
	// a number less than 10 can be preceded only by a 'tens' number > 10 (20,30,40)
	// likewise, 'tens' > 20 can only be followed by a 'ones' number, thousand, or million
	// The numbers>10 and <20 can only be followed by hundred, thousand, or million
	// 10 can only be followed by thousand or million
	
	// Tackling the whole number:
	// 1) check for 'million' - parse millions if exists
	// 2) check for 'thousand' - parse past million (if it exists) and up to 'thousand' if exists
	// 3) parse everything else, if exists
	
	// Parsing each group
	// check word count before million
	// 4:	should contain 'hundred' because it automatically should be greater than 100
	// 			and contain a 'double word' number like twenty five
	// 3:	also should contain 'hundred' and a single-word number (1-20)
	// 2:	first word should be a 'tens' number > 10 followed by 1-9
	//		alternately, if the section contains 'hundred', then the first word should be 1-9
	// 1:	word must NOT be hundred
	
	if(wordcount>0)
	{		
		bool parsemillion=false,parsethousand=false,parseelse=false;
		
		int32 offset=src.IFindFirst("million");
		if(offset!=B_ERROR)
		{
			parsemillion=true;
			offset=src.IFindFirst("thousand",offset);
			if(offset!=B_ERROR)
			{
				parsethousand=true;
				
				// TODO: check this to make sure it's the right value to add to the offset
				if(src.CountChars()>offset+7)
					parseelse=true;
			}
			else
			{
				// TODO: check this to make sure it's the right value to add to the offset
				if(src.CountChars()>offset+6)
					parseelse=true;
			}
		}
		else
		{
			offset=src.IFindFirst("thousand");
			if(offset!=B_ERROR)
			{
				parsethousand=true;
				
				// TODO: check this to make sure it's the right value to add to the offset
				if(src.CountChars()>offset+7)
					parseelse=true;
			}
			else
			{
				parseelse=true;
			}
		}
		
		// Now that we know what to parse, let's do it!
		BString millions(""),thousands(""),hundreds(""),workstr;
		int32 workvalue, workvalue2;
		int32 millwordcount=0,thouwordcount=0;
	
		if(parsemillion)
		{
			workstr=src;
			
			// TODO: check to make sure that this is the right # of chars to remove
			workstr.Truncate(workstr.IFindFirst("million"));

			int32 millcount=CountWords(workstr.String());
			millwordcount=millcount;
			switch(millcount)
			{
				case 4:
				{
					// Case: single-word hundred double-word million
					workstr=*((BString*)wordlist.ItemAt(1));
					if(workstr.ICompare("hundred")!=0)
						return B_ERROR;
					returnvalue=100;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(0))->String());
					if(workvalue==-1)
						return B_ERROR;
					returnvalue*=workvalue;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(2))->String());
					if(workvalue==-1)
						return B_ERROR;
					returnvalue+=workvalue;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(3))->String());
					if(workvalue==-1)
						return B_ERROR;
					returnvalue+=workvalue;
					returnvalue*=1000000;
					break;
				}
				case 3:
				{
					// Case: single-word hundred single-word million
					workstr=*((BString*)wordlist.ItemAt(1));
					if(workstr.ICompare("hundred")!=0)
						return B_ERROR;
					returnvalue=100;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(0))->String());
					if(workvalue==-1)
						return B_ERROR;
					returnvalue*=workvalue;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(2))->String());
					if(workvalue==-1)
						return B_ERROR;
					returnvalue+=workvalue;
					returnvalue*=1000000;
					break;
				}
				case 2:
				{
					// Cases:
					// single-word hundred million
					// double-word million
					workstr=*((BString*)wordlist.ItemAt(1));
					if(workstr.ICompare("hundred")!=0)
					{
						workvalue=WordToNumber(((BString*)wordlist.ItemAt(0))->String());
						if(workvalue==-1)
							return B_ERROR;
						returnvalue=workvalue*100000000;
					}
					else
					{
						workvalue=WordToNumber(((BString*)wordlist.ItemAt(0))->String());
						if(workvalue==-1)
							return B_ERROR;
						returnvalue=workvalue*1000000;
					}
					break;
				}
				case 1:
				{
					// Cases:
					// single-word million
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(0))->String());
					if(workvalue==-1)
						return B_ERROR;
					returnvalue=workvalue*1000000;
					break;
				}
				default:
				{
					// uh-oh.....
					return B_ERROR;
					break;
				}
			}
		} // end if parsemillion
		
		if(parsethousand)
		{
			workstr=src;
			
			// TODO: check to make sure that this is the right # of chars to remove
			workstr.Truncate(workstr.IFindFirst("thousand"));
			if(parsemillion)
				workstr.Remove(0,workstr.IFindFirst("million")+8);
			
			int32 thoucount=CountWords(workstr.String());
			thouwordcount=thoucount;
			
			switch(thoucount)
			{
				case 4:
				{
					// Case: single-word hundred double-word thousand
					workstr=*((BString*)wordlist.ItemAt(1+millwordcount+1));
					if(workstr.ICompare("hundred")!=0)
						return B_ERROR;
					workvalue2=100;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(0+millwordcount+1))->String());
					if(workvalue==-1)
						return B_ERROR;
					workvalue2*=workvalue;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(2+millwordcount+1))->String());
					if(workvalue==-1)
						return B_ERROR;
					workvalue2+=workvalue;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(3+millwordcount+1))->String());
					if(workvalue==-1)
						return B_ERROR;
					workvalue2+=workvalue;
					workvalue2*=1000;
					returnvalue+=workvalue2;
					break;
				}
				case 3:
				{
					// Case: single-word hundred single-word thousand
					workstr=*((BString*)wordlist.ItemAt(1+millwordcount+1));
					if(workstr.ICompare("hundred")!=0)
						return B_ERROR;
					workvalue2=100;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(0+millwordcount+1))->String());
					if(workvalue==-1)
						return B_ERROR;
					workvalue2*=workvalue;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(2+millwordcount+1))->String());
					if(workvalue==-1)
						return B_ERROR;
					workvalue2+=workvalue;
					workvalue2*=1000;
					returnvalue+=workvalue2;
					break;
				}
				case 2:
				{
					// Cases:
					// single-word hundred thousand
					// double-word thousand
					workstr=*((BString*)wordlist.ItemAt(1));
					if(workstr.ICompare("hundred")==0)
					{
						workvalue=WordToNumber(((BString*)wordlist.ItemAt(0+millwordcount+1))->String());
						if(workvalue==-1)
							return B_ERROR;
						returnvalue+=workvalue*100000;
					}
					else
					{
						workvalue=WordToNumber(((BString*)wordlist.ItemAt(0+millwordcount+1))->String());
						if(workvalue==-1)
							return B_ERROR;
						returnvalue+=workvalue*1000;
					}
					break;
				}
				case 1:
				{
					// Case: single-word thousand
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(0+millwordcount+1))->String());
					if(workvalue==-1)
						return B_ERROR;
					returnvalue+=workvalue*1000;
					break;
				}
				default:
				{
					// uh-oh.....
					return B_ERROR;
					break;
				}
			}
		} // end if parsethousand

		if(parseelse)
		{
			workstr=src;
			
			// TODO: check to make sure that this is the right # of chars to remove
			if(parsemillion)
				workstr.Remove(0,workstr.IFindFirst("million")+8);
			if(parsethousand)
				workstr.Remove(0,workstr.IFindFirst("thousand")+9);
			
			int32 millcount=CountWords(workstr.String());

			switch(millcount)
			{
				case 4:
				{
					// Case: single-word hundred double-word
					workstr=*((BString*)wordlist.ItemAt(1+millwordcount+thouwordcount+2));
					if(workstr.ICompare("hundred")!=0)
						return B_ERROR;
					workvalue2=100;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(0+millwordcount+thouwordcount+2))->String());
					if(workvalue==-1)
						return B_ERROR;
					workvalue2*=workvalue;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(2+millwordcount+thouwordcount+2))->String());
					if(workvalue==-1)
						return B_ERROR;
					workvalue2+=workvalue;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(3+millwordcount+thouwordcount+2))->String());
					if(workvalue==-1)
						return B_ERROR;
					workvalue2+=workvalue;
					returnvalue+=workvalue2;
					break;
				}
				case 3:
				{
					// Case: single-word hundred single-word
					workstr=*((BString*)wordlist.ItemAt(1+millwordcount+thouwordcount+2));
					if(workstr.ICompare("hundred")!=0)
						return B_ERROR;
					workvalue2=100;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(0+millwordcount+thouwordcount+2))->String());
					if(workvalue==-1)
						return B_ERROR;
					workvalue2*=workvalue;
					
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(2+millwordcount+thouwordcount+2))->String());
					if(workvalue==-1)
						return B_ERROR;
					workvalue2+=workvalue;
					returnvalue+=workvalue2;
					break;
				}
				case 2:
				{
					// Cases:
					// single-word hundred
					// double-word
					workstr=*((BString*)wordlist.ItemAt(1));
					if(workstr.ICompare("hundred")==0)
					{
						workvalue=WordToNumber(((BString*)wordlist.ItemAt(0+millwordcount+thouwordcount+2))->String());
						if(workvalue==-1)
							return B_ERROR;
						returnvalue+=workvalue*100;
					}
					else
					{
						workvalue=WordToNumber(((BString*)wordlist.ItemAt(0+millwordcount+thouwordcount+2))->String());
						if(workvalue==-1)
							return B_ERROR;
						returnvalue+=workvalue;
					}
					break;
				}
				case 1:
				{
					// Case: single-word
					workvalue=WordToNumber(((BString*)wordlist.ItemAt(0+millwordcount+thouwordcount+2))->String());
					if(workvalue==-1)
						return B_ERROR;
					returnvalue+=workvalue;
					break;
				}
				default:
				{
					// uh-oh.....
					return B_ERROR;
					break;
				}
			}
		} // end if parseelse
	} // end if wordcount>0

	for(int32 i=0; i<wordlist.CountItems(); i++)
	{
		tempstr=(BString*)wordlist.ItemAt(i);
		delete tempstr;
	}
	char string[50];
	sprintf(string,"%ld",returnvalue);
	out=string;
	
	return B_OK;
}

int32 WordToNumber(const char *source)
{
	const char numberstrings[][10]={"a","the","one","two","three","four","five","six","seven",
	"eight","nine","ten","eleven","twelve","thirteen","fourteen","fifteen","sixteen","seventeen",
	"eighteen","nineteen","twenty","thirty","forty","fifty","sixty","seventy","eighty",
	"ninety","hundred","thousand","million","first","second","third","fourth","fifth","sizth",
	"seventh","eighth","nineth","tenth","twelfth","fifteenth"};
	
	const int32 values[]={1,1,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
		30,40,50,60,70,80,90,100,1000,1000000,1,2,3,4,5,6,7,8,9,10,12,15};
	
	BString bstring(source);
	bstring.RemoveSet("1234567890");
	if(bstring.CountChars()==0)
	{
		bstring.SetTo(source);
		int32 number=atoi(bstring.String());
		return number;
	}
	bstring.SetTo(source);
	
	for(int32 i=0; i<43; i++)
	{
		if(bstring.ICompare(numberstrings[i],6)==0)
			return values[i];
	}
	
	// untested change from stuff like "21st" to 21
	if( (bstring.FindLast("th")==bstring.CountChars()-2) || (bstring.FindLast("th")==bstring.CountChars()-2) )
	{
		bstring.RemoveSet("ABCDEFGHIJKLMNOPQRSTUVMXYZabcdefghijklmnopqrstuvwxyz");
		if(bstring.CountChars()>0)
		{
			return atoi(bstring.String());
		}
	}
	
	return -1;
}

// Tokenizes a string by commas into a list of BStrings and removes only the whitespace which is not needed.
// Returns B_ERROR if unable to parse the name or there are no words in the string
// Returns B_BAD_VALUE if given a NULL pointer or the list is not empty
status_t TokenizeGroupString(const char *source, BStringList *stringlist)
{
	if(!source || !stringlist || !stringlist->IsEmpty())
		return B_BAD_VALUE;
	
	BString src(source);
	
	// Remove double commas
	while(src.FindFirst(",,")!=B_ERROR && src.CountChars()>0)
		src.ReplaceAll(",,",",");
	
	// Remove tabs
	while(src.FindFirst("\t")!=B_ERROR && src.CountChars()>0)
		src.ReplaceAll("\t"," ");

	// Remove double spaces
	while(src.FindFirst("  ")!=B_ERROR && src.CountChars()>0)
		src.ReplaceAll("  "," ");

	// Remove spaces following any commas
	while(src.FindFirst(", ")!=B_ERROR && src.CountChars()>0)
		src.ReplaceAll(", ",",");
	
	// Remove spaces preceding any commas
	while(src.FindFirst(" ,")!=B_ERROR && src.CountChars()>0)
		src.ReplaceAll(" ,",",");
	
	// Remove leading whitespace
	while(src.FindFirst(" ")==0 && src.CountChars()>0)
		src.RemoveFirst(" ");
	
	// Remove trailing commas
	while(src.FindLast(",")==src.CountChars()-1 && src.CountChars()>0)
		src.RemoveLast(",");
	
	if(src.CountChars()==0)
		return B_ERROR;
	
	char *workstr=new char[src.CountChars()+1];
	strcpy(workstr,src.String());
	
	strtok(workstr,",");
	
	char *token=strtok(NULL,","),*lasttoken=workstr;
	
	if(!token)
	{
		delete workstr;
		stringlist->AddItem(src.String());
		return B_OK;
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
		stringlist->AddItem(lasttoken);
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

	stringlist->AddItem(lasttoken);
	
	delete workstr;
	
	return B_OK;
}

bool IsDigitsOnly(const char *string)
{
	if(!string)
		return false;
	
	BString bstring(string);
	
	bstring.RemoveSet("1234567890");
	return (bstring.CountChars()==0)?true:false;
}

bool IsNumber(const char *string)
{
	
	if(!string)
		return false;
	
	BString bstring(string);
	
	// if we have nothing but numerics, it definitely is a number and by removing
	// all the number characters, we should (theoretically) have an empty string.
	bstring.RemoveSet("1234567890");
	if(bstring.CountChars()==0)
		return true;
	
	bstring=string;
	
	const char numberstrings[][10]={"a","the","one","two","three","four","five","six","seven",
	"eight","nine","ten","eleven","twelve","thirteen","fourteen","fifteen","sixteen","seventeen",
	"eighteen","nineteen","twenty","thirty","forty","fifty","sixty","seventy","eighty",
	"ninety","hundred","thousand","million","first","second","third","fourth","fifth","sizth",
	"seventh","eighth","nineth","tenth","twelfth","fifteenth"};
	
	for(int i=0;i<43;i++)
	{
		if(bstring.ICompare(numberstrings[i],6)==0)
			return true;
	}

	// untested change from stuff like "21st" to 21
	if( (bstring.FindLast("th")==bstring.CountChars()-2) || (bstring.FindLast("th")==bstring.CountChars()-2) )
	{
		bstring.RemoveSet("ABCDEFGHIJKLMNOPQRSTUVMXYZabcdefghijklmnopqrstuvwxyz");
		if(bstring.CountChars()>0)
		{
			return true;
		}
	}

	return false;
}

