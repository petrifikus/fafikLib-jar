#ifndef REPLACEALL_H
#define REPLACEALL_H



size_t replaceAll(wxString &strInput,const wxString strFrom,const wxString strTo,const size_t startPos=0, size_t MoveBy=0);

size_t replaceAll(wxString *strInput,const wxString strFrom,const wxString strTo,const size_t startPos=0, size_t MoveBy=0);
///ValidWhenStrIs_2kPlus0: 0 =2k+0(pair), 1 =2k+1(not pair)
size_t replaceAll_notAfterStr(wxString &strInput,const wxString strFrom,const wxString strTo,const wxString notAfterStr, const size_t startPos=0, size_t MoveBy=-1, const BYTE ValidWhenStrIs_2kPlus0=-1);
///ValidWhenStrIs_2kPlus0: 0 =2k+0(pair), 1 =2k+1(not pair)
size_t find_notAfterStr(wxString &strInput,const wxString strSearch,const wxString notAfterStr, const size_t startPos=0, const bool doOnlyOneCheck=false, const BYTE ValidWhenStrIs_2kPlus0=-1);
//size_t find_notAfterNAmStr(wxString &strInput,const wxString strSearch,const wxString notAfterStr, const size_t startPos=0, const bool doOnlyOneCheck=false, const BYTE nAmountOfStrForValid=0);

//size_t fafikLib_StringRestrictedFind(const wxString &in_str, const wxString find_str, const bool CaseInsensitive= false, const size_t from_pos= 0, const size_t to_pos= -1);
//made 2018-08-12, confirmed working, as of 2018-08-12 will be used instead of wxString.find() in some cases
///@param from_pos, to_pos searches as if inputed in_str.SubString(from_pos, to_pos) without subbing it
///@param from_posFind, to_posFind as if inputed find_str.SubString(from_pos, to_pos) without subbing it
size_t find_RestrictedString(const wxString &in_str, wxString find_str, const bool CaseInsensitive= false, const size_t from_pos= 0, size_t to_pos= -1, const size_t from_posFind= 0, size_t to_posFind= -1);

 ///ensure that string_in is HTML compatible
size_t HtmlFormat(wxString& strIO);
 ///ensure that string_in is HTML compatible
wxString HtmlFormatC(const wxString& strI);

#endif // REPLACEALL_H
