#ifndef GROUPLIST_H
#define GROUPLIST_H

#include "ListData.h"
#include "PeepsItem.h"

class GroupList
{
public:
	GroupList(void);
	~GroupList(void);
	
	GroupData *AddGroup(const char *name);
	void RemoveGroup(GroupData *data);
	bool HasGroup(const char *item);

	GroupData *FindGroup(const char *name);

	GroupItem *ItemForGroup(const char *name);
	GroupItem *ItemForGroup(GroupData *data);
};

#endif