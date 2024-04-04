/***************************************************************
 * Name:      pk2_jarMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    petrifikus ()
 * Created:   2024-03-11
 * Copyright: petrifikus ()
 * License:
 **************************************************************/
#include "ArchiveFile.h"


namespace fafik77 {
	ArchiveFile::~ArchiveFile()
	{
		Close();
	}
	ArchiveFile::ArchiveFile(const wxString& fileName)
	{
		arFiles.init_fileBaseNameAddition("mar");
		Open(fileName);
	}
	void ArchiveFile::Close()
	{
		arFiles.Close_ForceDrop();
	}
	void ArchiveFile::Open(const wxString& fileName)
	{
		Close();
		setNotDirty();
		arFiles.Open(fileName, true);
		arFiles.AtClose = UniFileStream_fileArr::AtClose_null;
		//check if archive was not fully saved
		//load archive parts
	}
	void ArchiveFile::setFilePartSize(wxFileOffset partMaxSize)
	{
		arFiles.init_PartSize(partMaxSize);
	}
	void ArchiveFile::Save()
	{
		if (!IsDirty()) return; //nothing changed

		if (IsDirtyInsert()) { //rebuild the archive
			//make new output file and write it all
			//arDictionary.DB_table
		}
		else if (IsDirtyAppend()) { //only change the end of the archive
			//save the old dictionary content to a temp file, && (dict starts at pos, append last pos)
			// 
			//open this file and change the end
		}
		setNotDirty();
	}
}