#include "ReplaceALL.h"

size_t replaceAll(wxString &strInput,const wxString strFrom,const wxString strTo,const size_t startPos, size_t MoveBy)
{
	size_t StartPos=startPos;
	unsigned Ocurence=0;
	while(StartPos!=wxString::npos)
	{
		StartPos = strInput.find(strFrom,StartPos);
		if(StartPos!=wxString::npos){
			Ocurence++;
			strInput.replace(StartPos,strFrom.Length(),strTo );
			StartPos+=MoveBy;
		}
	}
return Ocurence;
}


size_t replaceAll(wxString *strInput,const wxString strFrom,const wxString strTo,const size_t startPos, size_t MoveBy)
{
	size_t StartPos=startPos;
	unsigned Ocurence=0;
	while(StartPos!=wxString::npos)
	{
		StartPos = strInput->find(strFrom,StartPos);
		if(StartPos!=wxString::npos){
			Ocurence++;
			strInput->replace(StartPos,strFrom.Length(),strTo );
			StartPos+=MoveBy;
		}
	}
return Ocurence;
}


size_t replaceAll_notAfterStr(wxString &strInput,const wxString strFrom,const wxString strTo,const wxString notAfterStr, const size_t startPos, size_t MoveBy, const ValidWhenStrIs_2kPlus validWhen)
{
	if(MoveBy == size_t(-1)) MoveBy=strTo.Length();
	size_t StartPos=startPos;
	size_t Ocurence= 0;
	while(StartPos!=wxString::npos)
	{
		StartPos= strInput.find(strFrom,StartPos);
		if(StartPos==wxString::npos) return Ocurence;
		if(notAfterStr.empty() ) return Ocurence;
		  //if(validWhen!=0 && validWhen!=1){
		if(validWhen!= ValidDontCare){	//either on pair or odd
		  	size_t StartPos_back= StartPos;
		  	size_t ocured=0;
		  	wxString temp_str;
		  	while(true){
				temp_str = strInput.SubString( StartPos_back-notAfterStr.Length(), StartPos_back-1 );
				if(temp_str != notAfterStr)
					break;
				ocured++;
				StartPos_back-= notAfterStr.Length();
				if( !(StartPos_back>=notAfterStr.Length()))
					break;
		  	}
		  	if( (ocured&1) == (int)validWhen){
				Ocurence++;
				strInput.replace(StartPos,strFrom.Length(),strTo );
				StartPos+=MoveBy;
			}
			else
				StartPos++;
		  	//ok ?
		}
		else{	//dont care
			wxString temp_str = strInput.SubString( StartPos-notAfterStr.Length(), StartPos-1 );
			if( temp_str != notAfterStr ){
				Ocurence++;
				strInput.replace(StartPos,strFrom.Length(),strTo );
				StartPos+=MoveBy;
			}
			else
				StartPos++;
		}
	}
return Ocurence;
}

size_t find_notAfterStr(wxString &strInput,const wxString strSearch,const wxString notAfterStr, const size_t startPos, const bool doOnlyOneCheck, const ValidWhenStrIs_2kPlus validWhen)
{
	size_t StartPos= startPos;
	while(StartPos!=wxString::npos)
	{
		StartPos = strInput.find(strSearch,StartPos);
		if(StartPos==wxString::npos) return -1;
		if(notAfterStr.empty() ) return -1;
// TODO (main#9#07/28/18): validWhen change to occurrence
		if(validWhen!= ValidDontCare){
		  	size_t StartPos_back= StartPos;
		  	size_t ocured=0;
		  	wxString temp_str;
		  	while(true){
				temp_str = strInput.SubString( StartPos_back-notAfterStr.Length(), StartPos_back-1 );
				if(temp_str != notAfterStr)
					break;
				ocured++;
				StartPos_back-= notAfterStr.Length();
				if( !(StartPos_back>=notAfterStr.Length()) )
					break;
		  	}
		  	if( ((ocured &1)== (int)validWhen) || ocured==0)
				return StartPos;
			else
				StartPos++;
		  	//ok ?
		}
		else {
			wxString temp_str = strInput.SubString( StartPos-notAfterStr.Length(), StartPos-1 );
			if( temp_str != notAfterStr )
				return StartPos;
			else
				StartPos++;
		}
		if(doOnlyOneCheck) break;
	}
return -1;
}


size_t find_RestrictedString(const wxString &in_str, wxString find_str, const bool CaseInsensitive, const size_t from_pos, size_t to_pos, const size_t from_posFind, size_t to_posFind)
{
	if(CaseInsensitive) find_str.MakeUpper();
	size_t validFinds= from_posFind, validFinds_nextPos= from_pos;
	if(to_posFind> find_str.Length() ) to_posFind= find_str.Length();
	if(find_str.empty() || from_posFind>= to_posFind) return from_pos;

	if(in_str.Length()< to_pos ) to_pos= in_str.Length();
	for(size_t inspec_pos= from_pos; inspec_pos<to_pos; inspec_pos++){
		if(inspec_pos>to_pos) return -1;

		//size still to find
		if( (to_posFind-validFinds) > (to_pos-inspec_pos) )
			return -1;

		if( validFinds_nextPos== inspec_pos ){
			//ok
		}
		else
			validFinds= from_posFind;

		if(CaseInsensitive){	//CaseInsensitive exclusive implementation
			wxString inStr_1char( in_str.at(inspec_pos) );
			inStr_1char.MakeUpper();

			if(find_str.at(validFinds)== inStr_1char.at(0) ){
				validFinds++;
				validFinds_nextPos= inspec_pos+1;
			}
		} else {	//normal check
			if(find_str.at(validFinds)== in_str.at(inspec_pos) ){
				validFinds++;
				validFinds_nextPos= inspec_pos+1;
			}
		}
		if(validFinds>= to_posFind)
			return (inspec_pos- (to_posFind-from_posFind)+1 );
	}
return -1;
}


size_t HtmlFormat(wxString& strIO)
{
	size_t retVal= 0;
	retVal+= replaceAll(strIO, "&", "&amp", 0, 1);
	retVal+= replaceAll(strIO, "<", "&lt", 0, 1);
	retVal+= replaceAll(strIO, ">", "&gt", 0, 1);
	return retVal;
}
wxString HtmlFormatC(const wxString& strI)
{
	wxString retVal(strI);
	HtmlFormat(retVal);
	return retVal;
}










