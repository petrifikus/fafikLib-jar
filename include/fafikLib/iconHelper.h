/** \file iconHelper.h
 * helps manage icons
 */

#ifndef ICONHELPER_H
#define ICONHELPER_H

#include <winsock2.h>
#include <windows.h>

namespace fafik77
{
	 ///returns icon that represents the .exe file
	HICON ExtractIconFromExe(LPCWSTR pszPath);
};

#endif // ICONHELPER_H
