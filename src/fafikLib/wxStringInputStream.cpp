#include "wxStringInputStream.h"

wxStringInputStream::wxStringInputStream()
{
	LoadDefChars();
}
wxStringInputStream::wxStringInputStream(const wxString &str)
{
	this->inputString(str);
	LoadDefChars();
}
wxStringInputStream::wxStringInputStream(wxString *str)
{
	this->inputString( str );
	LoadDefChars();
}

wxStringInputStream::~wxStringInputStream()
{
	if( this->str_iOwnCopy ) delete this->str_iStr;
}
void wxStringInputStream::LoadDefChars()
{
	Chars_break.clear();
	Chars_digits.clear();
	Chars_break.reserve(3);
	Chars_digits.reserve(10);

	Chars_break.push_back( ' ' );
	Chars_break.push_back( '\t' );
	Chars_break.push_back( '\n' );

	for( char temp_c='0'; temp_c<='9'; temp_c++ ){
		Chars_digits.push_back( temp_c );
	}
//	Chars_digits.push_back( '-' );
//	Chars_digitsF= Chars_digits;
//	Chars_digitsF+= '.';
}


wxStringInputStream& wxStringInputStream::operator=(const wxString &str)
{
	this->inputString(str);

return (*this);
}
void wxStringInputStream::inputString(const wxString &str)
{
	this->str_iOwnCopy= true;
	this->str_iStr= new wxString( str );
	this->pos_curr= 0;
	lastError= 0;
}
void wxStringInputStream::inputString(wxString *str)
{
	if( this->str_iOwnCopy ) delete this->str_iStr;
	this->str_iOwnCopy= false;
	this->str_iStr= str;
	this->pos_curr= 0;
	lastError= 0;
}
wxStringInputStream& wxStringInputStream::operator>>(wxString &str)
{
	get_Line(str);
	return (*this);
}
bool wxStringInputStream::get_Line( wxString &out )
{
	out.clear();
	if(this->pos_curr> this->size() ){
		this->lastError= -1;
		return false;
	}
	size_t pos_from= this->str_iStr->find_first_not_of( Chars_break, this->pos_curr );
	size_t pos_to= this->str_iStr->find_first_of( Chars_break, this->pos_curr );
	if(pos_from> pos_to){
		out.clear();
		this->pos_curr= pos_to+1;
		return true;
	}
	if( pos_to==size_t(-1) ) pos_to= this->str_iStr->size();
	this->pos_curr= pos_to+1;

	out= this->str_iStr->SubString( pos_from, pos_to-1 );

	return true;
}
///in 0 +num, 1 +-num, 2|3 +-float.num
wxString wxStringInputStream::get_numberRawStr(BYTE dot_float)
{
//	std::string *Chars_digitsH= &Chars_digits;
//	if(dot_float) Chars_digitsH= &Chars_digitsF;

	size_t pos_digi_begin= this->str_iStr->find_first_of( Chars_digits, this->pos_curr );
	size_t pos_digi_end= this->str_iStr->find_first_not_of( Chars_digits, pos_digi_begin );
	if( dot_float>0 ){	//1 and more	2018-09-08
		if( pos_digi_begin>0 ){	//if so include '-' before numberString
			if( this->str_iStr->at( pos_digi_begin-1 )== wxUniChar('-') ){
				--pos_digi_begin;
			}
		}

		if( dot_float>1 ){ //2 and more
			if( pos_digi_end< this->str_iStr->Length() ){
				if( floatDotChar.find( this->str_iStr->at( pos_digi_end ) )!= floatDotChar.npos ){ //if so search again for end of number but after found dotChar
					pos_digi_end= this->str_iStr->find_first_not_of( Chars_digits, pos_digi_end+1 );
				}
			}
		}
	}


	if( pos_digi_end==size_t(-1) ) pos_digi_end= this->str_iStr->size();
	this->pos_curr= pos_digi_end;

	return this->str_iStr->SubString( pos_digi_begin, pos_digi_end-1 );
}
bool wxStringInputStream::get_Number( unsigned long &out )
{
	wxString temp_str= this->get_numberRawStr(0);
	if(temp_str.empty()) this->lastError= -1;
	return( temp_str.ToULong( &out ) );
}
bool wxStringInputStream::get_Number( long &out )
{
	wxString temp_str= this->get_numberRawStr(1);
	if(temp_str.empty()) this->lastError= -1;
	return( temp_str.ToLong( &out ) );
}
bool wxStringInputStream::get_Number( long long &out )
{
	wxString temp_str= this->get_numberRawStr(1);
	if(temp_str.empty()) this->lastError= -1;
	return( temp_str.ToLongLong( &out ) );
}
bool wxStringInputStream::get_Number( double &out )
{
	wxString temp_str= this->get_numberFloatRawStr();
	if(temp_str.empty()) this->lastError= -1;
	temp_str.Replace( ",", ".", false );
	return( temp_str.ToDouble( &out ) );
}

wxStringInputStream& wxStringInputStream::operator>>(char &char_here)
{
	if(this->pos_curr> this->size() ) this->lastError= -1;
	this->str_iStr->at( this->pos_curr++ ).GetAsChar(&char_here);

return (*this);
}
wxStringInputStream& wxStringInputStream::operator>>(wxUniChar &char_here)
{
	if(this->pos_curr> this->size() ) this->lastError= -1;
	char_here= this->str_iStr->at( this->pos_curr++ );

return (*this);
}
wxStringInputStream& wxStringInputStream::operator>>(long &out)
{
	this->get_Number( out );
return (*this);
}
wxStringInputStream& wxStringInputStream::operator>>(size_t &out)
{
	unsigned long temp_val;
	this->get_Number( temp_val );
	out= temp_val;
return (*this);
}
wxStringInputStream& wxStringInputStream::operator>>(BYTE &out)
{
	unsigned long temp_val;
	this->get_Number( temp_val );
	out= temp_val;
return (*this);
}
wxStringInputStream& wxStringInputStream::operator>>(int &out)
{
	long temp_val;
	this->get_Number( temp_val );
	out= temp_val;
return (*this);
}
wxStringInputStream& wxStringInputStream::operator>>(unsigned long &out)
{
	this->get_Number( out );
return (*this);
}
wxStringInputStream& wxStringInputStream::operator>>(double &out)
{
	this->get_Number( out );
return (*this);
}
wxStringInputStream& wxStringInputStream::operator>>(float &out)
{
	double temp_val;
	this->get_Number( temp_val );
	out= temp_val;
return (*this);
}
wxStringInputStream& wxStringInputStream::operator>>(bool &out)
{
	long temp_val;
	this->get_Number( temp_val );
	out= temp_val;
return (*this);
}
wxStringInputStream& wxStringInputStream::operator>>(long long &out)
{
	this->get_Number( out );
return (*this);
}

wxStringVector wxStringInputStream::getPathSerialized()const
{
	std::string path_seps= "/\\";
	size_t pos_start= 0;
	size_t pos_found= 0;
	wxStringVector ret_arr;

	while( pos_found!= wxString::npos )
	{
		pos_found= str_iStr->find_first_of(path_seps, pos_start);
		ret_arr.push_back( str_iStr->SubString(pos_start, pos_found-1) );
//wxLogMessage("arr part = |%s|", ret_arr.back() );
//wxString.ToStdString().c_str()
//printf("  arr part = |%s|\n", ret_arr.back().ToStdString().c_str() );
		if(pos_start== wxString::npos) break;
		pos_start= pos_found +1;
	}
return ret_arr;
}
wxString wxStringInputStream::getPathRelativeTo(const wxStringInputStream& relative_to)const
{
	wxString ret_path_new;
	wxUniChar temp_sepChar= str_iStr->find('\\')!=wxString::npos ? '\\' : '/';

	if(
		str_iStr->StartsWith(wxString("..")<< temp_sepChar) ||
		str_iStr->StartsWith(wxString(".")<< temp_sepChar) ||
		str_iStr->StartsWith(wxString(temp_sepChar)<< temp_sepChar)
	){
		return ret_path_new;	//already relative
	}

	wxStringVector temp_cwd_arr= relative_to.getPathSerialized();
	wxStringVector temp_path_arr= this->getPathSerialized();

	size_t temp_pos= 0;
	size_t temp_matching= 0;
	while(true)
	{
		if(temp_cwd_arr.size()> temp_pos && temp_path_arr.size()> temp_pos)
		{
			if( temp_cwd_arr.Item(temp_pos)== temp_path_arr.Item(temp_pos) ){
				++temp_matching;	//ok
			} else {
				break;
			}
			++temp_pos;
		}else break;
	}

	if( temp_matching>= temp_cwd_arr.size() ){	//is after cwd
		ret_path_new<< "."<< temp_sepChar;
		for( size_t temp_iter= temp_matching; temp_iter< temp_path_arr.size(); ++temp_iter){
			ret_path_new<< temp_path_arr.Item(temp_iter)<< temp_sepChar;
		}
		ret_path_new.resize( ret_path_new.size()-1 );
	} else if( temp_matching>0 ) {
		for( size_t temp_iter= temp_matching; temp_iter< temp_cwd_arr.size(); ++temp_iter){
			ret_path_new<< ".."<< temp_sepChar;
		}
		for( size_t temp_iter= temp_matching; temp_iter< temp_path_arr.size(); ++temp_iter){
			ret_path_new<< temp_path_arr.Item(temp_iter)<< temp_sepChar;
		}
		ret_path_new.resize( ret_path_new.size()-1 );
	}
return ret_path_new;
}




