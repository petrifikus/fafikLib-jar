#include "fafikLib_DB.h"
#include "wx/filename.h"

#include "f77_helpers.h"

fafikLib_DB::fafikLib_DB()
{
	oninit();
//	valueToKeyAssignChars.push_back("=");
//	valueToKeyAssignChars.push_back("\t");
//	valueToKeyAssignChars.push_back(":");
	//ctor
}

fafikLib_DB::~fafikLib_DB()
{
//	if(DB_fileIO.IsOpened())
//		DB_fileIO.Close();
	if(DB_fileIO){
		delete DB_fileIO;
		DB_fileIO= nullptr;
	}
	//this DB_interpreter_table is protected using a shared pointer, it will be destroyed only when no-one has a copy of the pointer
//	if( _b_TablesInherited &1 ){
//		DB_interpreter_table->Detach();	//protect against deleting this table
//	}
	//dtor
}

fafikLib_DB::fafikLib_DB(const fafikLib_DB& other)
{
	this->DB_interpreter_table= other.DB_interpreter_table;
//	_b_TablesInherited= 1;
	oninit();
	//copy ctor
}
// ///pass DB_interpreter_table
//fafikLib_DB::fafikLib_DB(itemEntry& DB_interpreter_table)
//{
//	this->DB_interpreter_table= std::shared_ptr<itemEntry>(&DB_interpreter_table);
////	_b_TablesInherited= 1;
//	oninit();
//}
 ///pass DB_interpreter_table
fafikLib_DB::fafikLib_DB(std::shared_ptr<itemEntry>& DB_interpreter_table_sp)
{
	this->DB_interpreter_table= DB_interpreter_table_sp;
//	_b_TablesInherited= 1;
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
//	tableInsert(DB_interpreter_table.get(), "main interpreter table" );	///Pointer Recursive Object Notation
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
//	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "e","[" );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "key","%u" );
//	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "e","]" );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "e","%a" );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "e" );				//E
	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "e","{" );			//e {}
	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "auto_key","[" );	//auto_key []
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "bytes" );
	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "e","*" );
	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "bytes","%n s" );
	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "exit" );
	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "e" );
//	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "e", "}[" );
	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "e", "}" );
//	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "auto_key", "][" );
	    temp_ins[4] = tableInsertSubTable(temp_ins[3], "auto_key", "]" );
//	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "key", "%u" );
//	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "e", "]" );
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
//	  temp_ins[2] = tableInsertSubTable(temp_ins[1], "v" );
//	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "v", " " );
//	   temp_ins[3] = tableInsertSubTable(temp_ins[2], "v", "\t" );
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

//printf("[ table\n");
//PrintTable( DB_interpreter_table.get() );
//printf("] end\n");

return true;
}
void fafikLib_DB::PrintTable(fafikLib_DB::itemEntry* iePrint, size_t dent)
{
	wxString strDent;
	strDent.resize(dent, wxUniChar(' ') );
	for( auto currIe: iePrint->sub_itemTable ){

		printf( "%s = %s\n", (strDent+ currIe->key).ToStdString().c_str(), currIe->value.ToStdString().c_str() );
		if( currIe->isTable() ) PrintTable( currIe, dent+1 );

	}
}


//BYTE fafikLib_DB::tableInsert(itemEntry *table, wxString key, wxString value, BYTE type_stores)
//{
//	if(!table) return -1;
////printf(" %s =",key.ToStdString().c_str());
////printf(" %s\n",value.ToStdString().c_str());
////printf("\t%i\n",table->depth);
//	table->key= key;
////	table->key.Shrink();
//	table->value= value;
////	table->value.Shrink();
//	table->type_stores= type_stores;
//	if(!value.empty()) table->type_stores|= ie_string;
////	BytesStored += value.size() ;
////	BytesStored += key.size() ;
//return 0;
//}

fafikLib_DB::itemEntry* fafikLib_DB::tableInsertSubTable(itemEntry *table, wxString key, wxString value, BYTE type_stores)
{
	if(!table) return nullptr;
	table->type_stores|= ie_Table;
	return table->Add_back( key, value, type_stores|ie_value );
//	table->type_stores |= ie_sub_itemTable;
////	table->sub_itemTable.resize(table->sub_itemTable.size()+1);
//	table->sub_itemTable.push_back(new itemEntry);
//	itemEntry &temp_itemE= *table->sub_itemTable.back();
//
////	tableInsert(temp_itemE, key, value, type_stores);
//	tableInsert( &temp_itemE , key, value, type_stores);
////printf("  temp_itemE.key=|%s|", temp_itemE.key.ToStdString().c_str());
////printf("\t%i\n",table->depth);
//
//	temp_itemE.parent = table;
//	temp_itemE.depth = table->depth+1;
//
//return &temp_itemE;
}

fafikLib_DB::itemEntry* fafikLib_DB::tableInsertNextSubTable(itemEntry *table, wxString key, wxString value, BYTE type_stores)
{
	if(!table || !table->parent) return nullptr;
	return tableInsertSubTable( table->parent, key, value, type_stores );
}

//fafikLib_DB& fafikLib_DB::operator=(const fafikLib_DB& rhs)
//{
//	if (this == &rhs) return *this; // handle self assignment
//	//assignment operator
//	return *this;
//}



unsigned short fafikLib_DB::ready_partForinterpreterOR(itemEntry *temp_tab, fastFind_struct *new_fF)
{
	new_fF->keys.resize(new_fF->keys.size()+1);
	///new key in "*new_fF"
	fastFind_struct_el* new_fF_ins= &new_fF->keys.back();
//printf("  with, %s=|%s| type=%u\n", temp_tab->key.ToStdString().c_str(), temp_tab->value.ToStdString().c_str(), temp_tab->type_stores );
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
//printf("  $new_fF_ins = |%s|\n",new_fF_ins->key.ToStdString().c_str());

	if( temp_tab->value.Length()>=2 && temp_tab->value.at(0)== wxUniChar('%')){
//		if(temp_tab->value.substr(0,2)== "%%" ){
		if(temp_tab->value.StartsWith("%%") ){
			new_fF_ins->key= temp_tab->value.SubString(1,-1);
		} else {
			wxStringInputStream tempStringStream( temp_tab->value.substr(2) );
//			std::stringstream tempStringStream(temp_tab->value.substr(2).ToStdString());
//			if(temp_tab->value.find("%a")==0){
			if( temp_tab->value.StartsWith("%a") ){
//printf("   |%s| starts with %%a\n", temp_tab->value.ToStdString().c_str() );
//				std::string tempSubN;
				size_t temp_L= 0;
//				if( tempStringStream.get_Number(temp_L) )
				tempStringStream>> temp_L;
//				tempStringStream >> tempSubN;
//printf("  koko 3.3 = %i\n",tempSubN.length());
				if( !(temp_L< curr_interpreter.ff_assign.size()) )
					temp_L= 0;
//printf("  koko 3.3.h  == %i \n", curr_interpreter.ff_assign.at(0).keys.size());///error, = empty
				new_fF_ins->key= ( curr_interpreter.ff_assign.at( temp_L ).keys.at(0).key );
//printf("   |%s|new_fF_ins->key\n", new_fF_ins->key.ToStdString().c_str() );
				new_fF_ins->isAssign= true;
				new_fF->isAssign= true;
			} else {
				new_fF_ins->element_type|= ff_mode_perc;
				new_fF_ins->key= temp_tab->value.substr(0,2);
			}
//			while(tempStringStream.tellg()!= -1){
			wxString tempSubS;
			tempStringStream>> tempSubS;
//			while( tempStringStream.Tell()!= -1 ){
			while( tempSubS.size() ){
//				std::string tempSubS;
//				wxString tempSubS;
//				tempStringStream>> tempSubS;
//printf("   tempStringStream = |%s|\n",tempStringStream.str().c_str());
				if(tempSubS== "%a"){
//					std::string tempSubN;
//					wxString tempSubN;
					size_t temp_L= 0;
					tempStringStream>> temp_L;
					if( !(temp_L< curr_interpreter.ff_assign.size()) )
						temp_L= 0;
					new_fF_ins->key_stringArgs.push_back( curr_interpreter.ff_assign.at( temp_L ).keys.at(0).key );
				} else {
					if(new_fF_ins->key == "%k"){
						if(tempSubS== "space") tempSubS=" ";
						else if(tempSubS== "tab") tempSubS="\t";
						else if(tempSubS== "\\t") tempSubS="\t";
//printf("    %k = |%s|\n", tempSubS.c_str());
					}
					new_fF_ins->key_stringArgs.push_back(tempSubS);
					if(new_fF_ins->key== "%n" && !curr_interpreter.overwrite_byte_type){
						if(tempSubS== "l")
							curr_interpreter.overwrite_byte_type= 1;
						else if(tempSubS== "s")
							curr_interpreter.overwrite_byte_type= 0;
					}
//printf("   tempSubS = |%s|\n",tempSubS.c_str() );
//printf("   tempSubS = %u\n",tempSubS.length() );
//printf("   tellg = %i\n",tempStringStream.tellg() );
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
//	put_here->resize(put_here->size()+1);
//	fastFind_struct *new_fF=&put_here->back();

	for( size_t main_TabI=0; main_TabI< temp_tab->sizeTable(); ++main_TabI ){
		if(temp_tab->at(main_TabI)->type_stores & ie_sub_itemTable ){	//type_stores = sub_itemTable
			put_here->resize(put_here->size()+1);
			for(size_t temp_u=0; temp_u<temp_tab->at(main_TabI)->sizeTable(); temp_u++){
//printf("[[ ----- ready_partForinterpreter\n");
//printf("temp_tab->???...size() = %u\n", temp_tab->at(main_TabI)->sizeTable());
//printf("temp_tab->...key = |%s|\n", temp_tab->at(main_TabI)->key.ToStdString().c_str());
				if(temp_tab->at(main_TabI)->key=="bytes" && !curr_interpreter.bytesIn){
					curr_interpreter.bytesIn= true;
					ready_partForinterpreter(temp_tab->at(main_TabI), &curr_interpreter.ff_table_bytes);
					put_here->back().element_type= ff_bytes;
//printf("table_bytes$ = %u\n", curr_interpreter.ff_table_bytes.size());
//for(size_t temp_tb=0; temp_tb<curr_interpreter.ff_table_bytes.size(); temp_tb++)
//{
//	printf("# %u = ", temp_tb);
//	printf("table_bytes.keys$ = %u\n", curr_interpreter.ff_table_bytes.at(temp_tb).keys.size() );
//	printf("  table_bytes.keys[0] = |%s|\n", curr_interpreter.ff_table_bytes.at(temp_tb).keys.at(0).key.ToStdString().c_str() );
//	printf("  table_bytes.keys[0] = %u\n", curr_interpreter.ff_table_bytes.at(temp_tb).keys.at(0).element_type );
//}
//getchar();
				}

				ready_partForinterpreterOR( temp_tab->at(main_TabI)->at(temp_u), &put_here->back() );

//				if(temp_tab->at(main_TabI)->at(temp_u)->key=="bytes"){
//
//				}
//printf("]] ----- ready_partForinterpreter\n");
//getchar();
			}
		}
		else if( temp_tab->at(main_TabI)->type_stores & ie_string){	//type_stores = value -string
			put_here->resize(put_here->size()+1);
			ready_partForinterpreterOR( temp_tab->at(main_TabI), &put_here->back() );
		}
	}

///checked & fixed to here, on version 2018-07-26-18.39
///checked & fixed to here, on version 2018-07-28-21.32
return 0;
}

BYTE fafikLib_DB::ready_interpreter(itemEntry &interpreterTable_p)
{
	{
		interpreterStru new_cI;	//make a fresh table(for when changing interpreter type) 2018-08-06
//		std::swap(curr_interpreter, new_cI);
		curr_interpreter= new_cI;
	}
//system("cls");
//printf("\n\t ========= ready_interpreter ========\n\n");

//printf("starts\n");
//	fastFind_struct_vect.clear();
	curr_interpreter.specialEntry=
//ready_partForinterpreter(
//	getItem_byKey_rec(interpreterTable_p,translate_StrToArr("specialEntry"),0,0) ;//, &curr_interpreter.specialEntry
	interpreterTable_p.find_linear("specialEntry", true);
//	getItem_byKey_rec(interpreterTable_p,translate_StrToArr("specialEntry"),0,0) ;//, &curr_interpreter.specialEntry
//														);
//printf("after special = %u\n", curr_interpreter.specialEntry);
//printf(" \ncurr_interpreter.specialEntry |%s|\n", curr_interpreter.specialEntry->key.ToStdString().c_str()   );
//printf(" curr_interpreter.specialEntry |%s|\n", curr_interpreter.specialEntry->at(0)->key.ToStdString().c_str()   );
//printf(" curr_interpreter.specialEntry |%s|\n", curr_interpreter.specialEntry->at(0)->at(0)->value.ToStdString().c_str()   );

	ready_partForinterpreter( //getItem_byKey_rec(
//	 interpreterTable_p,translate_StrToArr("assign"),0,0), &curr_interpreter.ff_assign
	 interpreterTable_p.find_linear("assign", true), &curr_interpreter.ff_assign
						 );
//printf("after ass.\n");
//	if(curr_interpreter.entry_assign){
//		BYTE temp_B = curr_interpreter.entry_assign->type_stores;
//		if(temp_B==0 || temp_B & 1){
//// TODO (main#9#06/10/18): handle "assign" type=0
//		}
//		for(size_t temp_u=0; temp_u<curr_interpreter.entry_assign->sub_itemTable.size(); temp_u++){
//			itemEntry * temp_tab = &curr_interpreter.entry_assign->sub_itemTable.at(temp_u);
//			ready_partForinterpreter(temp_tab);
//		}
//	}

	if(ready_partForinterpreter( //getItem_byKey_rec(
//		interpreterTable_p,translate_StrToArr("lineTable"),0,0), &curr_interpreter.ff_keyValue ) >=0 )
		interpreterTable_p.find_linear("lineTable", true), &curr_interpreter.ff_keyValue ) >=0 )
	{	//ok, found "lineTable"
		curr_interpreter.isLineTable = true;
	} else {	//try "key_value"
		ready_partForinterpreter( //getItem_byKey_rec(
//		 interpreterTable_p,translate_StrToArr("key_value"),0,0), &curr_interpreter.ff_keyValue );
		 interpreterTable_p.find_linear("key_value", true), &curr_interpreter.ff_keyValue );
	}


	curr_interpreter.entry_table=// getItem_byKey_rec(
	 interpreterTable_p.find_linear("table", true);

	if(curr_interpreter.entry_table){
		ready_partForinterpreter( //getItem_byKey_rec(
//		 *curr_interpreter.entry_table,translate_StrToArr("enter"),0,0) , &curr_interpreter.ff_table_enter);
		 curr_interpreter.entry_table->find_linear("enter",true) , &curr_interpreter.ff_table_enter);

		ready_partForinterpreter( //getItem_byKey_rec(
//		 *curr_interpreter.entry_table,translate_StrToArr("exit"),0,0) , &curr_interpreter.ff_table_exit );
		curr_interpreter.entry_table->find_linear("exit", true) , &curr_interpreter.ff_table_exit );
	}

//printf(" curr_interpreter.table.enter |%s|\n", curr_interpreter.ff_table_enter.at(1).keys.at(0).key  .ToStdString().c_str()   );

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



	itemEntry* temp_prior=// getItem_byKey_rec(
//	 interpreterTable_p,translate_StrToArr("priority"),0,0);
	 interpreterTable_p.find_linear("priority", true);
	curr_interpreter.recognitionPriority.push_back("");
	curr_interpreter.recognitionPriority.clear();
	curr_interpreter.recognitionPriority.resize(3);
	if(temp_prior){
//printf("found priority...\n");
		for(size_t tempP=0; tempP<3; tempP++){
			curr_interpreter.recognitionPriority[tempP]= temp_prior->sub_itemTable.at(tempP)->value;
		}
	} else {
//printf("NO priority... inserting\n");
		curr_interpreter.recognitionPriority[0]= "exit";	//mostly only 1 char
		curr_interpreter.recognitionPriority[1]= "enter";	//complicated multi detection
		curr_interpreter.recognitionPriority[2]= "key";		//assignment key, val
//printf("NO priority is now %u\n", curr_interpreter.recognitionPriority.size() );

	}

	itemEntry* temp_MaxApart=// getItem_byKey_rec(
//	 interpreterTable_p,translate_StrToArr("elementMaxDistanceApart"),0,0);
	 interpreterTable_p.find_linear("elementMaxDistanceApart", true);
	if(temp_MaxApart){
		long temp_L= 0;
		if(temp_MaxApart->value.ToLong( &temp_L ) ){
			curr_interpreter.elementMaxDistanceApart= abs(temp_L);
		}
	}
return 0;
}


const size_t fafikLib_DB::size_ti= size_t(-1);
BOOL fafikLib_DB::CreateDirTree(const wxString& str_path, LPSECURITY_ATTRIBUTES attribs)
{
	size_t posBS=2;
	BOOL ret_val= ERROR_PATH_NOT_FOUND;
	do{
		posBS= str_path.find_first_of("/\\", posBS+1);
		wxString str_DirPath= str_path.substr(0, posBS);
		replaceAll(str_DirPath, "/", "\\");
		if( !wxDirExists(str_DirPath) )
		{
			if( str_DirPath.IsSameAs("\\\\?\\") ) continue; //skip "\\?\" ... "Drive:\"
			ret_val= CreateDirectoryW(str_DirPath.ToStdWstring().c_str(), attribs);
			if( !wxDirExists(str_DirPath) )
			{
				break;
			}
		}
	} while( posBS!= wxString::npos );
	return ret_val;
}

///0 ok, 1 can not make file, 2 no interpreter auto found, mode is not implemented for now 2018-08-07
BYTE fafikLib_DB::Open(const wxString& file_path, modes_IO mode, const wxString overwriteInterpreter, settings *useSettings, bool clearsPrevious)
{
	this->Close();
	if(clearsPrevious) this->cleanup_DB();
//printf("interprteer == %u\n", curr_interpreter.ff_assign.size() );

	if(file_path.empty()) return 1;
	if( !wxFileExists(file_path) ){
		size_t dir_to= file_path.find_last_of( "\\/" );
		if( dir_to!= size_ti ){
			CreateDirTree( file_path.substr(0, dir_to ) );
		}
		if( wxDirExists(file_path) ) return 1;

// TODO (efficiency#9#09/16/18): check if file_path || file_path.new || file_path.old before creating file_path
		wxFFile tmpzs;
		tmpzs.Open( file_path, "w" );
		tmpzs.Close();
	}
	if(!DB_fileIO){
		DB_fileIO= new fafikLib_readLineByte_file;
		if(DB_fileIOLine) delete DB_fileIOLine;
	}
	DB_fileIO->Open(file_path, "rb" );
	DB_fileIOLine= DB_fileIO;
	if( !DB_fileIO->IsOpened() || DB_fileIO->Error() ) return 1;
//	DB_fileIOLine.use( DB_fileIO );
	P_DB_FileName= file_path;

	if(useSettings) UseSettings(*useSettings);
//printf("kokokkokk 2\n");
//wxLogError("inter wrong name = |%s|", thoseDB_settings.useInterpreter->key);
	UseInterpreter(overwriteInterpreter);
//printf("interprteer == %u\n", curr_interpreter.ff_assign.size() );
	size_t pos_nameExtSep= file_path.find_last_of('.');
	if( pos_nameExtSep!= size_ti ){
		wxString str_Ext= file_path.substr(pos_nameExtSep+1);
//printf("str_Ext = |%s|\n", str_Ext.ToStdString().c_str());
//getchar();
//wxLogError("inter wrong name = |%s|", thoseDB_settings.useInterpreter->key);
		if(!thoseDB_settings.useInterpreter){	//find me an interpreter
//printf("picking by ext...\n");

//			wxLogMessage("str_Ext = |%s| Error= %i", str_Ext,
			UseInterpreterAuto(str_Ext);
//printf("picking by ext end...\n");
//			);
		}
	} else {	//just allowing for file name without '.ext', if there is such a need
		if(!thoseDB_settings.useInterpreter){
			UseInterpreterAuto(wxEmptyString);
		}
	}
	if(!thoseDB_settings.useInterpreter) return 2;
//printf("loading ready_interpreter ....\n");
	ready_interpreter(*thoseDB_settings.useInterpreter);
//printf("loading ready_interpreter end\n");
return 0;
}
BYTE fafikLib_DB::Open(const wxString& file_path, const overrideInterpreter& useIterpr ,modes_IO mode, bool clearsPrevious )
{
	this->Close();
	if(clearsPrevious) this->cleanup_DB();

	if(file_path.empty()) return 1;
	if( !wxFileExists(file_path) ){
		size_t dir_to= file_path.find_last_of( "\\/" );
		if( dir_to!= size_ti ){
//			wxFileName::Mkdir( file_path.substr(0, dir_to ) ,wxS_DIR_DEFAULT,wxPATH_MKDIR_FULL );
			CreateDirTree( file_path.substr(0, dir_to ) );
		}
		if( wxDirExists(file_path) ) return 1;

// TODO (efficiency#9#09/16/18): check if file_path || file_path.new || file_path.old before creating file_path
		wxFFile tmpzs;
		tmpzs.Open( file_path, "w" );
		tmpzs.Close();
	}
	if(!DB_fileIO){
		DB_fileIO= new fafikLib_readLineByte_file;
		if(DB_fileIOLine) delete DB_fileIOLine;
	}
	DB_fileIO->Open(file_path, "rb" );
	DB_fileIOLine= DB_fileIO;
	if( !DB_fileIO->IsOpened() || DB_fileIO->Error() ) return 1;
//	DB_fileIOLine.use( DB_fileIO );
	P_DB_FileName= file_path;


	///clear interpreter pointer
	if( useIterpr.which ) thoseDB_settings.useInterpreter= nullptr;

	if(useIterpr.which== 1){
		UseInterpreter( useIterpr.over_string );
	} else if(useIterpr.which== 2){
		thoseDB_settings.useInterpreter= DB_interpreter_table->at( useIterpr.over_index );
	} else if( useIterpr.which== 3 ){
		size_t pos_nameExtSep= file_path.find_last_of('.');
		if( pos_nameExtSep!= size_ti ){
			UseInterpreterAuto( file_path.substr( pos_nameExtSep+1 ) );
		}
	} else if( useIterpr.which== 4 ){
		UseInterpreterAuto( useIterpr.over_string );
	}


	///make interpreter
	if( useIterpr.which ){
		if( thoseDB_settings.useInterpreter ) ready_interpreter(*thoseDB_settings.useInterpreter);
		else return 2;	//no interpreter
	}

return 0;
}
BYTE fafikLib_DB::Open(fafikLib_readLineByte_base* BaseIO, const wxString& UseInterpr, bool clearsPrevious)
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
//		DB_fileIOLine.use( DB_fileIO );
		_NextLine_iE= nullptr;	//we dont want access violation
	}

return 0;
}

bool fafikLib_DB::interpretEnd()
{
	if(DB_fileIOLine){
		fafikLib_readLineByte_EOL eol= DB_fileIOLine->getEol();
		return DB_fileIOLine->Close(); //this would reset EOL, so we need to copy it
		DB_fileIOLine->useEOL(eol);
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

//	fafikLib_readLineByte_file temps_news;
//	DB_fileIOLine= temps_news;
	_NextLine_iE= nullptr; //cant read file, cant add entries(also we will leave it as was, it might not have been finished)

	if(DB_file_write.IsOpened()) DB_file_write.Close();
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
	if( !wxIsWritable( temp_fileName ) ){
		return 0;
	}
	DB_fileIOLine->reClose();
//	DB_fileIO.Close();
	DB_file_write.Open( temp_fileName, "w+b" );

  if( (!DB_file_write.IsOpened()) || DB_file_write.Error() ){ //error
//		DB_file_write.Close();
//		DB_fileIO.Open( temp_fileName, "rb" );
		DB_fileIOLine->reOpen();
		return 0;
  }
  else { //ok
	WritePToFile();
//	if( !ret_ok ){
//	}
  }
//	DB_fileIO.Open( temp_fileName, "rb" );
	DB_fileIOLine->reOpen();
	DB_fileIOLine->Seek(0);
	return 1;
}
int fafikLib_DB::WriteToFile()
{
	if( thoseDB_settings.OpenMode== mode_append ) return WriteToFile_append();
	return WriteToFile_new();
}
int fafikLib_DB::WriteToFile_interpreter( wxString filePathName )
{
	if( !wxIsWritable( filePathName ) ) return 0;
	fafikLib_DB now_interpreter(this->DB_interpreter_table);
	now_interpreter.Open( filePathName );

	interpreterStru keep_interpreter= curr_interpreter;
	curr_interpreter= now_interpreter.curr_interpreter;

	now_interpreter.Close();	//just to be safe

	DB_file_write.Open( filePathName, "w+b" );
	int ret_ok= WritePToFile( DB_interpreter_table.get() );

//	now_interpreter.DB_interpreter_table->Detach();

	curr_interpreter= keep_interpreter;
return ret_ok;
}
int fafikLib_DB::WriteToFile_new()
{
	wxLogNull noLog;
	const wxString temp_fileName= DB_fileIOLine->GetName();
	DB_file_write.Open( temp_fileName+ ".new", "w+b" );
  if( (!DB_file_write.IsOpened()) || DB_file_write.Error()){
		DB_file_write.Close();
		wxRemoveFile( temp_fileName+ ".new" );
		return 1;
  } else {
	int ret_ok= WritePToFile();
	if( !ret_ok ){
		DB_fileIOLine->reClose();
		wxRenameFile(temp_fileName, temp_fileName+".old");
		wxRenameFile(temp_fileName+ ".new", temp_fileName);
//		DB_fileIO->Open( temp_fileName, "rb" );
		DB_fileIOLine->reOpen();
		DB_fileIOLine->Seek(0);
	}
  }
return 0;
}
int fafikLib_DB::WriteToNewFile( wxString filePathName )
{
	if( !wxIsWritable( filePathName ) ) return 0;
	DB_file_write.Open( filePathName, "w+b" );

	int ret_ok= WritePToFile();
return ret_ok;
}
int fafikLib_DB::WriteToFile_append()
{
	if( !wxIsWritable( DB_fileIOLine->GetName() ) ) return 0;
	DB_file_write.Open( DB_fileIOLine->GetName(), "ab" );
	int ret_ok= WritePToFile();
return ret_ok;
}
int fafikLib_DB::WritePToFile(itemEntry *IE_saveCustom)
{
	if( (!DB_file_write.IsOpened()) || DB_file_write.Error() ) return 0;
	if(!IE_saveCustom) IE_saveCustom= &DB_table;
//printf("writing size = %i\n", IE_saveCustom->sizeTable());
	for(size_t temp_u=0; temp_u<IE_saveCustom->sizeTable(); temp_u++){
		WritePToFile_rec( IE_saveCustom->at(temp_u) );
	}

	DB_file_write.Flush();
	DB_file_write.Close();
return 1;
}
int fafikLib_DB::WritePToFile_rec(itemEntry *item, size_t depthSpaces)
{
	if(!item) return 0;
//printf("key = |%s|\n", item->key.ToStdString().c_str() );

 if( item->isLink() ){
//printf(" isLink;\n");
 } else if( item->type_stores & ie_lore ){
	DB_file_write.Write( item->value );
	//2018-10-21 ( DB_fileIOLine.getEOL() ) was put in
	DB_file_write.Write( DB_fileIOLine->getEolRaw(), DB_fileIOLine->getEolSize() );
 } else if( item->isTable() ){
//printf("item.key = |%s| is table with .val = |%s|\n", item->key.ToStdString().c_str(), item->value.ToStdString().c_str() );
//printf(" isTable; inter.table.enter # %i\n", curr_interpreter.ff_table_enter.size() );
 	wxString preSpace; preSpace.Pad(depthSpaces, thoseDB_settings.onWriteLine_biginWithAutoIndent.at(0) );
	DB_file_write.Write(preSpace);
	for(size_t temp_u=0; temp_u<curr_interpreter.ff_table_enter.size(); temp_u++){	//and entries
		if( item->isAuto_key_keyed() &&
			curr_interpreter.isAssignAt_ff_table_enter>=0 &&
			temp_u<= size_t(curr_interpreter.isAssignAt_ff_table_enter) ){
//printf("  ...skipp cont\n");
				continue;	//searching for `=`
		} else if( !item->isWithBytes() && ((curr_interpreter.ff_table_enter.at(temp_u).element_type &(ff_mode_perc-1))== ff_bytes) ){
//printf("  ...isWithBytes cont\n");
			//for now, solution
			//skip bytes if not isWithBytes()
			continue;
		}


		fastFind_struct &temp_tab_and= curr_interpreter.ff_table_enter.at(temp_u);
//printf("  write table looks\n");

		DB_file_write.Write( WritePToFile_rLine(temp_tab_and, item) );
	}
	DB_file_write.Write( DB_fileIOLine->getEolRaw(), DB_fileIOLine->getEolSize() );


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
			DB_file_write.Write(preSpace);

			DB_file_write.Write( WritePToFile_rLine(temp_tab_and, item) );
			DB_file_write.Write( DB_fileIOLine->getEolRaw(), DB_fileIOLine->getEolSize() );
		}
// TODO (GB_DB#9#08/17/18): if (GB_DB) get rid of done tables

	}
//getchar();
 } else if( item->isValue() ){
 	wxString preSpace; preSpace.Pad(depthSpaces, thoseDB_settings.onWriteLine_biginWithAutoIndent.at(0) );
	DB_file_write.Write(preSpace);
	for(size_t temp_u=0; temp_u<curr_interpreter.ff_keyValue.size(); temp_u++){	//and entries
		if( item->isAuto_key_keyed() &&
			curr_interpreter.isAssignAt_ff_keyValue>=0 &&
			temp_u<= size_t(curr_interpreter.isAssignAt_ff_keyValue) ){
				continue;	//searching for `=`
		}
		fastFind_struct &temp_tab_and= curr_interpreter.ff_keyValue.at(temp_u);
		DB_file_write.Write( WritePToFile_rLine(temp_tab_and, item) );
	}
	DB_file_write.Write( DB_fileIOLine->getEolRaw(), DB_fileIOLine->getEolSize() );
 }

return 1;
}
wxString fafikLib_DB::WritePToFile_rLine(fastFind_struct &use_parser, itemEntry *item, bool on_ff_bytes )
{
//	bool canExit= false;
	wxString ret_writeStr;
	if( !on_ff_bytes && (use_parser.element_type &(ff_mode_perc-1))== ff_bytes ){
//		for( curr_interpreter.ff_table_bytes )
//		WritePToFile_rLine( curr_interpreter.ff_table_bytes, item );
	}

	for(size_t temp_uu=0; temp_uu<use_parser.keys.size(); temp_uu++){	//or entries
//		if(canExit) break;
//		if(!item->isAuto_key() && temp_uu>0 ) continue;

		///reversed from temp_uu
		size_t temp_or= (use_parser.keys.size()-1) -temp_uu;

//		if( item->isAuto_key() && use_parser.keys.at(temp_or).element_type&(ff_mode_perc-1)!= ff_auto_key ){
//			if(temp_or==0) temp_or= (use_parser.keys.size()-1);
//			else continue;
//		}

		fastFind_struct_el &temp_tab_or= use_parser.keys.at(temp_or);

		const BYTE uu_elType= temp_tab_or.element_type &(ff_mode_perc-1);
//printf("key = |%s|\n", temp_tab_or.key.ToStdString().c_str() );

		/*if( item->isAuto_key() )
//			if( item->isAuto_key_keyed() )
//		if( item->isValue() )
//			if( item->isLink() )
//			if( item->type_stores & ie_lore )
		if( item->type_stores & ie_TableNotClosed )*/


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
//				writeStr= &item->key;
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
//				bool singleQuote= 0;
				BYTE Quote= 1;
				if(temp_tab_or.key_stringArgs.size()>0){
//					singleQuote = (temp_tab_or.key_stringArgs.Item(0) == "\'");
					if(temp_tab_or.key_stringArgs.Item(0)=="\"") Quote=1;
					else if(temp_tab_or.key_stringArgs.Item(0)=="\'") Quote=2;
				}

				return fafikLib_Quotation::escapeString( ret_writeStr, Quote );

			} else if(temp_tab_or.key== "%S"){
//printf("make %%S\n");
				return ret_writeStr;
			} else if(temp_tab_or.key== "%k"){
				if(temp_tab_or.key_stringArgs.size()>0) ret_writeStr.Append( temp_tab_or.key_stringArgs.Item(0) );
				return ret_writeStr;
			} else if(temp_tab_or.key== "%n"){
//				solve it with log2?		//nope log2 is gonna take longer 2018-08-16
//				or is better with for(8) in static_cast<BYTE> &0xFF != 0
			}
		} else {
			return ret_writeStr;
		}




	}
return wxEmptyString;
}



BYTE fafikLib_DB::UseSettings(settings &useSettings)
{
	this->thoseDB_settings= useSettings;
return 0;
}
void fafikLib_DB::UseEol(const fafikLib_readLineByte_EOL& useEol_)
{
	if(DB_fileIOLine){
		DB_fileIOLine->useEOL(useEol_);
	}
}
BYTE fafikLib_DB::UseInterpreter(const wxString& interName)
{
	if( interName.empty() ) return -2;
	itemEntry* temp_ie1= DB_interpreter_table->findP(interName, fafikLib_DB::ie_Table, true);
	if( temp_ie1 ){
		thoseDB_settings.useInterpreter= temp_ie1;
	} else {
		return -1;
	}
return 0;
}
BYTE fafikLib_DB::UseInterpreterAuto(const wxString& byFileExt)
{
//	if(byFileExt.empty()) return -2;
//printf(" wavydddddddddddddddddddddddib byFileExt = |%s|\n", byFileExt.ToStdString().c_str() );
//printf(" wavydddddddddddddddddddddddib sized = %u\n", DB_interpreter_table->sizeTable() );
	bool exitsNow= false;
	for(size_t temp_inter=0; temp_inter<DB_interpreter_table->sizeTable(); temp_inter++){
		if(exitsNow) break;
		//contains types {fafikLib_PRON, lua tables, java tables}
//		itemEntry &temp_ie1= (*DB_interpreter_table->at(temp_inter))["opensFiles"];
		itemEntry *temp_ie1= DB_interpreter_table->at(temp_inter)->find_linear("opensFiles", true);
//printf(" hhhhhhhhhhhhhhhhhh temp_ie1 = %u\n", temp_ie1 );
//		if(temp_ie1.reset_invalidSearch())
		if(temp_ie1){
			if(temp_ie1->isValue() && !temp_ie1->isTable() ){
				if(byFileExt.IsSameAs( temp_ie1->value, false) ){
//printf("picked inter = %i", temp_inter);
					thoseDB_settings.useInterpreter= DB_interpreter_table->at(temp_inter);
					exitsNow= 1;
					break;
				}
			}
			for(size_t temp_ext=0; temp_ext<temp_ie1->sizeTable(); temp_ext++){
				if(byFileExt.IsSameAs( temp_ie1->at(temp_ext)->value, false) ){
//printf("picked inter = %i", temp_inter);
//printf("picked inter = %s", DB_interpreter_table->at(temp_inter)->key.ToStdString().c_str() );
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

int fafikLib_DB::loadInterpreterFile(fafikLib_readLineByte_base* item, const wxString& overwriteInterpreter)
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
	fafikLib_readLineByte_file iFromFile(file_path, true);
	return loadInterpreterFile(&iFromFile, overwriteInterpreter);
}
bool fafikLib_DB::itemEntry_isSame(const itemEntry& item_a, const itemEntry& item_b)
{
//printf(" &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& keys |%s| ?= ",item_a.key.ToStdString().c_str());
//printf("|%s|\n",item_b.key.ToStdString().c_str());
    if((item_a.key== item_b.key) &&
		(item_a.type_stores== item_b.type_stores) &&
			(item_a.value== item_b.value)
	){
//printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& return true\n");
		return true;
	}
return false;
}
fafikLib_DB::itemEntry *fafikLib_DB::itemEntry_getExistOrNew(wxVector<itemEntry*> &items_a, itemEntry &item_b)
{
	for(size_t temp_tu=0; temp_tu<items_a.size(); temp_tu++){
		if( itemEntry_isSame( *items_a.at(temp_tu), item_b) ) return items_a.at(temp_tu);
	}
	return nullptr;
}
fafikLib_DB::itemEntry *fafikLib_DB::itemEntry_getExistOrNew(itemEntry &Table_items_a, itemEntry &item_b)
{
	if(Table_items_a.isTable())
		return itemEntry_getExistOrNew(Table_items_a.sub_itemTable, item_b);
	return nullptr;
}

wxArrayString fafikLib_DB::getParentTree_asArray(const itemEntry &IE_in)
{
	wxArrayString retAStr;
	const itemEntry *getFrom= &IE_in;
	do{
		retAStr.Insert(getFrom->key,0);
		if(getFrom->parent){
			getFrom= getFrom->parent;
		} else break;
	} while(getFrom);
 return retAStr;
}
wxString fafikLib_DB::getParentTree_asStr(const itemEntry &IE_in, const wxString separator)
{
	wxArrayString temp_arrStr = getParentTree_asArray(IE_in);
	wxString ret_str="";
	for(size_t pos_u=0; pos_u<temp_arrStr.size(); pos_u++){
		ret_str.Append(temp_arrStr.Item(pos_u));
		ret_str.Append(separator);
	}
	return ret_str;
}

LONGLONG fafikLib_DB::interpretFile(bool Sort)
{
//[[ new code to activate 2018-08-08
	DB_table.type_stores|= ie_Table;
//Open( "me.n+" );
//DB_fileIO.Open("me.n+","rb");
	interpretStart();
	while( interpretNextLine()== 0 );
	_NextLine_iE= nullptr; //just in case...

	if(Sort)
		DB_table._sort(true);

//itemEntry temp_ie1;
//temp_ie1.linksTo= DB_table.at(0)->at(0)->at(0);
//temp_ie1.type_stores|= ie_link;
//
//DB_table.at(0)->push_back(temp_ie1);
//temp_ie1.Detach();

//wxLogMessage("linked key= |%s|", getParentTree_asStr(*DB_table.at(0)->last()->at(0)->at(0)->at(0)) );
//DB_table.at(0)->last()->type_stores|= ie_link;
//]] end new code

//	DB_fileIO.Open("me.n+","rb");
//	DB_table.type_stores|= ie_Table;
//	interpretNextLine( &DB_table );
//dumpToFile( DB_table.at(0) );
//WriteToNewFile( "interpret and save 2018-08-16.n+" );
//wxRenameFile("DB_To_File.n+", "interpret table(empty).n+");

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

//printf("entry->type_stores = %i\n", entry->type_stores);
//DB_interpreter_table->type_stores|= ie_Table;
//printf("DB_interpreter_table->type_stores = %i\n", DB_interpreter_table->type_stores);
	temp_strAdd=temp_str+fafikLib_Quotation::escapeString(entry->key);//entry->key;
	temp_oFile.Write(temp_strAdd);
	if( entry->isLink() ){
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
		temp_strAdd= fafikLib_Quotation::escapeString(entry->value);
		temp_oFile.Write(temp_strAdd);
		temp_oFile.Write("\n");

	}
return 0;
}

wxArrayString fafikLib_DB::translate_StrToArr(const wxString& findKey_levelRecursive_str, const wxUniChar separator)
{
	size_t pos_dot=0,pos_dot_last=0;
	wxArrayString findKey_levelRecursive;
	while(pos_dot != size_t(-1) ){
//printf("  %u -- ",pos_dot_last);
//printf("%u\n",pos_dot);
		pos_dot_last = pos_dot;
		pos_dot = findKey_levelRecursive_str.find(separator,pos_dot_last);
		if(pos_dot_last == size_t(-1))
			break;
		else {
			findKey_levelRecursive.push_back( findKey_levelRecursive_str.SubString(pos_dot_last,pos_dot-1) );
//printf("  findKey_levelRecursive.last = %s\n",findKey_levelRecursive.Last().ToStdString().c_str());
			if( pos_dot== size_t(-1))break;
			pos_dot++;
		}
	}
return findKey_levelRecursive;
}


//fafikLib_DB::itemEntry* fafikLib_DB::getItem_byKey(wxString findKey_levelRecursive_str, const wxUniChar separator, bool searchFromLastSearch)
//{
//	return getItem_byKey_rec(DB_table, translate_StrToArr(findKey_levelRecursive_str, separator),0,2*searchFromLastSearch);
//}
//fafikLib_DB::itemEntry* fafikLib_DB::getItem_byKey(wxArrayString findKey_levelRecursive, bool searchFromLastSearch)
//{
//	return getItem_byKey_rec(DB_table, findKey_levelRecursive,0,2*searchFromLastSearch);
//}
//fafikLib_DB::itemEntry* fafikLib_DB::getItem_byKey_FromInterpreter(wxString findKey_levelRecursive_str, const wxUniChar separator, bool searchFromLastSearch)
//{
//	return getItem_byKey_rec(DB_interpreter_table, translate_StrToArr(findKey_levelRecursive_str, separator),0,1*searchFromLastSearch);
//}
//fafikLib_DB::itemEntry* fafikLib_DB::getItem_byKey_FromInterpreter(wxArrayString findKey_levelRecursive, bool searchFromLastSearch)
//{
//	return getItem_byKey_rec(DB_interpreter_table, findKey_levelRecursive,0,1*searchFromLastSearch);
//}

//fafikLib_DB::itemEntry* fafikLib_DB::getItem_byKey_rec(itemEntry &Table, const wxArrayString& findKey_levelRecursive, size_t currPos, BYTE is_interpreter)
//{
////	itemEntry_iterate itemIters( &Table );
////	return itemIters.find( findKey_levelRecursive, true );
//
//
//	last_getItem_byKey* lastItem= nullptr;
////	switch (is_interpreter)
////	{
////	case 1:
////		lastItem = &last_getItem_byKey_interpreter;
////	break;
////	case 2:
////		lastItem = &last_getItem_byKey_DB;
////	break;
////
////	default:
////	break;
////	}
////
////  if(currPos==0 && lastItem){
/////// TODO (main#9#06/09/18): lastItem->str_arr
////	size_t goodOnes=0;
//////	for(size_t temp_u=0; temp_u<lastItem->str_arr.size();temp_u++){
////	for(size_t temp_u=0; temp_u<lastItem->itemEntry_p.size();temp_u++){
//////		if( lastItem->str_arr.Item(temp_u) == findKey_levelRecursive.Item(temp_u) ){
////		if( lastItem->itemEntry_p.at(temp_u)->key == findKey_levelRecursive.Item(temp_u) ){
////			goodOnes++;
////		}
////		else break;
////	}
////	if( goodOnes ){
//////		lastItem->str_arr.size()
//////			goodOnes
//////		findKey_levelRecursive.size()
////
//////		for(size_t temp_u=0; temp_u<lastItem->str_arr.size() ;temp_u++ ){
//////
//////		}
//////		< findKey_levelRecursive.size()-1
//////		1 3.2
////		return getItem_byKey_rec(*lastItem->itemEntry_p.at(goodOnes-1),findKey_levelRecursive,goodOnes,is_interpreter);
////	}
////  }
////
////
//	for(size_t temp_u=0; temp_u<Table.sub_itemTable.size();temp_u++){
////printf("currPos = %u\n",currPos);
////printf("s.size() = %u\n",findKey_levelRecursive.size());
//		if( Table.sub_itemTable.at(temp_u)->key == findKey_levelRecursive.Item(currPos) ){
//			if(currPos == findKey_levelRecursive.size()-1){
//				return Table.sub_itemTable.at(temp_u);
//			} else if(Table.sub_itemTable.at(temp_u)->isTable()){
//			  if(lastItem){
////				lastItem->str_arr.resize(currPos+1);
////				lastItem->str_arr[currPos] = findKey_levelRecursive.Item(currPos);
//				lastItem->depth = currPos;
//				lastItem->itemEntry_p.resize(currPos+1);
//				lastItem->itemEntry_p.at(currPos) = Table.sub_itemTable.at(temp_u);
//			  }
//
////printf("getItem_byKey_rec\n");
//				itemEntry * returns =
//					getItem_byKey_rec(*Table.sub_itemTable.at(temp_u),findKey_levelRecursive,currPos+1,is_interpreter);
//				if(!returns) continue;
//				else return returns;
//			}
//		} else{
//			continue;
//		}
//
//	}
//
//return nullptr;
//}


bool fafikLib_DB::interpretNextLine_TLine(wxString &in_line, fastFind_struct *fFs_AndEntry_in, wxVector<makesSearch> &lineInterpreterResults, size_t &pos_last, itemEntry &AdditionalTableInfo, wxString AdditionalStrInfo)
{
	bool CanExitLoopOr=false;
	size_t iMDA= curr_interpreter.elementMaxDistanceApart;
	if(pos_last== 0) iMDA= 0;

	if( !lineInterpreterResults.empty() && lineInterpreterResults.back().pos_to > in_line.Length() ) iMDA= in_line.Length();

//printf("  in_line = |%s|\n", in_line.ToStdString().c_str());
//printf("  in_line.sub = |%s|\n", in_line.substr(pos_last).ToStdString().c_str());

	for(size_t temp_uu=0; temp_uu<fFs_AndEntry_in->keys.size(); temp_uu++){	//or subEntries
		if(CanExitLoopOr) break;
		fastFind_struct_el* temp_tab_uu =& fFs_AndEntry_in->keys.at(temp_uu);
//		size_t last_pos_last= pos_last;

		const BYTE uu_elType= temp_tab_uu->element_type &(ff_mode_perc-1);
//if( (fFs_AndEntry_in->element_type &(ff_mode_perc-1) )== ff_bytes) return true;

		makesSearch mkS;
		mkS.interpreter_key= temp_tab_uu->key;
		mkS.pos_from= pos_last;
		mkS.element_type= temp_tab_uu->element_type;
//printf("mkS.element_type = %i\n", mkS.element_type);

		if(temp_tab_uu->element_type & ff_mode_perc){
			if(temp_tab_uu->key == "%u"){
//printf("!!!!!!!!!!!!! going %%u\n");
	//				list key
	//					which quote?		" '
	//					unquote of quote	" '
	//				list long
	//					which error?, all by default
				bool singleQuote = 0;
				BYTE remQuot=false;
				if(temp_tab_uu->key_stringArgs.size()>0){
					singleQuote = (temp_tab_uu->key_stringArgs.Item(0) == "\'");
				}
				if(temp_tab_uu->key_stringArgs.size()>1 && temp_tab_uu->key_stringArgs.Item(1)=="\"") remQuot=1;
				if(temp_tab_uu->key_stringArgs.size()>1 && temp_tab_uu->key_stringArgs.Item(1)=="\'") remQuot=2;

				fafikLib_Quotation temp_unqote( in_line, singleQuote, pos_last, remQuot );
				wxString extracted;
				bool isok=0;

				for(BYTE i=0; i<2; i++){
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
//printf("retry now... %i <=? %i\n", extracted.Length(), iMDA);
////printf("da %i\n", curr_interpreter.elementMaxDistanceApart );
//getchar();
						continue; //has a chance
					} else break;
				} // 2 times try

				if(isok){
//printf("!!!!!!!!!!! has has ok %%u\n");
					CanExitLoopOr= isok;
					pos_last = temp_unqote.getPosStartNext();
//printf("!!!!!!!!!!! getPosStartNext = %u\n", pos_last);

					mkS.interpreted_value = extracted;
					mkS.pos_to = pos_last;

	//					lineInterpreterResults.push_back(mkS);

				}
			}
			else if(temp_tab_uu->key == "%S"){
				mkS.pos_to = -1;
				pos_last ++; //just in case increase it by 1
				CanExitLoopOr = true;
				mkS.interpreted_value = in_line.substr(mkS.pos_from);
//printf("search for $S |%s|\n", mkS.interpreted_value.ToStdString().c_str());
//printf("\t in_line |%s|\n", in_line.ToStdString().c_str());
			}
			else if(temp_tab_uu->key == "%k"){
				size_t pos_lowest= -1;
				for(size_t keyArg=0; keyArg<temp_tab_uu->key_stringArgs.size(); keyArg++){
//					size_t pos_lowest_temp= in_line.find(temp_tab_uu->key_stringArgs.Item(keyArg),pos_last);
					size_t pos_lowest_temp= find_RestrictedString( in_line, temp_tab_uu->key_stringArgs.Item(keyArg), pos_last, pos_lowest );
					if(pos_lowest_temp < pos_lowest) pos_lowest = pos_lowest_temp;
					if(pos_lowest==0) break;
				}

				if(pos_lowest != size_t(-1) || temp_tab_uu->key_stringArgs.empty() ){//if found or will find(hopefully)
					CanExitLoopOr= true;
					mkS.pos_to= pos_lowest;
					mkS.interpreted_value= in_line.substr(mkS.pos_from, mkS.pos_to );
				}

			}
			else if(temp_tab_uu->key == "%#"){
				wxLogError("hash table not yet implemented... sorry");
			}
			else if(temp_tab_uu->key == "%n"){	// used in bytes
//printf("using %%n???????????????????????????????????????????????????\n");
				size_t temp_pos1= in_line.find_first_of( _digits_list, pos_last );
				if(temp_pos1> pos_last+iMDA){
					if( temp_pos1==size_t(-1) && !temp_tab_uu->args_L.empty()){
						CanExitLoopOr= true;
						mkS.interpreted_value= atos::toWxString(temp_tab_uu->args_L.Item(0));
						mkS.pos_from= pos_last;
						mkS.pos_to= pos_last;
						pos_last++;
					} else
						break;
				}

				size_t temp_pos2= in_line.find_first_not_of( _digits_list, temp_pos1);
				wxString temps_strSB= in_line.SubString( temp_pos1, temp_pos2-1 );
//printf("pos 1 = %i\n", temp_pos1);
//printf("pos 2 = %i\n", temp_pos2);
//printf("temps_strSB = |%s|\n", temps_strSB.ToStdString().c_str());
				long long temp_LL=0;
				if( temps_strSB.ToLongLong(&temp_LL) ){
					CanExitLoopOr= true;
					pos_last= temp_pos2;	//after last digit
					mkS.interpreted_value= atos::toWxString(temp_LL);
					mkS.pos_from= temp_pos1;
					mkS.pos_to= temp_pos2;
					if(temp_pos2>in_line.Length()) mkS.pos_to= in_line.Length()+1;
//printf(" LL = %s\n", temp_wLL.ToString().ToStdString().c_str());
				}
//getchar();
			}
		}
		else if(uu_elType == ff_EOL){	//simple check if EOL
			iMDA= 0;
//printf("koko e.2 1\n");
			if(!temp_tab_uu->args_L.empty()) iMDA= abs( temp_tab_uu->args_L.Item(0) );
			if( pos_last>= (in_line.Length()- iMDA) ) CanExitLoopOr=true;
			else if( !lineInterpreterResults.empty() && lineInterpreterResults.back().pos_to >= in_line.Length() ) CanExitLoopOr=true;
//printf("koko e.2 2 end\n");
		}
		else {
//			size_t foundOn = in_line.find(temp_tab_uu->key,pos_last);
//			if( !lineInterpreterResults.empty() && lineInterpreterResults.back().pos_to > in_line.Length() ) iMDA= in_line.Length();
			size_t foundOn = find_RestrictedString(in_line, temp_tab_uu->key, false, pos_last, pos_last+ temp_tab_uu->key.Length() +iMDA);
			if( (foundOn<= pos_last+iMDA) &&
				//can not find `"` between, or its useless
				find_RestrictedString(in_line, "\"", false, pos_last, foundOn)== size_t(-1)
			  ){
				CanExitLoopOr = true;
				mkS.interpreted_value = temp_tab_uu->key;
				mkS.pos_from = foundOn;
				mkS.pos_to = foundOn + temp_tab_uu->key.Length();
				pos_last= foundOn + temp_tab_uu->key.Length();
			}

		}

		if( uu_elType == ff_v){//voided, so even if false true
			CanExitLoopOr= true;
		}

		if(!lineInterpreterResults.empty()){
			makesSearch &lastSearch= lineInterpreterResults.back();
			if(lastSearch.pos_to == size_t(-1)){
				lastSearch.pos_to = mkS.pos_from;
				lastSearch.interpreted_value = in_line.substr(lastSearch.pos_from, lastSearch.pos_to - lastSearch.pos_from );
			}
		}


//		if( !CanExitLoopOr &&( uu_elType== ff_key || curr_interpreter.ff_assign[0].find(mkS.interpreter_key)!= size_t(-1) )&& AdditionalTableInfo.isAuto_key() ){	//auto_key 2018-07-29-10.50
//			CanExitLoopOr= true;
//		}// solution for now 2018-07-29

//printf("fFs_AndEntry_in->keys.size() = %u --------------/////>>>>>\n", fFs_AndEntry_in->keys.size());
		if(CanExitLoopOr){
			//after successful "or" find, precede...
//printf("  mkS.key = |%s|\n", mkS.interpreter_key.ToStdString().c_str());
//printf("  mkS.value = |%s|\n", mkS.interpreted_value.ToStdString().c_str());
//printf("  lineInterpreterResults.size = %u\n", lineInterpreterResults.size());
			lineInterpreterResults.push_back(mkS);	//save this "or" iteration
			return CanExitLoopOr;
		}
//		 else
//printf(" CanExitLoopOr = %i-------------////\n", CanExitLoopOr);
//printf(" mkS.value = |%s| -------------////\n", mkS.interpreted_value.ToStdString().c_str());
//printf(" mkS.key = |%s| -------------////\n", mkS.interpreter_key.ToStdString().c_str());
//printf(" mkS.element_type = %i -------------////\n", mkS.element_type);


	}	//or subEntries

//printf(" exited for() loop fFs_AndEntry_in //or\n");
	//does not match the search
	return CanExitLoopOr;
}


fafikLib_DB::itemEntry* fafikLib_DB::interpretNextLine_TEnter(itemEntry* Table, wxString in_line, const bool useAutoKey)
{
	wxVector<makesSearch> lineInterpreterResults;
	itemEntry_Bytes* optionBytes= nullptr;
//	fafikLib_virtualByteFile *optionBytes= nullptr;

	size_t pos_last=0;

	for(size_t temp_u=0; temp_u<curr_interpreter.ff_table_enter.size(); temp_u++){	//and entries
		if(useAutoKey &&
			temp_u<= size_t(curr_interpreter.isAssignAt_ff_table_enter) ) continue;
		fastFind_struct* temp_tab_u =& curr_interpreter.ff_table_enter.at(temp_u);

//printf("ff el type = %i\n", temp_tab_u->element_type);
		if( (temp_tab_u->element_type &(ff_mode_perc-1)) == ff_bytes){
			optionBytes= interpretNextLine_TBytes(in_line, Table, lineInterpreterResults, pos_last );//all origin (by reference)
			continue;
		}
		bool CanExitLoopOr=interpretNextLine_TLine(in_line, temp_tab_u, lineInterpreterResults, pos_last, *Table);
//printf("  tableEnter  lineInterpreterResults.size = %u\n", lineInterpreterResults.size());

		if( !CanExitLoopOr ){
			if(optionBytes){	//restore DB_fileIO-position
//                DB_fileIO.Seek( optionBytes->pos_start );
//				ByteData_itemEntry_all.pop_back();
				delete optionBytes;
				optionBytes= nullptr;
			}

			if( !useAutoKey && Table->isAuto_key() ){
				if( curr_interpreter.isAssignAt_ff_table_enter< 0 ) return nullptr;
				return interpretNextLine_TEnter( Table, in_line, true );
			}
//printf("CanExitLoopOr = %i\n", CanExitLoopOr);
//wxThread::Sleep(50000000);

			return nullptr;
		}

	}	//and entries
		//after all "and" entries, precede to combine & return*
	if(useAutoKey && lineInterpreterResults.empty())	return nullptr;

// @note (main#9#07/26/18): now combining all the information of finds
//	itemEntry *temp_ie_p= new itemEntry;
	 ///assign optionBytes if exists, else make new
	itemEntry &temp_ie= optionBytes ? *optionBytes : *(new itemEntry);

	if(optionBytes && optionBytes->Length()!= 0)
		DB_fileIOLine->Seek( optionBytes->pos_end );

//	temp_ie.ByteData_itemEntry_selfEntry= optionBytes;
	temp_ie.type_stores|= ie_Table;
	temp_ie.depth= Table->depth+1;
	temp_ie.parent= Table;
//printf("  tableEnter  lineInterpreterResults.size = %u\n", lineInterpreterResults.size());
//wxThread::Sleep(3000);
	for(size_t LIR=0; LIR<lineInterpreterResults.size(); LIR++){
		makesSearch &p_LIR= lineInterpreterResults.at(LIR);
		BYTE ElType= (p_LIR.element_type &(ff_mode_perc-1));

//printf("i_val = |%s|\n", p_LIR.interpreted_value.ToStdString().c_str());

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
//			temp_ie.value.Shrink();
		}
		else if(ElType == ff_key){
			temp_ie.key+= p_LIR.interpreted_value;
//			temp_ie.key.Shrink();
		}
		else if(ElType == ff_e){
//			printf("e empty...");
		}


	}
//	if(optionBytes)
//		optionBytes->FileName= temp_ie.key;

//	BytesStored+= temp_ie.value.size() ;
//	BytesStored+= temp_ie.key.size() ;

	Table->sub_itemTable.push_back(&temp_ie);
	return Table->sub_itemTable.back(); //ok at this point

//return -1;
}
fafikLib_DB::itemEntry* fafikLib_DB::interpretNextLine_TExit(itemEntry* Table, wxString in_line)
{
//	BYTE retVal=-1;
	wxVector<makesSearch> lineInterpreterResults;
	itemEntry_Bytes* optionBytes= nullptr;

	size_t pos_last=0;

	for(size_t temp_u=0; temp_u<curr_interpreter.ff_table_exit.size(); temp_u++){	//and entries
		fastFind_struct* temp_tab_u =& curr_interpreter.ff_table_exit.at(temp_u);
//printf("koko e.1\n");
		if(temp_tab_u->element_type & ff_bytes){
			optionBytes= interpretNextLine_TBytes(in_line, Table, lineInterpreterResults, pos_last );///all origin (by reference)
//			struct
//			{
//				returned
//			};
			continue;
		}
//printf("koko e.1 1\n");

		bool CanExitLoopOr=interpretNextLine_TLine(in_line, temp_tab_u, lineInterpreterResults, pos_last, *Table);
//printf("koko e.1 2\n");

		if( !CanExitLoopOr ){
			if(optionBytes){	//restore DB_fileIO-position
//				ByteData_itemEntry_all.pop_back();
				delete optionBytes;
				optionBytes= nullptr;
			}
			return nullptr;
		}


	}	//and entries
		//after all "and" entries, precede to combine & return*
//printf("koko e.1 end\n");

	for(size_t temp_u=0; temp_u<lineInterpreterResults.size(); temp_u++){
		if((lineInterpreterResults.at(temp_u).element_type &(ff_mode_perc-1) ) ==ff_key){
			if(lineInterpreterResults.at(temp_u).interpreted_value == Table->key){	//ok
			} else {
				if(optionBytes){	//restore DB_fileIO-position
//					ByteData_itemEntry_all.pop_back();
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
//		ByteData_itemEntry_all.pop_back();
		delete optionBytes;
		optionBytes= nullptr;
	}
	else if(optionBytes && optionBytes->Length()!= 0)
		DB_fileIOLine->Seek( optionBytes->pos_end );
//	if(optionBytes)
//		optionBytes->FileName= temp_ie.key;

//	if(thoseDB_settings.OpenMode== mode_append){
//		Table->sub_itemTable.
//	}

	return Table->parent; //ok at this point, return up a level
}
fafikLib_DB::itemEntry* fafikLib_DB::interpretNextLine_TKey(itemEntry* Table, wxString in_line, const bool useAutoKey)
{
//	bool useAutoKeyP= useAutoKey;
	wxVector<makesSearch> lineInterpreterResults;
	size_t pos_last=0;

	for(size_t temp_u=0; temp_u<curr_interpreter.ff_keyValue.size(); temp_u++){	//and entries
		if(useAutoKey &&
		    temp_u<= size_t(curr_interpreter.isAssignAt_ff_keyValue) ) continue;
		fastFind_struct* temp_tab_u =& curr_interpreter.ff_keyValue.at(temp_u);
//		if( useAutoKeyP )
//		{
//			if(temp_tab_u->isAssign) useAutoKeyP= false;
//
//printf("until AutoKey continue... =%i\n", useAutoKeyP);
////getchar();
//			continue;
//		}

		bool CanExitLoopOr=interpretNextLine_TLine(in_line, temp_tab_u, lineInterpreterResults, pos_last, *Table);
		if( !CanExitLoopOr ){
			if( !useAutoKey && Table->isAuto_key() ){
				if( curr_interpreter.isAssignAt_ff_keyValue< 0 ) return nullptr;
				return interpretNextLine_TKey( Table, in_line, true );
			}
//printf("CanExitLoopOr = %i\n", CanExitLoopOr);
//wxThread::Sleep(50000000);
//printf(" return nullptr in interpretNextLine_TKey()\n");
			return nullptr;
		}

	}	//and entries
		//after all "and" entries, precede to combine & return*

	if(useAutoKey && lineInterpreterResults.empty())	return nullptr;

// @note (main#9#07/26/18): now combining all the information of finds
	itemEntry &temp_ie= *(new itemEntry);
//	temp_ie.type_stores|= ie_Table;
	temp_ie.depth= Table->depth+1;
	temp_ie.parent= Table;

//printf("lineInterpreterResults.size() = %u\n", lineInterpreterResults.size());
//printf("curr_interpreter.ff_keyValue.size() = %u\n", curr_interpreter.ff_keyValue.size());
//printf("str |%s|\n", in_line.ToStdString().c_str() );


	for(size_t LIR=0; LIR<lineInterpreterResults.size(); LIR++){
		makesSearch &p_LIR= lineInterpreterResults.at(LIR);
		BYTE ElType= (p_LIR.element_type &(ff_mode_perc-1));

//printf("p_LIR = %u\n", p_LIR.element_type );
//printf("p_LIR = key |%s|\n", p_LIR.interpreter_key.ToStdString().c_str() );
//printf("p_LIR = val |%s|\n", p_LIR.interpreted_value.ToStdString().c_str() );

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
//			temp_ie.value.Shrink();
//printf("key_val value = |%s|\n", temp_ie.value.ToStdString().c_str() );
		}
		else if(ElType == ff_key){
			temp_ie.key+= p_LIR.interpreted_value;
//			temp_ie.key.Shrink();
		}
		else if(ElType == ff_e){
//			printf("e empty...");
		}


	}

//	BytesStored+= temp_ie.value.size() ;
//	BytesStored+= temp_ie.key.size() ;

	Table->sub_itemTable.push_back(&temp_ie);
//printf("added key_val.... ???\n");
//getchar();
	return Table; //ok at this point, return back a level

//return -1;
}


fafikLib_DB::itemEntry_Bytes* fafikLib_DB::interpretNextLine_TBytes(wxString &in_line, itemEntry* Table, wxVector<makesSearch> &lineInterpreterResults, size_t &pos_last )
{
	wxVector<makesSearch> this_lineInterpreterResults;

//printf("table_bytes# = %u\n", curr_interpreter.ff_table_bytes.size());
	for(size_t temp_tb=0; temp_tb<curr_interpreter.ff_table_bytes.size(); temp_tb++){
		fastFind_struct &temp_ffs= curr_interpreter.ff_table_bytes.at(temp_tb);
//printf(" temp_ffs.str = |%s|\n", temp_ffs.keys.at(0).key.ToStdString().c_str() );

		if(interpretNextLine_TLine(in_line, &temp_ffs, this_lineInterpreterResults, pos_last, *Table)){

//			continue;
		} else {
			return nullptr;
		}
	}

	itemEntry_Bytes& tempByteItem= *(new itemEntry_Bytes);
	tempByteItem.Open(DB_fileIOLine);
//	fafikLib_virtualByteFile temp_vBf;
	long long temp_LL=0;
	bool byte_type= false;
	if(curr_interpreter.overwrite_byte_type!= -1) byte_type= curr_interpreter.overwrite_byte_type;

	for(size_t temp_ob=0; temp_ob<this_lineInterpreterResults.size(); temp_ob++){
		makesSearch &t_ires= this_lineInterpreterResults.at(temp_ob);
		if(t_ires.element_type %ff_mode_perc == ff_bytes){
			if(t_ires.interpreter_key== "%n" ) {
				t_ires.interpreted_value.ToLongLong( &temp_LL);
	//			Table
			} else if(t_ires.interpreter_key== "s" ) {
				byte_type= 0;
			} else if(t_ires.interpreter_key== "l" ) {
				byte_type= 1;
			}
		}
	}

	if(byte_type && (temp_LL>=0 && temp_LL<=8) ){ //byte_type of long assign(number of bytes containing data length)
		if(temp_LL== 0){
//			temp_vBf.length= 0;
			tempByteItem.pos_start= DB_fileIOLine->Tell();
			tempByteItem.pos_end= tempByteItem.pos_start;
		}
		else {
			BYTE temp_chars[8]= {0};
			size_t read1= DB_fileIOLine->Read( &temp_chars[0], temp_LL);
			if(read1== size_t(temp_LL)){
				long long *temp_LL2_p= static_cast<long long*>(static_cast<void*>(temp_chars) );
//				temp_vBf.length= *temp_LL2_p;
				tempByteItem.pos_start= DB_fileIOLine->Tell();
				tempByteItem.pos_end= tempByteItem.pos_start+ *temp_LL2_p;
//				temp_LL2_p= nullptr;
//				if(temp_vBf.length!= 0)
//					DB_fileIO.Seek( temp_vBf.length, wxFromCurrent );
			}
			else {
wxLogError("Could not load %i Bytes from DB file, pos= %s.", temp_LL, atos::toWxString(DB_fileIOLine->Tell()) );
			}
		}
	}
	else {
//		temp_vBf.length= temp_LL;
		tempByteItem.pos_start= DB_fileIOLine->Tell();
		tempByteItem.pos_end= tempByteItem.pos_start+ temp_LL;
//		if(temp_vBf.length!= 0)
//			DB_fileIO.Seek( temp_vBf.length, wxFromCurrent );
	}

//printf("after interpretNextLine_TBytes;\n");
//getchar();
//itemEntry::push_back()

//	temp_vBf._bytes_fileIO= &DB_fileIO;
//	ByteData_itemEntry_all.push_back(temp_vBf);
//	return &ByteData_itemEntry_all.back();
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
//printf(" rading 1 %u\n", DB_interpreter_table->findP("fafikLib_PRON", ie_Table, true)  );
//dumpToFile( DB_interpreter_table->findP("fafikLib_PRON", ie_Table, true ) );
//printf(" dumped file\n");
	if( DB_fileIOLine->readLineByte(&lineContent) )	//fafikLib_readLineByte(DB_fileIO, )
	{
//printf(" rading end\n");
//printf(" lineContent no trim= |%s|\n", lineContent.ToStdString().c_str());
//printf("lineContent no trim= %i\n",  DB_fileIOLine.pass_useOnlyThisEndOL );

		bool skippInter= false;
//		wxVector<makesSearch> lineInterpreterResults;

		fafikLib_Quotation::lineBeginsWithNotBlank(lineContent);
//printf(" lineContent= |%s|\n", lineContent.ToStdString().c_str() );
//printf(" specialEntry= %u\n", curr_interpreter.specialEntry );
//printf(" table_= %u\n", curr_interpreter.ff_table_enter.size() );
//printf(" table_= %u\n", curr_interpreter.ff_table_exit.size() );
//printf(" table_= %u\n", curr_interpreter.ff_table_bytes.size() );
		if(curr_interpreter.specialEntry){
			for(size_t temp_se=0; temp_se<curr_interpreter.specialEntry->sizeTable(); temp_se++){
				itemEntry* entry_se= curr_interpreter.specialEntry->at(temp_se);

// TODO (main#9#07/28/18): implement specialEntry
				if(entry_se->key== "comment"){
					for(size_t getI=0; getI<entry_se->sizeTable(); getI++){
						if(lineContent.StartsWith( entry_se->at(getI)->value ) ){
//							itemEntry &temp_ie;
//							temp_ie.value= lineContent;
//							temp_ie.type_stores= ie_lore;

							if(p_out_type) *p_out_type= 1;
							_NextLine_iE->Add( "", lineContent, ie_lore );
//							temp_ie.Detach();
							skippInter= true;
							break;
						}
					}
				} else if( entry_se->value== lineContent ){
					skippInter= true;
					if(entry_se->key== "link"){
					}
					else if(entry_se->key== "auto_key"){
						_NextLine_iE->type_stores|= ie_auto_key;
					}
					else if(entry_se->key== "copy"){
					}

//printf("  found specialEntry ... |%s|\n",lineContent.ToStdString().c_str());
//					fafikLib_readLineByte(DB_fileIO, &lineContent);
//printf("   = |%s|\n",lineContent.ToStdString().c_str());
					break;
				}
			}
		}

//		if(skippInter) continue;
		if(skippInter) return 0;
//printf("lineContent = |%s|\n", lineContent.ToStdString().c_str());


//fafikLib_Quotation::extractNext();

//"content"="value"
//["table-key"]=
//{
//	"sub-content"="value"
//}
//printf("curr_interpreter.recognitionPriority.size = %u\n", curr_interpreter.recognitionPriority.size());
		for(size_t priorityLevel=0; priorityLevel<3; ++priorityLevel )
		{
//printf("  get # %u =", priorityLevel );
//printf("  "+curr_interpreter.recognitionPriority[priorityLevel]+"\n" );
			if(curr_interpreter.recognitionPriority[priorityLevel] == "enter"){
				itemEntry* temp_ie= interpretNextLine_TEnter(_NextLine_iE, lineContent);
				if(temp_ie){//ok
//printf("cough table, ok;\n");
//printf("|%s|\n", temp_ie->key.ToStdString().c_str());
					if(p_out_type) *p_out_type= 2;
					_NextLine_iE= temp_ie;
					break;	//ok done
				}
//printf("not a table;\n");
			}
			else if(curr_interpreter.recognitionPriority[priorityLevel] == "exit"){
				itemEntry* temp_ie= interpretNextLine_TExit(_NextLine_iE, lineContent);
				if(temp_ie){//ok
//printf("cough exit, ok;\n");
					if(p_out_type) *p_out_type= 0;
					_NextLine_iE= temp_ie;
					break;	//ok done
				}
//printf("not a exit;\n");
			}
			else if(curr_interpreter.recognitionPriority[priorityLevel] == "key"){
				itemEntry* temp_ie= interpretNextLine_TKey(_NextLine_iE, lineContent);
				if(temp_ie){//ok
//printf("cough key Value, ok;\n");
					if(p_out_type) *p_out_type= 1;
					break;	//ok done
				}
//printf("not key Value;;\n");
			} else {
wxLogError("error recognitionPriority does not match search;\n |%s|\n", curr_interpreter.recognitionPriority[priorityLevel]);
			}
		}//for recognitionPriority
		return 0;
	}
	else {
		while( true ){
			_NextLine_iE->type_stores|= ie_TableNotClosed;
//printf("!!!!!!!!!!!!! TableNotClosed  _NextLine_iE.key= |%s|\n",_NextLine_iE->key.ToStdString().c_str());
			if(!_NextLine_iE->parent) break;
			_NextLine_iE= _NextLine_iE->parent;
		}
//getchar();
	}
	return 1;
}


fafikLib_DB::itemEntry& fafikLib_DB::itemEntry::operator[](const wxString &item)
{
 if(invalidSearch){
	return *this;
 }

	for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++){
		if(this->sub_itemTable.at(temp_u)->key==item){
			return *(this->sub_itemTable.at(temp_u) );
		} else {
			continue;
		}
	}
invalidSearch= true;
return *this;
}


fafikLib_DB::itemEntry* fafikLib_DB::itemEntry::Add_back(const wxString& key_, const wxString& value_, BYTE type_stores_)
{
	if( value_.size() ) type_stores_|= ie_value;
	itemEntry* newBackItem= new itemEntry(key_, value_, type_stores_, depth+1);
	sub_itemTable.push_back(newBackItem);
	return newBackItem;
}
fafikLib_DB::itemEntry::iterator fafikLib_DB::itemEntry::find( const wxString &item_byKey, itemStores type, bool caseInsensitive)
{
	itemEntry templItem(item_byKey, type, ie_caseInsensitive*caseInsensitive );
	iterator atPos= binaryGetPosOf( templItem );

//for(auto itse: sub_itemTable) {
//	printf("   |%s|\n", wxString(itse->key).ToStdString().c_str() );
// }


	if( binaryGetPosOf_exists( atPos, templItem ) ){
		return atPos;
	}
return sub_itemTable.end();
}
fafikLib_DB::itemEntry* fafikLib_DB::itemEntry::find_linear(const wxString &item_byKey, bool caseInsensitive)
{
	for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++)
	{
		if( item_byKey.IsSameAs( this->sub_itemTable.at(temp_u)->key, (!caseInsensitive) ) ){
//			if(outFoundPos) (*outFoundPos)= temp_u;
//printf(" find_linear(%s) at %u in |%s|\n", item_byKey.ToStdString().c_str(), temp_u, key.ToStdString().c_str() );
			return (this->sub_itemTable.at(temp_u) );
		} else {
			continue;
		}
	}

//if(outFoundPos) (*outFoundPos)= -1;
return nullptr;
}
fafikLib_DB::itemEntry* fafikLib_DB::itemEntry::find_byValue( const wxString &item_byValue, bool caseInsensitive, size_t* outFoundPos )
{
	for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++)
	{
		if( item_byValue.IsSameAs( this->sub_itemTable.at(temp_u)->value, (!caseInsensitive) ) ){
			if(outFoundPos) (*outFoundPos)= temp_u;
			return (this->sub_itemTable.at(temp_u) );
		} else {
			continue;
		}
	}

if(outFoundPos) (*outFoundPos)= -1;
return nullptr;
}

size_t fafikLib_DB::itemEntry::getPosition() const
{
	if(!parent) return -1;
	for(size_t temp_u=0; temp_u<parent->sub_itemTable.size(); temp_u++)
	{
		if(parent->sub_itemTable.at(temp_u)==this) return temp_u;
	}
return -1;
}

fafikLib_DB::itemEntry* fafikLib_DB::itemEntry::at(const size_t item, const bool auto_key_keyedSearch, const bool resolveLink)
{
	if( item>= this->sub_itemTable.size() ) return nullptr;
	if(auto_key_keyedSearch){	//2018-08-07
		if(!this->isAuto_key_keyed()) return nullptr;
		size_t autoToHere= -1;
		for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++){
			if( this->sub_itemTable.at(temp_u)->isAuto_key_keyed() ) autoToHere++;
			if( autoToHere== item){
//				if(this->sub_itemTable.at(temp_u)->isLink()) return this->sub_itemTable.at(temp_u)->linksTo;
				return this->sub_itemTable.at(temp_u);
			}
		}
		return nullptr;
	} else if( item < this->sub_itemTable.size()) {
//		if(resolveLink && this->sub_itemTable.at(item)->isLink()) return this->sub_itemTable.at(item)->linksTo;
		return this->sub_itemTable.at(item);
	}
return nullptr;
}
//fafikLib_DB::itemEntry* fafikLib_DB::itemEntry::last(const size_t substract_n, const bool resolveLink)
//{
//	if(!this->isTable()) return nullptr;
//	return at( (this->sizeTable()-1)- substract_n, 0, resolveLink );
//}
//fafikLib_DB::itemEntry* fafikLib_DB::itemEntry::get()
//{
////	if(this->isLink()) return this->linksTo;
//	return this;
//}
//const fafikLib_DB::itemEntry* fafikLib_DB::itemEntry::get() const
//{
////	if(this->isLink()) return this->linksTo;
//	return this;
//}


size_t fafikLib_DB::fastFind_struct::find(wxString str) const
{
	for(size_t temp_u=0; temp_u<keys.size(); temp_u++){
		if(keys.at(temp_u).key== str) return temp_u;
	}
return -1;
}


//bool fafikLib_DB::itemEntry::push_back(itemEntry &push)
//{
//	if(this->type_stores & ie_sub_itemTable) {} else return 0;
//	this->sub_itemTable.push_back(&push);
//return true;
//}
fafikLib_DB::itemEntry::~itemEntry()
{
//if(!parent){
//	printf("~itemEntry() dtor, val=%s\n", key.ToStdString().c_str());
// }

	FreeTable();
	size_t temp_pos= getPosition();
	if(temp_pos!=size_t(-1)){
		parent->sub_itemTable.at(temp_pos)= nullptr; //make sure that parent cant acces us anymore(for part delete)
	}

//	type_stores= 0;
//	parent= nullptr;
}
void fafikLib_DB::itemEntry::FreeTable()
{
//	const_iterator
//	sub_itemTable::const_iterator
//	for(size_t temp_u=0; temp_u<sub_itemTable.size(); temp_u++){
	for(fafikLib_DB::itemEntry::const_iterator temp_it= sub_itemTable.begin(); temp_it!= sub_itemTable.end(); ++temp_it){
//		itemEntry *toDel= sub_itemTable[temp_u]; //just to be sure that typeof is itemEntry
		itemEntry *toDel= *temp_it; //just to be sure that typeof is itemEntry
		//no nullptr
		if( !toDel ) continue; //dont try to access nullptr
		toDel->parent= nullptr;	//dont go upwards, we are taking care of this here
		delete toDel;	//run destructor for each
	}
	sub_itemTable.clear();	//clear vector it doesnt point to anything valid anymore
}


void fafikLib_DB::itemEntry::Transfer_Ownership_subItemsTo( itemEntry& transferOwnershipTo )
{
	transferOwnershipTo.FreeTable();
	transferOwnershipTo.sub_itemTable= this->sub_itemTable;
	transferOwnershipTo.type_stores|= ie_Table;

	setSubsParent( &transferOwnershipTo );
	this->Detach();
}
void fafikLib_DB::itemEntry::Transfer_Ownership_subItemsToMe( itemEntry& transferOwnershipFrom )
{
	this->FreeTable();
	this->sub_itemTable= transferOwnershipFrom.sub_itemTable;
	this->type_stores|= ie_Table;

	setSubsParent();
	transferOwnershipFrom.Detach();
}
void fafikLib_DB::itemEntry::setSubsParent(itemEntry* newParent)
{
	for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++ ){
		this->sub_itemTable.at(temp_u)->parent= newParent;
	}
}

//fafikLib_DB::itemEntry* fafikLib_DB::itemEntry::push_back_ptr(itemEntry* push)
//{
////	if(this->type_stores & ie_sub_itemTable) {} else return 0;	//2018-08-13 allowed
//	this->type_stores|= ie_sub_itemTable;
//
////	push->key.Shrink();
////	push->value.Shrink();
//	push->parent= this;
//	this->sub_itemTable.push_back(push);
////	return this->push_back( *push );
//	return push;
//}
bool fafikLib_DB::itemEntry::push_back_cpyTotal(itemEntry &push)
{
//	if(this->type_stores & ie_sub_itemTable) {} else return 0;
	itemEntry &new_ie= *new itemEntry;
	new_ie.key= push.key;
	new_ie.value= push.value;
	new_ie.type_stores= push.type_stores;
	new_ie.parent= this;
	new_ie.depth= this->depth+1;
//printf("%%%% copying |%s| = ",new_ie.key.ToStdString().c_str());
//printf("|%s|\n",new_ie.value.ToStdString().c_str());

	for(size_t temp_u=0; temp_u<push.sizeTable(); temp_u++){
		new_ie.type_stores|= ie_Table;
		new_ie.push_back_cpyTotal( *push.at(temp_u) );
	}
	this->sub_itemTable.push_back(&new_ie);

return true;
}
fafikLib_DB::itemEntry::iterator fafikLib_DB::itemEntry::AddItSmart( itemEntry* item )
{
	if(!item) return this->end();
	if( this->isAuto_key() ){
		item->parent= this; //assign this as parent
		item->type_stores|= ie_auto_key_keyed;
		if(!item->isLore()) item->type_stores|= ie_value;
		sub_itemTable.push_back(item);
		return this->end()-1;
	}
	else if( this->isTable() ){
		item->parent= this; //assign this as parent
		iterator new_Pos= binaryGetPosOf( *item );
		return insert( new_Pos, item );
	}
	return this->end();
}
fafikLib_DB::itemEntry::iterator fafikLib_DB::itemEntry::AddIt( const wxString& key_, const wxString& value_, BYTE type_stores_)
{
	itemEntry *new_IE= new itemEntry( key_, value_, type_stores_, this->depth+1, this );
	if( (!value_.empty()) && !new_IE->isLore() ){
		new_IE->type_stores|= ie_value;
	}
	if(this->isAuto_key()) //you should not try to search through non keyed Table(Array)
		return AddItSmart(new_IE);

	iterator new_Pos= binaryGetPosOf( *new_IE );
	return insert( new_Pos, new_IE );
}
fafikLib_DB::itemEntry* fafikLib_DB::itemEntry::Add( const wxString& key_, const wxString& value_, BYTE type_stores_ )
{
	return *AddIt( key_, value_, type_stores_ );
}
fafikLib_DB::itemEntry* fafikLib_DB::itemEntry::Add( BYTE type_stores_, const wxString& value_ )
{
	if( !this->isAuto_key() ) return nullptr;
	return Add_back( wxEmptyString, value_, type_stores_ | ie_auto_key_keyed );
}
fafikLib_DB::itemEntry* fafikLib_DB::itemEntry::Add( itemEntry* item )
{
	if(!item) return nullptr;
	if( this->isAuto_key() ){
		item->parent= this; //assign parent
		item->type_stores|= ie_auto_key_keyed;
		if(!item->isLore()) item->type_stores|= ie_value;
		sub_itemTable.push_back(item);
		return item;
	}
	else if( this->isTable() ){
		item->parent= this; //assign parent
		iterator new_Pos= binaryGetPosOf( *item );
		insert( new_Pos, item );
		return item;
	}
	return nullptr;
}
fafikLib_DB::itemEntry* fafikLib_DB::itemEntry::AddLink( itemEntry* itemToLink )
{
	 //we want to add this but with old parent
	if(!itemToLink) return nullptr;
	itemEntry* oldParrent= itemToLink->parent;
	iterator new_Pos= binaryGetPosOf( *itemToLink );
	itemToLink->parent= oldParrent;
	itemEntry* retVal= *insert( new_Pos, itemToLink );
	return retVal;
}
//bool fafikLib_DB::itemEntry::moveItem( size_t itemFromPos, size_t itemNewPos )
//{
//	if( this->sizeTable() < 2 ) return false;	//can not swap with self
//
//	if( itemFromPos>= this->sizeTable() ) itemFromPos= this->sizeTable()-1;
//	if( itemNewPos== size_t(-1) ) itemNewPos= this->sizeTable()-2;
//	if( itemNewPos>= this->sizeTable() ) itemNewPos= 0;
//
//	std::swap( this->sub_itemTable.at(itemFromPos), this->sub_itemTable.at(itemNewPos) );
//return true;
//}
//fafikLib_DB::itemEntry* fafikLib_DB::itemEntry::getOrAdd( const wxString& key_, BYTE type_, bool *out_found )
//{
//	itemEntry *temp_ie= this->find(key_);
//	if( !temp_ie ){
//		if( out_found ) *out_found= false;
//		return this->Add( key_, "", type_ );
//
////printf(" this->last() %i\n", this->last()->type_stores);
////getchar();
////		return this->last();
//	}
//	if( out_found ) *out_found= true;
//	return temp_ie;
//}
bool fafikLib_DB::itemEntry::_sort( bool sortAllChildren )
{
	std::sort( begin(), end(), _operLowerB_compareIE_1 );
	if( sortAllChildren ){
		for( iterator itemIt= begin(); itemIt!=end(); ++itemIt ){
			(*itemIt)->_sort( sortAllChildren );
		}
	}
return sortAllChildren;
}
fafikLib_DB::itemEntry::iterator fafikLib_DB::itemEntry::insert(iterator it, itemEntry* item)
{
	item->parent= this;
	return sub_itemTable.insert(it, item);
}
fafikLib_DB::itemEntry::iterator fafikLib_DB::itemEntry::getOrAddIt( const wxString& key_, BYTE type_, bool caseInsensitive, bool *out_found)
{
	itemEntry* templItem= new itemEntry(key_, type_, ie_caseInsensitive*caseInsensitive );

	iterator atPos= binaryGetPosOf( *templItem );
	if( binaryGetPosOf_exists( atPos, *templItem ) ){
		delete templItem;	//free it
		if( out_found ) *out_found= true;
//printf("got item |%s| %u\n", key_.ToStdString().c_str(), begin()-atPos );
		return atPos;
	}
//	if( atPos==end() ) --atPos;
//printf("added item |%s| %u\n", key_.ToStdString().c_str(), begin()-atPos );

	//add
	if( out_found ) *out_found= false;
	type_stores|= ie_Table;
//printf(" adds |%s| at %u end? %u\n", key_.ToStdString().c_str(), getIndex(atPos), atPos==end() );
	return insert( atPos, templItem);	//add it, lock it
}
fafikLib_DB::itemEntry *fafikLib_DB::itemEntry::getByType( BYTE by_type, size_t from_pos_ )
{
	for(size_t temp_u=from_pos_; temp_u<this->sub_itemTable.size(); temp_u++)
	{
		if( (this->sub_itemTable.at(temp_u)->type_stores &by_type)==by_type ){	//if trimmed== requested
			return this->sub_itemTable.at(temp_u);
		}
	}
	if( !this->parent ) return nullptr;
	return this->parent->getByType( by_type, this->getPosition()+1 );
}

bool fafikLib_DB::itemEntry::removeItem(size_t by_pos, bool silent)
{
	return removeItem( begin()+by_pos, silent );
}
bool fafikLib_DB::itemEntry::removeItems(iterator it1, iterator it2, bool silent)
{
	for( iterator currIt= it1; currIt!= it2; ++currIt ){
		delete *currIt;
		if(silent) currIt= nullptr;
	}

	if(!silent) sub_itemTable.erase( it1, it2);
	return true;
}
bool fafikLib_DB::itemEntry::removeItem(iterator it, bool silent)
{
	return removeItems( it, it+1, silent );
}
bool fafikLib_DB::itemEntry::removeItem(const wxString& by_string, bool silent)
{
	for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++)
	{
		if(this->sub_itemTable.at(temp_u)->key==by_string){
			return this->removeItem( temp_u, silent );
		} else {
			continue;
		}
	}
return false;
}
bool fafikLib_DB::itemEntry::removeItem(const itemEntry* by_id, bool silent)
{
	for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++)
	{
		if(this->sub_itemTable.at(temp_u)== by_id){
			return this->removeItem( temp_u, silent );
		} else {
			continue;
		}
	}
return false;
}
bool fafikLib_DB::itemEntry::removeItems(size_t pos_from, size_t amount, bool silent)
{
	return removeItems( begin()+pos_from, begin()+pos_from+amount, silent );
}
bool fafikLib_DB::itemEntry::removeItems(iterator it1, size_t amount, bool silent)
{
	return removeItems( it1, it1+ amount, silent );
}


fafikLib_virtualByteFile::fafikLib_virtualByteFile()
{

}
fafikLib_virtualByteFile::~fafikLib_virtualByteFile()
{

}

bool fafikLib_virtualByteFile::Seek(wxFileOffset ofs)
{
	if(ofs> this->length() ) ofs= this->length();
	this->_currPos= ofs;
	if(this->_bytes_fileIO->IsOpened() && this->_bytes_fileIO->Seek(this->pos_start + this->_currPos) ) return true;

return false;
}
size_t fafikLib_virtualByteFile::Read(void* pBuf, size_t amount)
{
	if(!pBuf) return -1;
	wxFileOffset old_currPos= this->_currPos;
	if( (old_currPos+ amount)> this->length() ) amount= this->length()- old_currPos;

	if(this->_bytes_fileIO->IsOpened() && this->_bytes_fileIO->Seek(this->pos_start + this->_currPos) ){
		this->_currPos+= amount;
		return this->_bytes_fileIO->Read( pBuf, amount );
	}

return -1;
}
BYTE fafikLib_virtualByteFile::FlushToFile(const wxString filePathName, const size_t buffSize)
{
	if(this->_bytes_fileIO->IsOpened() && this->_bytes_fileIO->Seek(this->pos_start) ){
		wxFileOffset old_currPos= this->_currPos;
		wxCharBuffer ret_char(buffSize);
		wxFFile outFile;
		outFile.Open(filePathName, "w+b");
		if(!outFile.IsOpened()) return 2;
		size_t read= 0;
		do{
			read= this->Read( &ret_char.data()[0], buffSize );
			if( read== size_t(0) || read== size_t(-1) )
				break;
			outFile.Write( ret_char, read);
			outFile.Flush();

		} while( read );
		outFile.Flush();
		outFile.Close();
		this->Seek(old_currPos);
		return 0;
	}

return 1;
}

//fafikLib_DB::itemEntry* fafikLib_DB::itemEntry_iterate::find( const wxString &findByStr, bool caseInsensitive )
//{
//	itemEntry aw;
////	itemEntry wwwww;
////	fafikLib_DB::get
//
//return 0;
//}
fafikLib_DB::itemEntry* fafikLib_DB::itemEntry_iterate::find( const wxArrayString& findByManyStr, bool caseInsensitive )
{
//	itemEntry_arrP;
	if(!LastFind) return nullptr;
	itemEntry* temp_recFind= LastFind;
	for(size_t temp_u=0; temp_u<findByManyStr.size(); temp_u++){
		temp_recFind= temp_recFind->findP(findByManyStr.Item(temp_u), 0, caseInsensitive);
		if(!temp_recFind) return LastFind=nullptr;
	}
return LastFind=temp_recFind;
}

wxVector<fafikLib_DB::itemEntry*> fafikLib_DB::itemEntry_iterate::get_TreeToNow()
{
	if(priv_B_fromLastByType){
		priv_B_fromLastByType= false;
		itemEntry_arrP.clear();
		itemEntry* currItem= LastFind->parent;
		while( currItem ){
			itemEntry_arrP.insert(0,currItem);
			currItem= currItem->parent;
		}
	}
	return itemEntry_arrP;
}

//fafikLib_DB::itemEntry_Stores::~itemEntry_Stores()
//{
//	FreeAll();
//	typeExt= 0;
//}
//void fafikLib_DB::itemEntry_Stores::FreeAll()
//{
//	if(ByteData) delete ByteData; ByteData= nullptr;
//	link= nullptr;
//	if(DB_linked) delete DB_linked; DB_linked= nullptr;
//}

bool fafikLib_DB::itemEntry::_operLowerB_compareIE( const_iterator item, const itemEntry& compa )
{
	return _operLowerB_compareIE_1( *item, &compa );
}
bool fafikLib_DB::itemEntry::_operLowerB_compareIE_1( const itemEntry* item, const itemEntry* compaa )
{
		//return ( a < b)
		///cast to Editor readable form
	const itemEntry& inFind= *item;
	const itemEntry& compa= *compaa;

	if( itemStores compaStores= (compa.type_stores & ( ie_lore| ie_auto_key_keyed )) ){
		//test pos is NOT Lore, compa is Lore
		//same then dont move, else do move closer To beginning()
		itemStores inFindStores= (inFind.type_stores & ( ie_lore| ie_auto_key_keyed ));
		return inFindStores > compaStores;
		//order should be
		//Lore
		//Auto_Key
		//
		//key = value
	}
	//then search for string
		///compares key or value
	const wxString* str_a= &inFind.key;
	const wxString* str_b= &compa.key;

	bool8 compRes= _operLowerB_compareStr( str_a, str_b, true );
	if( !compRes.getBit(1) )	//are not Identical (LowerCased)
		return compRes.getBit(0);

	if( str_a->size() == str_b->size() ){	//LowerCased strings are exact, compare with case
		compRes= _operLowerB_compareStr( str_a, str_b, false );
		return compRes.getBit(0);
	}

	return str_a->size()< str_b->size();
}
bool8 fafikLib_DB::itemEntry::_operLowerB_compareStr( const wxString* str_a, const wxString* str_b, bool caseInsensitive )
{
		///single char
	wxString str_ac('0'),str_bc('0');

	size_t strLen= str_a->size();
	size_t strCurrPos= 0;
	if( strLen> str_b->size() ) strLen= str_b->size();

	while( strCurrPos< strLen ) {
		str_ac= str_a->at(strCurrPos);
		str_bc= str_b->at(strCurrPos);
		if( caseInsensitive ){
			str_ac.LowerCase();	//we actually want to do that always, (Lower is used normally which means less lookups)
			str_bc.LowerCase();
		}
		if( str_ac< str_bc ) return bool8(0, true);
		if( str_ac> str_bc ) return bool8(0, false);
		++strCurrPos;
	}
	return bool8(1, true);
}

fafikLib_DB::itemEntry::iterator fafikLib_DB::itemEntry::binaryGetPosOf(const itemEntry& TemplateItem)
{
	return std::__lower_bound( sub_itemTable.begin(), sub_itemTable.end(), TemplateItem, fafikLib_DB::itemEntry::_operLowerB_compareIE );
}
bool fafikLib_DB::itemEntry::binaryGetPosOf_exists(const_iterator prevRetValue, const itemEntry& TemplateItem)const
{
	if(!prevRetValue) return 0;
	if(sub_itemTable.end()== prevRetValue) return 0;
	if( (*prevRetValue)->key== TemplateItem.key) return true;
return 0;
//	thatIt!= DF_list_p.end() && (*thatIt)->getPathName()== temp_findEnt.getPathName()
}



