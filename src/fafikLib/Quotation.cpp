#include "Quotation.h"

namespace fafik77{

Quotation::Quotation()
{
	//ctor
}
Quotation::Quotation(wxString &in_,bool isSingleQuote, size_t pos_start, QuoteType RemoveQuoteType)
{
	use(in_, isSingleQuote, pos_start,RemoveQuoteType);
}
Quotation::~Quotation()
{
	delete quotes;
}
void Quotation::use( wxString &in_, bool isSingleQuote, size_t pos_start, QuoteType RemoveQuoteType)
{
	this->isSingleQuote= isSingleQuote;
    unQuoteString= in_;
    if(quotes) delete quotes;
	quotes= new Quoting;
	quotes->pos_startNext= pos_start;
	this->RemoveQuoteType= RemoveQuoteType;
}



wxString Quotation::escapeString( const wxString &escapeString, QuoteType QuoteTypeIs )
{
	wxString str_rep (escapeString);
	if(!escapeString.empty()){
		str_rep.Replace("\\","\\\\");
		if(QuoteTypeIs == QuoteSingle)
			replaceAll_notAfterStr(str_rep,"\'","\\\'","\\", 0, -1, ValidWhenPair);	//fixed 2024-02-12 (from ""-->0)
		else //double quote be default (even if null)
			replaceAll_notAfterStr(str_rep,"\"","\\\"","\\", 0, -1, ValidWhenPair);	//fixed 2024-02-12 (from ""-->0)
		str_rep.Replace("\t","\\t");
		str_rep.Replace("\r","\\r");
		str_rep.Replace("\n","\\n");
		str_rep.Replace("\f","\\f");
		str_rep.Replace("\b","\\b");
	}
	if(QuoteTypeIs != QuoteUnspecified){
		wxUniChar AppQuote('\"');
		if(QuoteTypeIs == QuoteSingle)
			AppQuote = ('\'');
		str_rep.Prepend(AppQuote);
		str_rep.Append(AppQuote);
	}
	return str_rep;
}


wxString Quotation::unescapeString( const wxString &escapeString, QuoteType RemoveQuoteType )
{
	if(escapeString.empty()) return "";
	wxString str_rep (escapeString);

	replaceAll_notAfterStr(str_rep,"\\\"","\"","\\", 0, -1, ValidWhenPair);		//fixed 2024-02-12 (from ""-->0)
	replaceAll_notAfterStr(str_rep,"\\t","\t","\\");
	replaceAll_notAfterStr(str_rep,"\\r","\r","\\");
	replaceAll_notAfterStr(str_rep,"\\n","\n","\\");
	replaceAll_notAfterStr(str_rep,"\\f","\f","\\");
	replaceAll_notAfterStr(str_rep,"\\b","\b","\\");

	str_rep.Replace("\\\\","\\");
	if(RemoveQuoteType!= QuoteUnspecified){
		wxUniChar searchForQuote('\"');
		if(RemoveQuoteType == QuoteSingle)
			searchForQuote = ('\'');
		if(str_rep.at(0)==searchForQuote && str_rep.Last()==searchForQuote){
			str_rep.Remove(0,1);
			str_rep.RemoveLast();
		}
	}

	return str_rep;
}


BYTE Quotation::extractNext( wxString *out_ )
{
	const wxUniChar searchForQuote( isSingleQuote ? '\'' : '\"' );
	if(!quotes) return -2;
	const size_t pos_startNext = quotes->pos_startNext;
	size_t foundOn = quotes->pos_startNext;
	if(out_) out_->clear();

	foundOn= find_notAfterStr(unQuoteString, searchForQuote, "\\", foundOn,false,ValidWhenPair);	//fixed 2024-02-12 (from 2-->0)

	if( foundOn== size_t(-1) ){
		quotes->Quote.clear();
		return -1;
	}

	if( foundOn != pos_startNext ){
		quotes->pos_to = foundOn;
		quotes->pos_from=quotes->pos_startNext;
		quotes->Quote= unQuoteString.SubString( quotes->pos_from, quotes->pos_to -1 );

		quotes->Quote= unescapeString(quotes->Quote, RemoveQuoteType);

		if(out_) *out_= quotes->Quote;
		quotes->pos_startNext= foundOn;
		return 1;
	}

	quotes->pos_from= foundOn; //2018-07-28-12.13
	foundOn++;
	foundOn= find_notAfterStr(unQuoteString, searchForQuote, "\\", foundOn, false, ValidWhenPair);	//fixed 2024-02-12 (from 2-->0)

	if( foundOn== size_t(-1) ){
		quotes->pos_to= foundOn;
		quotes->Quote= unQuoteString.SubString( quotes->pos_from +1, quotes->pos_to -1 );
//		quotes->pos_to++;
		quotes->Quote= unescapeString(quotes->Quote);
		if(out_) *out_= quotes->Quote;
		quotes->pos_startNext= foundOn;
		return 2;
	}

	quotes->pos_to= foundOn;
	quotes->Quote= unQuoteString.SubString( quotes->pos_from +1, quotes->pos_to -1 );
	++quotes->pos_to;
	quotes->Quote= unescapeString(quotes->Quote);
	if(out_) *out_= quotes->Quote;
	quotes->pos_startNext=foundOn+1;	//(foundOn+1)==(quotes->pos_to)

return 0;
}


size_t Quotation::stringBeginsWithNotBlank(wxString &str)
{
	size_t removed=0,pos_cur=0;
	if(str.empty()) return -1;
	while(true){
		if( str.at(pos_cur)==wxUniChar(' ') || str.at(pos_cur)==wxUniChar('\t') || str.at(pos_cur)==wxUniChar(0) || str.at(pos_cur)==wxUniChar(10) || str.at(pos_cur)==wxUniChar(13) ){
//			str.Remove(pos_cur,1);
			removed++;
		} else {
			break;
		}
		pos_cur++;
	}
	if(removed)
		str= str.Remove(0,removed);
return removed;
}







}; //namespace
