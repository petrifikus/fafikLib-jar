#include "unifyNumber_lenght.h"

unifyNumber_lenght::unifyNumber_lenght()
{
	//ctor
}

unifyNumber_lenght::~unifyNumber_lenght()
{
	//dtor
}

wxString unifyNumber_lenght::unify(wxULongLong number, size_t amountTo)
{
	wxString to_ret = number.ToString();
	if(to_ret.length() < amountTo){
		for(size_t temp_i=0; temp_i<(to_ret.length() - amountTo) ; temp_i++){
			to_ret.insert(0,"0");
		}
	}

return to_ret;
}


