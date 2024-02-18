/** \brief a base class that lets you call all items recursively to save/load configuration using ItemEntry
 *
 */

#ifndef LOADCFG_CALL_H
#define LOADCFG_CALL_H

#include "itemEntry.h"

using namespace fafik77;

class LoadCfg_call
{
 public:
	LoadCfg_call();
	LoadCfg_call(itemEntry* callItem_):callItem(callItem_){};
	virtual ~LoadCfg_call();

	///call each element
	///when LoadCfg or SaveCfg called, one has this Vector to call
	wxVector<LoadCfg_call*> subCalls;
	itemEntry* callItem= nullptr;

	virtual int LoadCfg( itemEntry &item )=0;
	virtual int SaveCfg( itemEntry &item )=0;

	virtual int LoadCfg( const bool& autoHandle ){
		if(callItem && autoHandle)
			return LoadCfg( &callItem );
		return 0;
	}
	virtual int SaveCfg( const bool& autoHandle ){
		if(callItem && autoHandle)
			return SaveCfg( &callItem );
		return 0;
	}

	virtual bool callSubs_LoadCfg( itemEntry &load_item ){
		for( size_t item_i=0; item_i< subCalls.size(); item_i++ ){
			subCalls.at(item_i)->LoadCfg( load_item );
		}
		return subCalls.size();
	}
	virtual bool callSubs_SaveCfg( itemEntry &load_item ){
		for( size_t item_i=0; item_i< subCalls.size(); item_i++ ){
			subCalls.at(item_i)->SaveCfg( load_item );
		}
		return subCalls.size();
	}

};

#endif // LOADCFG_CALL_H
