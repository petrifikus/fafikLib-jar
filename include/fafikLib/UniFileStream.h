/** \brief Provides a unified method to: Read, Write, ReadLine for: files, strings, raw bytes, multiple files as one, parts of files
 *
 */

#ifndef FAFIK77_UNIFILESTREAM_H
#define FAFIK77_UNIFILESTREAM_H

#include <winsock2.h>
#include <windows.h>
#include <wx/ffile.h>
#include <wx/file.h>
#include "Byte_LL.h"
#include "ReplaceALL.h"
#include "f77_helpers.h"

namespace fafik77 {

 ///size_t(-1)
static const size_t invalid_size= (-1);
 ///-1LL
static const wxFileOffset invalidFileOffset= (-1);

enum class EOL{
	 /// '\0'
	Null= 0,
	 ///CR LF
	Windows= 1,
	 ///CR LF
	DOS= 1,
	 ///CR LF
	CRLF= 1,
	 /// '\n' LF
	Unix= 2,
	 /// '\n' LF
	GNU_Linux= 2,
	 /// '\n' LF
	MacOsX= 2,
	 /// '\n' LF
	LF= 2,
	 /// '\r' CR
	MacOs= 3,
	 /// '\r' CR
	CR= 3,
	 ///reverse of windows CRLF
	LFCR_reverseOfWindows= 4,
};

struct UniFileStream_EOL
{
 protected:
  //data
	 ///0: -1 means auto determine & re-save here, 1: -1 means dont use
	char char_eol[3]= {-1, -1, 0};
	BYTE str_size : 7;
	BYTE str_NON_Default : 1;
 public:
  //ctor
	 ///starts with default '\n' line end
	UniFileStream_EOL(){ setToDefault('\n');}
	 ///starts with specified line end, and validates str_size
	UniFileStream_EOL(char c1, char c2=-1){
		char_eol[0]= c1;
		char_eol[1]= c2;
		setSize(true);
	}
	void reset(){setToDefault('\n');}

  //setter-functions
	 ///determine valid string size
	void setSize(bool forced=false){
	  if( (!str_size) || forced ){
		str_size= 0;
		if(char_eol[0]!= char(-1) ){
			++str_size;
			if(char_eol[1]!= char(-1) ){
				++str_size;
			} else
				char_eol[1]= -1;
		}
		if(!str_size){
			str_size= 1;
			char_eol[0]= '\n';
			char_eol[1]= -1;
			str_NON_Default= false;
		} else {
			str_NON_Default= true;
		}
	  }
	}
	 ///does validate str_size
	void setTo(char c1= -1, char c2= -1){
		char_eol[0]= c1;
		char_eol[1]= c2;
		setSize(true);
	}
	 ///adapts other & validates str_size
	void setTo(const UniFileStream_EOL& other){
		char_eol[0]= other.char_eol[0];
		char_eol[1]= other.char_eol[1];
		setSize(true);
		str_NON_Default= other.str_NON_Default;
	}
	 ///sets by EOL enum
	void setTo(const EOL& EolType){
		if(EolType==EOL::Windows){
			setTo(13,10);
		}else if(EolType==EOL::Unix){
			setTo(10);
		}else if(EolType==EOL::MacOs){
			setTo(13);
		}else if(EolType==EOL::Null){
			setTo(0);
		}else if(EolType==EOL::LFCR_reverseOfWindows){	//reverse of windows
			setTo(10,13);
		}
		setSize(true);
	}

 //operator
	UniFileStream_EOL* operator = (const UniFileStream_EOL& other){ setTo(other); return this; }

  //getters
	const char* getEolRaw()const {return char_eol+ 0;}
	 ///if written to use @a setSize(true)
	const char* begin()const { return char_eol+0 ; }
	 ///@param (wchar_t)output where to store the converted EOL, has to be at least 3 long
	 ///@return len of EOL string
	BYTE getAsWchar(wchar_t output[])const { if(!output)return -1; output[2]= 0; if(str_size) output[0]= char_eol[0]; if(str_size>1) output[1]= char_eol[1]; return str_size; }
	inline BYTE getSize()const { return size();}
	inline BYTE size()const {if(str_NON_Default) return str_size; else return 1;}
	inline BYTE realSize()const {return str_size;}
	inline bool isNonDefault()const {return str_NON_Default;}
	inline bool isDefault()const {return !str_NON_Default;}
	inline void setNonDefault() {str_NON_Default= true;}

 protected:
	void setToDefault(char c1= -1, char c2= -1){
		char_eol[0]= c1;
		char_eol[1]= c2;
		str_size= 0;
		str_NON_Default= false;
	}
};

 ///@see UniFileStream_file
 ///!Warning! this creates temporary object each time called
 ///for correct behavior use UniFileStream_file
bool readLineByte( wxFFile &ffile, wxString *line, UniFileStream_EOL& useOnlyThisEndOL );


class UniFileStream_base;
class UniFileStream_baseWritable;
class UniFileStream_file;
class UniFileStream_string;
class UniFileStream_Binary;
class UniFileStream_arr;
class UniFileStream_part;
class UniFileStream_fileArr;
class UniFileStream_TextFile; //only a helper to read whole file (not derived from _base)


/** every inheritable class has to have "virtual ~dtor()".
 *	inherited has to take care of Virtual=0 methods
 */
class UniFileStream_base
{
 public:
 //def
 	enum storesObjectType {
		objectType_undefined= 0,
		objectType_base= 0,
		objectType_file,
		objectType_string,
		objectType_binary,
		objectType_arr,
		objectType_part,
		objectType_writeArr
 	};
 	enum PAlocManagement_deleteRules{
		PAM_owns_none= 0,
			///owns UniFileStream_base, always added when Add(wxFFile)
		PAM_owns_baseP= 1,
			///owns wxFFile & will close it automatically
		PAM_owns_wxffile= 2,
			///owns both above
		PAM_owns_all= PAM_owns_baseP|PAM_owns_wxffile,
 	};
 	struct PAlocItemManagement{
		UniFileStream_base* item= nullptr;
			///@param with PAlocManagement_deleteRules
		BYTE deleteRules= 0;
		PAlocItemManagement(){}
		PAlocItemManagement(UniFileStream_base* item_,BYTE deleteRules_= 0): item(item_), deleteRules(deleteRules_) {}
		virtual ~PAlocItemManagement();
		bool set(UniFileStream_base* it){ if(item)return false; item=it; deleteRules|=PAM_owns_baseP; return true;}
		PAlocItemManagement& operator = (UniFileStream_base* it) {set(it); return *this;}
 	};
 protected:
 //data
	 ///@see UniFileStream_base::storesObjectType
 	BYTE _typeOfObject= 0;
 	 ///if `UniFileStream_baseWritable` can be derived
 	bool _typeWritable= false;

	size_t read_max_size= 4096;
	std::string read_buff_str;
	wxFileOffset read_lastPos= 0;
	wxFileOffset _reOpenClose_size= -1;
	wxFileOffset _reOpenClose_pos= -1;
	 ///private Line reader with `read_max_size` String buffer
	bool readLineByte_base(wxString *line);

 public:
 //data store
	 ///how EOL looks
 	UniFileStream_EOL pass_useOnlyThisEndOL;
	 ///out wxString*
 	wxString* pass_p_line= nullptr;

	 ///base ctor (empty in base)
 	UniFileStream_base(){}	//this in self is meaningless (class is base virtual)
 	 ///virtual dtor (empty in base)
 	virtual ~UniFileStream_base(){}//that is empty as we dont allocate (new) memory

 	const char* getEolRaw()const {return pass_useOnlyThisEndOL.getEolRaw();}
 	 ///returns a <b>copy</b> of EOL
 	UniFileStream_EOL getEol()const {return pass_useOnlyThisEndOL;}
 	inline BYTE getEolSize()const {return pass_useOnlyThisEndOL.getSize();}
 	inline void setEolTo(const EOL& EolType) {pass_useOnlyThisEndOL.setTo(EolType);}
 	inline void setEolTo(const UniFileStream_EOL& setToThis) {pass_useOnlyThisEndOL.setTo(setToThis);}
 	inline void use( wxString *line ) {pass_p_line= line; }
	inline void useEOL( const UniFileStream_EOL& useOnlyThisEndOL ) { pass_useOnlyThisEndOL.setTo(useOnlyThisEndOL); }
	inline void useEOL(const EOL& EolType) {pass_useOnlyThisEndOL.setTo(EolType);}
 	 ///forget old EOL
 	void resetEol() {pass_useOnlyThisEndOL.reset();}
 	 ///forget buffer for ReadLine()
 	void resetLine() {read_lastPos= 0; read_buff_str.clear();}

  // [virtual pure
		///@param p_line can be overrode with call custom output
		///@return bool readLineByte( ... )
	virtual bool readLineByte( wxString* p_line= nullptr ) =0;
		///@return -1 on closed, else amount of read Bytes
	virtual size_t Read(void* p_buf, size_t n_count) =0;
		///@return name
	virtual const wxString& GetName() const =0;
		///@return true on can access, false no access
	virtual bool IsOpened() const =0;
		///@return -1 on closed, else size in Bytes
	virtual wxFileOffset Length() const =0;
		///@return true on ok, false on error(not opened)
	virtual bool Seek(wxFileOffset ofs, wxSeekMode mode = wxFromStart) =0;
		///@return -1 on closed, else pos in Bytes
	virtual wxFileOffset Tell() const =0;
		///@return FFile.GetKind() or wxFILE_KIND_UNKNOWN
	virtual wxFileKind GetKind() const =0;
		///close handlers, (doesnt mean destroy)
	virtual bool Close() =0;
		///FFile.Error() has it, so all should have it
	virtual bool Error() const =0;
		///used in Write-Arr to not worry about 500 files limit (will be added 2019-10-05) in `UniFileStream_arr::IsOpened()`
		///@return if its open
	virtual bool reOpen() =0;
		///@return if it got closed
	virtual bool reClose() =0;
		///set file pos in reClose() mode, will be effective now or after reOpen()
	virtual bool rePosSeek(const wxFileOffset& toPos) =0;
  // end virtual ]

		///reversed Seek(ofs)
	virtual bool SeekEnd(wxFileOffset ofs = 0) { return Seek(ofs, wxFromEnd); }
		///simple check if EOF
	virtual bool Eof() const{ return (Length()== -1 || Tell()>= Length()); }
		///@return !IsOpened();
	virtual bool IsClosed() const {return !(IsOpened());}

//		///re-Opens file as "r+b" if its Closed or "rb" if readOnly
//	static bool reOpenFile(wxFFile& file, bool readOnly= false);
	static bool reCloseFile(wxFFile& file) { return file.Close(); }

  //here are some calls to translate it upwards
	inline bool getAs_derived(UniFileStream_file** asDerived )		const {*asDerived= nullptr; if(objectType_file== _typeOfObject)		return( *asDerived= (UniFileStream_file*)this ); return false;}
	inline bool getAs_derived(UniFileStream_string** asDerived)	const {*asDerived= nullptr; if(objectType_string== _typeOfObject)	return( *asDerived= (UniFileStream_string*)this ); return false;}
	inline bool getAs_derived(UniFileStream_Binary** asDerived)		const {*asDerived= nullptr; if(objectType_binary== _typeOfObject)	return( *asDerived= (UniFileStream_Binary*)this ); return false;}
	inline bool getAs_derived(UniFileStream_arr** asDerived)		const {*asDerived= nullptr; if(objectType_arr== _typeOfObject)		return( *asDerived= (UniFileStream_arr*)this ); return false;}
	inline bool getAs_derived(UniFileStream_part** asDerived)		const {*asDerived= nullptr; if(objectType_part== _typeOfObject)		return( *asDerived= (UniFileStream_part*)this ); return false;}
	inline bool getAs_derived(UniFileStream_fileArr** asDerived)			const {*asDerived= nullptr; if(objectType_writeArr== _typeOfObject)	return( *asDerived= (UniFileStream_fileArr*)this ); return false;}
	inline bool getAs_derived(UniFileStream_baseWritable** asDerived)	const {*asDerived= nullptr; if(isWritableType())					return( *asDerived= (UniFileStream_baseWritable*)this ); return false;}

	inline BYTE getType_objectStored() const {return _typeOfObject;}
	inline bool isWritableType()const {return _typeWritable;}
};

class UniFileStream_baseWritable: public UniFileStream_base
{
 public:
	UniFileStream_baseWritable() {_typeWritable= true;}
//	virtual ~UniFileStream_baseWritable(){}
  //enum
	 ///BYTE 0xff
 	enum enumAtClose {
		AtClose_null= 0,
		AtClose_trim_s= 1,
		AtClose_save= 2,
		AtClose_trim_aply= 128,
		AtClose_trim= AtClose_trim_s | AtClose_trim_aply,
 	};
 protected:
  //store
 	 ///what happens on Close(), @see enumAtClose
	bool8 AtClose= 0;
	bool _writePermitted= true;
 public:
  // [virtual pure
	 ///@return amount of Bytes written
	virtual size_t Write(const void *pBuf, size_t nCount)= 0;
	virtual size_t Write(const wxString& str){
		auto asUtf= str.ToUTF8();
		return Write(asUtf.data(), asUtf.length());
	}

	virtual bool Flush()= 0;
	 ///trims file
	virtual bool setEndOfFile()= 0;
  // ]end virtual

	bool is_writePermitted()const {return _writePermitted;}
	bool can_write()const {return _writePermitted;}
};

 ///wraps wxFFile for readLineByte
class UniFileStream_file: public UniFileStream_baseWritable
{
  //data
 protected:
 	wxFFile* pass_p_ffile= nullptr;
 	wxFileOffset pass_p_ffile_LastPos= 0;
 	 ///created by class from use(FileName)
 	bool _ownedFile= false;
	 ///will auto delete pass_p_ffile;
 	bool autoDelete_pass_p_ffile= 0;
 	bool openReadOnly= false;
 public:
 	void setAutoDelete_ffile(bool setTo=true) { autoDelete_pass_p_ffile= setTo; }
 	void setReadOnly(bool setTo=true) { openReadOnly=setTo; _writePermitted= !setTo;}
  //ctor, dtor
	virtual ~UniFileStream_file(){
		Close(); //call a definitive close..
		_free(); //.. and delete the owned pointer
	}
	UniFileStream_file(){_typeOfObject= objectType_file;}
		///@see readLineByte
		///useOnlyThisEndOL= -2 auto determines "End Of Line" and use it every each time
	UniFileStream_file( wxFFile &ffile, wxString *line, const UniFileStream_EOL& useOnlyThisEndOL, bool AutoDelete_ffile=0 ):
		pass_p_ffile(&ffile), autoDelete_pass_p_ffile(AutoDelete_ffile)
	{
		_typeOfObject= objectType_file;
		pass_p_line= line;
		useEOL(useOnlyThisEndOL);	//use original assign
	};
	UniFileStream_file( wxFFile *ffile, wxString *line, const UniFileStream_EOL& useOnlyThisEndOL, bool AutoDelete_ffile=0 ):
		pass_p_ffile(ffile), autoDelete_pass_p_ffile(AutoDelete_ffile)
	{
		_typeOfObject= objectType_file;
		pass_p_line= line;
		useEOL(useOnlyThisEndOL);	//use original assign
	};
	UniFileStream_file( wxFFile *ffile, bool AutoDelete_ffile ):
		pass_p_ffile(ffile), autoDelete_pass_p_ffile(AutoDelete_ffile)
	{
		_typeOfObject= objectType_file;
	};
	 ///open a file by fileName
	UniFileStream_file( const wxString& fileName, const bool readOnly=true, wxString *line= nullptr )
	{
		if (!readOnly && !wxFileExists(fileName)) {
			wxFFile f(fileName, "w+");
		}
		pass_p_ffile= new wxFFile(fileName, (readOnly? "rb" : "r+b") );
		_ownedFile= autoDelete_pass_p_ffile= true;
		_typeOfObject= objectType_file;
		if(line) pass_p_line= line;
	};
	 ///open a file by fileName and mode
	UniFileStream_file( const wxString& fileName, const wxString& mode )
	{
		pass_p_ffile= new wxFFile(fileName, mode );
		_ownedFile= autoDelete_pass_p_ffile= true;
		_typeOfObject= objectType_file;
	};
	 ///cant redefine after, only if you used empty ctor()
	void use( wxFFile &ffile, wxString *line, const UniFileStream_EOL& useOnlyThisEndOL ){
		_free();
		pass_p_line= line;
		this->useEOL(useOnlyThisEndOL);	//use original assign
		if(pass_p_ffile==nullptr)
			pass_p_ffile= &ffile;
	}
	 ///cant redefine after, only if you used empty ctor()
	void use( wxFFile &ffile){
		_free();
		if(pass_p_ffile==nullptr)
			pass_p_ffile= &ffile;
	}

	bool Open( const wxString& fileName, const wxString& mode )
	{
		Close();
		_free();
		pass_p_ffile= new wxFFile(fileName, mode );
		_ownedFile= autoDelete_pass_p_ffile= true;
		return IsOpened();
	}
	bool OpenRo( const wxString& fileName, const bool readOnly, wxString *line= nullptr )
	{
		Close();
		_free();
		pass_p_ffile= new wxFFile(fileName, (readOnly? "rb" : "r+b") );
		_writePermitted= !readOnly;
		_ownedFile= autoDelete_pass_p_ffile= true;
		return IsOpened();
	}
  //operators
	UniFileStream_file& operator<< (const std::string& str){Seek(0, wxFromEnd); Write(str.data(), str.size()); return *this;}

  // [virtual override
		///@return bool readLineByte( ... )
	bool readLineByte( wxString *p_line= nullptr ) override;

	size_t Read(void* p_buf, size_t n_count) override;

	const wxString& GetName() const override;
		///true if (p_buf_in)
	bool IsOpened() const override;
	bool IsClosed() const {return !IsOpened();}
	wxFileOffset Length() const override;
	bool Seek(wxFileOffset ofs, wxSeekMode mode=wxFromStart) override;
	wxFileOffset Tell() const override;
		///@return FFile.GetKind() or wxString.unknown
	wxFileKind GetKind() const override;
		///use reClose() instead .. as this drops the pointer & can not be called again
	bool Close() override;
	bool Error() const override;
		///2019-10-05
	bool reOpen() override;
		///2019-10-05
	bool reClose() override;
	bool rePosSeek(const wxFileOffset& toPos) override { _reOpenClose_pos= pass_p_ffile_LastPos= toPos; if(IsOpened())return Seek(toPos); return true; }
  // end override]

  // [virtual override
	 ///@return amount of Bytes written, or -1 on error
	size_t Write(const void *pBuf, size_t nCount) override;
	virtual size_t Write(const wxString& str){
		auto asUtf= str.ToUTF8();
		return Write(asUtf.data(), asUtf.length());
	}
	 ///returns Flush(); or false if cant Flush
	bool Flush() override { if(pass_p_ffile && !openReadOnly && pass_p_ffile->IsOpened())return pass_p_ffile->Flush();  return false; }
	 ///trims file
	bool setEndOfFile() override;
  // ]end override

  //some other functions
	 ///get the ffile pointer (or null)
	wxFFile* get_FFile() {return pass_p_ffile;}
	 ///get the ffile pointer (or null)
	wxFFile* get_FFile()const {return pass_p_ffile;}
//		///returns Flush(); or false if cant Flush
//	bool Flush() { if(pass_p_ffile && !openReadOnly && pass_p_ffile->IsOpened())return pass_p_ffile->Flush();  return false; }
//		///returns the number of bytes written, or -1 on error
//	size_t Write(const void* pBuff, size_t nCount);
	FILE* fp() const { if(pass_p_ffile)return pass_p_ffile->fp();  return nullptr; }
 protected:
	void _free();
};

 ///wraps std::string in unified methods with wxFFile
class UniFileStream_string: public UniFileStream_baseWritable
{
 protected:
 	std::string* p_buf_in= nullptr;
 	bool owned= false;
 public:

 	virtual ~UniFileStream_string(){}
 	 ///if set to true will make own String, else just default/empty ctor
 	UniFileStream_string(const bool& makeOwn=false){
 		_typeOfObject= objectType_string;
 		if(makeOwn){
			owned= true;
			p_buf_in= new std::string;
		}
	}
 	UniFileStream_string( std::string& str_in ):
 		p_buf_in(&str_in)
	{_typeOfObject= objectType_string;}
	UniFileStream_string( std::string* str_in, bool makeOwner=false ):
		p_buf_in(str_in), owned(makeOwner)
	{_typeOfObject= objectType_string;}
 	UniFileStream_string( std::string& str_in, wxString *line_out, const UniFileStream_EOL& useOnlyThisEndOL ):
 		p_buf_in(&str_in)
	{
		_typeOfObject= objectType_string;
		pass_p_line= line_out;
		this->useEOL(useOnlyThisEndOL);	//use original assign
	};

	///uses passed string, no copy
	inline void use(std::string& str_in){
		Close();
		this->p_buf_in= &str_in;
		owned= false;
	}
	///uses passed string, no copy
	void use(std::string& str_in, wxString *line_out, const UniFileStream_EOL& useOnlyThisEndOL){
		Close();
		this->p_buf_in= &str_in;
		pass_p_line= line_out;
		owned= false;
		this->useEOL(useOnlyThisEndOL);	//use original assign
	}
	 ///opens a new owned string
	bool Open_new(){
		if(owned && p_buf_in){
			resetEol();
			resetLine();
			p_buf_in->clear();
			_lastPos= 0;
			return true;
		}
		Close();
		owned= true;
		p_buf_in= new std::string;
		return IsOpened();
	}
	 ///calls string.reserve(size)
	bool allocate(size_t size){
		if(p_buf_in) p_buf_in->reserve(size);
		return p_buf_in;
	}
	 ///calls string.reserve(size)
	bool reserve(size_t size){
		if(p_buf_in) p_buf_in->reserve(size);
		return p_buf_in;
	}
  //operators
	UniFileStream_string& operator<< (const std::string& str){Seek(0, wxFromEnd); Write(str.data(), str.size()); return *this;}

  // [virtual override
	///@return bool readLineByte( ... )
	bool readLineByte( wxString *p_line= nullptr ) override;

	size_t Read(void* p_buf, size_t n_count) override;

	const wxString& GetName() const override;
	///true if (p_buf_in)
	bool IsOpened() const override;
	bool IsClosed() const {return !IsOpened();}
	wxFileOffset Length() const override;
	bool Seek(wxFileOffset ofs, wxSeekMode mode = wxFromStart) override;
	wxFileOffset Tell() const override;
	///@return FFile.GetKind() or wxString.unknown
	wxFileKind GetKind() const override;
	///drops the pointer, can not be called again
	bool Close() override;
	bool Error() const override;
		///2019-10-05
	bool reOpen() override {return true;}
		///2019-10-05
	bool reClose() override {return false;}
	bool rePosSeek(const wxFileOffset& toPos) override {
		return Seek(toPos);
	}
  // end override]

  // [virtual override
	 ///@return amount of Bytes written
	size_t Write(const void *pBuf, size_t nCount) override;
	virtual size_t Write(const wxString& str){
		auto asUtf= str.ToUTF8();
		return Write(asUtf.data(), asUtf.length());
	}
	 ///is always a string
	bool Flush() override {return (IsOpened() && can_write());}
	 ///trims file
	bool setEndOfFile() override;
  // ]end override

	 ///provided str can only be read
	inline bool setReadOnly(bool readOnly=true){return _writePermitted= !readOnly;}
	 ///provided str can also be written to
	inline bool setWriteable(bool canWrite=true){return _writePermitted= canWrite;}

 protected:
	size_t _lastPos= 0;
};

 ///wraps any raw Binary data in unified methods with wxFFile (consider using _fromStr with write support as this is RO ReadOnly)
class UniFileStream_Binary: public UniFileStream_base
{
 protected:
 	const BYTE* p_buf_in= nullptr;
 	size_t size= 0;

 public:
 	virtual ~UniFileStream_Binary(){}

 	UniFileStream_Binary( const BYTE* p_buf_in, size_t size ):
 		p_buf_in(p_buf_in), size(size)
	{_typeOfObject= objectType_binary;}
 	UniFileStream_Binary( const char* p_buf_in, size_t size ):
 		p_buf_in( (BYTE*)p_buf_in ), size(size)
	{_typeOfObject= objectType_binary;}
 	UniFileStream_Binary( const BYTE* p_buf_in, size_t size, wxString *line_out, const UniFileStream_EOL& useOnlyThisEndOL ):
 		p_buf_in(p_buf_in), size(size)
	{
		_typeOfObject= objectType_string;
		pass_p_line= line_out;
		this->useEOL(useOnlyThisEndOL);	//use original assign
	};

	 ///opens a new data
	bool Open( const BYTE* p_buf_in, size_t size ){
		this->p_buf_in= p_buf_in;
		this->size= size;
		return IsOpened();
	}
  //operators

  // [virtual override
	///@return bool readLineByte( ... )
	bool readLineByte( wxString *p_line= nullptr ) override;

	size_t Read(void* p_buf, size_t n_count) override;

	const wxString& GetName() const override;
	///true if (p_buf_in)
	bool IsOpened() const override;
	bool IsClosed() const {return !IsOpened();}
	wxFileOffset Length() const override;
	bool Seek(wxFileOffset ofs, wxSeekMode mode = wxFromStart) override;
	wxFileOffset Tell() const override;
	///@return FFile.GetKind() or wxString.unknown
	wxFileKind GetKind() const override;
	///drops the pointer, can not be called again
	bool Close() override;
	bool Error() const override;
		///2019-10-05
	bool reOpen() override {return true;}
		///2019-10-05
	bool reClose() override {return false;}
	bool rePosSeek(const wxFileOffset& toPos) override {
		return Seek(toPos);
	}
  // end override]

 protected:
	size_t _lastPos= 0;
};

 ///!!! once element is added, it can not be modified (for proper iterating through data) !!!
 /// TODO (main#8#10/13/18): make UniFileStream_many_arrFiles
class UniFileStream_arr: public UniFileStream_base
{
 protected:
		///store pointers and their deleteRules in pointer to storing struct
	wxVector<PAlocItemManagement*> arrSources;
 public:

	virtual ~UniFileStream_arr();
		///frees space taken with Add()
	void FreeSources();
		///alias FreeSources();
	inline void FreeTable(){ FreeSources(); }
	UniFileStream_arr(){_typeOfObject= objectType_arr;};
	UniFileStream_arr(const UniFileStream_EOL& useOnlyThisEndOL){
		_typeOfObject= objectType_arr;
		this->useEOL(useOnlyThisEndOL);	//use original assign
	};
		///@param ownsAdd default PAM_owns_none(and will NOT delete file)
		///adds wxFFile to list
	void Add(wxFFile& file, BYTE ownsAdd=PAM_owns_none, bool openReadOnly_=0);	//follows ctor of UniFileStream_file
		///@param ownsAdd default PAM_owns_baseP(and will delete file)
		///adds wxFFile to list
	void Add(wxFFile* file, BYTE ownsAdd=PAM_owns_none, bool openReadOnly_=0);	//follows ctor of UniFileStream_file
		///@param ownsAdd=PAM_owns_baseP will delete it after
		///@param ownsAdd=PAM_owns_wxffile is NOT legal as we dont know if its a file
		///adds object(pointer) to list. handler has to exist After
	void Add(UniFileStream_base* baseClass, BYTE ownsAdd=PAM_owns_none, bool DontAddSize=0);

		///get pointer to UniFileStream_base* from arrSources.at(indx)
	UniFileStream_base* getPointer(size_t indx){
		return arrSources.at(indx)->item;
	}

  // [virtual override
		///@return bool readLineByte( ... )
	bool readLineByte( wxString *p_line= nullptr ) override;

	size_t Read(void* p_buf, size_t n_count) override;

	const wxString& GetName() const override;
		///true if items && item->IsOpened()
	bool IsOpened() const override;
	bool IsClosed() const {return !IsOpened();}
		///Length is calculated only at Add() time. (as changes during Read() iteration are forbidden)
	wxFileOffset Length() const override;
	bool Seek(wxFileOffset ofs, wxSeekMode mode = wxFromStart) override;
	wxFileOffset Tell() const override;
		///@return FFile.GetKind() or wxString.unknown
	wxFileKind GetKind() const override;
		///drops the table, can not be called again
	virtual bool Close() override;
	bool Error() const override;
		///2019-10-05
	bool reOpen() override {return arrSources.at(P_lastPosT)->item->reOpen(); }
		///2019-10-05
	bool reClose() override {return arrSources.at(P_lastPosT)->item->reClose(); }
	bool rePosSeek(const wxFileOffset& toPos) override {return arrSources.at(P_lastPosT)->item->rePosSeek(toPos); }
  // end override]
  bool SeekFromHere(wxFileOffset ofs);

 protected:
		///to return data size(sum of all ells) instantly
	wxFileOffset P_DataLength= 0;
	wxFileOffset P_lastPos= 0;
	size_t P_lastPosT= 0;
	void seekResetFrom(size_t indx, bool toState=0, bool reCloseAfter=false);
		///@return true -done, false -error(no index)
		///add 1 to P_lastPosT only if possible
	bool plus_P_lastPosT();
//		///after first Read() its reset to 0
//		///wheather to carry EOL through all elements(means that all have to match once provided EOL)
//	BYTE B_unifyAllEOL= 1;
};
 ///reusable class for limiting access on `UniFileStream_base` to size between(start, end),  UniFileStream_part (2018-10-30) to take care of ranges of supplied
class UniFileStream_part: public UniFileStream_base
{
 public:
		///set whether to go back to original position in *partOf after operation, or not
		/// https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
	void Set_ownPos(bool state= 1);
	bool Get_ownPos()const{return B_OwnPos& 1;}


	inline void use(wxString* line){
		//	redirect request to UniFileStream_base.use
		UniFileStream_base::use(line);
	}
	void use(UniFileStream_base* Part, const wxFileOffset& posFrom=0, const wxFileOffset& posTo= -1 );
	void use(wxFFile& FilePart, const wxFileOffset& posFrom=0, const wxFileOffset& posTo= -1 );
	void usePart(const wxFileOffset& posFrom=0, const wxFileOffset& posTo= -1 );

	virtual ~UniFileStream_part();
	UniFileStream_part(){_typeOfObject= objectType_part;};
	UniFileStream_part(UniFileStream_base* Part, const wxFileOffset& posFrom=0, const wxFileOffset& posTo= -1 ){
		_typeOfObject= objectType_part;
		use(Part, posFrom, posTo);
	}
	UniFileStream_part(wxFFile& FilePart, const wxFileOffset& posFrom=0, const wxFileOffset& posTo= -1 ){
		_typeOfObject= objectType_part;
		use(FilePart, posFrom, posTo);
	}



  // [virtual override
		///@return bool readLineByte( ... )
	bool readLineByte( wxString *p_line= nullptr ) override;

	size_t Read(void* p_buf, size_t n_count) override;

	const wxString& GetName() const override;
		///true if partOf
	bool IsOpened() const override;
	bool IsClosed() const {return !IsOpened();}
		///Length is (end - start)
	wxFileOffset Length() const override;
		///only adds virtual position
	bool Seek(wxFileOffset ofs, wxSeekMode mode = wxFromStart) override;
		///@return virtual position
	wxFileOffset Tell() const override;
		///@return FFile.GetKind() or wxString.unknown
	wxFileKind GetKind() const override;
		///drops pointer or cleans-up if new was used
	bool Close() override;
	bool Error() const override;
	bool reOpen() override {return partOf->reOpen(); }
	bool reClose() override {return partOf->reClose(); }
	bool rePosSeek(const wxFileOffset& toPos) override {return partOf->rePosSeek(toPos); }

  // end override]

 protected:
		///uses pass_useOnlyThisEndOL of its own, you can copy source->EOL to this->EOL
	UniFileStream_base* partOf= nullptr;
 	///position in file to start read from
	wxFileOffset P_OffsStart= 0;
		///position in file to which to read
	wxFileOffset P_OffsEnd= 0;
		///current pos, in case of B_OwnPos&1
	wxFileOffset P_posCurr= 0;
		///whether to go back to original position in *partOf after operation, or not
		///&1 =ownPos, &2 =used new(cleanup)
	bool8 B_OwnPos;

};


 ///writes to file(s) maintaining maxPartSize
class UniFileStream_fileArr: protected UniFileStream_arr
{
 protected:
		///like "./txt.txt"	firstFile
	wxString fileBaseName;
		///like ".MP_" + "000001"
	wxString fileBaseNameAddition;
		///will prepend 0 to number to match this constant len
	BYTE filePartKeepLen= 6;
		///alias name for this unwritable name "P_lastPosT"
	size_t& currFile= P_lastPosT;
		///0 means no limit why if(!filePartMaxSize && ...)
	wxFileOffset filePartMaxSize= 0;
		///@return new currFile, -1 on(existingOnly || currFile is not Files.size) and not found
	size_t makeNextPart(bool existingOnly=0, bool forceOverwrite=0);

 public:
 	enum enumAtClose: BYTE {
		 //nothing on close
		AtClose_null= 0,
		 //trim self end
		AtClose_trim_s= 1,
		 //save on close
		AtClose_save= 2,
		 //apply trim
		AtClose_trim_aply= 128,
		AtClose_trim= AtClose_trim_s | AtClose_trim_aply,
 	};
		///@param &1 trim
		///@param &2 save
		///@param ???? &4 trim every part to filePartMaxSize
		///@param &128 apply trim, otherwise its unwanted
		///we by default dont want any over files(trimFile())
 	bool8 AtClose= AtClose_trim_s;
		///default is WriteRead, but for source ReadOnly is enough
 	bool openReadOnly= 0;
 	UniFileStream_fileArr(){_typeOfObject= objectType_writeArr;}
 	UniFileStream_fileArr(const wxString& filePathName, const wxString& filePathNameAdd=wxEmptyString, const wxFileOffset& _filePartMaxSize=0, bool8 _AtClose=1, bool _openReadOnly=0);
		///we dont use any `new`, but under class does
 	virtual ~UniFileStream_fileArr(){Close_save(0);}



		///@return amount of Bytes written
	size_t Write(const void *pBuf, size_t nCount);
	inline size_t Write(const char *pBuf, size_t nCount){
		return Write( static_cast<const void*>(pBuf), nCount);
	}
	size_t Write(const wxString& str){
		auto asUtf= str.ToUTF8();
		return Write(asUtf.data(), asUtf.length());
	}
//		///@return true on success
//	bool Write(const wxString& s, const wxMBConv& conv= wxConvAuto());

//	void* onWriteExpandFirstPart(  );
		///@param arg that will be send is `UniFileStream_fileArr :: this`
		///@param void* will forward request of `onWriteExpandFirstPart_forward`
		///specify a function that will be called when Write() needs to expand to (part==1)
	void (*onWriteExpandFirstPart)( UniFileStream_fileArr*, void* )= nullptr;
		///	https://www.cprogramming.com/tutorial/function-pointers.html
	void* onWriteExpandFirstPart_forward= nullptr;

		///@param b @a makeNewOnFullSize will make new file when (fileThisPartSize==filePartMaxSize)
		///if(filePartMaxSize) will iterate through files using filePartKeepLen and add it to list
	size_t findLoadAllFiles(bool makeNewOnFullSize=0, size_t maxParts= -1);
		///not tested, (?works on start), use seekEnd() to init start pos to end
		///can move and make files if pos doesnt exist
	bool SeekMore(wxFileOffset pos);

	bool Flush();
		///trims file, and deletes parts after currentSeekPos
	bool setEndOfFile();
		///alias for "setEndOfFile"
	inline bool trimFile(){return setEndOfFile();}


		///this will init filePartMaxSize if(filePartMaxSize== 0, fileThisPartSize== 0)
	void init_PartSize(const wxFileOffset& initSize);
		///actually this will start from last seek pos
		///this will init filePartMaxSize if(filePartMaxSize== 0)
	void init_PartSizeToCurrLenght();
		///this will init fileBaseNameAddition
	void init_fileBaseNameAddition(const wxString& str);

	inline void init_constraints(const wxString& filePathNameAdd, const wxFileOffset& _filePartMaxSize){
		init_fileBaseNameAddition( filePathNameAdd );
		init_PartSize( _filePartMaxSize );
	}

	 ///init fileBaseName, and makeFile
	bool Open(const wxString& pathName, bool readOnly=false);

	 ///all handlers.Flush(), ::Close(), !!! Warning `AtClose` will be applied and by default trim files
	bool Close_save(bool save=1);
	bool Close_ForceDrop();

  // [virtual override, forwards request
		///forwards request
	bool readLineByte( wxString *p_line= nullptr ) { return UniFileStream_arr::readLineByte(p_line); }
		///forwards request
	size_t Read(void* p_buf, size_t n_count) { return UniFileStream_arr::Read(p_buf, n_count); }
		///@return fileBaseName
	const wxString& GetName() const override { return fileBaseName; }
		///true if items && item->IsOpened()
	bool IsOpened() const { return UniFileStream_arr::IsOpened(); }
	bool IsClosed() const {return !IsOpened();}
		///Length is ReCalculated, ///forwards request
	wxFileOffset Length() const { return UniFileStream_arr::Length(); }
		///interprets request writing or only seeking `ofs`, if doNotAddSize seek only forwards request to seek()
	bool Seek(wxFileOffset ofs, wxSeekMode mode = wxFromStart, bool doNotAddSize=0);
		///forwards request
	wxFileOffset Tell() const { return UniFileStream_arr::Tell(); }
		///forwards request
	wxFileKind GetKind() const { return UniFileStream_arr::GetKind(); }
		///drops the table, can not be called again
		///!!! Warning `AtClose` will be applied and by default trim files
	inline virtual bool Close() override { return Close_save(0); }
		///forwards request
	bool Error() const { return UniFileStream_arr::Error(); }
	bool reOpen() override {return arrSources.at(currFile)->item->reOpen();}
	bool reClose() override {return arrSources.at(currFile)->item->reClose();}
	bool rePosSeek(const wxFileOffset& toPos) override {return arrSources.at(currFile)->item->rePosSeek(toPos);}

  // end override, forwards request]


		///2018-12-20, will be useful for Options>WriteOnlyOn... when dst seeked & src is parted
		///@return amount of n Bytes moved
	wxFileOffset setToCurrFileBegin();


	const wxString& GetName_base() const {return fileBaseName;}
	const wxString& GetName_baseAdd() const {return fileBaseNameAddition;}
//	wxFileOffset Tell_currFile() const { return fWriteHand.at(currFile)->Tell(); }
	wxFileOffset Tell_currFile() const { return arrSources.at(currFile)->item->Tell(); }
//	wxFileOffset Length_currFile() const { return fWriteHand.at(currFile)->Length(); }
	wxFileOffset Length_currFile() const { return arrSources.at(currFile)->item->Length(); }
	const wxFileOffset& Length_part() const {return filePartMaxSize;}
	size_t GetParts() const { return arrSources.size(); }

//	const wxFileOffset Length_firstFile() const { return fWriteHand.at(0)->Length(); }
	const wxFileOffset Length_firstFile() const { return arrSources.at(0)->item->Length(); }
};


 ///this class is not derived from `UniFileStream_base` it only serves to read a whole small file and store all Lines in an array
class UniFileStream_TextFile
{
 protected:
  //data
	wxStringVector Lines;
	UniFileStream_base* _readsInput= nullptr;
	size_t _maxSize= 1* 1024* 1024; //1MiB
	bool _InputOwned= false;
 public:
  //ctor, dtor
	~UniFileStream_TextFile(){ if(_InputOwned && _readsInput){delete _readsInput; _readsInput=nullptr;} }
	UniFileStream_TextFile(UniFileStream_base* input, size_t maxSizeAllowed= -1): _readsInput(input) {this->init(maxSizeAllowed);}
	UniFileStream_TextFile(const wxString& fileToUse, bool readOnly, size_t maxSizeAllowed= -1): _InputOwned(true) {
		UniFileStream_file* openInFile= new UniFileStream_file(fileToUse, readOnly);
		_readsInput= openInFile;
		this->init(maxSizeAllowed);
	}
  //forward functions
	wxString& getLine(size_t line){return Lines.Item(line);}
	const wxString& getLine(size_t line)const {return Lines.Item(line);}
	void useEOL(const UniFileStream_EOL& useEol) { _readsInput->useEOL(useEol); }
	void useEOL(EOL EolType) { _readsInput->useEOL(EolType); }

	size_t Add(const wxString& str) {return Lines.Add(str);}
	void Insert(const wxString& str, size_t uiIndex) {return Lines.Insert(str,uiIndex);}
	void RemoveLine(size_t at, size_t amount=1) { if(at<Lines.size()) Lines.RemoveAt(at, amount); }
	void RemoveLines(size_t from, size_t to) { if(from<Lines.size()) Lines.RemoveAt(from, to-from); }
	wxStringVector::iterator erase(wxStringVector::iterator from, wxStringVector::iterator to) {return Lines.erase(from, to); };

	size_t getLines()const {return Lines.size();}
	bool IsEmpty()const {return Lines.IsEmpty();}
	bool IsNotEmpty()const {return Lines.size();}
	bool IsFilled()const {return Lines.size();}
  //functions
	size_t Write();
	inline size_t Save() {return Write();}
	size_t WriteTo(const wxString& FileTo)const;
	inline size_t SaveTo(const wxString& FileTo)const {return WriteTo(FileTo);}

 protected:
	void init(size_t maxSizeAllowed= -1);
	size_t init_readAllLines();
	size_t _WriteTo(wxFFile& writableFFile)const;
};


 ///converts WxString to multibyte in std::string
std::string WxStringToMultibyte(const wxString& str);
 ///converts multibyte std::string to WxString
inline wxString WxStringFromMultibyte(const std::string& str);

 ///@param onNotFound assigns EOL = onNotFound; if EOL was not found
 ///this only works if char_eol[0]==-1, and searches from curr_pos not from start
void UniFileStream_DetermineEOL( UniFileStream_base &baseClass, UniFileStream_EOL &EOL, const UniFileStream_EOL& onNotFound=UniFileStream_EOL('\n') );
 ///main function, auto redirected from readLineByte(wxFFile);
//bool readLineByte( UniFileStream_base &baseClass, wxString *line, BYTE useOnlyThisEndOL=-1, BYTE* getEndLType= nullptr );
bool readLineByte( UniFileStream_base &baseClass, wxString *line, UniFileStream_EOL &EOL );



}; //namespace
#endif // FAFIK77_UNIFILESTREAM_H
