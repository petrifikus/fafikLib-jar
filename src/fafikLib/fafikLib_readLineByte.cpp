#include "fafikLib_readLineByte.h"
#include "unifyNumber_lenght.h"
#include <wx/filename.h>

void fafikLib_readLineByte_DetermineEOL( fafikLib_readLineByte_base &baseClass, fafikLib_readLineByte_EOL &EOL, const fafikLib_readLineByte_EOL& onNotFound )
{
	if(baseClass.IsClosed()){
		EOL= onNotFound;
		return;
	}
	if(EOL.isNonDefault()) return;
	size_t pos_lenght_eol= -1;
	wxFileOffset pos_start= baseClass.Tell();
	std::string temp_str;
	temp_str.resize(4097, 0);	//4096 +1 when next might be out of scope
	size_t reed= 0;
	const size_t invUint= -1;
	char eolChars[]= {13,10};

	while(true)
	{
		reed= baseClass.Read( &temp_str[0] , 4096 );
		if( !reed|| reed==invUint ){
			break;
		}
		pos_lenght_eol= temp_str.find_first_of( &eolChars[0], 0, 2 );
        if( pos_lenght_eol<reed ){	// pos_lenght_eol!=invUint
        	if(pos_lenght_eol== 4095){
				reed+= baseClass.Read( &temp_str[4096] , 1 );	//we can add because -1 wont be returned
        	}
        	char retChars[3]= {-1};

        	retChars[1]= temp_str.at( pos_lenght_eol );
        	if( pos_lenght_eol>0 )
				retChars[0]= temp_str.at( pos_lenght_eol-1 );
			if( pos_lenght_eol+1<reed )
				retChars[2]= temp_str.at( pos_lenght_eol+1 );

			BYTE pos_start= -1;
			BYTE pos_len= 0;
			for(BYTE i=0; i<3; ++i ){
				if( retChars[i]==eolChars[0] || retChars[i]==eolChars[1] ){
					if(pos_start==BYTE(-1)) pos_start= i;
					++pos_len;
				}
			}
			if(pos_len) EOL.setTo(retChars[pos_start], pos_len>1 ? retChars[pos_start+1] : -1 );
        }	//if reed
	}	//while

	if(EOL.isDefault() ){	//EOL= onNotFound; error proof
		EOL= onNotFound;
	}
baseClass.Seek(pos_start);
}

const wxFileOffset fafikLib_readLineByte_base::invalidFileOffset= -1LL;

bool fafikLib_readLineByte_base::fafikLib_readLineByte( wxString *line )
{
	reOpen();
	if( pass_useOnlyThisEndOL.isDefault() ){
		fafikLib_readLineByte_DetermineEOL(*this, pass_useOnlyThisEndOL);
		if( pass_useOnlyThisEndOL.getSize() == 0 ) return false;	//error there is no EOL
	}
	line->clear();

	size_t& toRead_now= read_max_size;
	std::string temp_str;
	std::string complete_strNew;
		///is read pointer, write Create New
	std::string* complete_str= &complete_strNew;

	const wxFileOffset pos_start= this->Tell();
//printf( "%I64d == %I64d (%i)\n", read_lastPos, pos_start, read_lastPos == pos_start );
//printf( "  @@ size= %I64d\n", this->Length() );
	if( read_lastPos == pos_start ){
		///ok	2019-10-04
		complete_str= &read_buff_str;
	} else {
		read_buff_str.clear();
	}
	temp_str.resize(toRead_now, 0);


	bool pos_end= 1;	//normal
	bool hasLine= false;
	size_t reed= 1;
	size_t toReed= 0;
	size_t pos_lenght_eol= -1;

	while(true)
	{
//Sleep(100);
//printf(" read_buff_str# = %u\n", read_buff_str.size() );
		if( complete_str->size() ){
			pos_lenght_eol= complete_str->find( pass_useOnlyThisEndOL.getEolRaw(), 0, pass_useOnlyThisEndOL.getSize() );
//printf("pos_lenght_eol (in %i)= %i\n", complete_str->size(), pos_lenght_eol);
			if( pos_lenght_eol!= complete_str->npos ){
				toReed= pos_lenght_eol;
				hasLine= true;
				break;
			}
		}
		if(complete_str!= &complete_strNew){
//			complete_str= &complete_strNew;
//			complete_str->append( read_buff_str );
			this->Seek(read_buff_str.size(), wxFromCurrent);	//we already read those #bytes, skip them
		}

//printf("reads ...\n");
		reed= this->Read( &temp_str.at(0) , toRead_now );
//printf("reads  ends...\n");
//printf(" reed = %i\n", reed );
		if( !reed || reed==temp_str.npos ){	//if 0 or -1
//			if(!toReed && complete_str->empty()) return false;	//we didnt read at all (so no pos change)
			if( pos_lenght_eol== temp_str.npos ){}	///2019-10-05, could not read more, enter not found
			else if(!toReed) return false;	//we didnt read at all (so no pos change)
			pos_end= 0;		//the input doesnt contain FULL EOL, (so assume thats everything thats left)
			break;
		}

		if(complete_str!= &complete_strNew){
			complete_str= &complete_strNew;
			complete_str->append( read_buff_str );				//we didnt finish the file with those, so add them
		}
		complete_str->append(temp_str, 0, reed);
	}

//Sleep(100);
//printf( "  #complete_str= %u\n", complete_str->size() );
	if(toReed){
		this->Seek( pos_start+ toReed );

		temp_str= complete_str->substr(0, toReed);
		read_buff_str= complete_str->substr( toReed+ pass_useOnlyThisEndOL.getSize() );
//printf("read_buff_str[0]= %u\n", read_buff_str[0]);
//printf("read_buff_str[1]= %u\n", read_buff_str[1]);
		wxString temp_wxStr( &temp_str.at(0) , wxConvUTF8 );
		line->swap(temp_wxStr);
//wprintf( L"line[0, %u]= %u, %u\n", line->size()-1, line->at(0), line->at(line->size()-1) );
//wprintf( L"  line[0, %u]= %u, %u\n", temp_str.size()-1, temp_str.at(0), temp_str.at(temp_str.size()-1) );
//wxLogMessage( "|%s|", *line );
	} else if(hasLine){
		read_buff_str.erase(0, pass_useOnlyThisEndOL.getSize() );
	} else if(complete_str->size()){
		wxString temp_wxStr( &complete_str->at(0) , wxConvUTF8 );
		line->swap(temp_wxStr);
		read_buff_str.clear();
	} else {
		return false;
	}

	if( pos_end )
		this->Seek( pass_useOnlyThisEndOL.getSize(), wxFromCurrent );	//add pass_useOnlyThisEndOL length
	else this->SeekEnd();	//not normal, so goto EOF
	read_lastPos= this->Tell();

	return true;
}




bool fafikLib_readLineByte_file::readLineByte( wxString *p_line )
{
	reOpen();
	if(!p_line) p_line= pass_p_line;
	if(!p_line) return false;
	if(!pass_p_ffile) return false;
//	return fafikLib_readLineByte( *this, p_line, pass_useOnlyThisEndOL );
//	return _readLineByte( p_line );
	return fafikLib_readLineByte_base::fafikLib_readLineByte( p_line );
}
size_t fafikLib_readLineByte_file::Read(void* p_buf, size_t n_count)
{
	reOpen();
	if(this->IsClosed()) return -1;
	return pass_p_ffile->Read(p_buf, n_count);
}
const wxString& fafikLib_readLineByte_file::GetName() const
{
	if(!pass_p_ffile) return wxEmptyStringWx;
	return pass_p_ffile->GetName();
}
bool fafikLib_readLineByte_file::IsOpened() const
{
	if(!pass_p_ffile) return false;
	return pass_p_ffile->IsOpened();
}
wxFileOffset fafikLib_readLineByte_file::Length() const
{
//	reOpen();
	if(this->IsClosed()) return _reOpenClose_size;
	return pass_p_ffile->Length();
}
bool fafikLib_readLineByte_file::Seek(wxFileOffset ofs, wxSeekMode mode )
{
	reOpen();
	if(this->IsClosed()) return false;
	return pass_p_ffile->Seek(ofs, mode);
}
wxFileOffset fafikLib_readLineByte_file::Tell() const
{
//	reOpen();
	if(this->IsClosed()) return _reOpenClose_pos;
	return pass_p_ffile->Tell();
}
wxFileKind fafikLib_readLineByte_file::GetKind() const
{
	if(!pass_p_ffile) return wxFileKind::wxFILE_KIND_UNKNOWN;;
	return pass_p_ffile->GetKind();
}
bool fafikLib_readLineByte_file::Close()
{
	if(this->IsClosed()) return false;
	if(_writePermitted){
		if(AtClose.getBit(0) && AtClose.getBit(7) ){
			AtClose.changeBit(7);
			setEndOfFile();	//trim to last pos
		}
		if(AtClose.getBit(1)){
			Flush();
		}
	}
	_reOpenClose_size= invalidFileOffset;
	_reOpenClose_pos= invalidFileOffset;
	pass_p_ffile_LastPos= 0;
	resetEol();
	resetLine();
	return (pass_p_ffile->Close() );
}
bool fafikLib_readLineByte_file::Error() const
{
	if(!pass_p_ffile) return true;
	return pass_p_ffile->Error();
}
void fafikLib_readLineByte_file::_free()
{
	resetEol();
	resetLine();
	if( (autoDelete_pass_p_ffile || _ownedFile) && pass_p_ffile){
		delete pass_p_ffile;	//free up the wxFFile object
		pass_p_ffile= nullptr;	//might be necessary (if this object is pointed by many)
	}
	_ownedFile= autoDelete_pass_p_ffile= false;
}


bool fafikLib_readLineByte_file::reOpen()
{
	if(!pass_p_ffile) return false;
	if( pass_p_ffile->IsOpened() ) return true;
//printf("reopen file\n");
//printf( " reOpens file = |%s| at %i\n", pass_p_ffile->GetName().ToStdString().c_str(), int(pass_p_ffile_LastPos) );
	if( pass_p_ffile->GetName().size() ){
		bool retVal= false;
		wxString openMode;
		if(openReadOnly)
			openMode= "rb";
		else
			openMode= "r+b";
		wxLogNull noLog;
		retVal= pass_p_ffile->Open( pass_p_ffile->GetName(), openMode );
		if(!pass_p_ffile->IsOpened() || pass_p_ffile->Error()){
			return false;
		}

		if(pass_p_ffile_LastPos== invalidFileOffset) //seek the last pos
			pass_p_ffile->SeekEnd();
		else
			pass_p_ffile->Seek( pass_p_ffile_LastPos );
//		_reOpenClose_size= invalidFileOffset;
		_reOpenClose_pos= invalidFileOffset;
//		_reOpenClose_size= pass_p_ffile->Length();	//refresh this info just in case something changed
		return retVal;
	}
return false;
}
bool fafikLib_readLineByte_file::reClose()
{
//printf("re closing |%s|..\n", pass_p_ffile->GetName().ToStdString().c_str() );
	if( pass_p_ffile && this->IsOpened() ){
		_reOpenClose_pos=
		 pass_p_ffile_LastPos= pass_p_ffile->Tell();
		_reOpenClose_size= pass_p_ffile->Length();
		return pass_p_ffile->Close();
	}
return false;
}
size_t fafikLib_readLineByte_file::Write(const void* pBuff, size_t nCount)
{
	 //prepare for Write
	if(openReadOnly) return -1;
	if(!pass_p_ffile) return -1;
	if(!reOpen()) return -1;
	 //call Write and return its result
	return pass_p_ffile->Write(pBuff, nCount);
}
bool fafikLib_readLineByte_file::setEndOfFile()
{
	if( !reOpen() ) return false;
	wxFile tempFile;
	tempFile.Open( GetName(), wxFile::read_write );
	tempFile.Seek( Tell() );

	HANDLE hFile= (HANDLE)_get_osfhandle( tempFile.fd() );
	int retVal= ::SetEndOfFile( hFile );
	tempFile.Flush();
	tempFile.Close();
	Seek(0, wxFromEnd);
return retVal;
}



bool fafikLib_readLineByte_fromStr::readLineByte( wxString *p_line )
{
	if(!p_line) p_line= pass_p_line;
	if(!p_line) return false;
	return fafikLib_readLineByte( p_line );
}
size_t fafikLib_readLineByte_fromStr::Read(void* p_buf, size_t n_count)
{
	if(this->IsClosed()) return -1;

	if( (_lastPos+n_count)>Length() ){
		n_count= Length()- _lastPos;
	}
	if( n_count== 0 ) return 0;

	memcpy( p_buf, static_cast<void*>(& p_buf_in->at(_lastPos) ), n_count );
	_lastPos+= n_count;

	return n_count;
}
const wxString& fafikLib_readLineByte_fromStr::GetName() const
{
	return wxEmptyStringWx;
}
bool fafikLib_readLineByte_fromStr::IsOpened() const
{
	return( p_buf_in );
}
wxFileOffset fafikLib_readLineByte_fromStr::Length() const
{
	if(this->IsClosed()) return -1;
	return p_buf_in->size();
}
bool fafikLib_readLineByte_fromStr::Seek(wxFileOffset ofs, wxSeekMode mode)
{
	if(this->IsClosed()) return false;

	if( mode==wxFromStart ){
		_lastPos= ofs;
	} else if( mode==wxFromCurrent ){
		_lastPos+= ofs;
	} else if( mode==wxFromEnd ){
		_lastPos= this->Length() -ofs;
	}

	if( _lastPos> this->Length() ) {
		_lastPos= this->Length();
//		return false;
	}
	return true;
}
wxFileOffset fafikLib_readLineByte_fromStr::Tell() const
{
	if(this->IsClosed()) return -1;
	return _lastPos;
}
wxFileKind fafikLib_readLineByte_fromStr::GetKind() const
{
	return wxFileKind::wxFILE_KIND_UNKNOWN;
}
bool fafikLib_readLineByte_fromStr::Close()
{
	resetEol();
	resetLine();
	if(this->IsClosed()) return false;
	if(owned){
		delete p_buf_in;
		owned= false;
	}
	p_buf_in= nullptr;
	_lastPos= 0;
	return true;
}
bool fafikLib_readLineByte_fromStr::Error() const
{
	return !IsOpened();
}

size_t fafikLib_readLineByte_fromStr::Write(const void *pBuf, size_t nCount)
{
	if(IsOpened() && can_write()){
		if(_lastPos== Length()) //we are at the end, we can just append
			p_buf_in->append( (const char*)pBuf, nCount);
		else { //well we have to simulate overwrite
		  try{
			p_buf_in->erase(_lastPos, nCount); //over..
		  } catch(...) {}
		  try{
			p_buf_in->insert( _lastPos, (const char*)pBuf, nCount); //..write
		  } catch(...) {return -1;} //error was thrown, string was not modified
		}
		_lastPos+= nCount;
		return nCount;
	}
	return -1;
}
bool fafikLib_readLineByte_fromStr::setEndOfFile()
{
	if(IsOpened() && can_write()){
		p_buf_in->resize(_lastPos, 0x00);
		return true;
	}
	return false;
}




fafikLib_readLineByte_base::PAlocItemManagement::~PAlocItemManagement()
{
  if(item){
	if( deleteRules & PAM_owns_baseP ){
		delete item;
	}
	item= nullptr;
  }
}


fafikLib_readLineByte_arr::~fafikLib_readLineByte_arr()
{
	FreeSources();
}
void fafikLib_readLineByte_arr::FreeSources()
{
//printf("  close Byte_arr...\n");
	for( size_t temp_u=0; temp_u<arrSources.size(); temp_u++ ){
		PAlocItemManagement* tempEl= arrSources.at(temp_u);
		if(tempEl){
			delete tempEl;
		}
	}

	arrSources.clear();
	P_DataLength= 0;
	P_lastPos= 0;
	P_lastPosT= 0;
	resetEol();
	resetLine();
//printf("  End closed Byte_arr...\n");
}
void fafikLib_readLineByte_arr::Add(fafikLib_readLineByte_base* baseClass, BYTE ownsAdd, bool DontAddSize)
{
//	if(baseClass->pass_p_line== this->pass_p_line){
//		//thats an error, but we will fix this (as this could overwrite our output)
//		baseClass->pass_p_line= &outPartStr;
//	}
	PAlocItemManagement* tempEl= new PAlocItemManagement(baseClass, ownsAdd);
	arrSources.push_back(tempEl);
	if(DontAddSize){
		//nothing
	} else {
		P_DataLength+= baseClass->Length();
	}
	if(arrSources.size()>1 ) //close following files (not the first one)
		baseClass->reClose();
}

void fafikLib_readLineByte_arr::Add(wxFFile& file, BYTE ownsAdd, bool openReadOnly_)
{
	fafikLib_readLineByte_file* temp_push= new fafikLib_readLineByte_file(file, nullptr, pass_useOnlyThisEndOL);
//	temp_push->openReadOnly= openReadOnly_;
	temp_push->setReadOnly(openReadOnly_);
	temp_push->setAutoDelete_ffile( ownsAdd & PAM_owns_wxffile );
	Add( temp_push, ownsAdd );	//unused tag here PAM_owns_baseP
}

void fafikLib_readLineByte_arr::Add(wxFFile* file, BYTE ownsAdd, bool openReadOnly_)
{
	fafikLib_readLineByte_file* temp_push= new fafikLib_readLineByte_file(file, nullptr, pass_useOnlyThisEndOL);
	temp_push->setReadOnly(openReadOnly_);
	temp_push->setAutoDelete_ffile( ownsAdd & PAM_owns_wxffile );
	Add( temp_push, ownsAdd );	//unused tag here PAM_owns_baseP
}

bool fafikLib_readLineByte_arr::readLineByte( wxString *p_line )
{
	if(!p_line) p_line= pass_p_line;
	if(!p_line) return false;
	arrSources.at(P_lastPosT)->item->reOpen();
	return fafikLib_readLineByte( p_line );
}



//void fafikLib_readLineByte_arr::syncAllEOL()
//{
//	for( size_t temp_u=0; temp_u<arrSources.size(); temp_u++ ){
//		arrSources.at(temp_u)->pass_useOnlyThisEndOL= pass_useOnlyThisEndOL;
//	}
//}
size_t fafikLib_readLineByte_arr::Read(void* p_buf, size_t n_count)
{
	arrSources.at(P_lastPosT)->item->reOpen();
	if(this->IsClosed()) return -1;
	if( (P_lastPos+n_count)> Length() ){	//we cant read over the size of file(s)
		n_count= wxFileOffset( Length()- P_lastPos );
	}
	if( !n_count ) return 0;

	size_t n_read= 0;
	size_t temp_n_count;
	while(true){
		temp_n_count= 0;
		if( !arrSources.at(P_lastPosT)->item->reOpen() ) return -1;
		try{
			temp_n_count= arrSources.at(P_lastPosT)->item->Read( static_cast<void*>(static_cast<BYTE*>(p_buf)+n_read), (n_count-n_read) );
		} catch(...){
			temp_n_count= size_ti;
		}
		if( temp_n_count!= size_ti ){	//we really dont want to add (-1)
			n_read+= temp_n_count;
		} else {
			return (n_read? n_read: -1); //return what was ok, else error -1
		}
		if(n_read< n_count){	//if not yet fulfilled
			arrSources.at(P_lastPosT)->item->reClose();
			plus_P_lastPosT();
		}
else break;	//we read what we had to read
	}

	P_lastPos+= n_count;
	return n_read;
}
const wxString& fafikLib_readLineByte_arr::GetName() const
{
	if(P_lastPosT< arrSources.size()) return arrSources.at(P_lastPosT)->item->GetName();
return wxEmptyStringWx;
}
bool fafikLib_readLineByte_arr::IsOpened() const
{
	//we have to have items
	if(arrSources.size() >0){
		return arrSources.at(P_lastPosT)->item->IsOpened();
	}
return 0;
}
wxFileOffset fafikLib_readLineByte_arr::Length() const
{
	return P_DataLength;
}
void fafikLib_readLineByte_arr::seekResetFrom(size_t indx, bool toState, bool reCloseAfter)
{
	for(size_t temp_u= indx; temp_u<arrSources.size(); temp_u++){
		if(toState){
			arrSources.at(temp_u)->item->rePosSeek(-1);
			if(arrSources.at(temp_u)->item->IsOpened()) arrSources.at(temp_u)->item->SeekEnd();
		} else {
			arrSources.at(temp_u)->item->rePosSeek(0);
			if(arrSources.at(temp_u)->item->IsOpened()) arrSources.at(temp_u)->item->Seek(0);
		}

		if(reCloseAfter) arrSources.at(temp_u)->item->reClose();
	}
}
bool fafikLib_readLineByte_arr::plus_P_lastPosT()
{
	++P_lastPosT;
	if(P_lastPosT< arrSources.size() ){
		arrSources.at(P_lastPosT-1)->item->reClose();
		return true;
	} else {
		P_lastPosT= arrSources.size()-1;
		return false;
	}
}
bool fafikLib_readLineByte_arr::SeekFromHere(wxFileOffset ofs)
{
	if(!ofs){	// as in Seek+0
		arrSources.at(P_lastPosT)->item->reOpen();
		return true;
	}
	wxFileOffset AimNewPos= P_lastPos+ ofs;
	wxFileOffset ofsMoveMaxHere= 0;
	if(AimNewPos> P_DataLength) AimNewPos= P_DataLength;	//overflow protection
	while(true){
//		arrSources.at(P_lastPosT)->item->reOpen();
		ofsMoveMaxHere= arrSources.at(P_lastPosT)->item->Length()- arrSources.at(P_lastPosT)->item->Tell();
		if(ofsMoveMaxHere){
			if( ofsMoveMaxHere> ofs) ofsMoveMaxHere= ofs;	//we dont want more ofs
			arrSources.at(P_lastPosT)->item->Seek(ofsMoveMaxHere, wxFromCurrent);
			P_lastPos+= ofsMoveMaxHere;
			ofs-= ofsMoveMaxHere;
		}
		//	we again check if there is anything left in the file, if not add posT
		ofsMoveMaxHere= arrSources.at(P_lastPosT)->item->Length()- arrSources.at(P_lastPosT)->item->Tell();
		if( !ofsMoveMaxHere ){	//has nothing left
			plus_P_lastPosT();	//takes care of reClose()
		}

		if(P_lastPos== AimNewPos){
			//	we are at requested pos, now reset to (0) everything after that
			seekResetFrom(P_lastPosT+1, 0, true);
			//	and go to next steps
		}else if(P_lastPos< AimNewPos){
			//	we aint quite there yet, repeat
			continue;
		}

		return true;
	}
return true;
}

bool fafikLib_readLineByte_arr::Seek(wxFileOffset ofs, wxSeekMode mode)
{
	arrSources.at(P_lastPosT)->item->reOpen();	//we need to have the file opened
	if(this->IsClosed()) return false;

	if( mode== wxFromStart ){
		P_lastPos= 0;
		P_lastPosT= 0;
		seekResetFrom(0);	//return to 0 all items
		return SeekFromHere( ofs );
	} else if( mode== wxFromCurrent ){
		if(ofs<0) return Seek(P_DataLength+ ofs);
		return SeekFromHere( ofs );
	} else if( mode== wxFromEnd ){
		if(!ofs){	//only make it unreadable
			P_lastPos= P_DataLength;
			P_lastPosT= arrSources.size()-1;
			seekResetFrom(0,1);
		} else {
			return Seek( P_DataLength- ofs );
		}
	}
return true;
}
wxFileOffset fafikLib_readLineByte_arr::Tell() const
{
	return P_lastPos;
}
wxFileKind fafikLib_readLineByte_arr::GetKind() const
{
	return arrSources.at(P_lastPosT)->item->GetKind();
	//what kind, kind of what???
}
bool fafikLib_readLineByte_arr::Close()
{
	FreeSources();
return 1;
}
bool fafikLib_readLineByte_arr::Error() const
{
	if(arrSources.empty()) return true;
	return arrSources.at(P_lastPosT)->item->Error();
}






void fafikLib_readLineByte_part::Set_ownPos(bool state)
{
	B_OwnPos.changeBit(0,state);
}
void fafikLib_readLineByte_part::use(fafikLib_readLineByte_base* Part, const wxFileOffset& posFrom, const wxFileOffset& posTo )
{
	Close();
	partOf= Part;
	usePart(posFrom, posTo);
}
void fafikLib_readLineByte_part::use(wxFFile& FilePart, const wxFileOffset& posFrom, const wxFileOffset& posTo )
{
	Close();
	fafikLib_readLineByte_base* tempAdd= new fafikLib_readLineByte_file(FilePart, pass_p_line, pass_useOnlyThisEndOL);
	B_OwnPos.changeBit(1,1);
	use(tempAdd, posFrom, posTo);
}
void fafikLib_readLineByte_part::usePart(const wxFileOffset& posFrom, const wxFileOffset& posTo )
{
	if(!partOf) return;	//no compare available

	P_OffsStart= posFrom;
	if(P_OffsStart<0) P_OffsStart= 0;

	P_OffsEnd= posTo;

	if(P_OffsEnd> partOf->Length() || P_OffsEnd< 0){
		P_OffsEnd= partOf->Length();
	}
//	if(P_OffsEnd<P_OffsStart ){
//		 if(partOf ) P_OffsEnd= partOf->Length();
//		 else P_OffsEnd= P_OffsStart;
//	}
}
fafikLib_readLineByte_part::~fafikLib_readLineByte_part()
{
	Close();
}

bool fafikLib_readLineByte_part::readLineByte( wxString *p_line )
{
	if(!p_line) p_line= pass_p_line;
	if(!p_line || !partOf) return false;
	return fafikLib_readLineByte_base::fafikLib_readLineByte( p_line );
}
size_t fafikLib_readLineByte_part::Read(void* p_buf, size_t n_count)
{
	if( (P_posCurr+ n_count)> Length() ){
		n_count= Length()- P_posCurr;
	}
	if( !n_count ) return 0;

	wxFileOffset posOriginal= partOf->Tell();
	partOf->Seek( P_OffsStart+ P_posCurr );

	Seek(n_count, wxFromCurrent);	//add n_count (this override does not change file cursor unless its actually reading)
	n_count= partOf->Read(p_buf, n_count);	//read and save ret_val

	if(Get_ownPos()) partOf->Seek(posOriginal);
return n_count;
}
const wxString& fafikLib_readLineByte_part::GetName() const
{
	if(this->IsClosed()) return wxEmptyStringWx;
	return partOf->GetName();
}
bool fafikLib_readLineByte_part::IsOpened() const
{
	return partOf;
}
wxFileOffset fafikLib_readLineByte_part::Length() const
{
	if(this->IsClosed()) return invalidFileOffset;
	return P_OffsEnd- P_OffsStart;
}
bool fafikLib_readLineByte_part::Seek(wxFileOffset ofs, wxSeekMode mode)
{
	if(this->IsClosed()) return false;

	if( mode== wxFromStart ){
		P_posCurr= ofs;
	} else if( mode== wxFromCurrent ){
		P_posCurr+= ofs;
	} else if( mode== wxFromEnd ){
		P_posCurr= this->Length() -ofs;
	}

	if( P_posCurr> this->Length() ) {
		P_posCurr= this->Length();
	}
	return true;
}
wxFileOffset fafikLib_readLineByte_part::Tell() const
{
	if(this->IsClosed()) return invalidFileOffset;
	return P_posCurr;
}
wxFileKind fafikLib_readLineByte_part::GetKind() const
{
	if(this->IsClosed()) return wxFILE_KIND_UNKNOWN;
	return partOf->GetKind();
}
bool fafikLib_readLineByte_part::Close()
{
	//cleanup if used new
	if(partOf && B_OwnPos.getBit(1)){
		delete partOf;
	}
	B_OwnPos.clearBit(1);	//remove tag "new"
	partOf= nullptr;	//close= drop handler
	resetEol();
	resetLine();
return true;
}
bool fafikLib_readLineByte_part::Error() const
{
	if(!partOf) return true;
	return partOf->Error();
}


void fafikLib_readLineByte_TextFile::init(size_t maxSizeAllowed)
{
	if(maxSizeAllowed && maxSizeAllowed!=size_t(-1)) _maxSize= maxSizeAllowed;
	if( !_readsInput) return; //no errors
	if( !_readsInput->reOpen() ) return; //not opened
	 //read all lines
	init_readAllLines();
	if( !_InputOwned) _readsInput->reClose(); //we dont need this opened anymore
}
size_t fafikLib_readLineByte_TextFile::init_readAllLines()
{
	if( _readsInput->Length() > _maxSize ) return -1;
	_readsInput->Seek( 0 ); //go back to start
	wxString tempStrToAdd;
	while( _readsInput->readLineByte( &tempStrToAdd ) ){ //while read lines
		Lines.Add( tempStrToAdd );
	}
	return Lines.size();
}
size_t fafikLib_readLineByte_TextFile::Write()
{
	//write to owned file
	if( !_InputOwned) return -1;
	_readsInput->reClose(); //unlock write permissions
	wxFFile toFile_w(_readsInput->GetName(), "wb"); //open with write permissions and clear the file

  #if _WIN32 == 1
	_readsInput->useEOLType( fafikLib_readLineByte_EOL::Windows_EOL );
  #endif // _WIN32

	return _WriteTo( toFile_w );
}
size_t fafikLib_readLineByte_TextFile::WriteTo(const wxString& FileTo)const
{
	wxFFile toFile_w(FileTo, "wb");
	if(toFile_w.Error() || !toFile_w.IsOpened() ) return -1;
  #if _WIN32 == 1
	_readsInput->useEOLType( fafikLib_readLineByte_EOL::Windows_EOL );
  #endif // _WIN32
	return _WriteTo( toFile_w );
}
size_t fafikLib_readLineByte_TextFile::_WriteTo(wxFFile& writableFFile)const
{
	writableFFile.Seek(0);
	size_t ret_written= 0;
	wxArrayString::const_iterator atLineIter= Lines.begin();
	for( ; atLineIter!= Lines.end(); ++atLineIter ){
		 //write line content
		if(writableFFile.Write( *atLineIter, wxConvUTF8 ) ){
			ret_written+= (*atLineIter).size();
		}
		 //write new line
		ret_written+= writableFFile.Write( _readsInput->getEolRaw(), _readsInput->getEolSize() );
	}
	return ret_written;
}

//#include "unifyNumber_lenght.h"
size_t fafikLib_write_fileArr::makeNextPart(bool existingOnly, bool forceOverwrite)
{
//	currFile= P_lastPosT;
	///first adds 1 to currFile, then tests if there is need to add one
	++currFile;
	if( (currFile)== arrSources.size() ){
		//ok we can add a file, we are at the end
	} else {
		return -1;
	}

	wxFFile* newFFile= nullptr;
	wxString newFileName= fileBaseName+ fileBaseNameAddition+ unifyNumber_lenght::unify( currFile, filePartKeepLen);
//wxLogWarning("loads part name= |%s|", newFileName);
	if( (!forceOverwrite) && wxFileExists(newFileName) ){
	  if(openReadOnly){
		newFFile= new wxFFile( newFileName, "rb" );	//read openReadOnly
	  } else {
		newFFile= new wxFFile( newFileName, "r+b" );	//read write
	  }
	} else if(existingOnly){
		--currFile;
		return -1;
	} else {
		newFFile= new wxFFile( newFileName, "w+b" );	//null write read
	}

//	if( fWriteHand.size() ){	//2019-10-05// 2021-09-17 moved to Add()
//printf( "closed part= |%s|\n", fWriteHand.back()->GetName().ToStdString().c_str()  );
//		if(fWriteHand.back()->IsOpened())
//			fWriteHand.back()->Close();
//	}
//	fileThisPartSize= 0;
//	fWriteHand.push_back(newFFile);
//	fafikLib_readLineByte_arr::Add(newFFile, PAM_owns_all, forceOverwrite);
	fafikLib_readLineByte_arr::Add(newFFile, PAM_owns_all, openReadOnly);
//	P_lastPosT= currFile;
return currFile;
}

size_t fafikLib_write_fileArr::Write(const void *pBuf, size_t nCount)
{
	///read returns wrong amount ()
	size_t nCountWritten= 0;
//	currFile= P_lastPosT;
//	fileThisPartSize
	//bool reCalcLen= (nCount && ((currFile+1)< fWriteHand.size()) );

	while( nCount ){
		bool notFit= 0;
		fafikLib_readLineByte_file* writeTo= nullptr;
		arrSources.at(currFile)->item->getAs_derived( &writeTo );
		if(!writeTo){ //its not a file
			 //safety exit 666 (if not a file)
			fprintf(stderr, "Write operation expected a file Handle(got 0x%x)\n", arrSources.at(currFile)->item->getType_objectStored());
			return -1;
		}
		if(!arrSources.at(currFile)->item->reOpen()){	//we needed this file opened
			return -1;
		}
		wxFileOffset nCountPF= nCount;
		wxFileOffset nCountPFMaxHere= nCountPF;
//		fileThisPartSize= writeTo->Length() -(writeTo->Length()- writeTo->Tell());
		wxFileOffset fileThisPartSize= writeTo->Tell();

		if(filePartMaxSize){
//			nCountPFMaxHere= filePartMaxSize- writeTo->Length() - writeTo->Tell();
			nCountPFMaxHere= filePartMaxSize- fileThisPartSize;
			if( nCountPF>nCountPFMaxHere ){
				nCountPF= nCountPFMaxHere;	//trim it to (max_size/file_part)
				notFit= 1;
			}
		}

		if(nCountPF){	//omit (0) (eg last time was perfect, now we have (0) size left)
			wxFileOffset oldLen= writeTo->Length();
			size_t wrote= writeTo->Write( static_cast<const void*>(static_cast<const BYTE*>(pBuf)+nCountWritten), nCountPF );
			if( wrote && wrote!= size_ti){
				nCount-= wrote;
				nCountWritten+= wrote;

				wxFileOffset actualNew= writeTo->Length()- oldLen;
				if( actualNew> 0 )	//we dont want negative numbers
					P_DataLength+= actualNew;
				P_lastPos+= wrote;
			}
		}
		if(notFit){
			if( onWriteExpandFirstPart && currFile==0 ){
				onWriteExpandFirstPart(this,onWriteExpandFirstPart_forward);
			}
			writeTo->Flush();	//save the changes
			arrSources.at(currFile)->item->reClose(); //we dont need this file opened
			makeNextPart(0,1);	//we have to make sure that file is created Empty
			continue;
		}
		break;	//there was some other reason
	}

//	if( nCountWritten )	//reCalcLen
//	{	//we have made changes in length not (EOF)addition only
//		AtClose.setBit(7);	//we had written something, we should activate that bit
////wxLogMessage("ReCalc length.. from not right = %u", size_t(P_DataLength) );
//		P_DataLength= 0;
//		for(size_t tempu=0; tempu<arrSources.size(); ++tempu){
//			fafikLib_readLineByte_base* tempClas= arrSources.at(tempu)->item;
//			if(tempClas && tempClas->IsOpened() && (tempClas->Length()!= -1) ){
//				P_DataLength+= tempClas->Length();
//			}
//		}
//	}

return nCountWritten;
}

wxFileOffset fafikLib_write_fileArr::setToCurrFileBegin()
{
	wxFileOffset retVal= arrSources.at(P_lastPosT)->item->Tell();
	arrSources.at(P_lastPosT)->item->Seek(0);
	return retVal;
}
bool fafikLib_write_fileArr::Seek(wxFileOffset ofs, wxSeekMode mode, bool doNotAddSize)
{
	if(doNotAddSize) return fafikLib_readLineByte_arr::Seek( ofs, mode );
	if( mode== wxFromStart ){
		return SeekMore(ofs);
	} else if( mode== wxFromCurrent ){
		return SeekMore(P_DataLength+ ofs);
	} else if( mode== wxFromEnd ){
		if( ofs<Length() )
			return Seek( Length() -ofs );
		return Seek( Length() );
	}
return 0;
}
	/// TODO (main#9#11/16/18): make sure it works	`SeekMore`, doesnt work
bool fafikLib_write_fileArr::SeekMore(wxFileOffset pos)
{
//	if( !IsOpened() ) return 0;
	if( pos> P_DataLength ){ //if needs to seek more
//		fafikLib_readLineByte_file
//		reOpenFile( *fWriteHand.back() );
		 //remove some left over size from this file
		P_DataLength-= arrSources.back()->item->Length();
		P_lastPos= P_DataLength;
		 //remove size that already exists
		pos-= P_DataLength;

		fafikLib_readLineByte_file* writeTo= nullptr;
		const char writeChar= 0x00;
		while( pos ){
			arrSources.back()->item->getAs_derived(&writeTo);
			if(!writeTo){ //its not a file
				fprintf(stderr, "Seek+Write operation expected a file Handle(got 0x%x)\n", arrSources.at(currFile)->item->getType_objectStored());
				return false;
			}


			wxFileOffset addHere= pos;
			if(filePartMaxSize){
				if( addHere> filePartMaxSize ){
					addHere= filePartMaxSize;
					makeNextPart();
				}
			}
			if( addHere ){
				if( writeTo->Length()< addHere ){
					writeTo->Seek( addHere- 1 );
					writeTo->Write( &writeChar, 1 );	//this is propably changing 1 Byte of data to 0x00
				} else {
					writeTo->SeekEnd();
				}
				pos-= addHere;
				P_DataLength+= addHere;
				P_lastPos+= addHere;
			} else
				break;
		}
	} else {
		return fafikLib_readLineByte_arr::Seek(pos);
	}
return 1;
}
bool fafikLib_write_fileArr::Flush()
{
	bool overall= 0;
	fafikLib_readLineByte_file* pointToFile= nullptr;
	for(size_t tempU=0; tempU<arrSources.size(); ++tempU){
		arrSources.at(tempU)->item->getAs_derived(&pointToFile);	//translate into _File
		if(pointToFile){	//if ok then try to flush
			pointToFile->Flush();
		}
	}
return overall;
}
void fafikLib_write_fileArr::init_PartSize(const wxFileOffset& initSize)
{
	if(filePartMaxSize== 0 && Length()== 0){
//	if( (!filePartMaxSize) || (initSize< filePartMaxSize) ){
		filePartMaxSize= initSize;
	}
}
void fafikLib_write_fileArr::init_PartSizeToCurrLenght()
{
	wxFileOffset initSize= Tell();
	if( (!filePartMaxSize) || (initSize< filePartMaxSize) ){
		filePartMaxSize= initSize;
	}
}
void fafikLib_write_fileArr::init_fileBaseNameAddition(const wxString& str)
{
	if(fileBaseNameAddition.empty()){
		fileBaseNameAddition= str;
	}
}
bool fafikLib_write_fileArr::Open(const wxString& pathName)
{
	Close_save(0);
	AtClose.changeBit(7);	//reset trim to (0)

	fileBaseName= pathName;

	wxFFile* newFFile= new wxFFile;
	wxString newFileName= fileBaseName;
	if( wxFileExists(newFileName) ){
	  if(openReadOnly){ //Read
		newFFile->Open( newFileName, "rb" );		//read openReadOnly
	  }
	  else { //Read-Write
	  	if( wxFileName::IsFileWritable( newFileName ) )
			newFFile->Open( newFileName, "r+b" );	//read write
	  }
	}
	else { //does not exist, Write
		wxLogNull noLog;
		newFFile->Open( newFileName, "w+b" );	//null write read
	}

	if(newFFile && newFFile->IsOpened() && (!newFFile->Error())){
		//ok
	} else {
		delete newFFile;
		return false;
	}

	fafikLib_readLineByte_arr::Add(newFFile, PAM_owns_all);

//	P_DataLength+= newFFile->Length();
return 1;
}
bool fafikLib_write_fileArr::Close_save(bool save)
{
//printf("  closing write_fileArr...\n");
	if(AtClose.getBit(0) && AtClose.getBit(7) ){
		AtClose.changeBit(7);
		setEndOfFile();	//trim to last pos
	}
	if(save || AtClose.getBit(1)){
		Flush();
	}
	fafikLib_readLineByte_arr::Close();
//	fWriteHand.clear();
	P_DataLength= P_lastPos= 0;
	currFile= 0;
//printf("  End close write_fileArr\n");
return 1;
}
bool fafikLib_write_fileArr::Close_ForceDrop()
{
	fafikLib_readLineByte_arr::Close();
	P_DataLength= P_lastPos= 0;
	currFile= 0;
return 1;
}

fafikLib_write_fileArr::fafikLib_write_fileArr(const wxString& filePathName, const wxString& filePathNameAdd, const wxFileOffset& _filePartMaxSize, bool8 _AtClose, bool _openReadOnly)
{
	_typeOfObject= objectType_writeArr;
	openReadOnly= _openReadOnly;
	Open(filePathName);
	fileBaseNameAddition= filePathNameAdd;
	filePartMaxSize= _filePartMaxSize;
	AtClose= _AtClose;
}
size_t fafikLib_write_fileArr::findLoadAllFiles(bool makeNewOnFullSize, size_t maxParts)
{
//	if( (!filePartMaxSize) || fWriteHand.at(0)->Length()== invalidFileOffset ) return -1;
	if( (!filePartMaxSize) || arrSources.at(0)->item->Length()== invalidFileOffset ) return -1;
	if( !Length() ) return -1;
	size_t retVal= 0;
//	if( IsOpened() );
//printf("start P_DataLength... %u\n", P_DataLength);
	wxFileOffset fileThisPartSize= 0;
	while( makeNextPart(1)!= size_ti && maxParts ) {
//printf("checking file... %u\n", currFile);

//		fileThisPartSize= fWriteHand.at(currFile)->Length();
		fileThisPartSize= arrSources.at(currFile)->item->Length();

//printf(" got size = %u\n", fileThisPartSize);
//		P_DataLength+= fileThisPartSize;
		++retVal;
		--maxParts;
	}
	if( makeNewOnFullSize && (fileThisPartSize==filePartMaxSize) ){
		makeNextPart();
//		fileThisPartSize= 0;
	}
Seek(0);
return retVal;
}
bool fafikLib_write_fileArr::setEndOfFile()
{
	bool retVal= 0;
//wxLogMessage("Tell(%u) < Length(%u)", size_t(Tell()), size_t(Length())  );
	if( !(Tell()< Length() && (!openReadOnly)) )
	{
		return retVal;
	}
	arrSources.at(currFile)->item->reOpen();
	wxString tempFileName( arrSources.at(currFile)->item->GetName() );
//	replaceAll( tempFileName, "/", "\\" );
//	replaceAll( tempFileName, "\\\\", "\\", 2 );
	wxFile tempFile;
	tempFile.Open( tempFileName, wxFile::read_write );
	tempFile.Seek( arrSources.at(currFile)->item->Tell() );
//printf("tempFile.len= %u, pos= %u\n", tempFile.Length(), tempFile.Tell() );

		//HANDLE hFile= (HANDLE)( fWriteHand.at(currFile)->fp() );
	HANDLE hFile= (HANDLE)_get_osfhandle( tempFile.fd() );
//	fWriteHand.at(currFile)->Close();
//		retVal= SetEndOfFile( fWriteHand.at(currFile)->fp() );
	retVal= ::SetEndOfFile( hFile );
//printf("  last err= %u\n", GetLastError() );
	tempFile.Flush();
	tempFile.Close();
//		retVal= ::SetEndOfFile( tempFile.fp() );
//		retVal= ::Set( tempFile.fp() );
//fWriteHand.at(currFile)->Flush();
//printf("\t now size= %u, now pos= %u\n", size_t( fWriteHand.at(currFile)->Length() ), size_t( fWriteHand.at(currFile)->Tell() ) );
//printf("  last err= %u\n", GetLastError() );
	if( (currFile+1)< arrSources.size() ){
		for(size_t tempu= currFile+1; tempu<arrSources.size(); ++tempu){
			arrSources.at(tempu)->~PAlocItemManagement();	//we need to close the file, to be able to delete it
//			delete arrSources.at(tempu);	//we need to close the file, to be able to delete it
//			arrSources.at(tempu)= nullptr;	//null it
			wxString newFileName= fileBaseName+ fileBaseNameAddition+ unifyNumber_lenght::unify( tempu, filePartKeepLen);
			if( wxFileExists(newFileName) ){
				wxRemoveFile( newFileName );	//remove file
			}
		}
			//	clear 2 tables
		arrSources.resize( currFile+1 );
//		fWriteHand.resize( currFile+1 );
	}

	P_DataLength= 0;
	fafikLib_readLineByte_base* tempClas= nullptr;
	for(size_t tempu=0; tempu<arrSources.size(); ++tempu){
		tempClas= arrSources.at(tempu)->item;
		if(tempClas && (tempClas->Length()!= invalidFileOffset ) ){
			P_DataLength+= tempClas->Length();
		}
	}
return retVal;
}










