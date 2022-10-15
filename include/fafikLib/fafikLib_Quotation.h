#ifndef FAFIKLIB_QUOTATION_H
#define FAFIKLIB_QUOTATION_H

#include "ReplaceALL.h"

class fafikLib_Quotation
{
 public:
	fafikLib_Quotation();
	fafikLib_Quotation(wxString &in_, bool isSingleQuote_=false, size_t pos_start=0, BYTE removeQuoteS=false);
	virtual ~fafikLib_Quotation();
	bool isSingleQuote=false;
	/// 1 -rem ("), 2 -rem (')
	BYTE use_removeQuoteS=false;


	struct Quoting
	{
		size_t pos_from=0;
		size_t pos_to=0;
		size_t pos_startNext=0;
		wxString Quote="";
	};




	bool use( wxString &in_, bool isSingleQuote_=false, size_t pos_start=0, BYTE removeQuoteS=false );

	Quoting* getLast(wxString *out_=nullptr){
		if(out_) *out_=quotes->Quote;
		return quotes;
	}

	///-2 Error need to call use(wxString),-1 end, 0 ok, 1 not in "", 2 opened & not closed ""
	BYTE extractNext( wxString *out_ );

/*XML
<Item name="Enable HASH" type="numeric">0</Item>
JSON
"glossary": {
 "title": "example glossary",
 "number":100,
 "GlossDiv": {
 }
}

[[
 "%." void string sequence between other
JSON={
 table={
  start="%u%.:%.{"
  end="}"
 }
 table={
  start="%u:["
  end="]"
 }
 value={
  assign=":"
  key="%u"
  value="%u"

 }
}
]]
*/

	 ///for escaping wxString text
	 ///@param QuoteString=1 - ("), QuoteString=2 - (')
	static wxString escapeString( const wxString &escapeString, BYTE QuoteString=true );
	///for 1 element at a time use extractNext() which is more precise, removeDoubleQuoteString=1 -add ("), removeQuoteS=2 -add (')
	static wxString unescapeString( const wxString &escapeString, BYTE removeQuoteS=false );

	static size_t stringBeginsWithNotBlank(wxString &str);
	static size_t lineBeginsWithNotBlank(wxString &str){return stringBeginsWithNotBlank(str);}
	///-2 not initialized, -1 EOL
	size_t getPosStartNext(){
		if(!quotes) return -2;
		return quotes->pos_startNext;
	}
//	begin

 protected:
 	wxString unQuoteString;
	Quoting *quotes=nullptr;

 private:
};

//std::string fafikLib_toHex(void* p_obj, size_t obj_size);

#endif // FAFIKLIB_QUOTATION_H
