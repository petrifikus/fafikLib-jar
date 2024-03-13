#include <wx/string.h>
#include <wx/vector.h>
#include <windows.h>
#ifndef FONTMANAGER_H
#define FONTMANAGER_H

 ///a class Wrapper for safely Loading custom Fonts, Do not deliver from it
class FontManager
{
 public:
	FontManager();
	~FontManager();
	struct CustomFontsAndDw{
		wxString name;
		DWORD flags;
		CustomFontsAndDw(LPCWSTR name_, DWORD flags_): name(name_), flags(flags_) {}
		CustomFontsAndDw(const wxString &name_, DWORD flags_): name(name_), flags(flags_) {}
	};

	 ///an auto handled wrapper for AddFontResourceExW()
	 ///@param flags: FR_PRIVATE, FR_NOT_ENUM
	 ///@return 0 error, or numer of files loaded
	static int LoadCustomFont(const CustomFontsAndDw &fontToLoad);
	 ///an auto handled wrapper for AddFontResourceExW()
	 ///@param flags: FR_PRIVATE, FR_NOT_ENUM
	 ///@return 0 error, or numer of files loaded
	inline static int LoadCustomFont(LPCWSTR name, DWORD flags){ return LoadCustomFont( CustomFontsAndDw(name, flags) ); }
	 ///an auto handled wrapper for AddFontResourceExW()
	 ///@param flags: FR_PRIVATE, FR_NOT_ENUM
	 ///@return 0 error, or numer of files loaded
	inline static int AddCustomFont(const CustomFontsAndDw &fontToLoad){ return LoadCustomFont(fontToLoad); }
	 ///an auto handled wrapper for AddFontResourceExW()
	 ///@param flags: FR_PRIVATE, FR_NOT_ENUM
	 ///@return 0 error, or numer of files loaded
	inline static int AddCustomFont(LPCWSTR name, DWORD flags){ return LoadCustomFont( CustomFontsAndDw(name, flags) ); }

	static int UnLoadCustomFont(const CustomFontsAndDw &fontToUnload);
	inline static int RemoveCustomFont(const CustomFontsAndDw &fontToUnload){ return UnLoadCustomFont(fontToUnload); }

//  	struct _UnLoadCustomFonts_dst{
//		 ///will safely call RemoveFontResourceExW for all fonts loaded with LoadCustomFont()
//		void UnLoadCustomFonts();
//		~_UnLoadCustomFonts_dst(){ UnLoadCustomFonts(); }
// 	};
 protected:
 	void UnLoadCustomFonts();
 	 //define a Vector (needs to be crated after in .cpp)
	static wxVector<FontManager::CustomFontsAndDw> _CustomFontsVector;

 private:
};

#define wxAddFont FontManager::LoadCustomFont
#define wxLoadFont FontManager::LoadCustomFont


#endif // FONTMANAGER_H
