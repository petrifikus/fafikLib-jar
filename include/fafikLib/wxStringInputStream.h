/** @brief lets you extract parts from input string
*
*/

#ifndef WXSTRINGINPUTSTREAM_H
#define WXSTRINGINPUTSTREAM_H

#include <wx/app.h>
#include "f77_helpers.h"

class wxStringInputStream
{
 public:
	wxStringInputStream();
		///makes a local copy
	wxStringInputStream(const wxString &str);
		///uses original(probably long string)
	wxStringInputStream(wxString *str);
	virtual ~wxStringInputStream();


		///copy
	void inputString(const wxString &str);
		///pointer original(mainly long strings)
	void inputString(wxString *str);
		///copy
	void setString(const wxString &str){inputString(str);}
		///pointer original(mainly long strings)
	void setString(wxString *str){inputString(str);}
	wxStringInputStream& operator=(const wxString &str);

	wxStringInputStream& operator>>(wxString &str);

	wxStringInputStream& operator>>(long &out);
	wxStringInputStream& operator>>(int &out);
	wxStringInputStream& operator>>(unsigned long &out);
	wxStringInputStream& operator>>(size_t &out);

	wxStringInputStream& operator>>(BYTE &out);
	wxStringInputStream& operator>>(bool &out);

	wxStringInputStream& operator>>(double &out);
	wxStringInputStream& operator>>(float &out);

	wxStringInputStream& operator>>(long long &out);

	bool get_Number( unsigned long &out );
	bool get_Number( long &out );
	bool get_Number( long long &out );
	bool get_Number( double &out );
	bool get_Line( wxString &out );

	wxStringInputStream& operator>>(char &char_here);
	wxStringInputStream& operator>>(wxUniChar &char_here);

		///@return string containing digits
	wxString get_numberRawStr(){ return get_numberRawStr(0); }
		///@return string containing digits, minus
	wxString get_numberSignedRawStr(){ return get_numberRawStr(1); }
		///@return string containing digits,minus,dot
	wxString get_numberFloatRawStr(){ return get_numberRawStr(3); }


	size_t size()const { return str_iStr->size(); }
	const wxString& getString()const { return *str_iStr; }
		///@return number new pos to start from
	size_t Tell()const { return pos_curr; }
		///get char at pos
	wxUniChar at(size_t pos)const { return str_iStr->at(pos); }
		///get nextChar
	wxUniChar get_nextChar()const { return str_iStr->at(pos_curr); }
		///@return is string finished
	bool Eof()const { return (lastError== -1); }
		///@param 0 ok
		///@param 1 no number
		///@param -1 string end
	int get_lastError()const { return lastError; };
	wxStringVector getPathSerialized()const;
		///@param relative_to has to be a full path, use wxGetCwd()/getAppPath
	wxString getPathRelativeTo(const wxStringInputStream& relative_to)const;

	 ///change the delimiters for string extraction
	void setDelimiter(const std::string& delims){Chars_break= delims;}
	 ///changes the '.' for float conversion into one of separators chars
	void setFloatSeparators(const wxString& separators){floatDotChar= separators;}

 //data
	 ///Space Tab NewLine, editable
	std::string Chars_break;
	 ///set char to be separator of "number.float"
	wxString floatDotChar= '.';
 protected:
	 /// from 0 to 9 and '-'
	std::string Chars_digits;

	size_t pos_curr= 0;
	wxString *str_iStr= nullptr;
	bool str_iOwnCopy= false;
	int lastError= 0;

	 ///private, look at 2nd one
	wxString get_numberRawStr(BYTE dot_float);
	void LoadDefChars();
 private:
};






#endif // WXSTRINGINPUTSTREAM_H
