/** \brief for c++ older then 14, lets you manipulate string and quotes around them,
 *	since c++14 you could use std::quoted stream modifier from <iomanip>
 */

#ifndef FAFIK77_QUOTATION_H
#define FAFIK77_QUOTATION_H

#include "ReplaceALL.h"

namespace fafik77 {


enum QuoteType{
	QuoteDouble,
	QuoteSingle,
	Quote_unspecified,
	QuoteUnspecified=Quote_unspecified,
	Quote_double= QuoteDouble,
	DoubleQuote= QuoteDouble,
	Quote_single= QuoteSingle,
};

 ///lets you manipulate string and quotes around them
class Quotation
{
 public:
 //data
	bool isSingleQuote= false;
	QuoteType RemoveQuoteType= QuoteUnspecified;
 //def
	struct Quoting {
		size_t pos_from=0;
		size_t pos_to=0;
		size_t pos_startNext=0;
		wxString Quote="";
	};
 //functions
	Quotation();
	Quotation(wxString &in_, bool isSingleQuote=false, size_t pos_start=0, QuoteType RemoveQuoteType=QuoteUnspecified);
	virtual ~Quotation();
	void use( wxString &in_, bool isSingleQuote=false, size_t pos_start=0, QuoteType RemoveQuoteType=QuoteUnspecified );

	Quoting* getLast(wxString *out_=nullptr){
		if(out_) *out_=quotes->Quote;
		return quotes;
	}

	 ///-2 Error need to call use(wxString),-1 end, 0 ok, 1 not in "", 2 opened & not closed ""
	BYTE extractNext( wxString *out_ );
	 ///-2 not initialized, -1 EOL
	size_t getPosStartNext(){ if(!quotes) return -2; return quotes->pos_startNext; }

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
 //static functions
	 ///for escaping wxString text
	 ///@param QuoteTypeIs (used to encase string in "quotes")
	static wxString escapeString( const wxString &escapeString, QuoteType QuoteTypeIs=DoubleQuote );
	 ///for 1 element at a time use extractNext() which is more precise
	 ///@param RemoveQuoteType (used to remove the 1st and last (quote) character of the string)
	static wxString unescapeString( const wxString &escapeString, QuoteType RemoveQuoteType=QuoteUnspecified);

	 ///removes blank characters from the beginning of a string
	 ///@return amount of changes
	static size_t stringBeginsWithNotBlank(wxString &str);
	 ///removes blank characters from the beginning of a string
	 ///@return amount of changes
	static inline size_t lineBeginsWithNotBlank(wxString &str) {return stringBeginsWithNotBlank(str);}
 protected:
 //data
 	wxString unQuoteString;
	Quoting *quotes= nullptr;
};


}; //namespace
#endif // FAFIK77_QUOTATION_H
