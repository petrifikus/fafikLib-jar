#ifndef LOADCFG_CALL_H
#define LOADCFG_CALL_H

#include "ItemEntryIncl.h"

class LoadCfg_call
{
 public:
	LoadCfg_call();
	LoadCfg_call(fafikLib_DB::itemEntry* callItem_):callItem(callItem_){};
	virtual ~LoadCfg_call();

	///call each element
	///when LoadCfg or SaveCfg called, one has this Vector to call
	wxVector<LoadCfg_call*> subCalls;
	fafikLib_DB::itemEntry* callItem= nullptr;

	virtual int LoadCfg( fafikLib_DB::itemEntry &item )=0;
	virtual int SaveCfg( fafikLib_DB::itemEntry &item )=0;

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

	virtual bool callSubs_LoadCfg( fafikLib_DB::itemEntry &load_item ){
		for( size_t item_i=0; item_i< subCalls.size(); item_i++ ){
			subCalls.at(item_i)->LoadCfg( load_item );
		}
		return subCalls.size();
	}
	virtual bool callSubs_SaveCfg( fafikLib_DB::itemEntry &load_item ){
		for( size_t item_i=0; item_i< subCalls.size(); item_i++ ){
			subCalls.at(item_i)->SaveCfg( load_item );
		}
		return subCalls.size();
	}

};

#endif // LOADCFG_CALL_H
