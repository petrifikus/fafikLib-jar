/*	fafik77 grants rights to use this(fafikLib_DB) source code to anyone, as long as link to fafikLib_DB(with source download link) is supplied when used
 *
 *	fafikLib_DB allows you to read/write text && Binary content
 *	 with additional possibilities of changing interpreter type when used
 *	saving/loading configuration data from files (using ItemEntry), those tasks can propagate in Window ctrl (deriving from LoadCfg_call)
 created on:	2018.06
 last modify:	2024.02.12
 see "fafikLib_DB explanation" for details & info of usage
 */

//	see https://www.stack.nl/~dimitri/doxygen/manual/commands.html

#ifndef FAFIKLIB_DB_H
#define FAFIKLIB_DB_H

#include "itemEntry.h"

#include "Quotation.h"
#include "wxStringInputStream.h"
//#include "../fafikLib/fafikLib_regex.h"

namespace fafik77{

	///a helper class that should delete given pointer when goes out of scope (made 2019-10-07)
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



 ///!Warning this class is NOT Multi-Thread Safe
class fafikLib_DB
{
 public:
  //static val
	 ///Pointer Recursive Object Notation
	static const std::string fafikLib_PRON;
  //static functions
	 ///translates strings separated by '.' into Table
	static wxStringVector translate_StrToArr(const wxString& findKey_levelRecursive_str, const wxUniChar separator='.');

  //def types

  //ctor
	fafikLib_DB();
	virtual ~fafikLib_DB();
	fafikLib_DB(const fafikLib_DB& other);
	bool oninit();
	void PrintTable(itemEntry* iePrint, size_t dent=0);

	enum modes_IO{
		///"rb", read, edit, save later
		mode_read,
	//		///"wb"
	//		mode_write,
		///"r+b", no edit - append only, save later(faster)
		mode_append,
	};


		///compares 2 itemEntry, checks: type, key, value (without checking the items table)
	bool itemEntry_isSame(const itemEntry& item_a, const itemEntry& item_b);
		///checks if item_b exists in items_a
		///@return pointer if found in items_a, else nullptr
		///@see itemEntry_isSame
	itemEntry* itemEntry_getExistOrNew(wxVector<itemEntry*> &items_a, itemEntry &item_b);
	itemEntry* itemEntry_getExistOrNew(itemEntry &Table_items_a, itemEntry &item_b);

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
		wxStringVector files_DB;
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

//	enum ItemEntry_Stores_typeExt{	TODO
//		typeExt_unknown= 0,
//		typeExt_Number= 1,
//		typeExt_virtFFile= 2,
//		typeExt_link= 4,
//		typeExt_DB= 8,
//	};
//	class itemEntry_Stores
//	{		TODO
//	 public:
//		itemEntry_Stores(){}
//			///FreeAll,
//		virtual ~itemEntry_Stores();
//			///destroy all elements inside and mark as nullptr
//		void FreeAll();
//			///flushes DB_linked using its native Save()
//		bool save_DB_linked();
//
//			///describes what (this) should store
//		unsigned short typeExt= 0;
//
//	// [[pointers
//			///link to virtually limited part of FFile, created on request...
//		fafikLib_virtualByteFile* ByteData= nullptr;
//
//			///soft link to already existing object (object can not be deleted)
//			///points exact entry for "DB_linked"
//		fafikLib_DB::itemEntry* link= nullptr;
//			///link to underling DB, created on request...
//		fafikLib_DB* DB_linked= nullptr;
//		wxLongLong Number_store= 0;
//
//	//	fafikLib_DB;	//Recursive pointer(PRON)
//	//		link ItemEntry;	//to point exact location
//	// ]]
//	 protected:
//	};


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
		overrideInterpreter(size_t over_index_):which(2), over_index(over_index_){}
	};

	BYTE write_Byte_importFile(wxString file_in);
	BYTE write_Byte(const char *c_in);
	BYTE write_Byte(const wxCStrData &c_in);
	BYTE write_Byte(const wxString &c_in);

	BYTE read_Byte_ExportFile(wxString file_out);
	BYTE read_Byte(const char *out);
	BYTE read_Byte(const wxCStrData &out);
	BYTE read_Byte(const wxString &out);

	static BOOL CreateDirTree(const wxString& str_path, LPSECURITY_ATTRIBUTES attribs=NULL);


		///what if MDFC_Parted file???
		///@param file_path opens such file, returns 1 if Error
		///@param mode= mode_read
		///@param b: clearsPrevious= close previous file & clean DB
		///@return 0 ok, 1 can not make file, 2 no interpreter auto found, mode is not implemented for now 2018-08-07
// TODO (main#9#06/01/18): Open( wxString file ) what if MDFC_Parted file???
	BYTE Open(const wxString& file_path, modes_IO mode= mode_read, const wxString overwriteInterpreter= wxEmptyString, settings *useSettings= nullptr, bool clearsPrevious= true);
		///@return 0 -ok, 1 -no file, 2 -no interpreter,
		///new on 2018-10-15
	BYTE Open(const wxString& file_path, const overrideInterpreter& useIterpr ,modes_IO mode= mode_read, bool clearsPrevious= true );
	 ///you need to provide interpreter name/extension (json)
	BYTE Open(UniFileStream_base* BaseIO, const wxString& UseInterpr, bool clearsPrevious= true);
	BYTE UseDB( wxString file_path, modes_IO mode= mode_read ){
		return this->Open( file_path, mode );
	}
  // [ new todo now 2018-10-13
	 ///@return -1 -error can not open file, 0 ok
	 ///reload DB from file
	BYTE OpenAgain();
  // ] end new todo
		///@return true ok, false Error
		///closes interpret file file
	bool Close();
		///@return -1 on error, +int interpreters added
	int loadInterpreterFile(UniFileStream_base* item, const wxString& overwriteInterpreter= "fafikLib_PRON");
		///@return -1 on error, +int interpreters added
	int loadInterpreterFile(const wxString& file_path, const wxString& overwriteInterpreter= "fafikLib_PRON");
/// TODO (main#8#09/28/18): defineInterpreter
//	///make interpreter rules
	int defineInterpreter();

	int WriteTo(UniFileStream_baseWritable& stream, itemEntry *IE_saveCustom=nullptr);
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
		///@param Sort = true by default
	LONGLONG interpretFile(bool Sort=true);
		///close file handle, but dont drop settings
	bool interpretEnd();
		///manual interpret 1 by 1 (for GB_DB use mainly),(2018-08-07 not sure how saving will work(prob read 1 write 1))
		///@return 0 ok, -1 Error, 1 Eof, not 0 Error
		///@param p_out_type: 0 Exit, 1 Key/Comment, 2 Enter Table,  -1 unknown other
 	BYTE interpretNextLine(int *p_out_type= nullptr);	//itemEntry* Table
		///@return 0 success, 1 error
		///@param p_out_type pass it the value you got from interpretNextLine(int*)
 	BYTE discardPreviousEntries(const int &p_out_type);


	BYTE ready_interpreter(itemEntry &interpreterTable_p);
	BYTE UseInterpreter_redyInterpreter(){
		return ready_interpreter( *thoseDB_settings.useInterpreter );
	}

		///@return pointer to new subTable
	itemEntry* tableInsertSubTable(itemEntry *table, wxString key, wxString value="", BYTE type_stores=1);
	itemEntry* tableInsertNextSubTable(itemEntry *table, wxString key, wxString value="", BYTE type_stores=1);

//[[ new on 2018-07-20, not implemented yet	TODO
	//no type yet
	itemEntry* table_insert_string();
	itemEntry* table_insert_table();
	itemEntry* table_insert_ByteData();
		itemEntry* table_insert_file();
		itemEntry* table_insert_folder();
		///data base (another fafikLib_DB from another file(note that it might be reconstructed to fit the format))
	itemEntry* table_insert_DB();
//]]

	short dumpToFile(itemEntry* entry);
	short dumpToFileRec(itemEntry* entry, int indent=0);


//[[ cleanup, & dump	TODO
	 ///0 -ok, -1 -empty
	 ///!warning!, changing interpreter will be impossible
	BYTE cleanup_interpreter();
		///@return 0 -ok, 1 -already clean
		///!warning!, this should be done ONLY when this data will be no longer used
	BYTE cleanup_bigFiles();
		///@return 0 -ok, 1 -already clean
		///!warning!, frees DB
	BYTE cleanup_DB();
//]]

  //functions that will be used for jar like format .zip	TODO
	BYTE DataIntoFile();
	BYTE FileIntoData();
	 ///a recursive version of FileIntoData()
	BYTE FolderIntoData();


	wxFileOffset LengthFile() const{
		if( !DB_fileIOLine && !DB_fileIOLine->IsOpened() ) return -1;
		return DB_fileIOLine->Length();
	}
	const wxString& getFileName()const{ return P_DB_FileName; }

	inline UniFileStream_EOL getEolObj()const {return DB_fileIOLine->pass_useOnlyThisEndOL;}
	inline void setEolTo(const EOL& EolType) {DB_fileIOLine->setEolTo(EolType);}
	inline void setEolTo(const UniFileStream_EOL& toEol) {DB_fileIOLine->setEolTo(toEol);}

 //data store
	 ///root item DB_table.key="" containing whole DB file
	itemEntry DB_table;
	std::shared_ptr<itemEntry> DB_interpreter_table;
	wxFFile temp_oFile;
 // end of public:
 protected:
 	struct fastFind_struct;
 //data store
	settings thoseDB_settings;
	 ///file always used by interpreter
 	UniFileStream_file* DB_fileIO= nullptr;
 	UniFileStream_baseWritable* DB_file_write= nullptr;
 	 ///more like an overriten virtual class
 	UniFileStream_base* DB_fileIOLine= nullptr;
	 ///helps in OpenAgain(); 2018-10-15 string only(later Array)
 	wxString P_DB_FileName;
 	itemEntry *_NextLine_iE= nullptr;
	last_getItem_byKey last_getItem_byKey_DB;
	last_getItem_byKey last_getItem_byKey_interpreter;
	wxString _digits_list;

		///0 on fail, 1 ok
 	int WritePToFile(itemEntry *IE_saveCustom= nullptr);
 	int WritePToFile_rec(itemEntry *item, size_t depthSpaces= 0);

 	bool interpretStart(itemEntry* Table);

		///@return subTable to write to from next time
 	itemEntry* interpretNextLine_TEnter(itemEntry* Table, wxString in_line, const bool _TryWithAutoKey= 0);
		///@return Table to write to from next time
 	itemEntry* interpretNextLine_TExit(itemEntry* Table, wxString in_line);
		///@return bool(if itemEntry)
	itemEntry* interpretNextLine_TKey(itemEntry* Table, wxString in_line, const bool _TryWithAutoKey= 0);


// TODO (main#9#06/02/18): connect OnRead, OnPaused Evt to pass to parent

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
		wxStringVector recognitionPriority;

		///find will be valid even if next search is max [n] chars after last search
		long elementMaxDistanceApart= 1;
//			specialEntry
//			0 enter
//			1 exit
//			2 key
	};
	interpreterStru curr_interpreter;

	///part of OR inside AND, containing all data of Entry Struct, interpret by this layer

	enum fastFind_struct_el_enum
	{
		ff_unknown = 0,
		ff_e		=1,
		ff_key		=2,
		ff_value	=3,
		ff_bytes	=4,
		ff_auto_key	=5,
		ff_hashTable=6,
		ff_v		=7,
		ff_EOL		=8,

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
		wxStringVector key_stringArgs;
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


	///@return CanExitLoopOr. Main interpreter checker
	bool interpretNextLine_TLine(wxString &in_line, fastFind_struct *fFs_AndEntry_in, wxVector<makesSearch> &lineInterpreterResults, size_t &pos_last, itemEntry &AdditionalTableInfo, wxString AdditionalStrInfo= wxEmptyString);

	///2018-08-15
	wxString WritePToFile_rLine(fastFind_struct &use_parser, itemEntry *item, bool on_ff_bytes=false);

	///run when encountered "bytes"
	///@return pointer of insert
	itemEntry_Bytes* interpretNextLine_TBytes(wxString &in_line, itemEntry* Table, wxVector<makesSearch> &lineInterpreterResults, size_t &pos_last );
 //end of protected:
 public:
 	///@return true ok, false file is Not Opened
	inline bool interpretStart(){return interpretStart(&DB_table); }
	BYTE UseSettings(settings &useSettings);
	void UseEol(const UniFileStream_EOL& useEol_);
	///@return 0 ok, -1 Error not found, -2 Error no string
	BYTE UseInterpreter(const wxString& interName);
	///@return 0 ok, -1 Error not found,
	///@see UseInterpreter
	BYTE UseInterpreterAuto(const wxString& byFileExt);

	fafikLib_DB(std::shared_ptr<itemEntry>& DB_interpreter_table_sp);

};


}; //namespace

#endif // FAFIKLIB_DB_H
