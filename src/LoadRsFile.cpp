//#ifndef LOADRSFILE_CPP
//#define LOADRSFILE_CPP

#include "LoadRsFile.h"
#include <wx/mstream.h>
#include <wx/msw/private.h>
#include <windows.h>


wxMemoryInputStream* GetResourceInputStream(const wxString& resource_name, const wxString& resource_type){
   HRSRC hrsrc=FindResourceW(wxGetInstance(), resource_name.ToStdWstring().c_str(), resource_type.ToStdWstring().c_str());
   if(hrsrc==NULL) return NULL;

   HGLOBAL hglobal=LoadResource(wxGetInstance(), hrsrc);
   if(hglobal==NULL) return NULL;

   void *data=LockResource(hglobal);
   if(data==NULL) return NULL;

   DWORD datalen=SizeofResource(wxGetInstance(), hrsrc);
   if(datalen==0) return NULL;

   return new wxMemoryInputStream(data, datalen);
}
//std::string *GetResourceInputStream_str(wxString resource_name, wxString resource_type)
//{
//   HRSRC hrsrc=FindResourceW(wxGetInstance(), resource_name, resource_type);
//   if(hrsrc==NULL) return NULL;
//
//   HGLOBAL hglobal=LoadResource(wxGetInstance(), hrsrc);
//   if(hglobal==NULL) return NULL;
//
//   void *data=LockResource(hglobal);
//   if(data==NULL) return NULL;
//
//   DWORD datalen=SizeofResource(wxGetInstance(), hrsrc);
//   if(datalen<1) return NULL;
//
//   return std::string(data, datalen);
//}

bool HasResource(const wxString& resource_name, const wxString& resource_type){
   HRSRC hrsrc=FindResourceW(wxGetInstance(), resource_name.ToStdWstring().c_str(), resource_type.ToStdWstring().c_str());
   if(hrsrc==NULL) return false;

   HGLOBAL hglobal=LoadResource(wxGetInstance(), hrsrc);
   if(hglobal==NULL) return false;

   return true;
}

//#endif
