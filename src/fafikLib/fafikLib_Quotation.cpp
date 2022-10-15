#include "fafikLib_Quotation.h"

fafikLib_Quotation::fafikLib_Quotation()
{
	//ctor
}
fafikLib_Quotation::fafikLib_Quotation(wxString &in_,bool isSingleQuote_, size_t pos_start, BYTE removeQuoteS)
{
	use(in_, isSingleQuote_, pos_start,removeQuoteS);
}

fafikLib_Quotation::~fafikLib_Quotation()
{
	delete quotes;
	//dtor
}






wxString fafikLib_Quotation::escapeString( const wxString &escapeString, BYTE QuoteString )
{
	wxString str_rep (escapeString);

	if(!escapeString.empty()){
		str_rep.Replace("\\","\\\\");

		if(QuoteString == 2)
			replaceAll_notAfterStr(str_rep,"\'","\\\'","\\");
		else
			replaceAll_notAfterStr(str_rep,"\"","\\\"","\\");

		str_rep.Replace("\t","\\t");
		str_rep.Replace("\r","\\r");
		str_rep.Replace("\n","\\n");
		str_rep.Replace("\f","\\f");
		str_rep.Replace("\b","\\b");
	}

	if(QuoteString){
		wxUniChar AppQuote('\"');
		if(QuoteString == 2){
			AppQuote = ('\'');
		}
			str_rep.Prepend(AppQuote);
			str_rep.Append(AppQuote);
	}

	return str_rep;
}


wxString fafikLib_Quotation::unescapeString( const wxString &escapeString, BYTE removeQuoteS )
{
	if(escapeString.empty()) return "";
	wxString str_rep (escapeString);

	replaceAll_notAfterStr(str_rep,"\\\"","\"","\\");
	replaceAll_notAfterStr(str_rep,"\\t","\t","\\");
	replaceAll_notAfterStr(str_rep,"\\r","\r","\\");
	replaceAll_notAfterStr(str_rep,"\\n","\n","\\");
	replaceAll_notAfterStr(str_rep,"\\f","\f","\\");
	replaceAll_notAfterStr(str_rep,"\\b","\b","\\");

	str_rep.Replace("\\\\","\\");
	if(removeQuoteS){
		wxUniChar searchForQuote('\"');
		if(removeQuoteS == 2){
			searchForQuote = ('\'');
		}
		if(str_rep.at(0)==searchForQuote && str_rep.Last()==searchForQuote){
			str_rep.Remove(0,1);
			str_rep.RemoveLast();
		}
	}

	return str_rep;
}



bool fafikLib_Quotation::use( wxString &in_, bool isSingleQuote_, size_t pos_start, BYTE removeQuoteS)
{
	isSingleQuote= isSingleQuote_;
    unQuoteString= in_;
    if(quotes) delete quotes;
	quotes= new Quoting;
	quotes->pos_startNext= pos_start;
	use_removeQuoteS= removeQuoteS;
return true;
}


BYTE fafikLib_Quotation::extractNext( wxString *out_ )
{
	wxUniChar searchForQuote('\"');
	if (isSingleQuote) searchForQuote = ('\'');
	if(!quotes) return -2;
	size_t pos_startNext = quotes->pos_startNext;
	size_t foundOn = quotes->pos_startNext;
	out_->clear();
//	while(true)
//	{
//	foundOn = find_notAfterStr(unQuoteString, searchForQuote, "\\", foundOn);
//printf("\t\tquotes->pos_startNext = %u\n", quotes->pos_startNext);
	foundOn = find_notAfterStr(unQuoteString, searchForQuote, "\\", foundOn,false,2);
//printf("\t\tfoundOn = %u\n", foundOn);

//		foundOn = unQuoteString.find( searchForQuote, foundOn );

//printf("foundOn = %i\n",foundOn);
//printf("foundOn2 = %i\n",foundOn2);

	if( foundOn== size_t(-1) ){
		quotes->Quote.clear();
		return -1;
	}
//		else if(foundOn==0) break;
//		if(foundOn >0 && unQuoteString.at(foundOn-1) !=wxUniChar('\\') ){
//			break;
//		} else {
//			foundOn=foundOn+1;
//		}
//	}

//printf(" %u --",foundOn);
//printf(" %u\n",pos_startNext);
	if( foundOn != pos_startNext ){
		quotes->pos_to = foundOn;
		quotes->pos_from=quotes->pos_startNext;
		quotes->Quote = unQuoteString.SubString( quotes->pos_from, quotes->pos_to-1 );

		quotes->Quote = unescapeString(quotes->Quote, use_removeQuoteS);

		*out_= quotes->Quote;
		quotes->pos_startNext = foundOn;
		return 1;
	}


//	quotes->pos_from = quotes->pos_to; //old quotes->pos_to
	quotes->pos_from = foundOn; //2018-07-28-12.13
//printf("\t%u",quotes->pos_to);
//printf("\t quotes->pos_to = %c\n",unQuoteString.at(quotes->pos_to).GetValue());
	foundOn++;
//size_t foundOn3 = find_notAfterStr(unQuoteString, searchForQuote, "\\", foundOn);
//printf("foundOn3 = %i\n",foundOn3);

////if(foundOn2==-1)
//wxThread::Sleep(500000);
//	while(true)
//	{
	foundOn = find_notAfterStr(unQuoteString, searchForQuote, "\\", foundOn,false,2);

//printf("\t\tfoundOn = %u\n", foundOn);
//		foundOn = unQuoteString.find( searchForQuote, foundOn );
//
//printf("foundOn = %i\n",foundOn);
//printf("foundOn2 = %i\n",foundOn2);

	if( foundOn== size_t(-1) ){
		quotes->pos_to = foundOn;
		quotes->Quote = unQuoteString.SubString( quotes->pos_from+1, quotes->pos_to-1 );
//		quotes->pos_to++;
		quotes->Quote = unescapeString(quotes->Quote);
		*out_= quotes->Quote;
		quotes->pos_startNext=foundOn;


//		quotes->Quote.clear();
		return 2;
	}
//		if(foundOn >0 && unQuoteString.at(foundOn-1) !=wxUniChar('\\') ){
//			break;
//		} else {
//			foundOn=foundOn+1;
//		}
//	}

//printf("foundOn = %i\n",foundOn);


	quotes->pos_to = foundOn;
	quotes->Quote = unQuoteString.SubString( quotes->pos_from+1, quotes->pos_to-1 );
	quotes->pos_to++;
	quotes->Quote = unescapeString(quotes->Quote);
	*out_= quotes->Quote;
	quotes->pos_startNext=foundOn+1;	//(foundOn+1)==(quotes->pos_to)


return 0;
}


size_t fafikLib_Quotation::stringBeginsWithNotBlank(wxString &str)
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

//wxString temp_str = str;
}


//std::string fafikLib_toHex(void* p_obj, size_t obj_size)
//{
//	std::string outputs;
//	std::string temp_chars;
////	char temp_chars[16];
//
//	for(char i=0; i<10; i++)temp_chars.push_back(char('0')+i);
//	for(char i=0; i<6; i++){
//			temp_chars.push_back(char('A')+i);
//printf(" @@@@ %c, ", (char('A')+i) );
//	}
//
//	BYTE *temp_ByteA = static_cast<BYTE*>(p_obj);
//
//	for(size_t temp_i=0; temp_i<obj_size; temp_i++){
//		outputs = (temp_chars[temp_ByteA[temp_i] & 0x0F]) + outputs;
//		outputs = (temp_chars[(temp_ByteA[temp_i] & 0xF0)/0xF]) + outputs;
//printf(" ==== %c, ", (temp_chars[temp_ByteA[temp_i] & 0x0F]));
//	}
//	temp_ByteA=nullptr;
//	temp_chars.clear();
//printf("@@@@@@@ obj_size = %u\n" ,obj_size);
//printf("|%c|",outputs[0]);
//printf("str size = %u\n",outputs.size());
//	return outputs;
//}


