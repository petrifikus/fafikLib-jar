/***************************************************************
 * Name:      pk2_jarMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    petrifikus ()
 * Created:   2024-03-11
 * Copyright: petrifikus ()
 * License:
 **************************************************************/
#ifndef FAFIK77_ARCHIVEFILE_H
#define FAFIK77_ARCHIVEFILE_H


#include "fafikLib_DB.h"
#include "itemEntryUniFile.h"

namespace fafik77 {
class ArchiveFile
{
public:
 //typedef, enum
	enum dirtyArTag {
		dirty_notDirty = 0,
		dirty_clean = dirty_notDirty,
		dirty_inserted = 1,
		dirty_appended = 2,
	};
	enum InsertOverwriteResponse {
		responde_none,
		responde_askUser = responde_none,
		responde_KeepOld,
		responde_KeepNew,
		responde_RemoveBoth,
	};
protected:
 //data
	 /*stores the file(s) that makes the archive*/
	UniFileStream_fileArr arFiles;
	 /*stores archive content*/
	fafikLib_DB arDictionary;
	 /*there are unsaved changes of dirtyArTag type*/
	BYTE dirty= dirty_notDirty;
	 /*how to handle insert name overlap*/
	BYTE responseOverwrite= responde_none;
	inline void setDirtyAppend() { dirty |= dirty_appended; }
	inline void setDirtyInsert() { dirty |= dirty_inserted; }
	inline void setNotDirty() { dirty = dirty_notDirty; }
public:

	~ArchiveFile();
	ArchiveFile(const wxString& fileName);
	
 //functions
	 /* closes this archive (without saving)*/
	void Close();
	 /* opens archive */
	void Open(const wxString& fileName);
	
	 /*appends file to the end of archive file*/
	void appendFile(const wxString& regex);
	/**
	 * @brief inserts file into sorted place
	 * @param regex -windows file regex to gather matching files
	 * @param [intoFolder] -place gathered files in this sub folder in archive
	*/
	void insertFile(const wxString& regex, const wxString& intoFolder="");
	 /*sorts the appended part of the archive files to be in alphabetical order*/
	void sort();
	 /*removes the matching files from the archive*/
	void remove(const wxString& regpath);
	 /*removes last n files from the archive*/
	void pop_back(int count);
	/**
	 * @brief extract files from the archive to the disk
	 * @param toFolder -destination folder (empty means ./)
	 * @param regpath -sellects matching files
	 * @return amount of files extracted
	*/
	size_t exctractFiles(const wxString& toFolder, const wxString& regpath);

	 /* saves archive */
	void Save();

	 /*split the archive file into smaller files that are sized partMaxSize*/
	void setFilePartSize(wxFileOffset partMaxSize);
	 /*automate the insert owerwriting file user choice, with one of InsertOverwriteResponse*/
	void setInsertOverwriteResponse(const InsertOverwriteResponse response) { responseOverwrite = response; }
	bool IsOpened() const { return arFiles.IsOpened(); }
	bool IsClosed() const { return !IsOpened(); }
	bool IsDirty() const { return dirty; }
	bool IsDirtyAppend() const { return dirty & dirty_appended; }
	bool IsDirtyInsert() const { return dirty & dirty_inserted; }

	//size_t getFileCount() const { return arDictionary.DB_table.size(); }
	//wxFileOffset Length() const { return arFiles.Length(); }
	//wxFileOffset Tell() const { return arFiles.Tell(); }
};

}
#endif //FAFIK77_ARCHIVEFILE_H