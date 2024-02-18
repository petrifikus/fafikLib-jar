#include "itemEntry.h"

namespace fafik77{

itemEntry::~itemEntry()
{
	FreeTable();
	size_t temp_pos= getPosition();
	if(temp_pos!=size_t(-1)){
		parent->sub_itemTable.at(temp_pos)= nullptr; //make sure that parent cant acces us anymore(for part delete)
	}
}

itemEntry* itemEntry::Add_back(const wxString& key_, const wxString& value_, itemEntryStores type_stores_)
{
	if( value_.size() ) type_stores_= itemEntryStores(type_stores_| ie_value);
	itemEntry* newBackItem= new itemEntry(key_, value_, type_stores_, depth+1);
	sub_itemTable.push_back(newBackItem);
	return newBackItem;
}
itemEntry::iterator itemEntry::find( const wxString &item_byKey, itemEntryStores type, bool caseInsensitive)
{
	itemEntry templItem(item_byKey, type, caseInsensitive ? ie_caseInsensitive : ie_unspecified);
	iterator atPos= binaryGetPosOf( templItem );

	if( binaryGetPosOf_exists( atPos, templItem ) ){
		return atPos;
	}
return sub_itemTable.end();
}
itemEntry* itemEntry::find_linear(const wxString &item_byKey, bool caseInsensitive)
{
	for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++) {
		if( item_byKey.IsSameAs( this->sub_itemTable.at(temp_u)->key, (!caseInsensitive) ) ){
			return (this->sub_itemTable.at(temp_u) );
		}
		else
			continue; //i dont know why it doesnt do it itself
	}
return nullptr;
}
itemEntry* itemEntry::find_byValue( const wxString &item_byValue, bool caseInsensitive, size_t* outFoundPos )
{
	for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++){
		if( item_byValue.IsSameAs( this->sub_itemTable.at(temp_u)->value, (!caseInsensitive) ) ){
			if(outFoundPos) (*outFoundPos)= temp_u;
			return (this->sub_itemTable.at(temp_u) );
		}
		else
			continue; //i dont know why it doesnt do it itself
	}
if(outFoundPos) (*outFoundPos)= -1;
return nullptr;
}

size_t itemEntry::getPosition() const
{
	if(!parent) return -1;
	for(size_t temp_u=0; temp_u<parent->sub_itemTable.size(); temp_u++) {
		if(parent->sub_itemTable.at(temp_u)==this) return temp_u;
	}
return -1;
}

itemEntry* itemEntry::at(const size_t item, const bool auto_key_keyedSearch, const bool resolveLink)
{
	if( item>= this->sub_itemTable.size() ) return nullptr;
	if(auto_key_keyedSearch){	//2018-08-07
		if(!this->isAuto_key_keyed()) return nullptr;
		size_t autoToHere= -1;
		for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++){
			if( this->sub_itemTable.at(temp_u)->isAuto_key_keyed() ) autoToHere++;
			if( autoToHere== item){
//				if(this->sub_itemTable.at(temp_u)->isLink()) return this->sub_itemTable.at(temp_u)->linksTo;
				return this->sub_itemTable.at(temp_u);
			}
		}
		return nullptr;
	}
	else if( item < this->sub_itemTable.size()) {
//		if(resolveLink && this->sub_itemTable.at(item)->isLink()) return this->sub_itemTable.at(item)->linksTo;
		return this->sub_itemTable.at(item);
	}
return nullptr;
}


void itemEntry::FreeTable()
{
	for(itemEntry::const_iterator temp_it= sub_itemTable.begin(); temp_it!= sub_itemTable.end(); ++temp_it){
		itemEntry *toDel= *temp_it; //just to be sure that typeof is itemEntry
		//no nullptr
		if( !toDel ) continue; //dont try to access nullptr
		toDel->parent= nullptr;	//dont go upwards, we are taking care of this here
		delete toDel;	//run destructor for each
	}
	sub_itemTable.clear();	//clear vector it doesnt point to anything valid anymore
}


void itemEntry::Transfer_Ownership_subItemsTo( itemEntry& transferOwnershipTo )
{
	transferOwnershipTo.FreeTable();
	transferOwnershipTo.sub_itemTable= this->sub_itemTable;
	transferOwnershipTo.type_stores|= ie_Table;

	setSubsParent( &transferOwnershipTo );
	this->Detach();
}
void itemEntry::Transfer_Ownership_subItemsToMe( itemEntry& transferOwnershipFrom )
{
	this->FreeTable();
	this->sub_itemTable= transferOwnershipFrom.sub_itemTable;
	this->type_stores|= ie_Table;

	setSubsParent();
	transferOwnershipFrom.Detach();
}
void itemEntry::setSubsParent(itemEntry* newParent)
{
	for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++ ){
		this->sub_itemTable.at(temp_u)->parent= newParent;
	}
}

bool itemEntry::push_back_cpyTotal(itemEntry &push)
{
//	if(this->type_stores & ie_sub_itemTable) {} else return 0;
	itemEntry &new_ie= *new itemEntry;
	new_ie.key= push.key;
	new_ie.value= push.value;
	new_ie.type_stores= push.type_stores;
	new_ie.parent= this;
	new_ie.depth= this->depth+1;
//printf("%%%% copying |%s| = ",new_ie.key.ToStdString().c_str());
//printf("|%s|\n",new_ie.value.ToStdString().c_str());

	for(size_t temp_u=0; temp_u<push.sizeTable(); temp_u++){
		new_ie.type_stores|= ie_Table;
		new_ie.push_back_cpyTotal( *push.at(temp_u) );
	}
	this->sub_itemTable.push_back(&new_ie);

return true;
}
void itemEntry::MergeArrays(itemEntry* takeFromItem)
{
	if(!takeFromItem) return; //cant work with nullptr
	if( !(this->isArray() && takeFromItem->isArray()) ) return; //both have to be Arrays
	this->sub_itemTable.reserve( this->size()+ takeFromItem->size() ); //make space
	for(auto& eachItem: *takeFromItem){
		eachItem->parent= this;
		this->sub_itemTable.push_back(eachItem);
	}
	takeFromItem->Detach();
}
itemEntry::iterator itemEntry::AddItSmart( itemEntry* item )
{
	if(!item) return this->end();
	if( this->isAuto_key() ){
		item->parent= this; //assign this as parent
		item->type_stores|= ie_auto_key_keyed;
		if(!item->isLore()) item->type_stores|= ie_value;
		sub_itemTable.push_back(item);
		return this->end()-1;
	}
	else if( this->isTable() ){
		item->parent= this; //assign this as parent
		iterator new_Pos= binaryGetPosOf( *item );
		return insert( new_Pos, item );
	}
	return this->end();
}
itemEntry::iterator itemEntry::AddIt( const wxString& key_, const wxString& value_, itemEntryStores type_stores_)
{
	itemEntry *new_IE= new itemEntry( key_, value_, type_stores_, this->depth+1, this );
	if( (!value_.empty()) && !new_IE->isLore() ){
		new_IE->type_stores|= ie_value;
	}
	if(this->isAuto_key()) //you should not try to search through non keyed Table(Array)
		return AddItSmart(new_IE);

	iterator new_Pos= binaryGetPosOf( *new_IE );
	return insert( new_Pos, new_IE );
}
itemEntry* itemEntry::Add( const wxString& key_, const wxString& value_, itemEntryStores type_stores_ )
{
	return *AddIt( key_, value_, type_stores_ );
}
itemEntry* itemEntry::Add( itemEntryStores type_stores_, const wxString& value_ )
{
	if( !this->isAuto_key() ) return nullptr;
	return Add_back( wxEmptyString, value_, itemEntryStores(type_stores_ | ie_auto_key_keyed) );
}
itemEntry* itemEntry::Add( itemEntry* item )
{
	if(!item) return nullptr;
	if( this->isAuto_key() ){
		item->parent= this; //assign parent
		item->type_stores|= ie_auto_key_keyed;
		if(!item->isLore()) item->type_stores|= ie_value;
		sub_itemTable.push_back(item);
		return item;
	}
	else if( this->isTable() ){
		item->parent= this; //assign parent
		iterator new_Pos= binaryGetPosOf( *item );
		insert( new_Pos, item );
		return item;
	}
	return nullptr;
}
itemEntry* itemEntry::AddLink( itemEntry* itemToLink )
{
	 //we want to add this but with old parent
	if(!itemToLink) return nullptr;
	itemEntry* oldParrent= itemToLink->parent;
	iterator new_Pos= binaryGetPosOf( *itemToLink );
	itemToLink->parent= oldParrent;
	return *insert( new_Pos, itemToLink );
}
bool itemEntry::_sort( bool sortAllChildren )
{
	std::sort( begin(), end(), _operLowerB_compareIE_1 );
	if( sortAllChildren ){
		for( iterator itemIt= begin(); itemIt!=end(); ++itemIt ){
			(*itemIt)->_sort( sortAllChildren );
		}
	}
return sortAllChildren;
}
itemEntry::iterator itemEntry::insert(iterator it, itemEntry* item)
{
	item->parent= this;
	return sub_itemTable.insert(it, item);
}
itemEntry::iterator itemEntry::getOrAddIt( const wxString& key_, itemEntryStores type_, bool caseInsensitive, bool *out_found)
{
	itemEntry* templItem= new itemEntry(key_, type_, caseInsensitive ? ie_caseInsensitive : ie_unspecified );
	iterator atPos= binaryGetPosOf( *templItem );
  //check if item exists, if so return the existing one
	if( binaryGetPosOf_exists( atPos, *templItem ) ){
		delete templItem;	//free it
		if( out_found ) *out_found= true;
		return atPos;
	}
  //add new item
	if( out_found ) *out_found= false;
	type_stores|= ie_Table;
	return insert( atPos, templItem);	//add it, lock it
}
itemEntry *itemEntry::getByType( itemEntryStores by_type, size_t from_pos_ )
{
	for(size_t temp_u=from_pos_; temp_u<this->sub_itemTable.size(); temp_u++)
	{
		if( (this->sub_itemTable.at(temp_u)->type_stores &by_type)==by_type ){	//if trimmed== requested
			return this->sub_itemTable.at(temp_u);
		}
	}
	if( !this->parent ) return nullptr;
	return this->parent->getByType( by_type, this->getPosition()+1 );
}



bool itemEntry::removeItems(iterator it1, iterator it2, bool silent)
{
	for( iterator currIt= it1; currIt!= it2; ++currIt ){
		delete *currIt;
		if(silent) currIt= nullptr;
	}

	if(!silent) sub_itemTable.erase( it1, it2);
	return true;
}
bool itemEntry::removeItem(iterator it, bool silent)
{
	return removeItems( it, it+1, silent );
}
bool itemEntry::removeItem(size_t by_pos, bool silent)
{
	return removeItem( begin()+by_pos, silent );
}
bool itemEntry::removeItem(const wxString& by_string, bool silent)
{
	for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++)
	{
		if(this->sub_itemTable.at(temp_u)->key==by_string){
			return this->removeItem( temp_u, silent );
		} else {
			continue;
		}
	}
return false;
}
bool itemEntry::removeItem(const itemEntry* by_id, bool silent)
{
	for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++)
	{
		if(this->sub_itemTable.at(temp_u)== by_id){
			return this->removeItem( temp_u, silent );
		} else {
			continue;
		}
	}
return false;
}
bool itemEntry::removeItems(size_t pos_from, size_t amount, bool silent)
{
	return removeItems( begin()+pos_from, begin()+pos_from+amount, silent );
}
bool itemEntry::removeItems(iterator it1, size_t amount, bool silent)
{
	return removeItems( it1, it1+ amount, silent );
}

wxStringVector getParentTree(const itemEntry* getFrom)
{
	wxStringVector retAStr;
	if(!getFrom) return retAStr;
	do{
		retAStr.Insert(getFrom->key, 0);
		if(getFrom->parent){ //go up the tree
			getFrom= getFrom->parent;
		}
		else
			break;
	} while(getFrom);
	return retAStr;
}
wxString getParentTree(const itemEntry* getFrom, const wxString separator)
{
	wxStringVector temp_arrStr= getParentTree(getFrom);
	wxString ret_str;
	for(size_t pos_u=0; pos_u<temp_arrStr.size(); pos_u++){
		ret_str.Append(temp_arrStr.Item(pos_u));
		ret_str.Append(separator);
	}
	return ret_str;
}



itemEntry& itemEntry::operator[](const wxString &item)
{
	if(this->isArray()) return *Add(ie_value, item);
	return *getOrAdd(item, ie_value);
// if(invalidSearch){
//	return *this;
// }
//
//	for(size_t temp_u=0; temp_u<this->sub_itemTable.size(); temp_u++){
//		if(this->sub_itemTable.at(temp_u)->key==item){
//			return *(this->sub_itemTable.at(temp_u) );
//		} else {
//			continue;
//		}
//	}
//invalidSearch= true;
//return *this;
}







bool itemEntry::_operLowerB_compareIE( const_iterator item, const itemEntry& compa )
{
	return _operLowerB_compareIE_1( *item, &compa );
}
bool itemEntry::_operLowerB_compareIE_1( const itemEntry* item, const itemEntry* compaa )
{
		//return ( a < b)
		///cast to Editor readable form
	const itemEntry& inFind= *item;
	const itemEntry& compa= *compaa;

	if( unsigned compaStores= (compa.type_stores & ( ie_lore| ie_auto_key_keyed )) ){
		//test pos is NOT Lore, compa is Lore
		//same then dont move, else do move closer To beginning()
		unsigned inFindStores= (inFind.type_stores & ( ie_lore| ie_auto_key_keyed ));
		return inFindStores > compaStores;
		//order should be
		//Lore
		//Auto_Key
		//
		//key = value
	}
	//then search for string
		///compares key or value
	const wxString* str_a= &inFind.key;
	const wxString* str_b= &compa.key;

	bool8 compRes= _operLowerB_compareStr( str_a, str_b, true );
	if( !compRes.getBit(1) )	//are not Identical (LowerCased)
		return compRes.getBit(0);

	if( str_a->size() == str_b->size() ){	//LowerCased strings are exact, compare with case
		compRes= _operLowerB_compareStr( str_a, str_b, false );
		return compRes.getBit(0);
	}

	return str_a->size()< str_b->size();
}
bool8 itemEntry::_operLowerB_compareStr( const wxString* str_a, const wxString* str_b, bool caseInsensitive )
{
		///single char
	wxString str_ac('0'),str_bc('0');

	size_t strLen= str_a->size();
	size_t strCurrPos= 0;
	if( strLen> str_b->size() ) strLen= str_b->size();

	while( strCurrPos< strLen ) {
		str_ac= str_a->at(strCurrPos);
		str_bc= str_b->at(strCurrPos);
		if( caseInsensitive ){
			str_ac.LowerCase();	//we actually want to do that always, (Lower is used normally which means less lookups)
			str_bc.LowerCase();
		}
		if( str_ac< str_bc ) return bool8(0, true);
		if( str_ac> str_bc ) return bool8(0, false);
		++strCurrPos;
	}
	return bool8(1, true);
}

itemEntry::iterator itemEntry::binaryGetPosOf(const itemEntry& TemplateItem)
{
	return std::__lower_bound( sub_itemTable.begin(), sub_itemTable.end(), TemplateItem, itemEntry::_operLowerB_compareIE );
}
bool itemEntry::binaryGetPosOf_exists(const_iterator prevRetValue, const itemEntry& TemplateItem)const
{
	if(!prevRetValue) return 0;
	if(sub_itemTable.end()== prevRetValue) return 0;
	if( (*prevRetValue)->key== TemplateItem.key) return true;
return 0;
//	thatIt!= DF_list_p.end() && (*thatIt)->getPathName()== temp_findEnt.getPathName()
}






itemEntry* itemEntry_iterate::find( const wxStringVector& findByManyStr, bool caseInsensitive )
{
//	itemEntry_arrP;
	if(!LastFind) return nullptr;
	itemEntry* temp_recFind= LastFind;
	for(size_t temp_u=0; temp_u<findByManyStr.size(); temp_u++){
		temp_recFind= temp_recFind->findP(findByManyStr.Item(temp_u), ie_none, caseInsensitive);
		if(!temp_recFind) return LastFind=nullptr;
	}
return LastFind=temp_recFind;
}
wxVector<itemEntry*> itemEntry_iterate::get_TreeToNow()
{
	if(priv_B_fromLastByType){
		priv_B_fromLastByType= false;
		itemEntry_arrP.clear();
		itemEntry* currItem= LastFind->parent;
		while( currItem ){
			itemEntry_arrP.insert(0,currItem);
			currItem= currItem->parent;
		}
	}
	return itemEntry_arrP;
}


};// namespace

