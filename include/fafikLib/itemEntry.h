/*	fafik77 grants rights to use this(itemEntry) source code to anyone, as long as link to itemEntry(with source download link) is supplied when used
 *
 *	itemEntry allows you to store a data tree, where Objects are auto binary sorted, while Arrays are NOT sorted
 created on:	2018.06
 last modify:	2024.02.12
 see "fafikLib_DB explanation" for details & info of usage
 */

#ifndef FAFIK77_ITEMENTRY_H
#define FAFIK77_ITEMENTRY_H


#include <algorithm>    // std::sort
#include <memory>		// smart pointer
#include "Byte_LL.h"
#include "UniFileStream.h"
#include "f77_helpers.h"


namespace fafik77{
 //predef
struct itemEntry;
struct itemEntry_Bytes;
 //to be-friend
class fafikLib_DB;
 //def
//		///universal call to what item will store
//	typedef BYTE itemStores;	//2019-10-09, bool8 also works


enum itemEntryStores {
	 ///also unwritable
	ie_none=0,
	 ///string value
	ie_string=1,
	ie_value=1,
	 ///when commented
	ie_lore=2,
	 ///Table
	ie_sub_itemTable=4,
	ie_Object=4,
	ie_Table=4,
	ie_TableNotClosed= 16,

	 ///requests further info from fafikLib_ItemEntry_Stores
	 ///unused 2018-09-21, see ExtDataLink
	ie_link= 64,
	 ///no name, exists under "auto_key"
	ie_auto_key_keyed= 32,
	ie_auto_key= 128,
	ie_Array= ie_Table | ie_auto_key,
	ie_Array_value= ie_value | ie_auto_key_keyed,
	ie_Array_string= ie_Array_value,

	ie_error= -1,
};
enum itemEntry_type_tags {
	ie_unspecified= 0,
	ie_caseInsensitive= 1,
	ie_notAString= 2,
};
enum class itemEntry_storeDerived {
	 ///stores nothing more
	sdie_normal,
	sdie_bytes,
	sdie_number_LL,
	sdie_number_D,
};


 ///@warning always use origin reference/pointer, never copy itemEntry
struct itemEntry
{
 //def
	typedef wxVector<itemEntry*> vectorItemType;
	typedef vectorItemType::iterator iterator;
	typedef vectorItemType::const_iterator const_iterator;
  //{storage
	//only sub_itemTable =0	 or	 only *ByteData_itemEntry_selfEntry =0
		///what value stores..
		///@see itemEntryStores
	BYTE type_stores= ie_none;
		///@see itemEntry_type_tags
	itemEntry_type_tags type_tags= itemEntry_type_tags::ie_unspecified;
		///auto added parent. child has to have a parent otherwise its an independent parent
	itemEntry* parent= nullptr;
		///auto incremented depth(child of n level)
	unsigned short depth= 0;
		///if auto_key then key = Position__;
	wxString key;
	wxString value;
   protected:
	friend class fafikLib_DB;
	 ///vector that stores all items of this table
	vectorItemType sub_itemTable;
	 ///stores the type of derived class @see itemEntry::itemEntry_storeDerived
	itemEntry_storeDerived _stores_derived= itemEntry_storeDerived::sdie_normal;
 public:
  //}storage end
	inline bool getAs_derived(itemEntry_Bytes** asDerived) const
	{*asDerived= nullptr; if(itemEntry_storeDerived::sdie_bytes== _stores_derived)	return( *asDerived= (itemEntry_Bytes*)this );}

	virtual ~itemEntry();
	itemEntry(){}
	itemEntry(
		const wxString& key_,
		const wxString& value_,
		itemEntryStores type_stores_,
		unsigned short depth_= 0,
		itemEntry *parent_= nullptr
	) : type_stores(type_stores_), parent(parent_), depth(depth_), key(key_), value(value_) {}
	itemEntry(
		const wxString& key_,
		itemEntryStores type_stores_,
		itemEntry_type_tags type_tags_
	) : type_stores(type_stores_), type_tags(type_tags_), key(key_) {}
	 ///useful for Add(item)
	itemEntry(
		const wxString& val_
	) : value(val_) {}


		///@param key = search for key
		///@param type_ = type to set when Add()
		///@param *out_found = bool to place true if success
		///@return already existing item [], or Add() and return new.. *out_found = if exists
	iterator getOrAddIt( const wxString& key_, itemEntryStores type_, bool caseInsensitive=true, bool *out_found=nullptr );
		///legacy call
	itemEntry* getOrAdd( const wxString& key_, itemEntryStores type_, bool *out_found= nullptr, bool caseInsensitive=true ) { return *( getOrAddIt(key_, type_, caseInsensitive, out_found) ); }

	iterator insert(iterator it, itemEntry* item);
		///returns iterator or end()
	iterator find( const wxString &item_byKey, itemEntryStores type, bool caseInsensitive=true );
//			///Legacy(2019-10-09 should be removed) @param outFoundPos -saves index of found item here
//		itemEntry* find( const wxString &item_byKey, bool caseInsensitive= false, size_t* outFoundPos=nullptr ){
//			iterator rets= find( item_byKey, ie_Table, caseInsensitive );
//			if( rets==end() ){
//				rets= find( item_byKey, ie_value, caseInsensitive );
//			}
//			if( rets==end() ){return nullptr;}
//			if(outFoundPos) (*outFoundPos)= begin()- rets;
//			return *rets;
//		}
		///New (2019-10-09) this one should exist
	itemEntry* findP( const wxString &item_byKey, itemEntryStores type=ie_value, bool caseInsensitive= true ){
		iterator its= find( item_byKey, type, caseInsensitive );
		if(its!= end()) return *its;
		return nullptr;
	}
		//specially here for ExcludeFiles, will remove item from Table and remove its parent (dont forget to delete it later)
	//itemEntry* getAndDelete( const wxString &item_byKey, itemStores type, bool caseInsensitive=true );
	itemEntry* getLore(const size_t& Indx=0){return at(0, false, false);}
	//itemEntry* getTable(const size_t& Indx=0);
		///Used only for interpreter
	itemEntry* find_linear(const wxString &item_byKey, bool caseInsensitive= true);
		///Used only for interpreter/Array
	itemEntry* Add_back(const wxString& key_, const wxString& value_=wxEmptyString, itemEntryStores type_stores_= ie_none);

	inline iterator end(){ return sub_itemTable.end(); }
	inline iterator endn(){ return sub_itemTable.end(); }
	inline const_iterator end()const{ return sub_itemTable.end(); }
	inline iterator begin(){ return sub_itemTable.begin(); }
	inline iterator beginn(){ return sub_itemTable.begin(); }
	inline const_iterator begin()const{ return sub_itemTable.begin(); }
 public:
		///changes iterator to pos
	inline size_t getIndex( iterator it )const{
		if(it== end()) return sizeTable();
		return it- begin();
	}




		///ToDo redo 2019-10-10
		///LUA like get
		///!!! for sure valid: check if(returned != input) or reset_invalidSearch()==0.. insecure -use GetItem_byKey() instead
	itemEntry& operator[](const wxString &item_byKeyName);
		///ToDo redo 2019-10-10
		///LUA like get
		///same as at( n, true, true )
	itemEntry& operator[](const size_t item_byNumber){
		return *at(item_byNumber, true);
	}
//			///@param outFoundPos -saves index of found item here
//		itemEntry* find( const wxString &item_byKeyName, bool caseInsensitive= false, size_t* outFoundPos=nullptr );
//			///alias for find(str, caseSense)
//		inline itemEntry* at( const wxString &item_byKeyName, bool caseInsensitive= false, size_t* outFoundPos=nullptr ){
//			return this->find(item_byKeyName, caseInsensitive, outFoundPos);
//		}
	itemEntry* find_byValue( const wxString &item_byValue, bool caseInsensitive= false, size_t* outFoundPos=nullptr );
		///detaches sub_itemTable so any inherent of this does not violate memory after this is deleted
	inline void Detach(){sub_itemTable.clear();}	//disables auto destructor for those elements
	 ///runs destructor for sub_itemTable, freeing used RAM, and removes all items from the list
	void FreeTable();
	 ///alias FreeTable() @see FreeTable
	inline void Free(){return FreeTable();}
	 ///alias FreeTable() @see FreeTable
	inline void clear(){return FreeTable();}

//		itemEntry* operator->();

//		itemEntry* get();
//		const itemEntry* get() const;
//		inline itemEntry* get_ptr() { return get(); }
//		inline itemEntry &get_ref() { return *get(); }
	 ///removes items [from, to) = first inclusive, last exclusive
	bool removeItems(iterator it1, iterator it2, bool silent=false);
	bool removeItem(iterator it, bool silent=false);
		///@return true if found removed
		///@param silent 0 -remove from table, 1 -nullify
	bool removeItem(const wxString& by_string, bool silent=false);
		///@return true if found removed
		///@param silent 0 -remove from table, 1 -nullify
	bool removeItem(size_t by_pos, bool silent=false);
		///@return true if found removed
		///@param silent 0 -remove from table, 1 -nullify
	bool removeItem(const itemEntry* by_id, bool silent=false);
		///auto calls parent->removeItem(this)
		///@param NoParentRemove: destroy this item even if it has no parent
	bool removeSelf(bool NoParentRemove=false){
		if(parent) return parent->removeItem(this);
		if(NoParentRemove){ delete this; return 1;}
		return 0;
	}
	bool pop_back() {return removeItem(size()-1);}
//		size_t pop_back(size_t amount=1);
	inline bool removeItems(size_t pos_from, size_t amount=1, bool silent=false);
	inline bool removeItems(iterator it1, size_t amount=1, bool silent=false);
//		bool removeItems(const itemEntry* by_id, size_t amount=1, bool silent=false);

		///@return first item that contains "by_type", then tries in parent, eventually parent=nullptr
	itemEntry *getByType( itemEntryStores by_type, size_t from_pos_= 0 );

	 ///
	 ///@return nullptr on not found
	itemEntry* at(const size_t item_byNumber, const bool auto_key_keyedSearch=false, const bool resolveLink=true);
		///@return index of self in parent Table
	size_t getPosition() const;
	inline bool isTable() const{ return (type_stores & ie_Table); }
	inline bool isObject() const{ return (type_stores & ie_Object); }
	inline bool isAuto_key() const{ return (type_stores & ie_auto_key); }
	 ///same as isAuto_key()
	inline bool isArray() const{ return (type_stores & ie_auto_key); }
	inline bool isAuto_key_keyed() const{ return (type_stores & ie_auto_key_keyed); }
	inline bool isValue() const{ return (type_stores & ie_value); }
	inline bool isLore() const{ return (type_stores & ie_lore); }
	 ///that is quite the invalid type
	inline bool isBothArrTable() const{ return (type_stores & (ie_Table|ie_auto_key) ); }
		///unused 2018-09-18, redo
	inline bool isLink() const{ return (type_stores & ie_link); }
	inline bool isCaseInsensitive() const{ if(type_tags & itemEntry_type_tags::ie_caseInsensitive) return true; return false; }

	inline bool isWithBytes() const{ return(itemEntry_storeDerived::sdie_bytes== _stores_derived); }


	inline size_t sizeTable() const{ return sub_itemTable.size(); }
	inline size_t size() const{ return sub_itemTable.size(); }
		///@param mask -determines what should be left (0 -null, 32 -findByAutoKey)
		///@return old type_stores
		///warning effect has no save point, although can be overwritten
	inline itemEntryStores setUnwritable( itemEntryStores mask= (itemEntryStores)(ie_auto_key_keyed|ie_auto_key) ){
		itemEntryStores ret= itemEntryStores(this->type_stores);
//		(itemEntryStores)this->type_stores&= itemEntryStores(mask);
		this->type_stores= itemEntryStores(this->type_stores & mask);
		return ret;
	}

//		bool push_back(itemEntry &push);
		///transfer SubItems Ownership to new item
	void Transfer_Ownership_subItemsTo( itemEntry& transferOwnershipTo );
		///transfer SubItems Ownership to self
	void Transfer_Ownership_subItemsToMe( itemEntry& transferOwnershipFrom );
		///sets SubItems' parent to newParent
	void setSubsParent( itemEntry* newParent );
		///sets SubItems' parent to self
	inline void setSubsParent(){ return setSubsParent(this); }
//			///@return pointer to added item
//			///appends pointer to object, 2018-08-13 allowed pushing to non Table(& sets Table)
//		itemEntry* push_back_ptr(itemEntry *push);
		///copies content recursively, reinterpreting eg: (parent, depth), omitting pointers
	bool push_back_cpyTotal(itemEntry &push);
//			///@return pointer to added item
//			///creates a shallow copy of item and appends
//			///@see push_back_ptr
//		inline itemEntry* push_back_cpy(itemEntry &push) {
//			itemEntry* temp=new itemEntry(push); return push_back_ptr(temp);}
//			///@return pointer to added item
//			///creates a copy of item and appends
//			///@see push_back_cpy
//		inline itemEntry* push_back(itemEntry &push){ return push_back_cpy(push);}
//			///@return pointer to added item
//			///@brief appends pointer to object
//			///@see push_back_ptr
//		inline itemEntry* push_back(itemEntry *push){ return push_back_ptr(push);}

	iterator AddIt( const wxString& key_, const wxString& value_=wxEmptyString, itemEntryStores type_stores_= ie_none );
	 ///adds *item into table or array depending on the type
	iterator AddItSmart( itemEntry* item );
		///adds new subItem with ( ... )
	itemEntry* Add( const wxString& key_, const wxString& value_=wxEmptyString, itemEntryStores type_stores_= ie_none );
		///@return true if ok, false if not "ie_auto_key"
		///adds new subItem with ( ... ), only when ie_auto_key
	itemEntry* Add( itemEntryStores type_stores_, const wxString& value_ );
	 ///adds IE *item into parent
	 ///@return *item if success, nullptr if wrong type
	itemEntry* Add( itemEntry* item );
		///add itemToLink to this.item table (you have to Detach it instead of Freeing it)
	itemEntry* AddLink( itemEntry* itemToLink );
	 ///moves all elements from takeFromItem into this
	 ///only works on Arrays
	void MergeArrays(itemEntry* takeFromItem);


	 ///get tree children to this getFrom itemEntry
	static wxStringVector getParentTreeArr(const itemEntry* getFrom);
	 ///pre-made call for getParentTree_asArray()+separator
	static wxString getParentTree(const itemEntry* getFrom, const wxString separator=".");


		///2019-10-09
	static bool _operLowerB_compareIE( const_iterator item, const itemEntry& compa );
		///2019-10-10
	static bool _operLowerB_compareIE_1( const itemEntry* item, const itemEntry* compaa );
	bool _sort( bool sortAllChildren=true );
 protected:
	static bool8 _operLowerB_compareStr( const wxString* str_a, const wxString* str_b, bool caseInsensitive=true );
	 ///gets a pointer to the item in Table
	iterator binaryGetPosOf(const itemEntry& TemplateItem);
	 ///check if given item really exists (3 checks)
	bool binaryGetPosOf_exists(const_iterator prevRetValue, const itemEntry& TemplateItem)const;
// private:
//	bool invalidSearch= false;
};

struct itemEntry_Bytes: public itemEntry
{
 public:
	itemEntry_Bytes() {_stores_derived= itemEntry_storeDerived::sdie_bytes; }
	itemEntry_Bytes(UniFileStream_base* IO, const wxFileOffset& start_p, const wxFileOffset& end_p)
		{_stores_derived= itemEntry_storeDerived::sdie_bytes; streamIO=IO; pos_curr=0; pos_start= start_p; pos_end= end_p; }
	 ///empty dtor as we dont allocate new
	virtual ~itemEntry_Bytes() {};

 //easy functions
	void Open(UniFileStream_base* IO)
		{streamIO=IO;}
	void Open(UniFileStream_base* IO, const wxFileOffset& start_p, const wxFileOffset& end_p)
		{streamIO=IO; pos_curr=0; pos_start= start_p; pos_end= end_p; }
	void OpenSized(UniFileStream_base* IO, const wxFileOffset& start_p, const wxFileOffset& size)
		{streamIO=IO; pos_curr=0; pos_start= start_p; pos_end= start_p+ size; }

	wxFileOffset Length()const {return pos_end- pos_start;}
	wxFileOffset Tell()const {return pos_curr- pos_start;}
	bool Eof()const {return (pos_curr== pos_end);}

 //data store
	wxFileOffset pos_start= 0;
	wxFileOffset pos_end= 0;
 protected:
 //data store
	wxFileOffset pos_curr= 0;
	UniFileStream_base* streamIO= nullptr;

 protected:
};



class itemEntry_iterate
{
 public:
	itemEntry_iterate(){}
	itemEntry_iterate(itemEntry* rootItem):LastFind(rootItem){}
		///see find_next?
	itemEntry* find( const wxString &findByStr, bool caseInsensitive= true );
	itemEntry* find( const wxStringVector& findByManyStr, bool caseInsensitive= true );
	itemEntry* find_re( wxString &findByStrSeparated, wxUniChar separator= '.', bool caseInsensitive= true );
	itemEntry* find_next( wxString &findByStr, bool caseInsensitive= true );
	itemEntry* find_new( wxString &findByStr, bool caseInsensitive= true );
	itemEntry* getByType( BYTE by_type, BYTE not_withType );
	//using LastFind, LastFind_index

	bool up( size_t nLevels=1 );
	inline bool GoUp(size_t nLevels=1){return this->up(nLevels);}

	bool clear_TreeToNow(){
		priv_B_fromLastByType= false;
		itemEntry_arrP.clear();
		LastFind= nullptr;
		LastFind_index= 0;
		return true;
	};

//		fafikLib_DB::translate_StrToArr
	inline itemEntry* get_LastFind()const{
		return LastFind;
	}
	inline size_t get_LastFind_index() const{
		return LastFind_index;
	}
	wxVector<itemEntry*> get_TreeToNow();
		///auto for fafikLib_DB::getParentTree_asStr
	wxString getParentTree(const wxString separator=".") const {
		if(LastFind)
			return itemEntry::getParentTree(LastFind, separator);
		return wxEmptyString;
	}

	itemEntry* LastFind= nullptr;
 protected:
	bool priv_B_fromLastByType= false;
	size_t LastFind_index= 0;
	wxVector<itemEntry*> itemEntry_arrP;
};


}; //namespace

typedef fafik77::itemEntry ItemEntry;

#endif // FAFIK77_ITEMENTRY_H
