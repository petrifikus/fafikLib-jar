#include "fafikLib_DB.h"
#include <wx/filename.h>

#include "f77_helpers.h"

namespace fafik77{


fafikLib_DB::fafikLib_DB()
{	//ctor
	oninit();
}

fafikLib_DB::~fafikLib_DB()
{	//dtor
	if(DB_fileIO){
		delete DB_fileIO;
		DB_fileIO= nullptr;
	}
}

fafikLib_DB::fafikLib_DB(const fafikLib_DB& other)
{	//copy (interpreter) ctor
	this->DB_interpreter_table= other.DB_interpreter_table;
	oninit();

}
 ///pass DB_interpreter_table
fafikLib_DB::fafikLib_DB(std::shared_ptr<itemEntry>& DB_interpreter_table_sp)
{	//copy interpreter ctor
	this->DB_interpreter_table= DB_interpreter_table_sp;
	oninit();
}




const std::string fafikLib_DB::fafikLib_PRON= "fafikLib_PRON";
 /// constructor -->> Use_DataBase(){oninit();};
bool fafikLib_DB::oninit()
{
	if(_digits_list.empty()){
		for(size_t temp_di=0; temp_di<=9; temp_di++){
			_digits_list.Append( char('0'+temp_di) );
		}
	}
/**	interpreter goes from START of declaration(in OR)
 *	writing Save() goes from END of declaration(in OR)
 */


	if(!DB_interpreter_table.use_count())
		this->DB_interpreter_table= std::shared_ptr<itemEntry>(new itemEntry);

 if(DB_interpreter_table->sizeTable()==0) {
	//tableInsert(DB_interpreter_table.get(), "fafikLib_CON" );	///Compacted Object Notation
	DB_interpreter_table->key= "main interpreter table";	///Pointer Recursive Object Notation
	itemEntry* temp_ins[5];
		//fafikLib_PRON
	temp_ins[0] = tableInsertSubTable(DB_interpreter_table.get(), "fafikLib_PRON" );
	 temp_ins[1] = tableInsertSubTable(temp_ins[0], "assign" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "e", "=" );
	 temp_ins[1] = tableInsertSubTable(temp_ins[0], "key_value" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "key", "%u" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "e", "%a" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "value", "%u" );
	 temp_ins[1] = tableInsertSubTable(temp_ins[0], "table" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "enter" );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "key","%u" );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "e","%a" );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "e" );				//E
	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "e","{" );			//e {}
	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "auto_key","[" );	//auto_key []
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "bytes" );
	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "e","*" );
	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "bytes","%n s" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "exit" );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "e" );
	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "e", "}" );
	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "auto_key", "]" );
	 temp_ins[1] = tableInsertSubTable(temp_ins[0], "specialEntry" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "auto_key", "%auto_key%" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "comment", "", ie_auto_key );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "", "//", ie_auto_key_keyed );
	 temp_ins[1] = tableInsertSubTable(temp_ins[0], "elementMaxDistanceApart", "1" );
	 temp_ins[1] = tableInsertSubTable(temp_ins[0], "opensFiles", "", ie_auto_key );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "", "fafikLib_PRON", ie_auto_key_keyed );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "", "json", ie_auto_key_keyed );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "", "PRON", ie_auto_key_keyed );

		//ini files
	temp_ins[0] = tableInsertSubTable(DB_interpreter_table.get(), "INI" );
	 temp_ins[1] = tableInsertSubTable(temp_ins[0], "assign" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "e", "=" );
	 temp_ins[1] = tableInsertSubTable(temp_ins[0], "key_value" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "key", "%S" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "e", "%a" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "value", "%S" );
	 temp_ins[1] = tableInsertSubTable(temp_ins[0], "table" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "enter" );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "e","[" );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "key","%S" );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "e","]" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "exit" );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "EOL", "1" );
	 temp_ins[1] = tableInsertSubTable(temp_ins[0], "specialEntry" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "comment", "", ie_auto_key );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "", ";", ie_auto_key_keyed );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "", "#", ie_auto_key_keyed );
	 temp_ins[1] = tableInsertSubTable(temp_ins[0], "elementMaxDistanceApart", "1" );
	 temp_ins[1] = tableInsertSubTable(temp_ins[0], "opensFiles", "", ie_auto_key );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "","ini", ie_auto_key_keyed );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "","cfg", ie_auto_key_keyed );
 }
return true;
}
void fafikLib_DB::PrintTable(itemEntry* iePrint, size_t dent)
{
	wxString strDent;
	strDent.resize(dent, wxUniChar(' ') );
	for( auto currIe: iePrint->sub_itemTable ){

		printf( "%s = %s\n", (strDent+ currIe->key).ToStdString().c_str(), currIe->value.ToStdString().c_str() );
		if( currIe->isTable() ) PrintTable( currIe, dent+1 );

	}
}



itemEntry* fafikLib_DB::tableInsertSubTable(itemEntry *table, wxString key, wxString value, BYTE type_stores)
{
	if(!table) return nullptr;
	table->type_stores|= ie_Table;
	return table->Add_back( key, value, itemEntryStores(type_stores|ie_value) );
}

itemEntry* fafikLib_DB::tableInsertNextSubTable(itemEntry *table, wxString key, wxString value, BYTE type_stores)
{
	if(!table || !table->parent) return nullptr;
	return tableInsertSubTable( table->parent, key, value, type_stores );
}




unsigned short fafikLib_DB::ready_partForinterpreterOR(itemEntry *temp_tab, fastFind_struct *new_fF)
{
	new_fF->keys.resize(new_fF->keys.size()+1);
	///new key in "*new_fF"
	fastFind_struct_el* new_fF_ins= &new_fF->keys.back();
	if(!new_fF_ins->element_type){//	protection against overwrite
		if(temp_tab->key== "e") new_fF_ins->element_type=ff_e;
		else if(temp_tab->key== "key") new_fF_ins->element_type=ff_key;
		else if(temp_tab->key== "value") new_fF_ins->element_type=ff_value;
		else if(temp_tab->key== "bytes"){
	//printf("found Bytes....\n"); Sleep(1000);
			new_fF_ins->element_type=ff_bytes;
			new_fF->element_type= ff_bytes;
		}
		else if(temp_tab->key== "auto_key") new_fF_ins->element_type=ff_auto_key;
	//			else if(temp_tab->key =="#") new_fF_ins->element_type=6;
		else if(temp_tab->key== "v") new_fF_ins->element_type=ff_v;
		else if(temp_tab->key== "EOL"){
			new_fF_ins->element_type=ff_EOL;
			return 0;
		}
	}
	new_fF_ins->key= temp_tab->value;

	if( temp_tab->value.Length()>=2 && temp_tab->value.at(0)== wxUniChar('%')){
		if(temp_tab->value.StartsWith("%%") ){
			new_fF_ins->key= temp_tab->value.SubString(1,-1);
		}
		else {
			wxStringInputStream tempStringStream( temp_tab->value.substr(2) );
			if( temp_tab->value.StartsWith("%a") ){
				size_t temp_L= 0;
				tempStringStream>> temp_L;
				if( !(temp_L< curr_interpreter.ff_assign.size()) )
					temp_L= 0;
				new_fF_ins->key= ( curr_interpreter.ff_assign.at( temp_L ).keys.at(0).key );
				new_fF_ins->isAssign= true;
				new_fF->isAssign= true;
			} else {
				new_fF_ins->element_type|= ff_mode_perc;
				new_fF_ins->key= temp_tab->value.substr(0,2);
			}
			wxString tempSubS;
			tempStringStream>> tempSubS;
			while( tempSubS.size() ){
				if(tempSubS== "%a"){
					size_t temp_L= 0;
					tempStringStream>> temp_L;
					if( !(temp_L< curr_interpreter.ff_assign.size()) )
						temp_L= 0;
					new_fF_ins->key_stringArgs.push_back( curr_interpreter.ff_assign.at( temp_L ).keys.at(0).key );
				}
				else {
					if(new_fF_ins->key == "%k"){
						if(tempSubS== "space") tempSubS=" ";
						else if(tempSubS== "tab") tempSubS="\t";
						else if(tempSubS== "\\t") tempSubS="\t";
					}
					new_fF_ins->key_stringArgs.push_back(tempSubS);
					if(new_fF_ins->key== "%n" && !curr_interpreter.overwrite_byte_type){
						if(tempSubS== "l")
							curr_interpreter.overwrite_byte_type= 1;
						else if(tempSubS== "s")
							curr_interpreter.overwrite_byte_type= 0;
					}
					long temp_L=0;
					if( tempSubS.length()>0 && wxString(tempSubS).ToLong(&temp_L) ){
						new_fF_ins->args_L.push_back(temp_L);
					}
				}
				tempStringStream>> tempSubS;
			}
		}
	}

return new_fF_ins->element_type;
}
short fafikLib_DB::ready_partForinterpreter(itemEntry *temp_tab, wxVector<fastFind_struct> *put_here)
{
	if(!temp_tab)	return -1;
	if(!put_here)	return -2;

	for( size_t main_TabI=0; main_TabI< temp_tab->sizeTable(); ++main_TabI ){
		if(temp_tab->at(main_TabI)->type_stores & ie_sub_itemTable ){	//type_stores = sub_itemTable
			put_here->resize(put_here->size()+1);
			for(size_t temp_u=0; temp_u<temp_tab->at(main_TabI)->sizeTable(); temp_u++){
				if(temp_tab->at(main_TabI)->key=="bytes" && !curr_interpreter.bytesIn){
					curr_interpreter.bytesIn= true;
					ready_partForinterpreter(temp_tab->at(main_TabI), &curr_interpreter.ff_table_bytes);
					put_here->back().element_type= ff_bytes;
				}
				ready_partForinterpreterOR( temp_tab->at(main_TabI)->at(temp_u), &put_here->back() );
			}
		}
		else if( temp_tab->at(main_TabI)->type_stores & ie_string){	//type_stores = value -string
			put_here->resize(put_here->size()+1);
			ready_partForinterpreterOR( temp_tab->at(main_TabI), &put_here->back() );
		}
	}
	//checked & fixed to here, on version 2018-07-26-18.39
	//checked & fixed to here, on version 2018-07-28-21.32
return 0;
}

BYTE fafikLib_DB::ready_interpreter(itemEntry &interpreterTable_p)
{
	{
		interpreterStru new_cI;	//make a fresh table(for when changing interpreter type) 2018-08-06
		curr_interpreter= new_cI;
	}
	curr_interpreter.specialEntry=
	interpreterTable_p.find_linear("specialEntry", true);

	ready_partForinterpreter(
	 interpreterTable_p.find_linear("assign", true), &curr_interpreter.ff_assign
	);

	if(ready_partForinterpreter(
		interpreterTable_p.find_linear("lineTable", true), &curr_interpreter.ff_keyValue ) >=0 )
	{	//ok, found "lineTable"
		curr_interpreter.isLineTable = true;
	}
	else {	//try "key_value"
		ready_partForinterpreter( //getItem_byKey_rec(
		 interpreterTable_p.find_linear("key_value", true), &curr_interpreter.ff_keyValue );
	}


	curr_interpreter.entry_table=
	 interpreterTable_p.find_linear("table", true);

	if(curr_interpreter.entry_table){
		ready_partForinterpreter(
		 curr_interpreter.entry_table->find_linear("enter",true) , &curr_interpreter.ff_table_enter);
		ready_partForinterpreter(
		 curr_interpreter.entry_table->find_linear("exit", true) , &curr_interpreter.ff_table_exit );
	}

	for(size_t temp_u=0; temp_u<curr_interpreter.ff_keyValue.size(); temp_u++){	//and entries
		fastFind_struct &temp_tab_u= curr_interpreter.ff_keyValue.at(temp_u);
		if(temp_tab_u.isAssign) {
			curr_interpreter.isAssignAt_ff_keyValue= temp_u;
			break;
		}
	}
	for(size_t temp_u=0; temp_u<curr_interpreter.ff_table_enter.size(); temp_u++){	//and entries
		fastFind_struct &temp_tab_u= curr_interpreter.ff_table_enter.at(temp_u);
		if(temp_tab_u.isAssign) {
			curr_interpreter.isAssignAt_ff_keyValue= temp_u;
			break;
		}
	}



	itemEntry* temp_prior=
	 interpreterTable_p.find_linear("priority", true);
	curr_interpreter.recognitionPriority.push_back("");
	curr_interpreter.recognitionPriority.clear();
	curr_interpreter.recognitionPriority.resize(3);
	if(temp_prior){
		for(size_t tempP=0; tempP<3; tempP++){
			curr_interpreter.recognitionPriority[tempP]= temp_prior->sub_itemTable.at(tempP)->value;
		}
	} else {
		curr_interpreter.recognitionPriority[0]= "exit";	//mostly only 1 char
		curr_interpreter.recognitionPriority[1]= "enter";	//complicated multi detection
		curr_interpreter.recognitionPriority[2]= "key";		//assignment key, val

	}

	itemEntry* temp_MaxApart=
	 interpreterTable_p.find_linear("elementMaxDistanceApart", true);
	if(temp_MaxApart){
		long temp_L= 0;
		if(temp_MaxApart->value.ToLong( &temp_L ) ){
			curr_interpreter.elementMaxDistanceApart= abs(temp_L);
		}
	}
return 0;
}


BOOL fafikLib_DB::CreateDirTree(const wxString& str_path, LPSECURITY_ATTRIBUTES attribs)
{
	size_t posBS=2;
	BOOL ret_val= ERROR_PATH_NOT_FOUND;
	do{
		posBS= str_path.find_first_of("/\\", posBS+1);
		wxString str_DirPath= str_path.substr(0, posBS);
		replaceAll(str_DirPath, "/", "\\");
		if( !wxDirExists(str_DirPath) ) {
			if( str_DirPath.IsSameAs("\\\\?\\") )
				continue; //skip "\\?\" ... "Drive:\"
			ret_val= CreateDirectoryW(str_DirPath.ToStdWstring().c_str(), attribs);
			if( !wxDirExists(str_DirPath) ) {
				break;
			}
		}
	} while( posBS!= wxString::npos );
	return ret_val;
}

BYTE fafikLib_DB::Open(const wxString& file_path, modes_IO mode, const wxString overwriteInterpreter, settings *useSettings, bool clearsPrevious)
{
	this->Close();
	if(clearsPrevious) this->cleanup_DB();

	if(file_path.empty()) return 1;
	if( !wxFileExists(file_path) ){
		size_t dir_to= file_path.find_last_of( "\\/" );
		if( dir_to!= invalid_size ){
			CreateDirTree( file_path.substr(0, dir_to ) );
		}
		if( wxDirExists(file_path) ) return 1;

	// TODO (efficiency#9#09/16/18): check if file_path || file_path.new || file_path.old before creating file_path
		wxFFile tmpzs;
		tmpzs.Open( file_path, "w" );
		tmpzs.Close();
	}
	if(!DB_fileIO){
		DB_fileIO= new UniFileStream_file;
		if(DB_fileIOLine) delete DB_fileIOLine;
	}
	DB_fileIO->Open(file_path, "rb" );
	DB_fileIOLine= DB_fileIO;
	if( !DB_fileIO->IsOpened() || DB_fileIO->Error() ) return 1;
	P_DB_FileName= file_path;

	if(useSettings) UseSettings(*useSettings);
	UseInterpreter(overwriteInterpreter);
	size_t pos_nameExtSep= file_path.find_last_of('.');
	if( pos_nameExtSep!= invalid_size ){
		wxString str_Ext= file_path.substr(pos_nameExtSep+1);
		if(!thoseDB_settings.useInterpreter){	//find me an interpreter
			UseInterpreterAuto(str_Ext);
		}
	} else {	//just allowing for file name without '.ext', if there is such a need
		if(!thoseDB_settings.useInterpreter){
			UseInterpreterAuto(wxEmptyString);
		}
	}
	if(!thoseDB_settings.useInterpreter) return 2;
	ready_interpreter(*thoseDB_settings.useInterpreter);
return 0;
}
BYTE fafikLib_DB::Open(const wxString& file_path, const overrideInterpreter& useIterpr ,modes_IO mode, bool clearsPrevious )
{
	this->Close();
	if(clearsPrevious) this->cleanup_DB();

	if(file_path.empty()) return 1;
	if( !wxFileExists(file_path) ){
		size_t dir_to= file_path.find_last_of( "\\/" );
		if( dir_to!= invalid_size ){
			CreateDirTree( file_path.substr(0, dir_to ) );
		}
		if( wxDirExists(file_path) ) return 1;

		wxFFile tmpzs;
		tmpzs.Open( file_path, "w" );
		tmpzs.Close();
	}
	if(!DB_fileIO){
		DB_fileIO= new UniFileStream_file;
		if(DB_fileIOLine) delete DB_fileIOLine;
	}
	DB_fileIO->Open(file_path, "rb" );
	DB_fileIOLine= DB_fileIO;
	if( !DB_fileIO->IsOpened() || DB_fileIO->Error() ) return 1;
	P_DB_FileName= file_path;


	//clear interpreter pointer
	if( useIterpr.which ) thoseDB_settings.useInterpreter= nullptr;

	if(useIterpr.which== 1){
		UseInterpreter( useIterpr.over_string );
	}
	else if(useIterpr.which== 2){
		thoseDB_settings.useInterpreter= DB_interpreter_table->at( useIterpr.over_index );
	}
	else if( useIterpr.which== 3 ){
		size_t pos_nameExtSep= file_path.find_last_of('.');
		if( pos_nameExtSep!= invalid_size ){
			UseInterpreterAuto( file_path.substr( pos_nameExtSep+1 ) );
		}
	}
	else if( useIterpr.which== 4 ){
		UseInterpreterAuto( useIterpr.over_string );
	}

	//make interpreter
	if( useIterpr.which ){
		if( thoseDB_settings.useInterpreter )
			ready_interpreter(*thoseDB_settings.useInterpreter);
		else
			return 2;	//no interpreter
	}

return 0;
}
BYTE fafikLib_DB::Open(UniFileStream_base* BaseIO, const wxString& UseInterpr, bool clearsPrevious)
{
	this->Close();
	if(clearsPrevious) this->cleanup_DB();
	if(DB_fileIO){
	  //we need to invalidate the other way around
		delete DB_fileIO;
		DB_fileIO= nullptr;
	}
	DB_fileIOLine= BaseIO;

	if( UseInterpreter(UseInterpr) ) //not a name..
		UseInterpreterAuto(UseInterpr); //..try .ext

	if(!thoseDB_settings.useInterpreter) return 2;
	ready_interpreter(*thoseDB_settings.useInterpreter);
return 0;
}

BYTE fafikLib_DB::OpenAgain()
{
	this->Close();
	this->cleanup_DB();

	if( P_DB_FileName.empty() || !wxFileExists(P_DB_FileName) ) return -1;

	if(DB_fileIO){
		DB_fileIO->Open( P_DB_FileName, "rb" );
		DB_fileIOLine= DB_fileIO;
		if( !DB_fileIO->IsOpened() || DB_fileIO->Error() ) return -1;
		_NextLine_iE= nullptr;	//we dont want access violation
	}

return 0;
}

bool fafikLib_DB::interpretEnd()
{
	if(DB_fileIOLine){
		UniFileStream_EOL eol= DB_fileIOLine->getEol();
		bool retVal= DB_fileIOLine->Close(); //this would reset EOL, so we need to copy it
		DB_fileIOLine->useEOL(eol);
		return retVal;
	}
	return 0;
}
bool fafikLib_DB::Close()
{
	if(DB_fileIO) DB_fileIO->Close();
	if(DB_fileIOLine){
		DB_fileIOLine->resetEol();  //file changes, forget old EOL
		DB_fileIOLine->resetLine(); //file changes, forget old readLine buffer
	}

	_NextLine_iE= nullptr; //cant read file, cant add entries(also we will leave it as was, it might not have been finished)

	if(DB_file_write && DB_file_write->IsOpened()) DB_file_write->Close();
return 1;
}

BYTE fafikLib_DB::cleanup_DB()
{
	cleanup_bigFiles();
	if(DB_table.sizeTable()== 0) return 1;
	DB_table.FreeTable();

return 0;
}
BYTE fafikLib_DB::cleanup_bigFiles()
{
	//	if( ByteData_itemEntry_all.empty() ) return 1;
	//	ByteData_itemEntry_all.clear();
return 0;
}

bool fafikLib_DB::WriteToFile_sameFileForced()
{
	wxLogNull noLog;
	const wxString temp_fileName= DB_fileIOLine->GetName();
	if( wxFileExists(temp_fileName) && !wxIsWritable( temp_fileName ) ){
		return 0;
	}
	DB_fileIOLine->reClose();
	UniFileStream_file realFile;
	realFile.Open( temp_fileName, wxString("w+b") );
	DB_file_write= &realFile;

  if( (!DB_file_write->IsOpened()) || DB_file_write->Error() ){ //error
		DB_fileIOLine->reOpen();
		DB_file_write= nullptr;
		return 0;
  }
  else { //ok
	WritePToFile();
	DB_file_write->Flush();
	DB_file_write->Close();
	DB_file_write= nullptr;
  }
	DB_fileIOLine->reOpen();
	DB_fileIOLine->Seek(0);
	return 1;
}
int fafikLib_DB::WriteTo(UniFileStream_baseWritable& stream, itemEntry *IE_saveCustom)
{
	 ///save ptr to restore it later
	auto saved= DB_file_write;
	DB_file_write= &stream;
	auto retVal= WritePToFile(IE_saveCustom);
	DB_file_write= saved;
	return retVal;
}
int fafikLib_DB::WriteToFile()
{
	if( thoseDB_settings.OpenMode== mode_append ) return WriteToFile_append();
	return WriteToFile_new();
}
int fafikLib_DB::WriteToFile_interpreter( wxString filePathName )
{
	if( wxFileExists(filePathName) && !wxIsWritable( filePathName ) ) return 0;
	fafikLib_DB now_interpreter(this->DB_interpreter_table);
	now_interpreter.Open( filePathName );

	interpreterStru keep_interpreter= curr_interpreter;
	curr_interpreter= now_interpreter.curr_interpreter;

	now_interpreter.Close();	//just to be safe

	UniFileStream_file realFile;
	realFile.Open( filePathName, "w+b" );
	DB_file_write= &realFile;

	int ret_ok= WritePToFile( DB_interpreter_table.get() );
	DB_file_write->Flush();
	DB_file_write->Close();
	DB_file_write= nullptr;

	curr_interpreter= keep_interpreter;
return ret_ok;
}
int fafikLib_DB::WriteToFile_new()
{
	wxLogNull noLog;
	const wxString temp_fileName= DB_fileIOLine->GetName();

	UniFileStream_file realFile;
	realFile.Open( temp_fileName+ ".new", "w+b" );
	DB_file_write= &realFile;

  if( (!DB_file_write->IsOpened()) || DB_file_write->Error()){
		DB_file_write->Close();
		DB_file_write= nullptr;
		wxRemoveFile( temp_fileName+ ".new" );
		return 1;
  } else {
	int ret_ok= WritePToFile();
	if( !ret_ok ){
		DB_file_write->Flush();
		DB_file_write->Close();
		DB_file_write= nullptr;
		DB_fileIOLine->reClose();
		wxRenameFile(temp_fileName, temp_fileName+".old");
		wxRenameFile(temp_fileName+ ".new", temp_fileName);
		DB_fileIOLine->reOpen();
		DB_fileIOLine->Seek(0);
	}
  }
return 0;
}
int fafikLib_DB::WriteToNewFile( wxString filePathName )
{
	if( wxFileExists(filePathName) && !wxIsWritable( filePathName ) ) return 0;

	UniFileStream_file realFile;
	realFile.Open( filePathName, "w+b" );
	DB_file_write= &realFile;
	int ret_ok= WritePToFile();
	DB_file_write= nullptr;
return ret_ok;
}
int fafikLib_DB::WriteToFile_append()
{
	if( !wxIsWritable( DB_fileIOLine->GetName() ) ) return 0;

	UniFileStream_file realFile;
	realFile.Open( DB_fileIOLine->GetName(), "ab" );
	DB_file_write= &realFile;
	int ret_ok= WritePToFile();
	DB_file_write= nullptr;
return ret_ok;
}
int fafikLib_DB::WritePToFile(itemEntry *IE_saveCustom)
{
	if( (!DB_file_write->IsOpened()) || DB_file_write->Error() ) return 0;
	if(!IE_saveCustom) IE_saveCustom= &DB_table;
	for(size_t temp_u=0; temp_u<IE_saveCustom->sizeTable(); temp_u++){
		WritePToFile_rec( IE_saveCustom->at(temp_u) );
	}

	DB_file_write->Flush(); //dont close here
return 1;
}
int fafikLib_DB::WritePToFile_rec(itemEntry *item, size_t depthSpaces)
{
	if(!item) return 0;
 if( item->isLink() ){
	//printf(" isLink; not implemented yet\n");
 }
 else if( item->type_stores & ie_lore ){
	DB_file_write->Write( item->value );
	//2018-10-21 ( DB_fileIOLine.getEOL() ) was put in
	DB_file_write->Write( DB_fileIOLine->getEolRaw(), DB_fileIOLine->getEolSize() );
 }
 else if( item->isTable() ){
 	wxString preSpace; preSpace.Pad(depthSpaces, thoseDB_settings.onWriteLine_biginWithAutoIndent.at(0) );
	DB_file_write->Write(preSpace);
	for(size_t temp_u=0; temp_u<curr_interpreter.ff_table_enter.size(); temp_u++){	//and entries
		if( item->isAuto_key_keyed() &&
			curr_interpreter.isAssignAt_ff_table_enter>=0 &&
			temp_u<= size_t(curr_interpreter.isAssignAt_ff_table_enter) ){
				continue;	//searching for `=`
		}
		else if( !item->isWithBytes() && ((curr_interpreter.ff_table_enter.at(temp_u).element_type &(ff_mode_perc-1))== ff_bytes) ){
			//for now, solution
			//skip bytes if not isWithBytes()
			continue;
		}
		fastFind_struct &temp_tab_and= curr_interpreter.ff_table_enter.at(temp_u);
		DB_file_write->Write( WritePToFile_rLine(temp_tab_and, item) );
	}
	DB_file_write->Write( DB_fileIOLine->getEolRaw(), DB_fileIOLine->getEolSize() );


	for(size_t temp_u=0; temp_u<item->sizeTable(); temp_u++){	//and entries
		WritePToFile_rec( item->at(temp_u), depthSpaces+1 );
	}

	if(item->type_stores & ie_TableNotClosed){
		//actually nothing.
	} else {
		//here write ending
		for(size_t temp_u=0; temp_u<curr_interpreter.ff_table_exit.size(); temp_u++){	//and entries
			fastFind_struct &temp_tab_and= curr_interpreter.ff_table_exit.at(temp_u);
			wxString preSpace; preSpace.Pad(depthSpaces, thoseDB_settings.onWriteLine_biginWithAutoIndent.at(0) );
			DB_file_write->Write(preSpace);

			DB_file_write->Write( WritePToFile_rLine(temp_tab_and, item) );
			DB_file_write->Write( DB_fileIOLine->getEolRaw(), DB_fileIOLine->getEolSize() );
		}
	// TODO (GB_DB#9#08/17/18): if (GB_DB) get rid of done tables
	}
 }
 else if( item->isValue() ){
 	wxString preSpace; preSpace.Pad(depthSpaces, thoseDB_settings.onWriteLine_biginWithAutoIndent.at(0) );
	DB_file_write->Write(preSpace);
	for(size_t temp_u=0; temp_u<curr_interpreter.ff_keyValue.size(); temp_u++){	//and entries
		if( item->isAuto_key_keyed() &&
			curr_interpreter.isAssignAt_ff_keyValue>=0 &&
			temp_u<= size_t(curr_interpreter.isAssignAt_ff_keyValue) ){
				continue;	//searching for `=`
		}
		fastFind_struct &temp_tab_and= curr_interpreter.ff_keyValue.at(temp_u);
		DB_file_write->Write( WritePToFile_rLine(temp_tab_and, item) );
	}
	DB_file_write->Write( DB_fileIOLine->getEolRaw(), DB_fileIOLine->getEolSize() );
 }

return 1;
}
wxString fafikLib_DB::WritePToFile_rLine(fastFind_struct &use_parser, itemEntry *item, bool on_ff_bytes )
{
	wxString ret_writeStr;
	if( !on_ff_bytes && (use_parser.element_type &(ff_mode_perc-1))== ff_bytes ){
	//		for( curr_interpreter.ff_table_bytes )
	//		WritePToFile_rLine( curr_interpreter.ff_table_bytes, item );
	}

	size_t temp_or;
	for(size_t temp_uu=0; temp_uu<use_parser.keys.size(); temp_uu++){	//or entries
		 ///reversed from temp_uu
		temp_or= (use_parser.keys.size()-1) -temp_uu;
		fastFind_struct_el &temp_tab_or= use_parser.keys.at(temp_or);
		const BYTE uu_elType= temp_tab_or.element_type &(ff_mode_perc-1);

		if( item->isAuto_key() && uu_elType!= ff_auto_key && temp_or!= 0 ) continue;

		if( uu_elType== ff_e ){
			ret_writeStr= temp_tab_or.key;
		} else if( uu_elType== ff_key ){
			ret_writeStr= item->key;
		} else if( uu_elType== ff_value ){
			if( item->isValue() )
				ret_writeStr= item->value;
			else continue;
		} else if( uu_elType== ff_bytes && item->isWithBytes() ){
			//bytes
		} else if( uu_elType== ff_auto_key ){
			if( item->isAuto_key() )
				ret_writeStr= temp_tab_or.key;
			else continue;
		} else if( uu_elType== ff_hashTable ){
wxLogError("# hash table is not yet...");
			return "";
		} else if( uu_elType== ff_v ){
			return "";
		} else if( uu_elType== ff_EOL ){
			return "";
		}


		if(temp_tab_or.element_type & ff_mode_perc){
			if(temp_tab_or.key== "%u"){
				QuoteType Quote= QuoteDouble;
				if(temp_tab_or.key_stringArgs.size()> 0){
					if(temp_tab_or.key_stringArgs.Item(0)== "\"") Quote= QuoteDouble;
					else if(temp_tab_or.key_stringArgs.Item(0)== "\'") Quote= QuoteSingle;
				}
				return Quotation::escapeString( ret_writeStr, Quote );
			}
			else if(temp_tab_or.key== "%S"){
				return ret_writeStr;
			}
			else if(temp_tab_or.key== "%k"){
				if(temp_tab_or.key_stringArgs.size()>0) ret_writeStr.Append( temp_tab_or.key_stringArgs.Item(0) );
				return ret_writeStr;
			}
			else if(temp_tab_or.key== "%n"){
				//write bytes
//				solve it with log2?		//nope log2 is gonna take longer 2018-08-16
//				or is better with for(8) in static_cast<BYTE> &0xFF != 0
			}
		}
		else {
			return ret_writeStr;
		}
	}//for
return wxEmptyString;
}



BYTE fafikLib_DB::UseSettings(settings &useSettings)
{
	this->thoseDB_settings= useSettings;
return 0;
}
void fafikLib_DB::UseEol(const UniFileStream_EOL& useEol_)
{
	if(DB_fileIOLine){
		DB_fileIOLine->useEOL(useEol_);
	}
}
BYTE fafikLib_DB::UseInterpreter(const wxString& interName)
{
	if( interName.empty() ) return -2;
	itemEntry* temp_ie1= DB_interpreter_table->findP(interName, ie_Table, true);
	if( temp_ie1 ){
		thoseDB_settings.useInterpreter= temp_ie1;
	} else {
		return -1;
	}
return 0;
}
BYTE fafikLib_DB::UseInterpreterAuto(const wxString& byFileExt)
{
	wxString byFileExtLocal(byFileExt.Lower());
	while(byFileExtLocal.starts_with(".")){
		byFileExtLocal.Remove(0);
	}
	bool exitsNow= false;
	for(size_t temp_inter=0; temp_inter<DB_interpreter_table->sizeTable(); temp_inter++){
		if(exitsNow) break;
		//contains types {fafikLib_PRON, lua tables, java tables}
		itemEntry *temp_ie1= DB_interpreter_table->at(temp_inter)->find_linear("opensFiles", true);
		if(temp_ie1){
			if(temp_ie1->isValue() && !temp_ie1->isTable() ){
				if(byFileExtLocal.IsSameAs( temp_ie1->value, false) ){
					thoseDB_settings.useInterpreter= DB_interpreter_table->at(temp_inter);
					exitsNow= 1;
					break;
				}
			}
			for(size_t temp_ext=0; temp_ext<temp_ie1->sizeTable(); temp_ext++){
				if(byFileExtLocal.IsSameAs( temp_ie1->at(temp_ext)->value, false) ){
					thoseDB_settings.useInterpreter= DB_interpreter_table->at(temp_inter);
					exitsNow= 1;
					break;
				}
			}
		}
	}

if(!thoseDB_settings.useInterpreter) return -1;
return 0;
}

int fafikLib_DB::loadInterpreterFile(UniFileStream_base* item, const wxString& overwriteInterpreter)
{
	if(!item) return -1;
	fafikLib_DB temp_DB_interFile(this->DB_interpreter_table);
	BYTE tempB= temp_DB_interFile.Open(item, overwriteInterpreter);
	if(tempB) return -1;
  //actual interpret here
	temp_DB_interFile.interpretFile(false);
  //copy over all new interpreters
	int retAdded= 0;
	for(size_t temp_u=0; temp_u<temp_DB_interFile.DB_table.sizeTable(); temp_u++){
		itemEntry *retItem= itemEntry_getExistOrNew( *(DB_interpreter_table.get()), *temp_DB_interFile.DB_table.at(temp_u) );
		if(retItem){
		  //item already exists, dont copy
		}
		else {
		  //new item, copy over
			retAdded++;
			this->DB_interpreter_table->push_back_cpyTotal( *temp_DB_interFile.DB_table.at(temp_u) );
		}
	}

return retAdded;
}
int fafikLib_DB::loadInterpreterFile(const wxString& file_path, const wxString& overwriteInterpreter)
{
	if(file_path.empty()) return -1;
  //open file and convert to loadInterpreterFile()
	UniFileStream_file iFromFile(file_path, true);
	return loadInterpreterFile(&iFromFile, overwriteInterpreter);
}
bool fafikLib_DB::itemEntry_isSame(const itemEntry& item_a, const itemEntry& item_b)
{
    if((item_a.key== item_b.key) &&
		(item_a.type_stores== item_b.type_stores) &&
			(item_a.value== item_b.value)
	){
		return true;
	}
return false;
}
itemEntry *fafikLib_DB::itemEntry_getExistOrNew(wxVector<itemEntry*> &items_a, itemEntry &item_b)
{
	for(size_t temp_tu=0; temp_tu<items_a.size(); temp_tu++){
		if( itemEntry_isSame( *items_a.at(temp_tu), item_b) )
			return items_a.at(temp_tu);
	}
	return nullptr;
}
itemEntry *fafikLib_DB::itemEntry_getExistOrNew(itemEntry &Table_items_a, itemEntry &item_b)
{
	if(Table_items_a.isTable())
		return itemEntry_getExistOrNew(Table_items_a.sub_itemTable, item_b);
	return nullptr;
}


LONGLONG fafikLib_DB::interpretFile(bool Sort)
{
	 // new code  2018-08-08
	DB_table.type_stores|= ie_Table;
	interpretStart();	//start interpreter
	while( interpretNextLine()== 0 );	//while load all lines
	_NextLine_iE= nullptr; //just in case...
	if(Sort)
		DB_table._sort(true);	//re sort after
return 0;
}
bool fafikLib_DB::interpretStart(itemEntry* Table)
{
	if(DB_fileIOLine->IsOpened()){
		_NextLine_iE= Table;
		return true;
	}
return false;
}


short fafikLib_DB::dumpToFile(itemEntry* entry)
{
	if(!entry) return -2;
	temp_oFile.Open("DB_To_File.n+","w+b");
	if(temp_oFile.IsOpened()){
		dumpToFileRec(entry);
		temp_oFile.Flush();
		temp_oFile.Close();
		return 0;
	}
return -1;
}

short fafikLib_DB::dumpToFileRec(itemEntry* entry, int indent)
{

	wxString temp_str,temp_strAdd;
	for(int temp_i=0; temp_i < indent; temp_i++){
		temp_str+=" ";
	}
	temp_str.Replace("    ","\t");

	temp_strAdd=temp_str+Quotation::escapeString(entry->key);//entry->key;
	temp_oFile.Write(temp_strAdd);
	if( entry->isLink() ){
		//handle links
	//dumpToFileRec( entry->get(), indent+1 );
	} else if(entry->isTable() ){
		temp_oFile.Write("={\n");
		for(size_t temp_u=0; temp_u < entry->sub_itemTable.size(); temp_u++){
			dumpToFileRec( entry->sub_itemTable.at(temp_u),indent+1);
		}
		temp_strAdd=temp_str+"}\n";
		temp_oFile.Write(temp_strAdd);
	} else if(entry->isValue() ){
		temp_oFile.Write("=");
		temp_strAdd= Quotation::escapeString(entry->value);
		temp_oFile.Write(temp_strAdd);
		temp_oFile.Write("\n");
	}
return 0;
}

wxStringVector fafikLib_DB::translate_StrToArr(const wxString& findKey_levelRecursive_str, const wxUniChar separator)
{
	size_t pos_dot= 0, pos_dot_last= 0;
	wxStringVector findKey_levelRecursive;
	while(pos_dot != invalid_size ){
		pos_dot_last= pos_dot;
		pos_dot= findKey_levelRecursive_str.find(separator,pos_dot_last);
		if(pos_dot_last == invalid_size)
			break;
		else {
			findKey_levelRecursive.push_back( findKey_levelRecursive_str.SubString(pos_dot_last,pos_dot-1) );
			if( pos_dot== invalid_size)
				break;
			pos_dot++;
		}
	}
return findKey_levelRecursive;
}


 /// TODO (main#9#02/13/24): Quotation is called multiple times (should shave off some calls)
bool fafikLib_DB::interpretNextLine_TLine(wxString &in_line, fastFind_struct *fFs_AndEntry_in, wxVector<makesSearch> &lineInterpreterResults, size_t &pos_last, itemEntry &AdditionalTableInfo, wxString AdditionalStrInfo)
{
	bool CanExitLoopOr= false;
	size_t iMDA= curr_interpreter.elementMaxDistanceApart;
	if(pos_last== 0) iMDA= 0;

	if( !lineInterpreterResults.empty() && lineInterpreterResults.back().pos_to > in_line.Length() )
		iMDA= in_line.Length();
	 ///moved up 2024-02-13
	makesSearch mkS;
	 ///unquoted string
	wxString extracted;
	for(size_t temp_uu=0; temp_uu<fFs_AndEntry_in->keys.size(); temp_uu++){	//or subEntries
		if(CanExitLoopOr) break;
		fastFind_struct_el* temp_tab_uu= & fFs_AndEntry_in->keys.at(temp_uu);
		const BYTE uu_elType= temp_tab_uu->element_type &(ff_mode_perc-1);

		mkS.pos_to= 0; //moved before loop 2024-02-13
		mkS.interpreted_value.clear();
		mkS.interpreter_key= temp_tab_uu->key;
		mkS.pos_from= pos_last;
		mkS.element_type= temp_tab_uu->element_type;
		if(temp_tab_uu->element_type & ff_mode_perc){
			if(temp_tab_uu->key == "%u"){
	//				list key
	//					which quote?		" '
	//					unquote of quote	" '
	//				list long
	//					which error?, all by default
				bool singleQuote = 0;
				QuoteType remQuot= QuoteUnspecified;
				if(temp_tab_uu->key_stringArgs.size()>0){
					singleQuote= (temp_tab_uu->key_stringArgs.Item(0) == "\'");
				}
				if(temp_tab_uu->key_stringArgs.size()>1 && temp_tab_uu->key_stringArgs.Item(1)=="\"") remQuot= DoubleQuote;
				if(temp_tab_uu->key_stringArgs.size()>1 && temp_tab_uu->key_stringArgs.Item(1)=="\'") remQuot= QuoteSingle;

				Quotation temp_unqote( in_line, singleQuote, pos_last, remQuot );
//				wxString extracted;
				bool isok= 0;

				for(BYTE i= 0; i!= 2; i++){
					BYTE ret_extracted = temp_unqote.extractNext( &extracted );
					//any +number, except (1)
					if(temp_tab_uu->args_L.empty() && ret_extracted >= BYTE(0) && ret_extracted!= BYTE(1) && ret_extracted <= BYTE(127) ){
						isok=true;
					} else {
						for(size_t check_L=0; check_L<temp_tab_uu->args_L.size(); check_L++){
							if(ret_extracted == BYTE(temp_tab_uu->args_L.Item(check_L) ) ){
								isok=true;
								break;
							}
						}
					}
					//is not ok, not total Error, in range of iMDA
					if(!isok && ret_extracted<= BYTE(127) && extracted.Length()<= iMDA ){
						continue; //has a chance
					} else break;
				} // 2 times extract
				if(isok){
					CanExitLoopOr= isok;
					pos_last = temp_unqote.getPosStartNext();
					mkS.interpreted_value = extracted;
					mkS.pos_to = pos_last;
				}
			}
			else if(temp_tab_uu->key == "%S"){
				mkS.pos_to = -1;
				pos_last ++; //just in case increase it by 1
				CanExitLoopOr = true;
				mkS.interpreted_value = in_line.substr(mkS.pos_from);
			}
			else if(temp_tab_uu->key == "%k"){
				size_t pos_lowest= -1;
				for(size_t keyArg=0; keyArg<temp_tab_uu->key_stringArgs.size(); keyArg++){
					size_t pos_lowest_temp= find_RestrictedString( in_line, temp_tab_uu->key_stringArgs.Item(keyArg), pos_last, pos_lowest );
					if(pos_lowest_temp < pos_lowest) pos_lowest = pos_lowest_temp;
					if(pos_lowest==0) break;
				}
				if(pos_lowest != invalid_size || temp_tab_uu->key_stringArgs.empty() ){//if found or will find(hopefully)
					CanExitLoopOr= true;
					mkS.pos_to= pos_lowest;
					mkS.interpreted_value= in_line.substr(mkS.pos_from, mkS.pos_to );
				}
			}
			else if(temp_tab_uu->key == "%#"){
				wxLogError("hash table not yet implemented... sorry");
			}
			else if(temp_tab_uu->key == "%n"){	// used in bytes
				size_t temp_pos1= in_line.find_first_of( _digits_list, pos_last );
				if(temp_pos1> pos_last+iMDA){
					if( temp_pos1==invalid_size && !temp_tab_uu->args_L.empty()){
						CanExitLoopOr= true;
						mkS.interpreted_value= atos::toWxString(temp_tab_uu->args_L.Item(0));
						mkS.pos_from= pos_last;
						mkS.pos_to= pos_last;
						pos_last++;
					}
					else
						break;
				}
				size_t temp_pos2= in_line.find_first_not_of( _digits_list, temp_pos1);
				wxString temps_strSB= in_line.SubString( temp_pos1, temp_pos2-1 );
				long long temp_LL=0;
				if( temps_strSB.ToLongLong(&temp_LL) ){
					CanExitLoopOr= true;
					pos_last= temp_pos2;	//after last digit
					mkS.interpreted_value= atos::toWxString(temp_LL);
					mkS.pos_from= temp_pos1;
					mkS.pos_to= temp_pos2;
					if(temp_pos2>in_line.Length()) mkS.pos_to= in_line.Length()+1;
				}
			}
		} //temp_tab_uu->element_type & ff_mode_perc
		else if(uu_elType == ff_EOL){	//simple check if EOL
			iMDA= 0;
			if(!temp_tab_uu->args_L.empty()) iMDA= abs( temp_tab_uu->args_L.Item(0) );
			if( pos_last>= (in_line.Length()- iMDA) ) CanExitLoopOr=true;
			else if( !lineInterpreterResults.empty() && lineInterpreterResults.back().pos_to >= in_line.Length() ) CanExitLoopOr=true;
		}
		else {
			size_t foundOn = find_RestrictedString(in_line, temp_tab_uu->key, false, pos_last, pos_last+ temp_tab_uu->key.Length() +iMDA);
			if( (foundOn<= pos_last+iMDA) &&
				//can not find `"` between, or its useless
				find_RestrictedString(in_line, "\"", false, pos_last, foundOn)== invalid_size
			  ){
				CanExitLoopOr = true;
				mkS.interpreted_value = temp_tab_uu->key;
				mkS.pos_from = foundOn;
				mkS.pos_to = foundOn + temp_tab_uu->key.Length();
				pos_last= foundOn + temp_tab_uu->key.Length();
			}

		}

		if( uu_elType == ff_v){ //voided, so even if false true
			CanExitLoopOr= true;
		}

		if(!lineInterpreterResults.empty()){
			makesSearch &lastSearch= lineInterpreterResults.back();
			if(lastSearch.pos_to == invalid_size){
				lastSearch.pos_to = mkS.pos_from;
				lastSearch.interpreted_value = in_line.substr(lastSearch.pos_from, lastSearch.pos_to - lastSearch.pos_from );
			}
		}

		if(CanExitLoopOr){
			//after successful "or" find, precede...
			lineInterpreterResults.push_back(mkS);	//save this "or" iteration
			return CanExitLoopOr;
		}
	}	//or subEntries

	//does not match the search
	return CanExitLoopOr;
}


itemEntry* fafikLib_DB::interpretNextLine_TEnter(itemEntry* Table, wxString in_line, const bool useAutoKey)
{
	wxVector<makesSearch> lineInterpreterResults;
	itemEntry_Bytes* optionBytes= nullptr;

	size_t pos_last=0;

	for(size_t temp_u=0; temp_u<curr_interpreter.ff_table_enter.size(); temp_u++){	//and entries
		if(useAutoKey &&
			temp_u<= size_t(curr_interpreter.isAssignAt_ff_table_enter) ) continue;
		fastFind_struct* temp_tab_u =& curr_interpreter.ff_table_enter.at(temp_u);

		if( (temp_tab_u->element_type &(ff_mode_perc-1)) == ff_bytes){
			optionBytes= interpretNextLine_TBytes(in_line, Table, lineInterpreterResults, pos_last );//all origin (by reference)
			continue;
		}
		bool CanExitLoopOr= interpretNextLine_TLine(in_line, temp_tab_u, lineInterpreterResults, pos_last, *Table);

		if( !CanExitLoopOr ){
			if(optionBytes){	//restore DB_fileIO-position
				delete optionBytes;
				optionBytes= nullptr;
			}

			if( !useAutoKey && Table->isAuto_key() ){
				if( curr_interpreter.isAssignAt_ff_table_enter< 0 ) return nullptr;
				return interpretNextLine_TEnter( Table, in_line, true );
			}
			return nullptr;
		}
	}//for	//and entries
	 //after all "and" entries, precede to combine & return*
	if(useAutoKey && lineInterpreterResults.empty())
		return nullptr;

	// @note (main#9#07/26/18): now combining all the information of finds
	 ///assign optionBytes if exists, else make new
	itemEntry &temp_ie= optionBytes ? *optionBytes : *(new itemEntry);

	if(optionBytes && optionBytes->Length()!= 0)
		DB_fileIOLine->Seek( optionBytes->pos_end );

	temp_ie.type_stores|= ie_Table;
	temp_ie.depth= Table->depth+1;
	temp_ie.parent= Table;
	for(size_t LIR=0; LIR<lineInterpreterResults.size(); LIR++){
		makesSearch &p_LIR= lineInterpreterResults.at(LIR);
		BYTE ElType= (p_LIR.element_type &(ff_mode_perc-1));

		//from highest to lowest
		if(ElType == ff_v){}
		else if(ElType == ff_hashTable){
			wxLogMessage("# hash table not in yet.");
		}
		else if(ElType == ff_auto_key){
			temp_ie.type_stores|= ie_auto_key;
		}
		else if(ElType == ff_bytes){
			wxLogMessage("bytes WIP.");
		}
		else if(ElType == ff_value){
			temp_ie.type_stores|= ie_value;
			temp_ie.value+= p_LIR.interpreted_value;
		}
		else if(ElType == ff_key){
			temp_ie.key+= p_LIR.interpreted_value;
		}
		else if(ElType == ff_e){
			//printf("e empty WIP.");
		}
	}

	Table->sub_itemTable.push_back(&temp_ie);
	return Table->sub_itemTable.back(); //ok at this point
}
itemEntry* fafikLib_DB::interpretNextLine_TExit(itemEntry* Table, wxString in_line)
{
	wxVector<makesSearch> lineInterpreterResults;
	itemEntry_Bytes* optionBytes= nullptr;
	size_t pos_last= 0;

	for(size_t temp_u=0; temp_u<curr_interpreter.ff_table_exit.size(); temp_u++){	//and entries
		fastFind_struct* temp_tab_u =& curr_interpreter.ff_table_exit.at(temp_u);
		if(temp_tab_u->element_type & ff_bytes){
			optionBytes= interpretNextLine_TBytes(in_line, Table, lineInterpreterResults, pos_last ); //all origin (by reference)
			continue;
		}
		bool CanExitLoopOr= interpretNextLine_TLine(in_line, temp_tab_u, lineInterpreterResults, pos_last, *Table);
		if( !CanExitLoopOr ){
			if(optionBytes){	//restore DB_fileIO-position
				delete optionBytes;
				optionBytes= nullptr;
			}
			return nullptr;
		}
	}	//and entries
		//after all "and" entries, precede to combine & return*
	for(size_t temp_u=0; temp_u<lineInterpreterResults.size(); temp_u++){
		if((lineInterpreterResults.at(temp_u).element_type &(ff_mode_perc-1) ) ==ff_key){
			if(lineInterpreterResults.at(temp_u).interpreted_value == Table->key){	//ok
			}
			else {
				if(optionBytes){	//restore DB_fileIO-position
					delete optionBytes;
					optionBytes= nullptr;
				}
				return nullptr;
			}
		}
	}

	itemEntry &temp_ie= *Table;
	temp_ie.type_stores|= ie_Table;
	if(optionBytes){
		delete optionBytes;
		optionBytes= nullptr;
	}
	else if(optionBytes && optionBytes->Length()!= 0)
		DB_fileIOLine->Seek( optionBytes->pos_end );

	return Table->parent; //ok at this point, return up a level
}
itemEntry* fafikLib_DB::interpretNextLine_TKey(itemEntry* Table, wxString in_line, const bool useAutoKey)
{
	wxVector<makesSearch> lineInterpreterResults;
	size_t pos_last= 0;

	for(size_t temp_u= 0; temp_u<curr_interpreter.ff_keyValue.size(); temp_u++){	//and entries
		if(useAutoKey &&
		    temp_u<= size_t(curr_interpreter.isAssignAt_ff_keyValue) ) continue;
		fastFind_struct* temp_tab_u =& curr_interpreter.ff_keyValue.at(temp_u);

		bool CanExitLoopOr= interpretNextLine_TLine(in_line, temp_tab_u, lineInterpreterResults, pos_last, *Table);
		if( !CanExitLoopOr ){
			if( !useAutoKey && Table->isAuto_key() ){
				if( curr_interpreter.isAssignAt_ff_keyValue< 0 ) return nullptr;
				return interpretNextLine_TKey( Table, in_line, true );
			}
			return nullptr;
		}

	}	//and entries
		//after all "and" entries, precede to combine & return*

	if(useAutoKey && lineInterpreterResults.empty())	return nullptr;

	// @note (main#9#07/26/18): now combining all the information of finds
	itemEntry &temp_ie= *(new itemEntry);
	temp_ie.depth= Table->depth +1;
	temp_ie.parent= Table;

	for(size_t LIR=0; LIR<lineInterpreterResults.size(); LIR++){
		makesSearch &p_LIR= lineInterpreterResults.at(LIR);
		BYTE ElType= (p_LIR.element_type &(ff_mode_perc-1));
		//from highest to lowest
		if(ElType == ff_v){}
		else if(ElType == ff_hashTable){
			wxLogMessage("# hash table not in yet.");
		}
//		else if(ElType == ff_auto_key){
//			temp_ie.type_stores|= ie_auto_key;
//		}
//		else if(ElType == ff_bytes){
//			wxLogMessage("bytes WIP.");
//		}
		else if(ElType == ff_value){
			temp_ie.type_stores|= ie_value;
			temp_ie.value+= p_LIR.interpreted_value;
		}
		else if(ElType == ff_key){
			temp_ie.key+= p_LIR.interpreted_value;
		}
		else if(ElType == ff_e){
			//printf("e empty...");
		}
	}
	Table->sub_itemTable.push_back(&temp_ie);
	return Table; //ok at this point, return back a level
}


itemEntry_Bytes* fafikLib_DB::interpretNextLine_TBytes(wxString &in_line, itemEntry* Table, wxVector<makesSearch> &lineInterpreterResults, size_t &pos_last )
{
	wxVector<makesSearch> this_lineInterpreterResults;

	for(size_t temp_tb=0; temp_tb<curr_interpreter.ff_table_bytes.size(); temp_tb++){
		fastFind_struct &temp_ffs= curr_interpreter.ff_table_bytes.at(temp_tb);
		if(interpretNextLine_TLine(in_line, &temp_ffs, this_lineInterpreterResults, pos_last, *Table)){
			//continue;
		}
		else {
			return nullptr;
		}
	}

	itemEntry_Bytes& tempByteItem= *(new itemEntry_Bytes);
	tempByteItem.Open(DB_fileIOLine);
	long long temp_LL=0;
	bool byte_type= false;
	if(curr_interpreter.overwrite_byte_type!= -1) byte_type= curr_interpreter.overwrite_byte_type;

	for(size_t temp_ob=0; temp_ob<this_lineInterpreterResults.size(); temp_ob++){
		makesSearch &t_ires= this_lineInterpreterResults.at(temp_ob);
		if(t_ires.element_type %ff_mode_perc == ff_bytes){
			if(t_ires.interpreter_key== "%n" ) {
				t_ires.interpreted_value.ToLongLong( &temp_LL);
			}
			else if(t_ires.interpreter_key== "s" ) {
				byte_type= 0;
			}
			else if(t_ires.interpreter_key== "l" ) {
				byte_type= 1;
			}
		}
	}

	if(byte_type && (temp_LL>= 0 && temp_LL<= 8) ){ //byte_type of long assign(number of bytes containing data length)
		if(temp_LL== 0){
			tempByteItem.pos_start= DB_fileIOLine->Tell();
			tempByteItem.pos_end= tempByteItem.pos_start;
		}
		else {
			BYTE temp_chars[8]= {0};
			size_t read1= DB_fileIOLine->Read( &temp_chars[0], temp_LL);
			if(read1== size_t(temp_LL)){
				long long *temp_LL2_p= static_cast<long long*>(static_cast<void*>(temp_chars) );
				tempByteItem.pos_start= DB_fileIOLine->Tell();
				tempByteItem.pos_end= tempByteItem.pos_start+ *temp_LL2_p;
			}
			else {
wxLogError("Could not load %i Bytes from DB file, pos= %s.", temp_LL, atos::toWxString(DB_fileIOLine->Tell()) );
			}
		}
	}
	else { //if bytes
		tempByteItem.pos_start= DB_fileIOLine->Tell();
		tempByteItem.pos_end= tempByteItem.pos_start+ temp_LL;
	}
	return &tempByteItem;
}

BYTE fafikLib_DB::discardPreviousEntries(const int &p_out_type)
{
	if(p_out_type == 0){	//Exit Table
		_NextLine_iE->FreeTable();	//remove sub items
		return false;
	}
	if(p_out_type == 1 && _NextLine_iE->sizeTable() > 1 ){	//Key/Comment
		_NextLine_iE->removeItems( _NextLine_iE->begin(), _NextLine_iE->end()-1 );
		return false;
	}
	if(p_out_type == 2 && _NextLine_iE->sizeTable() > 1 ){	//Enter Table
		itemEntry *tempRemoveItems= _NextLine_iE->parent;
		if(tempRemoveItems)
			tempRemoveItems->removeItems( tempRemoveItems->begin(), tempRemoveItems->end()-1 );
		return false;
	}
	return true;
}
BYTE fafikLib_DB::interpretNextLine(int *p_out_type)	//itemEntry* Table
{
	if(p_out_type) *p_out_type= -1;
	if(!_NextLine_iE) return -1; //one type of error

	if( !DB_fileIOLine->IsOpened() || DB_fileIOLine->Error() ) return -1;
 	wxString lineContent;
	if( DB_fileIOLine->readLineByte(&lineContent) )	//fafikLib_readLineByte(DB_fileIO, )
	{
		bool skippInter= false;
		Quotation::lineBeginsWithNotBlank(lineContent);
		if(curr_interpreter.specialEntry){
			for(size_t temp_se=0; temp_se<curr_interpreter.specialEntry->sizeTable(); temp_se++){
				itemEntry* entry_se= curr_interpreter.specialEntry->at(temp_se);
				if(entry_se->key== "comment"){
					for(size_t getI=0; getI<entry_se->sizeTable(); getI++){
						if(lineContent.StartsWith( entry_se->at(getI)->value ) ){
							if(p_out_type) *p_out_type= 1;
							_NextLine_iE->Add( "", lineContent, ie_lore );
							skippInter= true;
							break;
						}
					}
				}
				else if( entry_se->value== lineContent ){
					skippInter= true;
					if(entry_se->key== "link"){
					}
					else if(entry_se->key== "auto_key"){
						_NextLine_iE->type_stores|= ie_auto_key;
					}
					else if(entry_se->key== "copy"){
					}
					break;
				}
			}
		}
		if(skippInter) return 0;

	//"content"="value"
	//["table-key"]=
	//{
	//	"sub-content"="value"
	//}
		for(size_t priorityLevel=0; priorityLevel<3; ++priorityLevel ) {
			if(curr_interpreter.recognitionPriority[priorityLevel] == "enter"){
				itemEntry* temp_ie= interpretNextLine_TEnter(_NextLine_iE, lineContent);
				if(temp_ie){//ok
					if(p_out_type) *p_out_type= 2;
					_NextLine_iE= temp_ie;
					break;	//ok done
				}
			}
			else if(curr_interpreter.recognitionPriority[priorityLevel] == "exit"){
				itemEntry* temp_ie= interpretNextLine_TExit(_NextLine_iE, lineContent);
				if(temp_ie){//ok
					if(p_out_type) *p_out_type= 0;
					_NextLine_iE= temp_ie;
					break;	//ok done
				}
			}
			else if(curr_interpreter.recognitionPriority[priorityLevel] == "key"){
				itemEntry* temp_ie= interpretNextLine_TKey(_NextLine_iE, lineContent);
				if(temp_ie){//ok
					if(p_out_type) *p_out_type= 1;
					break;	//ok done
				}
			} else {
wxLogError("error recognitionPriority does not match search;\n |%s|\n", curr_interpreter.recognitionPriority[priorityLevel]);
			}
		}//for recognitionPriority
		return 0;
	}
	else {
		while( true ){
			_NextLine_iE->type_stores|= ie_TableNotClosed;
			if(!_NextLine_iE->parent) break;
			_NextLine_iE= _NextLine_iE->parent;
		}
	}
	return 1;
}


size_t fafikLib_DB::fastFind_struct::find(wxString str) const
{
	for(size_t temp_u=0; temp_u<keys.size(); temp_u++){
		if(keys.at(temp_u).key== str) return temp_u;
	}
return -1;
}















};//namespace
