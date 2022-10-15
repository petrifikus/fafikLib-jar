#ifndef RECURSIVERMDIR_H
#define RECURSIVERMDIR_H

#include <wx/dir.h>

///thought this would be bigger...
class recursiveRmDir
{
	public:
		recursiveRmDir();
		virtual ~recursiveRmDir();
		static size_t recRmDir(const wxString& dir_path);

	protected:

	private:
};

#endif // RECURSIVERMDIR_H
