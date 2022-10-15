#include "FontManager.h"

#include <wingdi.h>	//AddFontResource()	//requires Linker->Liblary>Add ("Gdi32").lib/.dll

FontManager::FontManager()
{
}
FontManager::~FontManager()
{
	UnLoadCustomFonts();
}

 //create a Vector
wxVector<FontManager::CustomFontsAndDw> FontManager::_CustomFontsVector;
 //create an object of this so when program ends we will be able to safely run ~dst
FontManager _FontManagerLoadedFonts;

int FontManager::LoadCustomFont(const CustomFontsAndDw &fontToLoad)
{
	 //someone wants to load "" which is not gonna work
	if( fontToLoad.name.empty() ) return 0;
	 //check if its already added
	for(auto item_it= _CustomFontsVector.begin(); item_it!= _CustomFontsVector.end(); ++item_it){
		const CustomFontsAndDw &currItem= *item_it;
		if( fontToLoad.name == currItem.name ){
			 //it already exists, exit with status= 0 fonts loaded
			return 0;
		}
	}
	 //try to load it
	_CustomFontsVector.push_back( fontToLoad );
	return AddFontResourceExW( fontToLoad.name.ToStdWstring().c_str(), fontToLoad.flags, NULL );
}
void FontManager::UnLoadCustomFonts()
{
	for(auto item_it= _CustomFontsVector.begin(); item_it!= _CustomFontsVector.end(); ++item_it){
		const CustomFontsAndDw &currItem= *item_it;
//wprintf(L"removes: %s\n", currItem.name.ToStdWstring().c_str() );
		RemoveFontResourceExW( currItem.name.ToStdWstring().c_str(), currItem.flags, NULL );
	}
	_CustomFontsVector.clear();
}

int FontManager::UnLoadCustomFont(const CustomFontsAndDw &fontToUnload)
{
	for(auto item_it= _CustomFontsVector.begin(); item_it!= _CustomFontsVector.end(); ++item_it){
		const CustomFontsAndDw &currItem= *item_it;
		if( fontToUnload.name == currItem.name ){
			_CustomFontsVector.erase( item_it );
			return RemoveFontResourceExW( fontToUnload.name.ToStdWstring().c_str(), fontToUnload.flags, NULL );
		}
	}
return 0; //error
}




