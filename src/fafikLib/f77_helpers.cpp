#include "f77_helpers.h"
#include "ReplaceALL.h"

bool app_creation_info::app_runby_Admin= false;
bool app_creation_info::app_SE_RESTORE_NAME= false;

wxColour_F77::wxColour_F77(const std::string& ColorHex)
{
	int len= ColorHex.size();
	const char* chIter= ColorHex.data();
	ChannelType store[4]= {0};
	store[3]= wxALPHA_OPAQUE;
	 //discard 0x or # from the beginning
	if(ColorHex.at(1)== 'x'){
		chIter+= 2;
		len-= 2;
	}
	else if(ColorHex.at(0)== '#'){
		chIter+= 1;
		len-= 1;
	}

	if(len== 3 || len== 4){
		//short version of RGB
		for(int tempI=0; tempI!= len; ++tempI){
			store[tempI]= hexToValue(chIter +tempI, false);
		}
	} else if(len== 6 || len== 8){
		//long version of RGBa
		for(int tempI=0; tempI!= len; tempI+=2){
			store[tempI>>1]= hexToValue(chIter +tempI, true);
		}
	}
	this->Set(store[0], store[1], store[2], store[3]);
}
wxColour_F77::ChannelType wxColour_F77::hexToValue(const char* hexChar, bool len2) const
{
	ChannelType outVal= 0;
	if(len2){
		outVal=
		  hexToValue_part(hexChar, false)<<4
		  | hexToValue_part(hexChar +1, false);
	} else {
		outVal= hexToValue_part(hexChar, true);
	}
	return outVal;
}
wxColour_F77::ChannelType wxColour_F77::hexToValue_part(const char* hexChar, bool expand) const
{
	ChannelType outVal= 0;

	const char& hex_tr= *hexChar;
	if(hex_tr>= '0' && hex_tr<= '9')
		outVal= hex_tr- '0';
	else if(hex_tr>= 'A' && hex_tr<= 'F')
		outVal= (hex_tr- 'A')+10;
	else if(hex_tr>= 'a' && hex_tr<= 'f')
		outVal= (hex_tr- 'a')+10;
	if(expand){
		outVal= outVal<<4 | outVal;
	}
	return outVal;
}

wxString PathFormatToWindowsLongPath_f77(const wxString& path)
{	//prefix(\\?\) does not support (/ . ..)
	wxString retStr(path);
		// change '/' to '\'
	replaceAll(retStr,"/", "\\", 0,0); // replace '/' to '\\'
		// double '\' to single
	replaceAll(retStr,"\\\\", "\\", 2,0); // replace double '\\' to single '\'
		// remove '.'
	replaceAll(retStr,"\\.\\", "\\" ); // remove '.'
	if(retStr.StartsWith(".\\")) retStr.erase(retStr.begin(),retStr.begin()+2);
		// go back '..'
	size_t backUpADir= retStr.find("\\..\\");
	size_t backUpADir_beffore= 0;
	size_t pos_maxof= 0;
	while(backUpADir!= retStr.npos){
		backUpADir_beffore= 0;
		for(pos_maxof= backUpADir -1; pos_maxof!= 0; --pos_maxof){
			if(retStr.at(pos_maxof)== wxUniChar('\\') ){ backUpADir_beffore= pos_maxof; break;}
		}
		if(retStr.StartsWith("..\\") || retStr.StartsWith("\\..\\")) break;	//cant go back up to nothing
		retStr.erase(retStr.begin()+backUpADir_beffore, retStr.begin()+backUpADir+3);
		backUpADir= retStr.find("\\..\\");
	}
	return retStr;
}

 ///convert [in] @b data to Hex format (lower case)
std::string toHex(const void* const data, size_t sizeData, bool reverse)
{
	std::string toRet;
	toRet.reserve(sizeData*2);
	const BYTE* dataP= static_cast<const BYTE*>(data)+ (reverse? 0 : (sizeData-1) );
	const char numHexTb[]= "0123456789abcdef";
	char conv= 0;
	while(sizeData){
		conv= numHexTb[ (*dataP)>> 4 ];
		toRet+= conv;

		conv= numHexTb[ (*dataP)& 0x0f ];
		toRet+= conv;

		--sizeData;
		if(reverse)
			++dataP;
		else
			--dataP;
	}
	return toRet;
}
std::string toHex(const HashSizez& data, size_t sizeData, bool reverse)
{
	return toHex(&data, sizeData, reverse);
}


void wxTextEntryDialog_seekable::SetInsertionPointEnd()
{
	m_textctrl->SetInsertionPointEnd();
}
void wxTextEntryDialog_seekable::SetInsertionPoint(long pos)
{
	m_textctrl->SetInsertionPoint(pos);
}

