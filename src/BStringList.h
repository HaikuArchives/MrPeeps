#ifndef	_BSTRINGLIST_H
#define	_BSTRINGLIST_H

#include <BeBuild.h>
#include <SupportDefs.h>
#include <List.h>
#include <String.h>

class BStringList
{
public:
	BStringList(int32 itemsPerBlock = 20);
	virtual	~BStringList();
	
	bool	AddItem(const char *string);
	bool	AddItem(const char *string, int32 atIndex);
	bool	AddList(BStringList *newItems);
	bool	AddList(BStringList *newItems, int32 atIndex);
	bool	RemoveItem(BString *item);
	BString *RemoveItem(int32 index);
	bool	RemoveItems(int32 index, int32 count);
	bool	ReplaceItem(int32 index, BString *newItem);
	void	MakeEmpty(bool freemem=false);

//	void	SortItems(void);
	bool	SwapItems(int32 indexA, int32 indexB) { return fList.SwapItems(indexA,indexB); }
	bool	MoveItem(int32 fromIndex, int32 toIndex)  { return fList.MoveItem(fromIndex,toIndex); }
	
	BString *FindItem(const char *string) const;
	BString	*ItemAt(int32 index) const { return (BString*)fList.ItemAtFast(index); }
	BString *ItemAtFast(int32 index) const { return (BString*)fList.ItemAtFast(index); }
	BString *FirstItem() const { return (BString*)fList.FirstItem(); }
	BString *LastItem() const { return (BString*)fList.LastItem(); }
	BString *Items() const { return (BString*)fList.Items(); }

	bool	HasItem(const char *string) const;
	bool	HasItem(BString *item) const { return fList.HasItem(item); }
	int32	IndexOf(BString *item) const { return fList.IndexOf(item); }
	int32	CountItems() const { return fList.CountItems(); }
	bool	IsEmpty() const { return fList.IsEmpty(); }
	
	void PrintToStream(void);
	
//	void	DoForEach(bool (*func)(void *));
//	void	DoForEach(bool (*func)(void *, void *), void *);

private:
	BList fList;
};

#endif
