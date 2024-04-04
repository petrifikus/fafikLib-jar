/*****************************************************************//**
 * \file   itemEntryUniFile.h
 * \brief  
 * 
 * \author petrifikus
 * \date   3.2024
 *********************************************************************/
#ifndef FAFIK77_ITEMENTRYUNIFILE_H
#define FAFIK77_ITEMENTRYUNIFILE_H

#include "itemEntry.h"
#include <memory>
namespace fafik77 {

class FileOutOfBounds : public std::exception
{
public:
	FileOutOfBounds(const wxFileOffset& pos) noexcept : exception(std::string("File inaccessible from pos: "+std::to_string(pos)).data(), 1), pos(pos)
	{}
	wxFileOffset pos;
};

struct itemEntryUniFile : public itemEntry
{
 //data
	std::shared_ptr<UniFileStream_base> uniFile;
 //functions
	virtual ~itemEntryUniFile() {};
	itemEntryUniFile() { _stores_derived = itemEntry_storeDerived::sdie_UniFile; this->type_stores = itemEntryStores::ie_none; }
	itemEntryUniFile(const wxString& key, std::shared_ptr<UniFileStream_base>& uniFile );
	itemEntryUniFile(const wxString& key, const wxString& fileName);
	 /*makes sure that file is open
	  @return true if open, false if error
	 */
	bool open();
	/*check if file is open
	 @return true if open, false if error
	*/
	bool IsOpened()const { if (uniFile.get())return uniFile.get()->IsOpened(); return false; }
	/*try to re/open file
	 @return true if open, false if error
	*/
	inline bool reOpen() { return open(); }
	/*re close file
	 @return true if success, false if error
	*/
	bool reClose() { if (uniFile.get())return uniFile.get()->reClose(); return false; }
	/*get file size
	 @return size of file if open, -1 on error
	*/
	wxFileOffset Length()const { if (uniFile.get())return uniFile.get()->Length(); return -1; }

	/*writes content of this uni stream to given file
	 @return amount of bytes written, -1 on error
	 @param readSize specifies how many bytes should be read at once (def 10MB)
	*/
	wxFileOffset WriteTo(UniFileStream_baseWritable* writeToFile, size_t readSize= 10*1024*1024);
};

}// namespace
#endif //FAFIK77_ITEMENTRYUNIFILE_H