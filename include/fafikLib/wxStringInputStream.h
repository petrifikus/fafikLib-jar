#ifndef WXSTRINGINPUTSTREAM_H
#define WXSTRINGINPUTSTREAM_H


class wxStringInputStream
{
 public:
	wxStringInputStream();
		///makes a local copy
	wxStringInputStream(const wxString &str);
		///uses original(probably long string)
	wxStringInputStream(wxString *str);
	virtual ~wxStringInputStream();

		///set char to be separator of "number.float"
	wxString floatDotChar= '.';


		///copy
	void inputString(const wxString &str);
		///pointer original(mainly long strings)
	void inputString(wxString *str);
	void setString(const wxString &str){inputString(str);}
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

	wxStringInputStream& operator>>(char &char_here);
	wxStringInputStream& operator>>(wxUniChar &char_here);

		///@return string containing digits
	wxString get_numberRawStr(){ return get_numberRawStr(0); }
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
	wxArrayString getPathSerialized()const;
		///@param relative_to has to be a full path, use `wxGetCwd()`
	wxString getPathRelativeTo(const wxStringInputStream& relative_to)const;

		///Space Tab, editable
	std::string Chars_break;
 protected:
		/// from 0 to 9 and '-'
	std::string Chars_digits;
//	///and '.'
//	std::string Chars_digitsF;
	void LoadDefChars();

	size_t pos_curr= 0;
	wxString *str_iStr= nullptr;
	bool str_iOwnCopy= false;
	int lastError= 0;

	///private, look at 2nd one
	wxString get_numberRawStr(BYTE dot_float);
 private:
};

///@param B isSignFloat : 0= unsigned, 1=signed, 2=float
template<class Tnume> class wxStringToNumber
{
 bool _success= 0;
 public:
 	Tnume stores= 0;
 	BYTE isSignFloat= 0;

 	Tnume setNumber(const wxString& inStr);
 	Tnume getNumber()const{return stores;}
 	bool wasSuccess()const{return _success;}
 	bool Error()const{return !_success;}

	wxStringToNumber(){}
	wxStringToNumber(const wxString& inStr, BYTE isSignFloat_= 0){
		isSignFloat= isSignFloat_;
		setNumber(inStr);
	}

	Tnume operator=(const wxString& inStr){return setNumber(inStr);}


};
template<class Tnume> Tnume wxStringToNumber<Tnume>::setNumber(const wxString& inStr)
{
	if(isSignFloat== 1){
		long long toMe= 0;
		_success =inStr.ToLongLong( &toMe );
		if(_success) stores= toMe;
	} else if(isSignFloat== 2){
		double toMe= 0;
		_success =inStr.ToDouble( &toMe );
		if(_success) stores= toMe;
	} else {	//	(0)
		unsigned long long toMe= 0;
		_success =inStr.ToULongLong( &toMe );
		if(_success) stores= toMe;
	}
return stores;
}


#endif // WXSTRINGINPUTSTREAM_H
