#ifndef F77_HELPERS_H
#define F77_HELPERS_H

#include <utility>

class app_creation_info{
 public:
 	 ///on creation Admin privileges are checked
	static bool app_runby_Admin;
	static bool app_SE_RESTORE_NAME;


	static app_creation_info & getAppInfo(){
		static app_creation_info appInfo;
		return appInfo;
	}
	static app_creation_info & get_singleton(){
		return getAppInfo();
	}
};

wxString PathFormatToWindowsLongPath_f77(const wxString& path);

class wxColour_F77: public wxColour
{
 public:
 	 ///this translates color 0xRRGGBB(aa) into R,G,B,(a) .. warning as other types dont work
	wxColour_F77(const int& ColorHex){
		ChannelType store[4]= {0};
		memcpy( store, &ColorHex, 4);
		if( store[3] )
			this->Set(store[3], store[2], store[1], store[0]);
		else
			this->Set(store[2], store[1], store[0]);
	}
	 ///@param ColorHex: (0x or # also works) "RRGGBBaa" or "RGBa"
	 ///translates "0xHex RGB(a)" into RGBa Values
	wxColour_F77(const std::string& ColorHex);
	wxColour_F77(ChannelType r, ChannelType g, ChannelType b, ChannelType alpha=wxALPHA_OPAQUE ): wxColour(r,g,b,alpha) {}

	ChannelType hexToValue(const char* hexChar, bool len2=false) const;
	ChannelType hexToValue_part(const char* hexChar, bool expand=false) const;
};

 ///class for handles of FindFirstFileW()
class auto_find_handle
{
 public:
  //data
 	HANDLE findHandle= NULL;
  //ctr
 	auto_find_handle() {}
 	auto_find_handle(const HANDLE& findHandle_): findHandle(findHandle_) {}
  //dstr
	void close(){
		if(findHandle!=NULL)
			FindClose(findHandle);
		findHandle= NULL;
	}
 	~auto_find_handle(){
		close();
 	}
  //operators
 	auto_find_handle& operator = (const HANDLE& findHandle_) {findHandle= findHandle_; return *this;}
 	operator HANDLE() const { return findHandle; }
// 	 ///move operator, this one does not invalidate the resource
// 	auto_find_handle& operator = (auto_find_handle&& other) noexcept{
//		// https://en.cppreference.com/w/cpp/language/operators
//		// https://en.cppreference.com/w/cpp/utility/exchange
//		// https://en.cppreference.com/w/cpp/named_req/MoveConstructible
//		if(this == &other)
//			return *this; //can not assign self
//		//delete this objects old data..
//		//assign data from the &other to this
//		//findHandle= std::exchange(other.findHandle, NULL);
//		findHandle= other.findHandle;
//			other.findHandle= NULL;
//		return *this;
// 	}
 private:
 	 ///no copy of this class
 	auto_find_handle& operator = (const auto_find_handle& other) {return *this;}
};

class atos {
 public:
 	static wxString toWxString(const int& input) {wxString retStr; retStr<< input; return retStr;}
 	static wxString toWxString(const unsigned int& input) {wxString retStr; retStr<< input; return retStr;}
 	static wxString toWxString(const long& input) {wxString retStr; retStr<< input; return retStr;}
 	static wxString toWxString(const unsigned long& input) {wxString retStr; retStr<< input; return retStr;}
 	static wxString toWxString(const long long& input) {wxString retStr; retStr<< input; return retStr;}
 	static wxString toWxString(const unsigned long long& input) {wxString retStr; retStr<< input; return retStr;}

 	static wxString toWxString(const wxULongLong& input) {return input.ToString();}
 	static wxString toWxString(const wxLongLong& input) {return input.ToString();}
};

union HashSizez{
    BYTE u512[512/8]= {0};
    BYTE u384[384/8];
    BYTE u256[256/8];
    BYTE u224[224/8];
	uint64_t u64;
	uint32_t u32;
	uint16_t u16;
	uint8_t u8;

	BYTE u_any[0];
	char u_anyc[0];
};

 ///convert data block to hex form
std::string toHex(const void* data, size_t sizeData, bool reverse=false);
std::string toHex(const HashSizez& data, size_t sizeData, bool reverse=false);



class wxTextEntryDialog_seekable: public wxTextEntryDialog
{
 public:
	wxTextEntryDialog_seekable(wxWindow *parent,
				  const wxString& message,
				  const wxString& caption = wxGetTextFromUserPromptStr,
				  const wxString& value = wxEmptyString,
				  long style = wxTextEntryDialogStyle,
				  const wxPoint& pos = wxDefaultPosition)
	{
		Create(parent, message, caption, value, style, pos);
	}
	void SetInsertionPointEnd();
	void SetInsertionPoint(long pos);
};

 ///a class(wrapper) that does not crash
class wxStopWatch_f77: public wxStopWatch
{
 public:
	long Time() const {return long((TimeInMicro().GetValue())/1000); }
};

#endif // F77_HELPERS_H
