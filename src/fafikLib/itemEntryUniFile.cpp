/*****************************************************************//**
 * \file   itemEntryUniFile.cpp
 * \brief  
 * 
 * \author petrifikus
 * \date   3.2024
 *********************************************************************/
#include "itemEntryUniFile.h"

namespace fafik77 {

itemEntryUniFile::itemEntryUniFile(const wxString& key, std::shared_ptr<UniFileStream_base>& uniFile)
	: uniFile(uniFile)
{
	this->key = key;
	this->type_stores = itemEntryStores::ie_none;
	_stores_derived = itemEntry_storeDerived::sdie_UniFile;
}

itemEntryUniFile::itemEntryUniFile(const wxString& key, const wxString& fileName)
{
	this->key = key;
	this->value = fileName;
	this->type_stores = itemEntryStores::ie_none;
	_stores_derived = itemEntry_storeDerived::sdie_UniFile;
}

bool itemEntryUniFile::open()
{
	if (uniFile.get()) { //file part, already opened
		return uniFile.get()->reOpen();
	}
	else if(this->value.size()){ //file by name
		uniFile = std::make_shared<UniFileStream_file>(this->value);
		return uniFile.get()->reOpen();
	}
	return false; //failed to open file
}

wxFileOffset itemEntryUniFile::WriteTo(UniFileStream_baseWritable* writeToFile, size_t readSize)
{
	if (!this->reOpen() || !writeToFile || !writeToFile->reOpen() || !writeToFile->can_write() )
		return wxFileOffset(-1);
	if (readSize == (size_t)-1 || readSize< size_t(1024)*1024) {
		readSize = (size_t(10) * 1024 * 1024);
	}
	wxFileOffset writtenTotal = 0;
	size_t reedNow, writtenNow;
	std::string buff;
 //try 2 times to allocate space
	try {
		buff.resize(readSize);
	}
	catch (...) {
		std::cerr << "could not allocate " << readSize << " Bytes\n";
		readSize = 1024;
	}
	if (buff.empty()) {
		try {
			buff.resize(readSize);
		}
		catch (...) {
			std::cerr << "!!could not allocate " << readSize << " Bytes\n";
			throw std::bad_alloc();
		}
	}

	auto uniFilePtr = uniFile.get();
	uniFilePtr->Seek(0); //make sure to get the file from beginning
	while (reedNow = uniFilePtr->Read(&buff.at(0), readSize)) {
		if (!reedNow) break; //read nothing
		writtenNow = writeToFile->Write(&buff.at(0), reedNow);
		if (writtenNow != reedNow) {
			//could not write what was reedNow
			throw FileOutOfBounds(writeToFile->Tell());
		}
		writtenTotal += writtenNow;
	}
	return writtenTotal;
}




}