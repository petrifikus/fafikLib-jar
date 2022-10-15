#include "recursiveRmDir.h"

recursiveRmDir::recursiveRmDir()
{
	//ctor
}

recursiveRmDir::~recursiveRmDir()
{
	//dtor
}

size_t recursiveRmDir::recRmDir( const wxString& dir_path )
{
	size_t ret_val= 0;
	wxDir *dir= new wxDir;
	if(dir->Open( dir_path ))
	{
		wxString temp_name;
		wxString fileName;
		if(dir->GetFirst( &temp_name )){
			do{
				fileName= dir_path+ "\\"+ temp_name;
				if( wxDirExists( fileName ) ){
					recRmDir(fileName);
				} else if( wxFileExists( fileName ) ){
					wxRemoveFile( fileName );
				}
			}
			while( dir->GetNext( &temp_name ) );
		}
	}

	if(wxDirExists(dir_path)) ret_val+= wxDir::Remove(dir_path);
	else {
		if(wxFileExists(dir_path))
		ret_val+= wxRemoveFile( dir_path );
	}
delete dir;
return ret_val;
}


