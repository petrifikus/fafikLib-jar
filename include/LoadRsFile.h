#ifndef LOADRSFILE_CPP
#define LOADRSFILE_CPP

#include <wx/mstream.h>
//#include <windows.h>


 ///you have to delete the result
wxMemoryInputStream* GetResourceInputStream(const wxString& resource_name, const wxString& resource_type);


bool HasResource(const wxString& resource_name, const wxString& resource_type);

#endif

