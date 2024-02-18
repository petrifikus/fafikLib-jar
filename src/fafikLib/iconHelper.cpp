/** \file iconHelper.cpp
 *
 */
#include "iconHelper.h"


namespace fafik77
{
	HICON ExtractIconFromExe(LPCWSTR pszPath){
		//get icon for this exe file
		SHFILEINFOW sfi;
		if( 0!= SHGetFileInfoW(pszPath, 0, &sfi, sizeof(sfi), SHGFI_ICON) ){
			return sfi.hIcon;
		}
		return NULL;
	}









} //namespace
