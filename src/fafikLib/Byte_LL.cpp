#include "Byte_LL.h"

bool8b bool8::at(BYTE pos){
	return bool8b(this, pos);
}
bool8b bool8::operator[](BYTE pos) {
	return at(pos);
}
#ifndef _MSC_VER
const bool8b bool8::at(BYTE pos)const{
	return bool8b(this, pos);
}
const bool8b bool8::operator[](BYTE pos) const{
	return at(pos);
}
#endif

BYTE* FafikEC::BytesP_fromLL( long long &from_LL )
{
	return static_cast<BYTE*>( static_cast<void*>(&from_LL) );
}
size_t FafikEC::Bytes_getSize_fromBp( BYTE* byte_arr )
{
	size_t ret_am= 0;

	for(size_t temp_i=0; temp_i<8; ++temp_i){
		//efficient to run code (checks 8 bits for null)
		if( byte_arr[temp_i] != 0 ){
			ret_am= temp_i+1;
		}
	}

return ret_am;
}
long long FafikEC::Bytes_ToLL( BYTE *from_Byte, size_t len )
{
	if( len>8 ) len=8;

	BYTE temp_Bytes[8]= {0};

	for(size_t temp_i=0; temp_i<len; ++temp_i){
		temp_Bytes[temp_i]= from_Byte[temp_i];
	}

	return *Bytes_ToPLL( &temp_Bytes[0] );
}
long long* FafikEC::Bytes_ToPLL( BYTE *from_Byte )
{
	return static_cast<long long*>( static_cast<void*>(from_Byte) );
}

wxString FafikEC::Bytes_ToCharString( BYTE *from_Byte, size_t len )
{
	if( len== 0 ) return wxEmptyString;
	wxString ret_str;
	ret_str.resize(len);
	for(size_t temp_i=0; temp_i<len; ++temp_i){
		ret_str.at(temp_i)= from_Byte[temp_i];
	}
return ret_str;
}
long long FafikEC::Bytes_FromCharString_LL( wxString &str_ofLLChars )
{
	BYTE temp_Bytes[8]= {0};
	for(size_t temp_i=0; temp_i<str_ofLLChars.size(); ++temp_i){
		temp_Bytes[temp_i]= str_ofLLChars.at(temp_i);
	}
return *Bytes_ToPLL( &temp_Bytes[0] );
}






