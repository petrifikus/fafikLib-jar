#ifndef F77_HELPERS_H
#define F77_HELPERS_H

#include <utility>
#include <wx/app.h>
#include <wx/string.h>
#include <wx/colour.h>
#include <wx/stopwatch.h>
#include <wx/textdlg.h>
#include <wx/textentry.h>
#include <wx/msgdlg.h>
#include <winsock2.h>
#include <windows.h>

 ///this is more efficient than wxarraystr
class wxStringVector: public wxVector<wxString> {
 public:
 //extra functions
	 ///@return iterator to found item or end() if not found
	iterator find(const wxString& item);
	const_iterator find(const wxString& item)const {return const_cast<const_iterator>(find(item));}
	 ///@return index in vector, or -1 if end()
	size_t getIndex(const const_iterator& item)const {if(item== end()) return -1; return item- begin();}

 //compatibility with wxarraystr
	size_t Add(const wxString& str) {push_back(str); return size()-1;}
	void Insert(const wxString& str, size_t uiIndex) { if(uiIndex>size())uiIndex=size(); this->insert(begin()+ uiIndex, str); }
	void RemoveAt(size_t nIndex, size_t nRemove = 1) {if(!nRemove)return; --nRemove; this->erase(begin()+ nIndex, begin()+ nIndex+ nRemove); }
	size_t GetCount() const { return size(); }
	size_t Count() const { return size(); }
	bool IsEmpty() const { return size() == 0; }

	wxString& Item(size_t nIndex)
	{
		wxASSERT_MSG( nIndex < size(),
					  wxT("wxStringVector: index out of bounds") );
		return this->at(nIndex);
	}
  const wxString& Item(size_t nIndex) const { return const_cast<wxStringVector*>(this)->Item(nIndex); }

	 // get last item
	wxString& Last() {
	  wxASSERT_MSG( !IsEmpty(),
					wxT("wxStringVector: index out of bounds") );
	  return Item(GetCount() - 1);
	}
	const wxString& Last() const { return const_cast<wxStringVector*>(this)->Last(); }
	void Clear() {clear();}
};

 ///lets you quarry information if application was run as Admin
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
 ///formats given path into valid windows Long path
wxString PathFormatToWindowsLongPath_f77(const wxString& path);
 ///transform single path string into a list of directories
wxStringVector PathToArray(const wxString& path);

 ///lets you initialize colors in a lot more ways, including #RRGGBBaa
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
	wxColour_F77(): wxColour() {}
	wxColour_F77(ChannelType r, ChannelType g, ChannelType b, ChannelType alpha=wxALPHA_OPAQUE ): wxColour(r,g,b,alpha) {}
  //operators
	bool operator==(const wxColour& colour) const {
		return this->GetRGBA() == colour.GetRGBA();
	}
	wxColour_F77& operator = (const wxColour& colour) {
		this->SetRGBA( colour.GetRGBA() );
		return *this;
	}
	operator wxColour&() {return *this;}


 protected:
	ChannelType hexToValue(const char* hexChar, bool len2=false) const;
	ChannelType hexToValue_part(const char* hexChar, bool expand=false) const;
};

 ///class for handles of FindFirstFileW(), will auto close on ~dtor()
class auto_find_handle
{
 public:
  //data
 	HANDLE findHandle= NULL;
  //ctr
 	auto_find_handle() {}
 	auto_find_handle(const HANDLE& findHandle_): findHandle(findHandle_) {}
  //dstr
	~auto_find_handle(){
		close();
	}
	void close(){
		if(findHandle!=NULL)
			FindClose(findHandle);
		findHandle= NULL;
	}
	bool isOk() const {return findHandle && findHandle!= INVALID_HANDLE_VALUE; }
	bool error() const {return !isOk(); }

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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4200 )
#endif // _MSC_VER
 ///defines most common hashing sizes
union HashSizez{
    BYTE u512[512/8]= {0};
    BYTE u384[384/8];
    BYTE u256[256/8];
    BYTE u224[224/8];
	uint64_t u64;
	uint32_t u32;
	uint16_t u16;
	uint8_t u8;
	 ///quarry data as BYTE
	BYTE u_any[0];
	 ///quarry data as char
	char u_anyc[0];
};
#ifdef _MSC_VER
#pragma warning( pop )
#endif // _MSC_VER
 ///convert data block to hex form
std::string toHex(const void* data, size_t sizeData, bool reverse=false);
 ///convert data block to hex form
std::string toHex(const HashSizez& data, size_t sizeData, bool reverse=false);


 ///a class that lets you set the insertion point in wxTextEntryDialog, +able to set font
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
	 ///sets font of all underlying elements
	void SetFontRec(const wxFont &font, wxSizer* fromSizer=nullptr);
	wxTextCtrl* ExposeCtrl() {return m_textctrl;}
};

 ///changes font for all sizer elements inside given window
void SetFontRecSizer(const wxFont &font, wxWindow* fromSizerWindow, wxSizer* fromSizer= nullptr);

 ///a class(wrapper) that does not crash
class wxStopWatch_f77: public wxStopWatch
{
 public:
	long Time() const {return long((TimeInMicro().GetValue())/1000); }
};

#endif // F77_HELPERS_H
