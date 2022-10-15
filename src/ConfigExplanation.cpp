#include "ConfigExplanation.h"

ConfigExplanation::ConfigExplanation()
{}

ConfigExplanation::~ConfigExplanation()
{}


size_t ConfigExplanation::findBy_name( wxString search_name )
{
	for(size_t temp_i=0; temp_i<ItemExplanation.size(); temp_i++ ){
		if( ItemExplanation.at(temp_i).name == search_name ) return temp_i;
	}
return -1;
}

size_t ConfigExplanation::findBy_userName( wxString search_name )
{
	for(size_t temp_i=0; temp_i<ItemExplanation.size(); temp_i++ ){
		if( ItemExplanation.at(temp_i).user_name == search_name ) return temp_i;
	}
return -1;
}


size_t ConfigExplanation::findItemInString(wxString &findIn)
{
	for(size_t temp_i=0; temp_i<ItemExplanation.size(); temp_i++ ){
		if(findIn.Find(ItemExplanation.at(temp_i).name)) return temp_i;
	}
	return -1;
}








