/*	fafik77 grants rights to use this(fafikLib_DB) source code to anyone, as long as link to fafikLib_DB(with source download link) is supplied when used
 *
 *	fafikLib_DB allows you to read/write text && Binary content
 *	 with additional possibilities of changing interpreter type when used
 created on:	2018.06
 last modify:	2018.08.13
 see "fafikLib_DB explanation" for details & info of usage
 */

//	see https://www.stack.nl/~dimitri/doxygen/manual/commands.html

#ifndef FAFIKLIB_DB_H
#define FAFIKLIB_DB_H


#include "fafikLib/fafikLib_readLineByte.h"
#include "fafikLib/fafikLib_Quotation.h"
#include "fafikLib/wxStringInputStream.h"


	///made 2019-10-07, a helper class that should delete given pointer when goes out of scope
template<typename TempTPtr>
class autoPtrDestructor
{
	TempTPtr aDeletes;
 public:
		///expects pointer to said Type
	autoPtrDestructor(TempTPtr& objIn): aDeletes(objIn){}
	inline ~autoPtrDestructor(){
		delete aDeletes;
	}
};


class fafikLib_virtualByteFile;

class fafikLib_DB
{
 private:
	BYTE _b_TablesInherited= false;
 public:
		///size_t(-1)
	static const size_t size_ti;

	fafikLib_DB();
	virtual ~fafikLib_DB();
	fafikLib_DB(const fafikLib_DB& other);
	bool oninit();

	enum modes_IO{
		///"rb", read, edit, save later
		mode_read,
//		///"wb"
//		mode_write,
		///"r+b", no edit - append only, save later(faster)
		mode_append,
	};
		///Pointer Recursive Object Notation
	static const std::string fafikLib_PRON;
	class itemEntry_Stores;


///counts all bytes stored in: key, value
wxULongLong BytesStored=0;	//decided useless, redo, 2018-08-06

	enum itemEntry_type_stores {
			///also unwritable
		ie_none=0,
			///string value
		ie_string=1,
		ie_value=1,
			///when commented
		ie_lore=2,
			///Table
		ie_sub_itemTable=4,
		ie_Table=4,
		ie_TableNotClosed= 16,

			///requests further info from fafikLib_ItemEntry_Stores
			///unused 2018-09-21, see ExtDataLink
		ie_link= 64,
			///no name, exists under "auto_key"
		ie_auto_key_keyed= 32,
		ie_auto_key= 128,

		ie_error= -1,
	};
	enum itemEntry_type_tags {
		ie_caseInsensitive= 1,
		ie_notAString= 2,
	};
		///universal call to what item will store
	typedef BYTE itemStores;	//2019-10-09, bool8 also works

		///@warning always use origin reference/pointer, never copy itemEntry
	struct itemEntry
	{
		typedef wxVector<itemEntry*>::iterator iterator;
		typedef wxVector<itemEntry*>::const_iterator const_iterator;
	  //{storage
		//only sub_itemTable =0	 or	 only *ByteData_itemEntry_selfEntry =0
			///what value stores..
			///@see itemEntry_type_stores
			///0 =no value, 1 =string, 2 =lore(specialEntry "comment"), 4 =sub_itemTable(can be 0 size), 64 =link(specialEntry "link"),	? 128=auto_key
		itemStores type_stores= 0;
			///@see itemEntry_type_tags
		itemStores type_tags= 0;

			///auto added parent. child has to have a parent otherwise its an independent parent
		itemEntry* parent= nullptr;

			///auto incremented depth(child of n level)
		unsigned short depth= 0;
			///if auto_key then key = Position__;
		wxString key;
		wxString value;

			///Table
			///see itemEntry::Free, itemEntry::Detach
//	 protected:
		wxVector<itemEntry*> sub_itemTable;
//	 public:
			///when "bytes" statement is in place
		fafikLib_virtualByteFile* ByteData_itemEntry_selfEntry= nullptr;
	// [new 2018-09-21
//		//on specialEntry "link" points to requested itemEntry, value is string "requested"
//		///instead of ByteData_itemEntry_selfEntry,
//		fafikLib_ItemEntry_Stores** linksTo= nullptr; //pointer(here) to pointer(wxVector) to object(new)
//			on ~delete:: {delete **linksTo, *linksTo= 0, //linksTo= 0 }
	// ]end new
	  //}end storage



		virtual ~itemEntry();
		itemEntry(){}
		itemEntry(
			const wxString& key_,
			const wxString& value_,
			itemStores type_stores_,
			unsigned short depth_= 0,
			itemEntry *parent_= nullptr
		) : type_stores(type_stores_), parent(parent_), depth(depth_), key(key_), value(value_) {}
		itemEntry(
			const wxString& key_,
			itemStores type_stores_,
			itemStores type_tags_
		) : type_stores(type_stores_), type_tags(type_tags_), key(key_) {}


		iterator getOrAdd( const wxString& key_, BYTE type_, bool caseInsensitive=true, bool *out_found=nullptr );
		iterator insert(iterator it, itemEntry* item);
			///returns iterator or end()
		iterator find( const wxString &item_byKey, itemStores type, bool caseInsensitive=true );

		iterator end(){ return sub_itemTable.end(); }
		const_iterator end()const{ return sub_itemTable.end(); }
		iterator begin(){ return sub_itemTable.begin(); }
		const_iterator begin()const{ return sub_itemTable.begin(); }



			///LUA like get
			///!!! for sure valid: check if(returned != input) or reset_invalidSearch()==0.. insecure -use GetItem_byKey() instead
		itemEntry& operator[](const wxString &item_byKeyName);
			///LUA like get
			///same as at( n, true, true )
		inline itemEntry& operator[](const size_t item_byNumber){
			return *at(item_byNumber, true);
		}
			///@param outFoundPos -saves index of found item here
		itemEntry* find( const wxString &item_byKeyName, bool caseInsensitive= false, size_t* outFoundPos=nullptr );
			///alias for find(str, caseSense)
		inline itemEntry* at( const wxString &item_byKeyName, bool caseInsensitive= false, size_t* outFoundPos=nullptr ){
			return this->find(item_byKeyName, caseInsensitive, outFoundPos);
		}
		itemEntry* find_byValue( const wxString &item_byValue, bool caseInsensitive= false, size_t* outFoundPos=nullptr );
			///detaches sub_itemTable so any inherent of this does not violate memory after this is deleted
		inline void Detach(){sub_itemTable.clear();}	//disables auto destructor for those elements
			///runs destructor for sub_itemTable, freeing used RAM
		void FreeTable();
			///alias FreeTable() @see FreeTable
		inline void Free(){return FreeTable();}

//		itemEntry* operator->();

		itemEntry* get();
		const itemEntry* get() const;
		inline itemEntry* get_ptr() { return get(); }
		inline itemEntry &get_ref() { return *get(); }
			///@return true if found removed
			///@param silent 0 -remove from table, 1 -nullify
		bool removeItem(const wxString& by_string, bool silent=false);
			///@return true if found removed
			///@param silent 0 -remove from table, 1 -nullify
		bool removeItem(size_t by_pos, bool silent=false);
			///@return true if found removed
			///@param silent 0 -remove from table, 1 -nullify
		bool removeItem(const itemEntry* by_id, bool silent=false);
			///@param NoParentRemove: destroy this item even if has no parent
			///auto calls parent->removeItem(this)
		bool removeSelft(bool NoParentRemove=false){
			if(parent) return parent->removeItem(this);
			if(NoParentRemove){ this->~itemEntry(); return 1;}
			return 0;
		}
		size_t pop_back(size_t amount=1);
		bool removeItems(size_t pos_from, size_t amount=1, bool silent=false);
		bool removeItems(const itemEntry* by_id, size_t amount=1, bool silent=false);

			///@return first item that contains "by_type", then tries in parent, eventually parent=nullptr
		itemEntry *getByType( BYTE by_type, size_t from_pos_= 0 );

			///@param key = search for key
			///@param type_ = type to set when Add()
			///@param *out_found = bool to place true if success
			///@return already existing item [], or Add() and return new.. *out_found = if exists
		itemEntry* getOrAdd( const wxString& key_, BYTE type_, bool *out_found= nullptr );
//			///@see getOrAdd
//			///@return already existing item [], or Add() and return new
//		inline itemEntry &AddOrGet( wxString key_, BYTE type_ ){return *getOrAdd(key_, type_);}
			///use after [str] to check & reset error
			///@return 1 error, 0 ok
			///@see operator[] (const wxString &item_byKeyName)
		inline bool reset_invalidSearch(){
			bool old_invalidSearch= invalidSearch;
			invalidSearch= 0;
			return old_invalidSearch;
		}
			///@return nullptr on not found
		itemEntry* at(const size_t item_byNumber, const bool auto_key_keyedSearch=false, const bool resolveLink=true);
		itemEntry* last(const size_t substract_n=0, const bool resolveLink=true);
//		itemEntry* at(size_t item_byNumber, bool auto_key_keyedSearch=false) const{
//			return at(item_byNumber, auto_key_keyedSearch);
//		}
			///@return index of self in parent Table
		size_t getPosition() const;
		inline bool isTable() const{
			return (type_stores & ie_Table);
		}
		inline bool isAuto_key() const{
			return (type_stores & ie_auto_key);
		}
		inline bool isAuto_key_keyed() const{
			return (type_stores & ie_auto_key_keyed);
		}
		inline bool isValue() const{
			return (type_stores & ie_value);
		}
		inline bool isLore() const{
			return (type_stores & ie_lore);
		}
			///unused 2018-09-18, redo
		inline bool isLink() const{
			return (type_stores & ie_link);
		}
			///will not be inline, couse (will be redone)
		bool isWithBytes() const{
			if(ByteData_itemEntry_selfEntry) return true;
			return false;
		}
		bool isCaseInsensitive() const{
			if(type_tags & ie_caseInsensitive) return true;
			return false;
		}

		inline size_t sizeTable() const{
			return sub_itemTable.size();
		}

			///@param mask -determines what should be left (0 -null, 32 -findByAutoKey)
			///@return old type_stores
			///warning effect has no save point, although can be overwritten
		inline BYTE setUnwritable( BYTE mask= ie_auto_key_keyed|ie_auto_key ){
			BYTE ret= this->type_stores;
			this->type_stores&= mask;
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
		inline void setSubsParent();
			///@return pointer to added item
			///appends pointer to object, 2018-08-13 allowed pushing to non Table(& sets Table)
		itemEntry* push_back_ptr(itemEntry *push);
			///copies content recursively, reinterpreting eg: (parent, depth), omitting pointers
		bool push_back_cpyTotal(itemEntry &push);
			///@return pointer to added item
			///creates a shallow copy of item and appends
			///@see push_back_ptr
		inline itemEntry* push_back_cpy(itemEntry &push) {
			itemEntry* temp=new itemEntry(push); return push_back_ptr(temp);}
			///@return pointer to added item
			///creates a copy of item and appends
			///@see push_back_cpy
		inline itemEntry* push_back(itemEntry &push){ return push_back_cpy(push);}
			///@return pointer to added item
			///@brief appends pointer to object
			///@see push_back_ptr
		inline itemEntry* push_back(itemEntry *push){ return push_back_ptr(push);}

			///adds new subItem with ( ... )
		itemEntry* Add( const wxString& key_, const wxString& value_, BYTE type_stores_= 0 );
			///adds new subItem with ( ... )
		itemEntry* Add( const wxString& key_);
			///@return true if ok, false if not "ie_auto_key"
			///adds new subItem with ( ... ), only when ie_auto_key
		itemEntry* Add( BYTE type_stores_, const wxString& value_= wxEmptyString );
			///@see Add moveItem
		bool AddBefore( const wxString& key_, const wxString& value_, BYTE type_stores_= 0, size_t beforeItem= -1 ){
			Add( key_, value_, type_stores_ );
			return moveItem( -1, beforeItem );
		}
//			///@return pointer to new item
//		itemEntry* AddNew(const wxString& key_,const wxString& value_, itemStores type_stores_= 0 );

			///@param itemFromPos = -1 end, or pos (aligns to size)
			///@param itemNewPos = -1 before end, or pos (aligns to size)
		bool moveItem( size_t itemFromPos= -1, size_t itemNewPos= -1 );
		inline bool swapItem( size_t itemFromPos= -1, size_t itemNewPos= -1 ){
			return moveItem(itemFromPos, itemNewPos);
		}

			///2018-10-21
		virtual bool Adv_isNumber(){return 0;}
		virtual wxLongLong Adv_getNumber_LL(){return 0;}

			///2019-10-09
		static bool _operLowerB_compareIE( wxVector<fafikLib_DB::itemEntry*>::iterator item, const itemEntry& compa );
	 protected:
			///gets a pointer to the item in Table
		iterator binaryGetPosOf(const itemEntry& TemplateItem);
		bool binaryGetPosOf_exists(iterator prevRetValue, const itemEntry& TemplateItem)const;
	 private:
		bool invalidSearch= false;
	};
		///2018-10-21
	struct itemEntry_LL: public itemEntry
	{
		bool Adv_isNumber() override {return 1;}
		wxLongLong Adv_getNumber_LL() override {return LL;}
		wxLongLong LL= 0;

	};
		///compares 2 itemEntry, checks: type, key, value
	bool itemEntry_isSame(itemEntry& item_a, itemEntry& item_b);
		///checks if item_b exists in items_a
		///@return pointer if found in items_a, else nullptr
		///@see itemEntry_isSame
	itemEntry *itemEntry_getExistOrNew(wxVector<itemEntry*> &items_a, itemEntry &item_b);
	itemEntry *itemEntry_getExistOrNew(itemEntry &Table_items_a, itemEntry &item_b);

//		///pointer to a pointer
//		///rename me
//	wxVector<itemEntry_Stores*>p_to_val;


	struct last_getItem_byKey
	{
		size_t depth= 0;
		wxVector<itemEntry*> itemEntry_p;
		///from 2018-10-13 its recommended to use 2p**
		///wxVector<itemEntry**> itemEntry_pp;
	};
	struct settings
	{
//	 protected:
		BYTE OpenMode= mode_read;
		itemEntry* useInterpreter= nullptr;
			///basically file .ext ..my, XML, LUA
		wxString FileType="";
//	 public:

			///DB files
		wxArrayString files_DB;
// TODO (main#9#08/06/18): autoContinueDB_files after 1 Eof continue with next file
			///if DB is in multiple files(e.g due to FAT) auto find & use
		bool autoContinueDB_files= true;
		/*wxVector<onReadLine_ReplaceStr_struct> onReadLine_ReplaceStr_list;*/

// TODO (main#8#05/31/18): allowRepeatableKeys , make fast waypoint wxStringArr
			///false -linear check of each previous key, skip if already used
		bool allowRepeatableKeys= true;

// TODO (main#9#05/31/18): to implement: switchToGB_DB, todo: define in ctor()
			///will pause after each 1 itemEntry.. next call will free previous itemEntry
		bool switchToGB_DB= false;

			///void(ignore) BlankChars at line start
		bool LineStartVoidBlankChars= true;

			///writeLine after indent, def "\t" || "  ", warning do NOT use any char of NewLine indication
		wxString onWriteLine_biginWithAutoIndent= "  ";
			/// 4 spaces to tab, 0 to disable this
		BYTE onWriteLine_biginWithAutoIndent_autoExpandNSpacesToTab = 4;

	};

	class itemEntry_iterate
	{
	 public:
	 	itemEntry_iterate(){}
	 	itemEntry_iterate(itemEntry* rootItem):LastFind(rootItem){}
			///see find_next?
		itemEntry* find( wxString &findByStr, bool caseInsensitive= true );
		itemEntry* find( const wxArrayString& findByManyStr, bool caseInsensitive= true );
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
		inline wxString getParentTree(const wxString separator=".")const{
			if(LastFind)
				return fafikLib_DB::getParentTree_asStr(*LastFind, separator);
			return wxEmptyString;
		}

		itemEntry* LastFind= nullptr;
	 protected:
	 	bool priv_B_fromLastByType= false;
		size_t LastFind_index= 0;
		wxVector<itemEntry*> itemEntry_arrP;
	};

	enum ItemEntry_Stores_typeExt{
		typeExt_unknown= 0,
		typeExt_Number= 1,
		typeExt_virtFFile= 2,
		typeExt_link= 4,
		typeExt_DB= 8,
	};
	class itemEntry_Stores
	{
	 public:
		itemEntry_Stores(){}
			///FreeAll,
		virtual ~itemEntry_Stores();
			///destroy all elements inside and mark as nullptr
		void FreeAll();
			///flushes DB_linked using its native Save()
		bool save_DB_linked();

			///describes what (this) should store
		unsigned short typeExt= 0;

	// [[pointers
			///link to virtually limited part of FFile, created on request...
		fafikLib_virtualByteFile* ByteData= nullptr;

			///soft link to already existing object (object can not be deleted)
			///points exact entry for "DB_linked"
		fafikLib_DB::itemEntry* link= nullptr;
			///link to underling DB, created on request...
		fafikLib_DB* DB_linked= nullptr;
		wxLongLong Number_store= 0;

	//	fafikLib_DB;	//Recursive pointer(PRON)
	//		link ItemEntry;	//to point exact location
	// ]]
	 protected:
	};
	struct overrideInterpreter
	{
			////@param -1 -do not change
			///@param 0 -def(dont change)
			///@param 1 -string name
			///@param 2 -index
			///@param 3 -string ext Auto
			///@param 4 -string ext specified
		BYTE which= 0;
		wxString over_string;
		size_t over_index= 0;
		overrideInterpreter(){}
		overrideInterpreter(const wxString& over_string_name_ext ):which(1), over_string(over_string_name_ext){}
			///@param extType -false: ext Auto, -true: ext specified
		overrideInterpreter(bool extType, const wxString& over_string_name_ext= wxEmptyString ):which(3+extType), over_string(over_string_name_ext){}
//		overrideInterpreter(const bool& doNotChange):which(-1){}
		overrideInterpreter(size_t over_index_):which(2), over_index(over_index_){}
	};

		///root item DB_table.key="" containing whole DB file
	itemEntry DB_table;

	itemEntry DB_interpreter_table;

	BYTE write_Byte_importFile(wxString file_in);
	BYTE write_Byte(const char *c_in);
	BYTE write_Byte(const wxCStrData &c_in);
	BYTE write_Byte(const wxString &c_in);

	BYTE read_Byte_ExportFile(wxString file_out);
	BYTE read_Byte(const char *out);
	BYTE read_Byte(const wxCStrData &out);
	BYTE read_Byte(const wxString &out);



	itemEntry* ReadLineContent();
//void* getItem_byKey( wxArrayString findKey_levelRecursive );
//	wxString searchBy();


		///what if MDFC_Parted file???
		///@param file_path opens such file, returns 1 if Error
		///@param mode= mode_read
		///@param b: clearsPrevious= close previous file & clean DB
// TODO (main#9#06/01/18): Open( wxString file ) what if MDFC_Parted file???
	BYTE Open(wxString file_path, modes_IO mode= mode_read, const wxString overwriteInterpreter= wxEmptyString, settings *useSettings= nullptr, bool clearsPrevious= true);
		///@return 0 -ok, 1 -no file, 2 -no interpreter,
		///new on 2018-10-15
	BYTE Open(const wxString& file_path, const overrideInterpreter& useIterpr ,modes_IO mode= mode_read, bool clearsPrevious= true );
	BYTE UseDB( wxString file_path, modes_IO mode= mode_read ){
		return this->Open( file_path, mode );
	}
  // [ new todo now 2018-10-13
		///@return -1 -error can not open file, 0 ok
		///reload DB from file
	BYTE OpenAgain();
		///alias OpenAgain()
	inline BYTE reOpen(){return OpenAgain();}
		///open only DB file
	BYTE OpenDB();
		///overload normal with many(default normal to many[1])
	BYTE Open_1();
		///overload to open with settings
	BYTE Open_2(const overrideInterpreter&);
  // ] end new todo
//	#define Use_DataBase Open()
		///@return true ok, false Error
		///closes interpret file file
	bool Close();
		///@return -1 on error, +int interpreters added
	int loadInterpreterFile(const wxString file_path, const wxString overwriteInterpreter= "fafikLib_PRON");
/// TODO (main#8#09/28/18): defineInterpreter
//	///make interpreter rules
	int defineInterpreter();

	int WriteToFile();
		///alias for WriteToFile()
	inline int Save(){return WriteToFile();}
//	/@return 0 -fail, 1 -written
	int WriteToFile_new();
		///@return 0 -fail, 1 -written
		///write to the same file(dont create new file) !!!Warning any un-loaded DB parts into RAM will NOT be written
	bool WriteToFile_sameFileForced();
		///alias WriteToFile_sameFileForced
	inline bool SaveForceSame(){return WriteToFile_sameFileForced(); }

	int WriteToFile_append();
	int WriteToNewFile( wxString filePathName );

		///saves interpreters to file
	int WriteToFile_interpreter( wxString filePathName );


		///auto "interpretNextLine" till Eof
	wxLongLong interpretFile();
		///manual interpret 1 by 1 (for GB_DB use mainly),(2018-08-07 not sure how saving will work(prob read 1 write 1))
//	bool interpretStart(itemEntry* Table);
//	bool interpretStart(){return interpretStart(DB_table); }

		///@return 0 ok, -1 Error, 1 Eof, not 0 Error
 	BYTE interpretNextLine();	//itemEntry* Table


	BYTE ready_interpreter(itemEntry &interpreterTable_p);
	BYTE UseInterpreter_redyInterpreter(){
		return ready_interpreter( *thoseDB_settings.useInterpreter );
	}



	BYTE tableInsert(itemEntry *table, wxString key, wxString value="", BYTE type_stores=1);
		///@return pointer to new subTable
	itemEntry* tableInsertSubTable(itemEntry *table, wxString key, wxString value="", BYTE type_stores=1);
	itemEntry* tableInsertNextSubTable(itemEntry *table, wxString key, wxString value="", BYTE type_stores=1);

//[[ new on 2018-07-20-16.49
	//no type yet
	itemEntry* table_insert_string();
	itemEntry* table_insert_table();
	itemEntry* table_insert_ByteData();
		itemEntry* table_insert_file();
		itemEntry* table_insert_folder();
		///data base (another fafikLib_DB from another file(note that it might be reconstructed to fit the format))
	itemEntry* table_insert_DB();
//]]

		///translates strings separated by '.' into Table
	itemEntry* getItem_byKey(wxString findKey_levelRecursive_str, const wxUniChar separator='.', bool searchFromLastSearch=true );
	itemEntry* getItem_byKey(wxArrayString findKey_levelRecursive, bool searchFromLastSearch=true);
		///translates strings separated by '.' into Table
	itemEntry* getItem_byKey_FromInterpreter(wxString findKey_levelRecursive_str, const wxUniChar separator='.', bool searchFromLastSearch=true);
	itemEntry* getItem_byKey_FromInterpreter(wxArrayString findKey_levelRecursive, bool searchFromLastSearch=true);

		///translates strings separated by '.' into Table
	static wxArrayString translate_StrToArr(wxString findKey_levelRecursive_str, const wxUniChar separator='.');

	wxFFile temp_oFile;
	short dumpToFile(itemEntry* entry);
	short dumpToFileRec(itemEntry* entry, int indent=0);


//[[ cleanup, & dump
//	///0 -ok, -1 -empty
		///!warning!, changing interpreter will be impossible
	BYTE cleanup_interpreter();
		///@return 0 -ok, 1 -already clean
		///!warning!, this should be done ONLY when this data will be no longer used
	BYTE cleanup_bigFiles();
		///@return 0 -ok, 1 -already clean
		///!warning!, frees DB
	BYTE cleanup_DB();
//	BYTE cleanup_();
//]]

	BYTE DataIntoFile();
//	///DataIntoFile()
//	#define export_DataIntoFile DataIntoFile
	BYTE FileIntoData();
//	///FileIntoData()
//	#define import_FileIntoData FileIntoData
		///a recursive version of FileIntoData()
	BYTE FolderIntoData();


		///get tree children to this IE_in itemEntry
	static wxArrayString getParentTree_asArray(const itemEntry &IE_in);
	static wxArrayString getParentTree(const itemEntry &IE_in) { return getParentTree_asArray(IE_in); }
		///pre-made call for getParentTree_asArray()+separator
	static wxString getParentTree_asStr(const itemEntry &IE_in, const wxString separator=".");


	wxFileOffset LengthFile() const{
		if( !DB_fileIO.IsOpened() ) return -1;
		return DB_fileIO.Length();
	}
	const wxString& getFileName()const{ return P_DB_FileName; }

// end of public:
 protected:
 	itemEntry* getItem_byKey_rec(itemEntry &Table, wxArrayString findKey_levelRecursive, size_t currPos=0, BYTE is_interpreter=2);
	settings thoseDB_settings;
		///file always used by interpreter
 	wxFFile DB_fileIO;
 	wxFFile DB_file_write;
 	fafikLib_readLineByte_file DB_fileIOLine;
		///0 on fail, 1 ok
 	int WritePToFile(itemEntry *IE_saveCustom= nullptr);
 	int WritePToFile_rec(itemEntry *item, size_t depthSpaces= 0);
		///helps in OpenAgain(); 2018-10-15 string only(later Array)
 	wxString P_DB_FileName;
// 	///2nd file (after std::swap) for keeping prev data/// or use wxFFile[bool 0/1]
// 	wxFFile DB_fileIO_2;



// 	/// 0 ok, -1 Error, 1 Eof, not 0 Error
// 	///shortcut into public: overwrite with no args options.. with use "*_NextLine_iE"
// 	BYTE interpretNextLine(itemEntry* Table);
 	itemEntry *_NextLine_iE= nullptr;
 	bool interpretStart(itemEntry* Table);

		///@return subTable to write to from next time
 	itemEntry* interpretNextLine_TEnter(itemEntry* Table, wxString in_line, const bool _TryWithAutoKey= 0);
		///@return Table to write to from next time
 	itemEntry* interpretNextLine_TExit(itemEntry* Table, wxString in_line);
		///@return bool(if itemEntry)
	itemEntry* interpretNextLine_TKey(itemEntry* Table, wxString in_line, const bool _TryWithAutoKey= 0);

// 	wxULongLong am_RAM_inBytes=0;

// TODO (main#9#06/02/18): connect OnRead, OnPaused Evt to pass to parent


	last_getItem_byKey last_getItem_byKey_DB;
	last_getItem_byKey last_getItem_byKey_interpreter;
struct fastFind_struct;
	struct interpreterStru
	{
		itemEntry* specialEntry=nullptr;
		itemEntry* entry_table=nullptr;

		wxVector<fastFind_struct> ff_assign;
		wxVector<fastFind_struct> ff_keyValue;
		///-1 unknown / none, >= 0 ok
		int isAssignAt_ff_keyValue= -1;
		  BYTE isLineTable= false;
		wxVector<fastFind_struct> ff_table_enter;
		///-1 unknown, -2 none, 0 to -3 ok
		int isAssignAt_ff_table_enter= -1;
		wxVector<fastFind_struct> ff_table_exit;

		///used as recursed, located inside "ff_table_enter/exit"
		wxVector<fastFind_struct> ff_table_bytes;
		  ///used to prevent endless "ff_table_bytes" add loop
		  bool bytesIn= false;
		BYTE overwrite_byte_type= -1;

		///recognition priority 2018-07-23, always needs 3 values from: {exit, enter, key_val}
		wxArrayString recognitionPriority;

		///find will be valid even if next search is max [n] chars after last search
		long elementMaxDistanceApart= 1;
//			specialEntry
//			0 enter
//			1 exit
//			2 key
	};
//	interpreterStru *curr_interpreter_p= nullptr;
//	interpreterStru &curr_interpreter= *curr_interpreter_p;
	interpreterStru curr_interpreter;

	///part of OR inside AND, containing all data of Entry Struct, interpret by this layer

	enum fastFind_struct_el_enum
	{
		ff_unknown = 0,

		ff_e	=1,
		ff_key	=2,
		ff_value	=3,
		ff_bytes	=4,
		ff_auto_key=5,
		ff_hashTable=6,
		ff_v	=7,
		ff_EOL	=8,

		ff_mode_perc = 256,
	};

	struct fastFind_struct_el
	{
		///1 -e, 2 -key, 3 -value, 4 -bytes, 5 -auto_key, 6 -hash table, 7 -v, 8 -EOL	, % +0x100(256)
		unsigned short element_type= 0;
		BYTE isAssign= 0;

		///stores key. used by "line_interpreter()"
		wxString key;
		///stores args for key. used by "line_interpreter()"
		wxArrayString key_stringArgs;
		///stores long int for key. used by "line_interpreter()"
		wxArrayLong args_L;

	};
	///parts of AND, when pushing everything into OR set "element_type", put into use by this layer
	struct fastFind_struct
	{
		///fastFind_struct_el
		///1 -e, 2 -key, 3 -value, 4 -bytes, 5 -auto_key, 6 -hash table, 7 -v	, % +0x100(256)
		unsigned short element_type= 0;
		bool isAssign= 0;

		wxVector<fastFind_struct_el> keys;

		size_t find(wxString str) const;
	};

	///@return -2 -missing put_here, -1 -missing temp_tab, >=0 -ok
 	short ready_partForinterpreter(itemEntry *temp_tab, wxVector<fastFind_struct> *put_here);
 	unsigned short ready_partForinterpreterOR(itemEntry *temp_tab, fastFind_struct *put_here);
 	wxString _digits_list;


///progressive search = str.find(strF):: {{str[i]==strF[i] i++}}
	struct makesSearch
	{
		///copy fastFind_struct_el.element_type, for final combining
		unsigned short element_type= 0;

		/// -1 unknown
		size_t pos_from=0;
		/// -1 unknown
		size_t pos_to=0;

		///output parsed requested Content
		wxString interpreted_value;
		///requests Content
		wxString interpreter_key;
	};

	///stores ByteData from all itemEntry.bytes
	///redo for use `*itemEntry_Stores`
	wxVector<fafikLib_virtualByteFile> ByteData_itemEntry_all;

	///@return CanExitLoopOr. Main interpreter checker
	bool interpretNextLine_TLine(wxString &in_line, fastFind_struct *fFs_AndEntry_in, wxVector<makesSearch> &lineInterpreterResults, size_t &pos_last, itemEntry &AdditionalTableInfo, wxString AdditionalStrInfo= wxEmptyString);

	///2018-08-15
	wxString WritePToFile_rLine(fastFind_struct &use_parser, itemEntry *item, bool on_ff_bytes=false);

	///run when encountered "bytes"
	///@return pointer of insert
	fafikLib_virtualByteFile* interpretNextLine_TBytes(wxString &in_line, itemEntry* Table, wxVector<makesSearch> &lineInterpreterResults, size_t &pos_last );
//end of protected:
 public:
 	///@return true ok, false file is Not Opened
	bool interpretStart(){return interpretStart(&DB_table); }
	BYTE UseSettings(settings &useSettings);
	///@return 0 ok, -1 Error not found, -2 Error no string
	BYTE UseInterpreter(const wxString& interName);
	///@return 0 ok, -1 Error not found,
	///@see UseInterpreter
	BYTE UseInterpreterAuto(const wxString& byFileExt);

//	class itemEntryArray
//	{
//	 public:
//		itemEntryArray();
//		~itemEntryArray();
//	 protected:
//	 	wxArrayPtrVoid m_values;
//	 private:
//	};
	fafikLib_DB(itemEntry& DB_interpreter_table);

};

///write will require export & import after Flush()
///actually (:public fafikLib_readLineByte_part) and apply method FlushToFile...  replace with newer(2018-10-30) fafikLib_readLineByte_part
class fafikLib_virtualByteFile // concept 2018-07-23
{
 public:
	fafikLib_virtualByteFile();
	virtual ~fafikLib_virtualByteFile();


	///Seek position in Bytes
	bool Seek(wxFileOffset ofs=0);
	///get position
	wxFileOffset Tell() const { return _currPos; }
	///get Length
	wxFileOffset Length() const{ return length(); }
	///get [amount] of Bytes into buffer
	size_t Read(void* pBuf, size_t amount);
	///export virtualByteFile to file in Path, 0 -ok, 1 -no read, 2 -could not open write
	BYTE FlushToFile(const wxString filePathName, const size_t buffSize= 1*1024*1024);

	///simple bool()
	bool Eof() const{ return (_currPos== -1 || _currPos>= length()); }

	wxString FileName="virtualByteFile.fafikLib_PRON";

 protected:
 	friend class fafikLib_DB;
	wxFileOffset pos_start=	0;
	wxFileOffset pos_end=	0;
	wxFileOffset _currPos=	0;

	wxFFile* _bytes_fileIO=	nullptr;

	//wxArrayString ExportedToFiles; //2018-07-23, may be used in the future
 public:
	wxFileOffset length()const {return pos_end- pos_start;}
};

/*class fafikLib_ItemEntry_Stores
{
 public:
	fafikLib_ItemEntry_Stores(){}
	///FreeAll,
	virtual ~fafikLib_ItemEntry_Stores();
	///destroy all elements inside and mark as nullptr
	void FreeAll();
	///flushes DB_linked using its native Save()
	bool DB_linked_save();

	enum typeExt_enum{
		typeExt_unknown= 0,
		typeExt_Number= 1,
		typeExt_virtFFile= 2,
		typeExt_link= 4,
		typeExt_DB= 8,
	};
	///describes what (this) should store
	unsigned short typeExt= 0;

// [[pointers
	///link to virtually limited part of FFile, created on request...
	fafikLib_virtualByteFile* ByteData= nullptr;

	///soft link to already existing object (object can not be deleted)
	///points exact entry for "DB_linked"
	fafikLib_DB::itemEntry* link= nullptr;
	///link to underling DB, created on request...
	fafikLib_DB* DB_linked= nullptr;
	wxLongLong Number_store= 0;

//	fafikLib_DB;	//Recursive pointer(PRON)
//		link ItemEntry;	//to point exact location
// ]]
 protected:
};*/

#endif // FAFIKLIB_DB_H
