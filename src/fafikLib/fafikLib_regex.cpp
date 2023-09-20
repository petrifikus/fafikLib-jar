#include "fafikLib_regex.h"
#include <chrono>


fafikLib_regex::fafikLib_regex( const wxString& strPattern, bool caseInsensitive, DWORD FileAttribs_type )
{
	i_lastErrorCode= setPattern( strPattern, caseInsensitive, FileAttribs_type );
}
fafikLib_regex::~fafikLib_regex()
{
}
size_t fafikLib_regex::separatePath( const wxString& PathAsString, wxArrayString& pathInParts )
{
	pathInParts.clear();
	const wxString::const_iterator iterCS= PathAsString.begin();
	size_t findRes= 0;
	size_t findRes_last= 0;
	while( findRes!= wxString::npos ){
		findRes= PathAsString.find_first_of("/\\", findRes);
		if(findRes== wxString::npos)
			break;
		pathInParts.Add( wxString(iterCS+ findRes_last, iterCS+ findRes) );
		++findRes;
		findRes_last= findRes;
	}
	pathInParts.Add( wxString(iterCS+ findRes_last, PathAsString.end()) );
return pathInParts.size();
}

bool fafikLib_regex::isMatchingAttribFile_dir(const DWORD &fileAttribs, const DWORD &mask_testAttribs)
{
	 //check if we want to match ANY file, and then that its not an directory
	if( (mask_testAttribs & FILE_ATTRIBUTE_NORMAL) && !(fileAttribs & FILE_ATTRIBUTE_DIRECTORY) ){
		return true;	//is ANY(open closed sparse temp unrieadable network zip unzip online offline encrypted virtual file) file
	}
	return( fileAttribs & mask_testAttribs); //matches the mask
}
wxString fafikLib_regex::getHashPart(const wxArrayString& PathInParts, const resultPosition* resultPos, _FileType_MDFC_Checksum* HashType_o)
{
	if(resultPos==nullptr || HashType_o==nullptr || resultPos->empty()) return wxEmptyString;
	const wxString& fileName= PathInParts.Last();
	const resultPosition_part& resultFile= resultPos->back();
	wxString retVal;
	for( const resultPosition_it& resultFileNamePos : resultFile){	//search for result that contains Hash info
		if( resultFileNamePos.isHash() ){
			retVal= fileName.SubString(resultFileNamePos.pos_from, resultFileNamePos.pos_to);	//extract the Hash info
			(*HashType_o)= (_FileType_MDFC_Checksum)resultFileNamePos.Hash_type; 	//get the Hash Type
			break;
		}
	}
	return retVal;
}


int fafikLib_regex::testPath( const wxArrayString& PathInParts, const DWORD &fileAttribs, const bool& SamePath, const bool& isIncludeMode, canSkippSomeDirs* O_canSkippDirs, resultPosition* resultPos ) const
{
	if(!b_compiled){
		wxLogWarning("! Warning ! Cant test with empty, not set Pattern.");
		return hitTest_failedMatch;
	}
	if( fileAttribs && !isMatchingAttribFile_dir(fileAttribs, regex_item.FileType) ){
		return hitTest_failed_fileType;	//fail cause: type is wrong (but has to provide a type first(in case something cant get those attribs))
	}
	 ///for when we are running the test in include mode, we want to test the Directory tree as much as possible
	bool isFolder_inIncludeMode= (isIncludeMode && (fileAttribs & FILE_ATTRIBUTE_DIRECTORY) );
	 //we need to deal also with Folders in Include mode
	if( (!regex_item.canTest(PathInParts.size())) && (!isFolder_inIncludeMode) ){
		return hitTest_failed_subDirs; //fail cause: isRooted & given_depth!=depth; or given_depth is not enough
	}
	 //special case for rooted, over-deep include-Folder
	if( isFolder_inIncludeMode && (!regex_item.canTest_includeFolder(PathInParts.size())) ){
		return hitTest_failed_subDirs; //fail cause: isRooted & given_depth> depth;
	}
	if( !regex_item.items_pathParts.size() )
		return hitTest_ok;	//just in case someone runs this without pattern, return ok (that is wont listen to compilation errors)

	 ///index to PathInParts
	size_t indexPos_strArr= regex_item.getDepthStart(PathInParts.size());
	 ///iterator to matcher
	auto iterPathPart= regex_item.items_pathParts.begin();
	 ///stores the most significant return error
	int errReturned= 0;
	 ///old value of *O_canSkippDirs, will be saved when greater then before
	canSkippSomeDirs old_O_canSkippDirs= 0;
	if(O_canSkippDirs){
		old_O_canSkippDirs= (*O_canSkippDirs);
		if(old_O_canSkippDirs>= regex_item.items_pathParts.size()) old_O_canSkippDirs= regex_item.items_pathParts.size()-1;
	}
	 //return the result of match, clean and resize
	if(resultPos){
		resultPos->clear();
		resultPos->resize(regex_item.items_pathParts.size());
	}
	if( SamePath ){	//implements: skip directories before last item (this is very thrusting in caller proper handling directory Tree)
		 //basically every time you enter or exit a folder this should be `false`
		if(O_canSkippDirs){	//skip a specific amount
			indexPos_strArr+= old_O_canSkippDirs;
			iterPathPart+= old_O_canSkippDirs;
		} else {	//skip to last element
			indexPos_strArr= PathInParts.size() -1;
			iterPathPart= regex_item.items_pathParts.end() -1;
		}
	}

	size_t Temp_indexPos_= indexPos_strArr;
	  //iterate all path parts to check if canTest()
	for( auto iterPathPart_canTest= iterPathPart; iterPathPart_canTest!= regex_item.items_pathParts.end(); ++iterPathPart_canTest ){
		if( Temp_indexPos_ >= PathInParts.size() ) break;	//we cant go that far (would error c05)
		 ///alias with type
		const item_base_pathParts& iterPathPart_named= *iterPathPart_canTest;
		 //check if can test
		if( !iterPathPart_named.canTest( PathInParts.Item(Temp_indexPos_).size() ) )
			return hitTest_failedMatch;
		++Temp_indexPos_; //goto next part of provided pathInParts
	}

	  //iterate all path parts to testPathPart()
	for( ; iterPathPart!= regex_item.items_pathParts.end(); ++iterPathPart ){
		if( indexPos_strArr>= PathInParts.size() ){
			//we cant test further as this is as far as this(Dir tree) goes
			if(O_canSkippDirs){
				if(old_O_canSkippDirs> (*O_canSkippDirs)) //if made progress then save new value
					(*O_canSkippDirs)= old_O_canSkippDirs;
			}
			if(isFolder_inIncludeMode) //ok we tested as much as we could, break and return match
				break;
			return hitTest_failed_subDirs; //somehow we got here even thou the (if) statement bellow `isFolder_inIncludeMode` should have dealt with that
		}
		 ///alias with type
		const item_base_pathParts& iterPathPart_named= *iterPathPart;
		 ///create this for each PathPart
		itemPath_tempStore itPath_IO;
		 //work with this string (from array)
		itPath_IO.tempPosIO.PathElement_str_p= &PathInParts.Item(indexPos_strArr);
		itPath_IO.tempPosIO.index_inPathArr= indexPos_strArr;
	 //save results of match
		 //we want to get the result of match
		resultPosition_part* resultPos_part= nullptr;
		if(resultPos){
			resultPos_part= &resultPos->at( iterPathPart- regex_item.items_pathParts.begin() );
			resultPos_part->resize( iterPathPart_named.getSize() );
		}

		 //test, arg_3( Out result of match )
		errReturned= testPathPart( iterPathPart_named, itPath_IO, resultPos_part );
		 //check if error
		if( errReturned ){ //failed to match
			return hitTest_failedMatch;
		}
		++indexPos_strArr;
		++old_O_canSkippDirs;
	}
	if(O_canSkippDirs){
		if(old_O_canSkippDirs>= regex_item.items_pathParts.size()) old_O_canSkippDirs= regex_item.items_pathParts.size()-1;
		if(old_O_canSkippDirs> (*O_canSkippDirs)) //if made progress then save new value
			(*O_canSkippDirs)= old_O_canSkippDirs;
	}
return hitTest_ok;
}
int fafikLib_regex::testPathPart( const item_base_pathParts& namedP, itemPath_tempStore& itPath_IO, resultPosition_part* resultPos_part ) const
{
	 //quick answer YES matches any
	if( namedP._matchesAny ) return hitTest_ok;

	 ///stores the most significant return error
	int errReturned= 0;
	 ///map alias
	item_posIO& tempPosIO= itPath_IO.tempPosIO;
	 //resize Vectors to hold results
	tempPosIO.item_returns.resize( namedP.pos_end- namedP.pos_start );
	tempPosIO.item_returns_rBack.resize( tempPosIO.item_returns.size() );
	tempPosIO.item_returns_hinted.resize( tempPosIO.item_returns.size() );

	char foundType= 'f';
	  //do a forward find
	errReturned= testPath_forward(namedP, itPath_IO);
	  //do a back find, small-error ?
	if( errReturned== hitTest_failedPartially_checkBackwards ){
		errReturned= testPath_backward(namedP, itPath_IO);
		foundType= 'r';
	}
	  //do a find with hints (as 2 previous have failed but gave us min&max,start&end positions of where the matching parts are located)
	if( errReturned== hitTest_failedPartially_checkBackwards ){
		errReturned= testPath_withHints(namedP, itPath_IO);
		foundType= 'h';
	}
	  //save back result pos
	if(resultPos_part){
		if(foundType== 'f')	//forward
			copyPos_StartEnd_toResult(itPath_IO.tempPosIO.item_returns, *resultPos_part, namedP);
		if(foundType== 'r')	//rBack
			copyPos_StartEnd_toResult(itPath_IO.tempPosIO.item_returns_rBack, *resultPos_part, namedP);
		if(foundType== 'h')	//hinted
			copyPos_StartEnd_toResult(itPath_IO.tempPosIO.item_returns_hinted, *resultPos_part, namedP);
	}

	if(errReturned== hitTest_failedMatch){
		return hitTest_failedMatch;
	}
	return hitTest_ok;
}
int fafikLib_regex::testPath_forward( const item_base_pathParts& namedP, itemPath_tempStore& itPath_IO ) const
{
	bool b_failedPartially_checkBackwards= false;
	item_posIO& tempPosIO= itPath_IO.tempPosIO;
	int& errReturned= itPath_IO.errReturned;
	size_t itersAtIndex= 0;
	 //specify directions
	tempPosIO.b_reverseCheck.setBit(option_rCheck_fromEnd, false);	//find

	for( item_base_pathParts::iterator iterPos= namedP.pos_start; iterPos!= namedP.pos_end; ++iterPos ) {
		const item_base& namedItem= **iterPos;
		item_posFoundO& namedItemReturns= tempPosIO.item_returns.at(itersAtIndex);
		namedItemReturns.start_pos= tempPosIO.pos_start_begin;
		namedItemReturns.start_max_offset= tempPosIO.pos_start_maxOff;
		namedItemReturns.max_max_offset= tempPosIO.pos_off_maxPos;

		errReturned= namedItem.tryTest( tempPosIO, namedItemReturns );
		namedItemReturns.returnState= errReturned;
		 //remove the max allowed offset, as we used it here
		tempPosIO.removeSet_off_maxPos( namedItemReturns.found_pos_from, namedItemReturns.start_pos );

		if(errReturned== hitTest_failedMatch){
			return hitTest_failedMatch;
		}
		else if(errReturned== hitTest_failedPartially_checkBackwards){
			if(0 == itersAtIndex) return hitTest_failedMatch;
			b_failedPartially_checkBackwards= true;
			tempPosIO.item_returns.at(itersAtIndex-1).reFind_pos_backward= (tempPosIO.reverseFind ? tempPosIO.reverseFind : namedItemReturns.found_pos_from);
			tempPosIO.reverseFind= 0;
		}
		++itersAtIndex;
	} //end for(forward check)

	if(b_failedPartially_checkBackwards)
		return hitTest_failedPartially_checkBackwards;
	return hitTest_ok;
}
int fafikLib_regex::testPath_backward( const item_base_pathParts& namedP, itemPath_tempStore& itPath_IO ) const
{
	size_t itersAtIndex;
	bool b_failedPartially_checkBackwards= false;
	itersAtIndex= (namedP.pos_end- namedP.pos_start) -1;
	++itersAtIndex;
	 //set some declarations
	int& errReturned= itPath_IO.errReturned;
	item_posIO& tempPosIO= itPath_IO.tempPosIO;
	 //specify directions
	tempPosIO.b_reverseCheck.setBit( option_rCheck_fromEnd, true );	//r_find

	tempPosIO.pos_start_begin= tempPosIO.PathElement_str_p->size() -1;
	tempPosIO.pos_start_maxOff= 0;
	tempPosIO.pos_off_maxPos= 0;

	for( item_base_pathParts::iterator iterPos= namedP.pos_end-1 ; iterPos!= namedP.pos_start-1; --iterPos ) {
		--itersAtIndex;
		tempPosIO.item_returns_index= itersAtIndex;
		const item_base& namedItem= **iterPos;
		item_posFoundO& namedItemReturns= tempPosIO.item_returns_rBack.at(itersAtIndex);
		 //set find start values
		namedItemReturns.start_pos= tempPosIO.pos_start_begin;
		namedItemReturns.start_max_offset= tempPosIO.pos_start_maxOff;
		namedItemReturns.max_max_offset= tempPosIO.pos_off_maxPos;

		errReturned= namedItem.tryTest( tempPosIO, namedItemReturns );
		namedItemReturns.returnState= errReturned;
		 //remove the max allowed offset, as we used it here
		tempPosIO.removeSet_off_maxPos( namedItemReturns.found_pos_from, namedItemReturns.start_pos );

		if( errReturned== hitTest_failedMatch ) {
			return hitTest_failedMatch;
		}
		else if( errReturned== hitTest_ok_cantQuestion) {
		 //we got result "anything is possible, everything is good"
		}
		else if(errReturned== hitTest_failedPartially_checkBackwards){
			b_failedPartially_checkBackwards= true;
			tempPosIO.reverseFind= 0;
		}
	} //end for(reverse check)

	if(b_failedPartially_checkBackwards){
		return hitTest_failedPartially_checkBackwards;
	}
	return hitTest_ok;
}
int fafikLib_regex::testPath_withHints( const item_base_pathParts& namedP, itemPath_tempStore& itPath_IO ) const
{
	size_t itersAtIndex;
	itersAtIndex= (namedP.pos_end- namedP.pos_start) -1;
	++itersAtIndex;
	 //set some declarations
	int& errReturned= itPath_IO.errReturned;
	item_posIO& tempPosIO= itPath_IO.tempPosIO;
	 //specify directions
	tempPosIO.b_reverseCheck.setBit( option_rCheck_fromEnd, false );	//find
	tempPosIO.b_reverseCheck.setBit( option_rCheck_hints,   true  );	//with hints

	tempPosIO.pos_start_begin= tempPosIO.PathElement_str_p->size() -1;
	tempPosIO.pos_start_maxOff= 0;
	tempPosIO.pos_off_maxPos= 0;
	 ///after we checked item that was flagged("at bad spot") we need to make sure the following item is correct
	BYTE reCheckNextItem= 0;

	for(size_t atPos= 0; atPos< tempPosIO.item_returns.size(); ++atPos) {
		 ///this front item
		item_posFoundO& namedIt_f= tempPosIO.item_returns.at(atPos);
		 ///this reverse item
		const item_posFoundO& namedIt_r= tempPosIO.item_returns_rBack.at(atPos);

		 //check if there is a recheck requested
		if( namedIt_r.returnState || reCheckNextItem ){	//if there is an error (full ok=256 is out of Byte bounds and is =0)
			tempPosIO.item_returns_index= atPos;
			const item_base& namedItem= **(namedP.pos_start+ atPos);
			 ///alias to .item_returns_hinted.now()
			item_posFoundO& namedItemReturns= tempPosIO.item_returns_hinted.at(atPos);
			 //specify hints(if not continuing)
			if( !reCheckNextItem ){
				tempPosIO.pos_start_begin= namedIt_f.found_pos_from+ 1;	//we need to find this item further than it was found
				tempPosIO.pos_start_maxOff= namedIt_f.start_max_offset;	//but with all the sets..
				tempPosIO.pos_off_maxPos= namedIt_f.max_max_offset;		//but with all the sets.
			}
			 //(forward)test
			errReturned= namedItem.tryTest( tempPosIO, namedItemReturns );
			 //check if error
			if( errReturned >= hitTest_failedMatch ) return errReturned;
			 //save error
			namedItemReturns.returnState= errReturned;
			 //remove the max allowed offset, as we used it here
			tempPosIO.removeSet_off_maxPos( namedItemReturns.found_pos_from, namedItemReturns.start_pos );

			 //save back the new results
			namedIt_f= namedItemReturns;
			 //check if any error level
			if( errReturned ) {
				if( !reCheckNextItem ){
					printf(" !! F Error %i at pos %i ! fatal, can't check further back\n", errReturned, atPos);
					return hitTest_failedMatch;
				}
				printf(" !! Error %i at pos %i ! rechecking\n", errReturned, atPos);
				atPos= 0;	//reset checker back to 0
				reCheckNextItem= 0;
				continue;
			}
			if(reCheckNextItem) --reCheckNextItem;

			 //check if found item is in bounds
			if( !(namedItemReturns.found_pos_from< namedIt_r.found_pos_from) ){ //too far, error
				printf("item was found too far, exiting with fail (%i < %i).\n", namedItemReturns.found_pos_from, namedIt_r.found_pos_from);
				return hitTest_failedMatch;
			} else { //ok
				if( (atPos+1) < tempPosIO.item_returns.size() ) { //check next item
					 ///next front item
					const item_posFoundO& namedIt_f_next= tempPosIO.item_returns.at(atPos+1);

					if( !(namedItemReturns.found_pos_to < namedIt_f_next.found_pos_from)){ //not(ok)
						++reCheckNextItem; // force a recheck of next item
						continue; //do we just skip to next item here?
					}

					const size_t FindPosMax= tempPosIO.get_maxPosOfFind( false );
					const size_t FindFullPosMax= tempPosIO.get_maxPosOfFind_full( false );
					if( namedIt_f_next.found_pos_from > FindPosMax ){ //check if next item start is not too far from this item (by maxOff)
						if(namedIt_f_next.found_pos_from > FindFullPosMax){ //thats just an error
							return hitTest_failedMatch;
						}
						++reCheckNextItem; // force a recheck of next item
						printf("next item needs Pos rechecking\n");
					}
				} //end if has next item
			} //end if found before `namedIt_r.found_pos_from`
		}
	}
	return hitTest_match;
}

int fafikLib_regex::copyPos_StartEnd_toResult(const wxVector<item_posFoundO>& inputVec, resultPosition_part& outputResult, const item_base_pathParts& context_itemBasePP)const
{
	int copied= 0;
	outputResult.resize(inputVec.size());
	auto copyTo= outputResult.begin();
	auto contextIt= context_itemBasePP.begin();
	for(auto iterIn= inputVec.begin(); iterIn!= inputVec.end(); ++iterIn){
		++copied;
		const item_posFoundO &refIt= *iterIn;
		copyTo->pos_from= refIt.found_pos_from;
		copyTo->pos_to= refIt.found_pos_to;
		copyTo->type_processes= (*contextIt)->type_processes;
		copyTo->Hash_type= (*contextIt)->HashType;
	  //fix "*" pos
		if( (*contextIt)->type_processes== item_processes_Star){
		  //at front
			if(copied== 1){ //starts with
				copyTo->pos_from= 0;
				if( (iterIn+1) == inputVec.end() ) //no next item
					copyTo->pos_to= -1;
				else { //get pos-1 of next item
					copyTo->pos_to= ((iterIn+1)->found_pos_from)- 1;
				}
			}
			else{ //not starts with
				copyTo->pos_from= ((iterIn-1)->found_pos_to)+ 1;
			}
		  //at back
			if(copied== (int)inputVec.size()){ //ends with
				copyTo->pos_to= -1;
			}
			else { //not ends with
				copyTo->pos_to= ((iterIn+1)->found_pos_from)- 1;
			}
		}
		++copyTo;
		++contextIt;
	}
	return copied;
}

int fafikLib_regex::setPattern( const wxString& strPattern, bool caseInsensitive, DWORD FileAttribs_type )
{
	regex_item.FileType= FileAttribs_type& (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_DIRECTORY);

	full_regex_item_ParserTempStore ParserTempStore;

	pattern_store_struct pattern_storeS;
	pattern_storeS.strPattern_p= &strPattern;
	pattern_storeS.iterAtPos_curr= strPattern.begin();
	pattern_storeS.iterAtPos_end= strPattern.end();

	pattern_storeS.optionsMask.setBit(option_CaseSense, caseInsensitive);

	if( strPattern.EndsWith("/") ){
		wxLogWarning("RegPath compile fail:\nReason: %s", "RegPath cant end with Directory Separator '/'" );
		return error_setPatternAdv_Error;
	}

	int retVal= 0;
	int optimize_count_QM= 0;

	while( pattern_storeS.iterAtPos_curr!= strPattern.end() ){
		 ///alias for current char
		const wxChar32& charAtPos= *pattern_storeS.iterAtPos_curr;
		 //compress "???" instead of 3 entry, make 1 entry of 3
		if( optimize_count_QM && charAtPos!= wxChar32('?') ){
			regex_item.AddItem( new item_match_anyChar( optimize_count_QM ) );
			optimize_count_QM= 0;
		}
		 //starts advanced
		if( charAtPos== wxChar32('{') ){
			retVal= setPattern_advancedOrString( pattern_storeS );
			if( retVal== true ){
				 ///just in case, copy where this entry begun
				const wxString::const_iterator iterAtPos_advancedStart= pattern_storeS.iterAtPos_curr;
				++pattern_storeS.iterAtPos_curr;	//advance
				retVal= setPattern_advanced_enter( pattern_storeS );	//compile advanced rule
				if( retVal>= error_setPatternAdv_Error ){
					 //thats a hard error, exit now
					 ///mark End pos of (detected)error
					wxString::const_iterator toIter= pattern_storeS.iterAtPos_curr;
					if(toIter!= pattern_storeS.iterAtPos_end) ++toIter;
					wxLogWarning("RegPath compile fail:\n%s\nReason: %s", wxString(iterAtPos_advancedStart, toIter), pattern_storeS.reason );
					i_lastErrorCode= retVal;
					return retVal;
				}
				continue;	//skip to next check
			}
		}
		 //dir separator | specifier of rooted
		else if( charAtPos== wxChar32('/') ){
			if(pattern_storeS.iterAtPos_curr== strPattern.begin() ){
				 //this is not a dir separator, but a specifier that its rooted
				regex_item.is_rooted= true;
			}
			else {
				retVal= setPattern_advancedOrString( pattern_storeS );
				if( retVal== true ){
					 //this is a dir separator
					ParserTempStore.tempPathElements.push_back( PathElement_len_limiter(ParserTempStore.lastPathElIndex, regex_item.items_p.size() ) );
					ParserTempStore.lastPathElIndex= regex_item.items_p.size();
				}
			}
		}
		 //match anything any amount
		else if( charAtPos== wxChar32('*') ){
			retVal= setPattern_advancedOrString( pattern_storeS );
			if( retVal== true ){
				regex_item.AddItem( new item_match_star );
			}
		}
		 //match any 1 Char
		else if( charAtPos== wxChar32('?') ){
			retVal= setPattern_advancedOrString( pattern_storeS );
			if( retVal== true ){
				++optimize_count_QM;	//add it later with optimized
			}
		}
		 //a standard string?
		else {
			if(!pattern_storeS.begun_itemStr){
				pattern_storeS.begun_itemStr= true;
				pattern_storeS.iterAtPos_itemStart= pattern_storeS.iterAtPos_curr;	//we start the normal string item
			}

			if( charAtPos== wxChar32('\\') ){	//check && update it last
				++pattern_storeS.backslashCount;
			} else {
				if(pattern_storeS.backslashCount)
					retVal= setPattern_advancedOrString( pattern_storeS, true );	//we only want to half the amount of '\\'backslash, and dont end the string matcher
				pattern_storeS.backslashCount= 0;
			}
		}
		++pattern_storeS.iterAtPos_curr;	//move to next
	}

	retVal= setPattern_advancedOrString( pattern_storeS );	//finish
	 //this is a dir separator
	ParserTempStore.tempPathElements.push_back( PathElement_len_limiter(ParserTempStore.lastPathElIndex, regex_item.items_p.size() ) );
	ParserTempStore.lastPathElIndex= regex_item.items_p.size();
	 //save path parts
	regex_item.separatePathElement( ParserTempStore );

	b_compiled= true;
	i_lastErrorCode= hitTest_ok;
	return hitTest_ok;
}
bool fafikLib_regex::setPattern_advancedOrString(pattern_store_struct& pattern_storeS, const bool dontFinalize){
	wxString halfed_part;
	 //escape special char with '\\'
	processEscape( pattern_storeS.iterAtPos_curr, pattern_storeS.backslashCount, halfed_part );

	if(pattern_storeS.begun_itemStr){
		pattern_storeS.begun_itemStr= false;
		if( pattern_storeS.itemStrToAdd== nullptr ){
			pattern_storeS.itemStrToAdd= new item_match_string;
			pattern_storeS.itemStrToAdd->optionsMask.copyBit( option_CaseSense, pattern_storeS.optionsMask );
		}
		pattern_storeS.itemStrToAdd->match_str.append( pattern_storeS.iterAtPos_itemStart, pattern_storeS.iterAtPos_curr- pattern_storeS.backslashCount );
		pattern_storeS.itemStrToAdd->match_str.append( halfed_part );
	}

	if(pattern_storeS.backslashCount & 1){	//its not paired, half and cancel
		pattern_storeS.backslashCount= 0;
		return false;
	} else {
		if( !dontFinalize && pattern_storeS.itemStrToAdd ){
			if(pattern_storeS.itemStrToAdd->match_str.size()){	//we check if we have some string (just in case)
				pattern_storeS.itemStrToAdd->update();
				regex_item.AddItem( pattern_storeS.itemStrToAdd );
			} else {
				delete pattern_storeS.itemStrToAdd;	//we dismiss this
			}
			pattern_storeS.itemStrToAdd= nullptr;	//done, leave empty for next
		}
		if( dontFinalize && pattern_storeS.itemStrToAdd ){
			pattern_storeS.begun_itemStr= true;
			pattern_storeS.iterAtPos_itemStart= pattern_storeS.iterAtPos_curr;
		}
		pattern_storeS.backslashCount= 0;
		return true;
	}
return 0;
}
bool fafikLib_regex::processEscape(const wxString::const_iterator& currPos, const size_t backslashCount, wxString& outputEscaped)
{
	outputEscaped.clear();
	if(backslashCount){
		outputEscaped.reserve( (backslashCount/2) +1 );
		for(wxString::const_iterator atPos_bsl= (currPos - backslashCount)+1;
			atPos_bsl<= currPos;
			atPos_bsl+= 2
		){
			outputEscaped.append( *atPos_bsl );
		}
	}
	if(backslashCount & 1){	//its not paired, half and cancel
		return false;
	}
	return true;
}
int fafikLib_regex::setPattern_advanced_enter(pattern_store_struct& pattern_storeS)
{
	int retVal= 0;
	pattern_storeAdvTemp_struct advTemp_store;
	pattern_storeS.advTemp_store= &advTemp_store;
	advTemp_store.optionsOverride= pattern_storeS.optionsMask;
	BYTE findState= 0;
	BYTE findType= 0;

	while( pattern_storeS.iterAtPos_curr!= pattern_storeS.iterAtPos_end ){
		const wxChar32& charAtPos= *pattern_storeS.iterAtPos_curr;
		++pattern_storeS.iterAtPos_curr;

	 //match modifiers
		if( findState<=0 && charAtPos== wxChar32('!') ){
			findState+= 1;
			advTemp_store.optionsOverride.setBit(option_Inverted, true );
		}
		else if( findState<=1 && charAtPos== wxChar32('-') ){
			findState+= 2;
			advTemp_store.forcedFindMinus= true; 	//check for minus '-'Number
		}
		else if( findState<=1 && charAtPos== wxChar32(':') ){
			findState+= 2;
			const wxChar32& subCharAtPos= *pattern_storeS.iterAtPos_curr;
			++pattern_storeS.iterAtPos_curr;
			if( subCharAtPos== wxChar32('s') || subCharAtPos== wxChar32('S') ){
				advTemp_store.optionsOverride.setBit(option_CaseSense, false); 	//sensitive
			} else if( subCharAtPos== wxChar32('i') || subCharAtPos== wxChar32('I') ){
				advTemp_store.optionsOverride.setBit(option_CaseSense, true); 	//insensitive
			} else {
printf("!Error expected ':s' or ':i', but got ':%c'\n", subCharAtPos);
			}
		}
	 //match type
		else if( findState<=3 && (charAtPos== wxChar32('*') || charAtPos== wxChar32('.')) ){	//Any character
			findState= 4;
			findType= item_processes_AnyChar;
		}
		else if( findState<=3 && charAtPos== wxChar32('[') ){	//Character range
			findState= 11;
			retVal= setPattern_advanced_anyOfChars( pattern_storeS );
		}
		else if( findState<=3 && charAtPos== wxChar32('/') ){	//Character range
			findState= 11;
			retVal= setPattern_advanced_anyOfChars_predefined( pattern_storeS );
		}
		else if( findState<=3 && charAtPos== wxChar32('0') ){	//Number range
			findState= 4;
			findType= item_processes_Number;
		}
		else if( findState<=3 && charAtPos== wxChar32('?') ){	//Any of Strings / None of Strings
			findState= 4;
			findType= item_processes_AnyOneOfText;
		}
		else if( findState==4 && (charAtPos== wxChar32(',') || charAtPos== wxChar32(';')) ){
			 //find & set the only allowed separator char
			advTemp_store.advancedSeparatorChar= charAtPos;
			findState= 11;

			if( findType== item_processes_AnyChar )
				retVal= setPattern_advanced_anyChar( pattern_storeS );
			else if( findType== item_processes_Number )
				retVal= setPattern_advanced_number( pattern_storeS );
			else if( findType== item_processes_AnyOneOfText )
				retVal= setPattern_advanced_anyOneOfText( pattern_storeS );
		}
		 //end
		else if( findState>=10 && charAtPos== wxChar32('}') ){	//Any of Strings / None of Strings
			 //we want to start after '}', there is more string
			return retVal;
		}
		 //an error in provided string to compile
		if( retVal>= error_setPatternAdv_Error )
			return retVal;
	}
pattern_storeS.reason= "Advanced Pattern ended without closure '}' ";
return error_setPatternAdv_Error;	//we run out of string without encountering '}'
}
int fafikLib_regex::setPattern_advanced_findNext_separated(pattern_store_struct& pattern_storeS)
{
	pattern_storeAdvTemp_struct& advTemp_store= *pattern_storeS.advTemp_store;

	while( pattern_storeS.iterAtPos_curr!= pattern_storeS.iterAtPos_end ){
		const wxChar32& charAtPos= *pattern_storeS.iterAtPos_curr;
		if( !advTemp_store.advancedSeparatorChar && ( charAtPos== wxChar32(',') || charAtPos== wxChar32(';') ) ){
			advTemp_store.advancedSeparatorChar= charAtPos;
			++pattern_storeS.iterAtPos_curr;
			return 0;
		}
		else if(charAtPos== advTemp_store.advancedSeparatorChar ){
			++pattern_storeS.iterAtPos_curr;
			return 0;
		} else if(charAtPos== wxChar32('}') ){
			return error_setPatternAdv_preemptiveExit;
		}
		++pattern_storeS.iterAtPos_curr;
	}
return error_setPatternAdv_preemptiveExit;
}
int fafikLib_regex::setPattern_advanced_findNext_hash(pattern_store_struct& pattern_storeS)
{
	pattern_storeAdvTemp_struct& advTemp_store= *pattern_storeS.advTemp_store;
	bool foundHash= false;
	 ///local only iterate
	size_t offsetPos= 0;

	while( (pattern_storeS.iterAtPos_curr+ offsetPos)!= pattern_storeS.iterAtPos_end ){
		const wxChar32& charAtPos= *(pattern_storeS.iterAtPos_curr+ offsetPos);
		if( foundHash==false && charAtPos== wxChar32('#') ){	//find a hash(only once)
			foundHash= true;
			advTemp_store.found_start= pattern_storeS.iterAtPos_curr+ offsetPos;
		}
		else if(charAtPos== advTemp_store.advancedSeparatorChar ){	//if found return 0
			advTemp_store.found_end= pattern_storeS.iterAtPos_curr+ offsetPos;
			if(foundHash) break;	//ok
			return error_setPatternAdv_slight;	//omitted (no Hash #)
		} else if(charAtPos== wxChar32('}') ){		//if found return 0
			advTemp_store.found_end= pattern_storeS.iterAtPos_curr+ offsetPos;
			if(foundHash) break;	//ok
			return error_setPatternAdv_slight;	//error not found (no Hash #)
		}
		++offsetPos;
	}
	if(foundHash){//ok
		pattern_storeS.iterAtPos_curr+= offsetPos;
		return 0; //ok
	}
return error_setPatternAdv_preemptiveExit;	//error not found
}
_FileType_MDFC_Checksum fafikLib_regex::setPattern_advanced_findNext_hashProcess(const pattern_store_struct& pattern_storeS)
{
	const pattern_storeAdvTemp_struct& advTemp_store= *pattern_storeS.advTemp_store;

 //get unified string
	wxString HashStr(advTemp_store.found_start, advTemp_store.found_end);
	if(HashStr.StartsWith("#")) HashStr.Remove(0,1);
	HashStr.LowerCase();
	 ///found on pos
	size_t foundPos= HashStr.npos;

	if( HashStr.npos!=(foundPos= HashStr.find("crc")) ){
		HashStr.Remove(0,foundPos+3);
		if(HashStr.Contains("16")) return _FileType_MDFC_Checksum_crc16;
		if(HashStr.Contains("32")) return _FileType_MDFC_Checksum_crc32;
		if(HashStr.Contains("64")) return _FileType_MDFC_Checksum_crc64;
	}
	else if( HashStr.npos!=(foundPos= HashStr.find("md")) ){
		HashStr.Remove(0,foundPos+2);
		if(HashStr.Contains("2")) return _FileType_MDFC_Checksum_MD2;
		if(HashStr.Contains("4")) return _FileType_MDFC_Checksum_MD4;
		if(HashStr.Contains("5")) return _FileType_MDFC_Checksum_MD5;
		if(HashStr.Contains("6")) return _FileType_MDFC_Checksum_MD6;
	}
	else if( HashStr.npos!=(foundPos= HashStr.find("sha")) ){
		HashStr.Remove(0,foundPos+3);
		if(HashStr.StartsWith("-")) HashStr.Remove(0,1);
		if(HashStr.StartsWith("_")) HashStr.Remove(0,1);
		if(HashStr.StartsWith(" ")) HashStr.Remove(0,1);

		if(HashStr.StartsWith("2")) {
			if(HashStr.Contains("224")) return _FileType_MDFC_Checksum_SHA2_224;
			if(HashStr.Contains("384")) return _FileType_MDFC_Checksum_SHA2_384;
			if(HashStr.Contains("512")) return _FileType_MDFC_Checksum_SHA2_512;

			return _FileType_MDFC_Checksum_SHA2_256; //sha2 256 by default
		}
		else if( HashStr.StartsWith("3") ) {
			if(HashStr.Contains("224")) return _FileType_MDFC_Checksum_SHA3_BLAKE2_224;
			if(HashStr.Contains("384")) return _FileType_MDFC_Checksum_SHA3_BLAKE2_384;
			if(HashStr.Contains("512")) return _FileType_MDFC_Checksum_SHA3_BLAKE2_512;

			return _FileType_MDFC_Checksum_SHA3_BLAKE2_256; //sha3 256 by default
		}
		else { //try sha2 variants without mentioning the sha-2
			if(HashStr.Contains("224")) return _FileType_MDFC_Checksum_SHA2_224;
			if(HashStr.Contains("256")) return _FileType_MDFC_Checksum_SHA2_256;
			if(HashStr.Contains("384")) return _FileType_MDFC_Checksum_SHA2_384;
			if(HashStr.Contains("512")) return _FileType_MDFC_Checksum_SHA2_512;
		}
		return _FileType_MDFC_Checksum_SHA1; //sha1 by default
	}

	return _FileType_MDFC_Checksum_none;
}
int fafikLib_regex::setPattern_advanced_findNumberStr(pattern_store_struct& pattern_storeS, BYTE find_dot_forDouble)
{
	pattern_storeAdvTemp_struct& advTemp_store= *pattern_storeS.advTemp_store;
	int retVal= 0;
	bool find_coma_forDouble= (find_dot_forDouble && advTemp_store.advancedSeparatorChar== wxChar32(';') );

	while( pattern_storeS.iterAtPos_curr!= pattern_storeS.iterAtPos_end ){
		const wxChar32& charAtPos= *pattern_storeS.iterAtPos_curr;

		if( charAtPos>= wxChar32('0') && charAtPos<= wxChar32('9') ){
			if(!retVal){
				retVal= 1;
				advTemp_store.found_start= pattern_storeS.iterAtPos_curr;
				if( *(pattern_storeS.iterAtPos_curr-1)== wxChar32('-') ){
					--advTemp_store.found_start;
				}
			}
		}
		else if(find_dot_forDouble== true && (charAtPos== wxChar32('.') || (find_coma_forDouble && charAtPos== wxChar32(',') ) ) ){
			if(!retVal){
				advTemp_store.found_start= pattern_storeS.iterAtPos_curr;
				if( *(pattern_storeS.iterAtPos_curr-1)== wxChar32('-') ){
					--advTemp_store.found_start;
				}
			}
			find_dot_forDouble= 2;
			retVal= 2 + (charAtPos== wxChar32(',')) ;
		}
		else if( charAtPos== advTemp_store.advancedSeparatorChar ){
			advTemp_store.found_end= pattern_storeS.iterAtPos_curr;
			return retVal;
		}
		else if( charAtPos== wxChar32('}') ){	//safety
			advTemp_store.found_end= pattern_storeS.iterAtPos_curr;
			return retVal;
		}
		else if( retVal ){
			break;
		}

		++pattern_storeS.iterAtPos_curr;	//advance iter
	}
	advTemp_store.found_end= pattern_storeS.iterAtPos_curr;
	return retVal;
}
int fafikLib_regex::setPattern_advanced_findFloatSpecyfic(pattern_store_struct& pattern_storeS)
{
	pattern_storeAdvTemp_struct& advTemp_store= *pattern_storeS.advTemp_store;
	wxString::const_iterator localIter= pattern_storeS.iterAtPos_curr;
	wxChar32 userQuotes= 0;
	wxChar32 userFloatSep= 0;

	while( localIter!= pattern_storeS.iterAtPos_end ){
		const wxChar32& charAtPos= *localIter;
		if( !userQuotes && ( charAtPos=='\'' || charAtPos=='\"' ) ){
			userQuotes= charAtPos;
		} else if(userQuotes && !userFloatSep){
			userFloatSep= charAtPos;
		} else if(userQuotes && userFloatSep ){	//we found start(".) and now we confirm the (")end
			if( charAtPos== userQuotes ){	//ok
				size_t offsetFromOrig= localIter- pattern_storeS.iterAtPos_curr;
				if(localIter+1 == pattern_storeS.iterAtPos_end)
					return error_setPatternAdv_Error;	//thats wrong syntax
				if( offsetFromOrig<=3 ){
					pattern_storeS.iterAtPos_curr= localIter;	//copy iter
					advTemp_store.floatSepChar= userFloatSep;	//copy char
					return error_setPatternAdv_ok;				//return ok
				} else {
					return error_setPatternAdv_slight;	//ended further then allowed
				}
			} else {	//we were so close
				pattern_storeS.iterAtPos_curr= localIter;	//copy end pos
				return error_setPatternAdv_Error;	//thats bad
			}
		} else if( !userQuotes && charAtPos=='}' ){
			return error_setPatternAdv_preemptiveExit;	//ended faster then wanted
		}
		++localIter;	//advance
	}
return error_setPatternAdv_slight;
}

int fafikLib_regex::setPattern_advanced_anyChar(pattern_store_struct& pattern_storeS)
{
	pattern_storeAdvTemp_struct& advTemp_store= *pattern_storeS.advTemp_store;
	 ///yes here we want to add this item to matter what does not follow
	item_match_anyChar* temp_itemAdd= new item_match_anyChar;
	int retVal= 0;
	long tempNumber_min= 0;
	long tempNumber_max= 0;
	wxString numberExtracted;

	retVal= setPattern_advanced_findNumberStr( pattern_storeS );
	if( retVal ){	//we found a number
		numberExtracted.assign( advTemp_store.found_start, advTemp_store.found_end );
		numberExtracted.ToLong( &tempNumber_min );

		setPattern_advanced_findNext_separated( pattern_storeS );	//find ',;'

		retVal= setPattern_advanced_findNumberStr( pattern_storeS );
		if( retVal ){	//we found a number
			numberExtracted.assign( advTemp_store.found_start, advTemp_store.found_end );
			numberExtracted.ToLong( &tempNumber_max );

			temp_itemAdd->set( tempNumber_min, tempNumber_max );
		} else {
			temp_itemAdd->set( tempNumber_min );
		}
	} else {
		temp_itemAdd->set( 1 );
	}

	regex_item.AddItem( temp_itemAdd );	//add the item

return 0;
}
int fafikLib_regex::setPattern_advanced_anyOfChars(pattern_store_struct& pattern_storeS)
{
	pattern_storeAdvTemp_struct& advTemp_store= *pattern_storeS.advTemp_store;
	item_match_anyOfChars* temp_itemAdd= new item_match_anyOfChars;
	 ///we have to delete the item if error occurs
	_safeItem_based safeItem_based( temp_itemAdd );
	 //copy options mask.( option_Inverted | )
	temp_itemAdd->optionsMask.copyBit(option_Inverted, advTemp_store.optionsOverride );

	int retVal= 0;
	wxChar32 LastChar= 0;
	size_t backslCount= 0;
	wxString tempStoreStr;


 //find ranges [a-z]
	bool findsRanges= true;
	bool ThisIsRange= false;
	advTemp_store.found_start= advTemp_store.found_end= pattern_storeS.iterAtPos_end;	//copy start & end pos

	while( pattern_storeS.iterAtPos_curr!= pattern_storeS.iterAtPos_end ){
		const wxChar32& charAtPos= *pattern_storeS.iterAtPos_curr;
		const wxChar32& charNext= *(pattern_storeS.iterAtPos_curr +1);

		if( charAtPos== wxChar32(']') || charAtPos== wxChar32('}') ){
			if( LastChar== wxChar32('\\') ){
				++backslCount;
				findsRanges= false;
				ThisIsRange= false;
				LastChar= 0;
			}
			 //see if that is the end OR if thats error end
			if( processEscape( pattern_storeS.iterAtPos_curr, backslCount, tempStoreStr ) ){
				 //finish
				if(LastChar && findsRanges) temp_itemAdd->add_char(LastChar); //add last char
				advTemp_store.found_end= pattern_storeS.iterAtPos_curr;	//copy end pos
				temp_itemAdd->add_char(advTemp_store.found_start, advTemp_store.found_end - backslCount);
				temp_itemAdd->add_char( tempStoreStr.begin(), tempStoreStr.end() );
				advTemp_store.found_start= advTemp_store.found_end;
				backslCount= 0;
				if( charAtPos== wxChar32('}') ) retVal= 2;
				if( charAtPos== wxChar32(']') && charNext== wxChar32('[') ){
					pattern_storeS.iterAtPos_curr += 2; //advance by 2 && continue
					advTemp_store.found_start= advTemp_store.found_end= pattern_storeS.iterAtPos_curr;
					 //allow {[a-z][,./-+=],1,2}
					findsRanges= false;
					ThisIsRange= false;
					LastChar= 0;
					continue;
				}
				break;
			} else {
				 //escaped, continue
				advTemp_store.found_end= pattern_storeS.iterAtPos_curr;	//copy end pos
				temp_itemAdd->add_char(advTemp_store.found_start, advTemp_store.found_end - backslCount);
				temp_itemAdd->add_char( tempStoreStr.begin(), tempStoreStr.end() );
				advTemp_store.found_end+= 1;
				advTemp_store.found_start= advTemp_store.found_end;
				backslCount= 0;
			}
		}
		else if(findsRanges && !LastChar){
			LastChar= charAtPos;	//just copy the char & end
		}
		else if(findsRanges && LastChar){
			if( charAtPos== wxChar32('-') ){
				 //we accept that as range (this-1, this+1)
				ThisIsRange= true;
			} else if(!ThisIsRange) {
				 //error, not a range
				findsRanges= false;	//no more ranges, move to chars list
				advTemp_store.found_start= pattern_storeS.iterAtPos_curr -1;	//here we already made sure that pos current-1 is ours
				if( LastChar== wxChar32('\\') )
					++backslCount;
			} else if(ThisIsRange) {
				 //ok, add range
				temp_itemAdd->add_range( item_CharsRange_FT(LastChar, charAtPos) );
				ThisIsRange= false;	//range done, clear
				LastChar= 0;		//range done, clear
			}
		}
		else if( !findsRanges ){
			if( charAtPos== wxChar32('\\') )
				++backslCount;
		}
		++pattern_storeS.iterAtPos_curr;	//advance
	} //end while{try to add ranges or chars to list}

	if(!findsRanges){
		 //add all single chars as list
		temp_itemAdd->add_char(advTemp_store.found_start, advTemp_store.found_end);
	}

	if(temp_itemAdd->empty()) {
		 //so for some reason we have an empty AnyOfChar matcher witch does NO good.
		pattern_storeS.reason= "Range/List [] is empty";
		return error_setPatternAdv_Error;
	}

	if( retVal== 2  || pattern_storeS.iterAtPos_curr== pattern_storeS.iterAtPos_end ){	//we found '}' which is bad, so just exit now
		pattern_storeS.reason= "incomplete: {[a-z],1}";
		return error_setPatternAdv_Error;
	}


 //prepare to find 2 numbers (min, max), AND #Hash
	retVal= setPattern_advanced_findNext_separated( pattern_storeS );	//find ',;'
	bool HashFound= false;
	if(!retVal){ // no error
	  //find Hash
		retVal= setPattern_advanced_findNext_hash( pattern_storeS );	//find '#'
		if(retVal == 0){
			HashFound= true;
			temp_itemAdd->HashType= setPattern_advanced_findNext_hashProcess( pattern_storeS );	//get Hash
			if(temp_itemAdd->HashType!= _FileType_MDFC_Checksum_none)
				temp_itemAdd->type_processes|= item_processes_xHashMask;
			retVal= setPattern_advanced_findNext_separated( pattern_storeS );	//find ',;'
		}
	}
	long tempNumber_min= 0;
	long tempNumber_max= 0;
	 //find numbers
	retVal= setPattern_advanced_findNumberStr( pattern_storeS );
	if(retVal){
		tempStoreStr.assign( advTemp_store.found_start, advTemp_store.found_end );
		if(retVal==3) tempStoreStr.Replace( ",", "." );
		tempStoreStr.ToLong( &tempNumber_min );

		retVal= setPattern_advanced_findNext_separated( pattern_storeS );	//find ',;'
		retVal= setPattern_advanced_findNumberStr( pattern_storeS );

		if(retVal){
			tempStoreStr.assign( advTemp_store.found_start, advTemp_store.found_end );
			tempStoreStr.ToLong( &tempNumber_max );
			if( temp_itemAdd->setAmount( tempNumber_min, tempNumber_max ) ){	//check if its not an error
				pattern_storeS.reason= "min can NOT be greater then max";
				return error_setPatternAdv_Error;
			}
		}
		else
			temp_itemAdd->setAmount( tempNumber_min );	//only 1 number, that means (min= max= Number)

		if(!HashFound){ // no hash found yet
		  //find Hash
			retVal= setPattern_advanced_findNext_hash( pattern_storeS );	//find '#'
		  //could not, find comma and retry
			if(retVal == error_setPatternAdv_slight){
				retVal= setPattern_advanced_findNext_separated( pattern_storeS );	//find ',;'
				retVal= setPattern_advanced_findNext_hash( pattern_storeS );	//find '#'
			}
		  //process Hash
			if(retVal == 0){
				HashFound= true;
				temp_itemAdd->HashType= setPattern_advanced_findNext_hashProcess( pattern_storeS );	//get Hash
				if(temp_itemAdd->HashType!= _FileType_MDFC_Checksum_none)
					temp_itemAdd->type_processes|= item_processes_xHashMask;
			}
		}
	} else {
		 //so we did not find a number to specify the amount, we assume =1
		temp_itemAdd->setAmount( 1 );
	}

	 //does this entry check #Hash
	if(temp_itemAdd->HashType != _FileType_MDFC_Checksum_none)
		regex_item.has_hash= true;
	 //we add the item to list, and disarm() the auto deleter
	regex_item.AddItem( temp_itemAdd );
	safeItem_based.disarm();

return 0;
}
int fafikLib_regex::setPattern_advanced_anyOfChars_predefined(pattern_store_struct& pattern_storeS)
{
	pattern_storeAdvTemp_struct& advTemp_store= *pattern_storeS.advTemp_store;
	item_match_anyOfChars* temp_itemAdd= new item_match_anyOfChars;
	regex_item.AddItem( temp_itemAdd );

	int retVal= 0;
	BYTE firstCharCounter= false;

	advTemp_store.found_end= pattern_storeS.iterAtPos_end;	//copy end pos (just in case we hit EOL)

	if( pattern_storeS.iterAtPos_curr== pattern_storeS.iterAtPos_end )
		return error_setPatternAdv_Error;

	const wxChar32& charAtPos= *pattern_storeS.iterAtPos_curr;
	switch (charAtPos) {
	//words only
	 case 'W':
		temp_itemAdd->optionsMask.setBit(option_Inverted, true);
	 case 'w':
		temp_itemAdd->add_range( item_CharsRange_FT('A', 'Z') );
		temp_itemAdd->add_range( item_CharsRange_FT('a', 'z') );
		temp_itemAdd->add_range( item_CharsRange_FT('0', '9') );
		temp_itemAdd->add_char('_');
	 break;
	//digits
	 case 'D':
		temp_itemAdd->optionsMask.setBit(option_Inverted, true);
	 case 'd':
		temp_itemAdd->add_range( item_CharsRange_FT('0', '9') );
	 break;
	//space chars
	 case 'S':
		temp_itemAdd->optionsMask.setBit(option_Inverted, true);
	 case 's':
		temp_itemAdd->charsToMatch= L" \t\n\x0B\f\r　";
	 break;
	//utf
	 case 'u':
		temp_itemAdd->optionsMask.setBit(option_Inverted, true);
	 case 'U':
		temp_itemAdd->add_range( item_CharsRange_FT(0, 127) );
	 break;

	 default:
		//thats an error
		firstCharCounter= 2;	//make an error
	 break;
	}

	if( firstCharCounter== 2 ){	//thats the error
		pattern_storeS.reason= "Unknown character class: '/";
		pattern_storeS.reason+= wxUniChar(charAtPos);
		pattern_storeS.reason+= "' for predefined character range";
		return error_setPatternAdv_Error;
	}

	++pattern_storeS.iterAtPos_curr;	//advance to next char
	if( pattern_storeS.iterAtPos_curr== pattern_storeS.iterAtPos_end )
		return error_setPatternAdv_Error;
	advTemp_store.found_start= pattern_storeS.iterAtPos_curr;	//we want to catch all chars after '/wsd' till separator ',;'

	retVal= setPattern_advanced_findNext_separated( pattern_storeS );	//find ',;'
	if( pattern_storeS.iterAtPos_curr== pattern_storeS.iterAtPos_end )
		return error_setPatternAdv_Error;

	if( !retVal )
		advTemp_store.found_end= pattern_storeS.iterAtPos_curr -1;	//we dont want the separator char itself
	else
		advTemp_store.found_end= pattern_storeS.iterAtPos_curr;	//we dont want the '}' char, nor do we want to omit 1 char before it

	 //now we want to add those chars to list
	temp_itemAdd->add_char( advTemp_store.found_start, advTemp_store.found_end );
	 //prepare to find 2 numbers (min max)
	long tempNumber_min= 0;
	long tempNumber_max= 0;
	wxString tempStoreStr;
	 //find numbers
	retVal= setPattern_advanced_findNumberStr( pattern_storeS );
	if(retVal){
		tempStoreStr.assign( advTemp_store.found_start, advTemp_store.found_end );
		if(retVal==3) tempStoreStr.Replace( ",", "." );
		tempStoreStr.ToLong( &tempNumber_min );

		retVal= setPattern_advanced_findNext_separated( pattern_storeS );	//find ',;'
		retVal= setPattern_advanced_findNumberStr( pattern_storeS );

		if(retVal){
			tempStoreStr.assign( advTemp_store.found_start, advTemp_store.found_end );
			tempStoreStr.ToLong( &tempNumber_max );
			if( temp_itemAdd->setAmount( tempNumber_min, tempNumber_max ) ){	//check if its not an error
				pattern_storeS.reason= "min can NOT be greater then max";
				return error_setPatternAdv_Error;
			}
		}
		else
			temp_itemAdd->setAmount( tempNumber_min );	//only 1 number, that means (min= max= Number)
	} else {
		 //so we did not find a number to specify the amount, we assume =1
		temp_itemAdd->setAmount( 1 );
	}
return 0;
}
int fafikLib_regex::setPattern_advanced_number(pattern_store_struct& pattern_storeS)
{
	pattern_storeAdvTemp_struct& advTemp_store= *pattern_storeS.advTemp_store;
	item_match_number* temp_itemAdd= new item_match_number;
	_safeItem_based safeItem_based( temp_itemAdd );
	 //copy options mask.( option_Inverted )
	temp_itemAdd->optionsMask.copyBit(option_Inverted, advTemp_store.optionsOverride);

	int retVal= 0;
	int retValTop= 0;
	size_t numberLenTop= 1;
	BYTE NumberExists[2]= {0};
	BYTE NumberNegative= advTemp_store.forcedFindMinus;	//copy the given forced '-'
	wxString tempStoreStr_min;
	wxString tempStoreStr_max;
	temp_itemAdd->NumbersExists= 0;	//reset it to 0
	 //find min
	retVal= setPattern_advanced_findNumberStr( pattern_storeS, true );
	NumberExists[0]= retVal;
	retValTop= retVal;
	if(retVal){
		temp_itemAdd->NumbersExists|= 1;
		tempStoreStr_min.assign( advTemp_store.found_start, advTemp_store.found_end );
		numberLenTop= tempStoreStr_min.size();
	}

	setPattern_advanced_findNext_separated( pattern_storeS );	//find ',;'
	 //find max
	retVal= setPattern_advanced_findNumberStr( pattern_storeS, true );
	NumberExists[1]= retVal;
	if(retVal> retValTop) retValTop= retVal;
	if(retVal){
		temp_itemAdd->NumbersExists|= 2;
		tempStoreStr_max.assign( advTemp_store.found_start, advTemp_store.found_end );
		if(tempStoreStr_max.size()> numberLenTop) numberLenTop= tempStoreStr_max.size();
	}
	 //check if any exist
	if( !temp_itemAdd->NumbersExists ){
		pattern_storeS.reason= "Number range does not have any range {0,min,max}";
		return error_setPatternAdv_Error;
	}

	 //turn into numbers
	if( retValTop== 1 ){	//its LL
		temp_itemAdd->NumberType= 1;	//set type to LL
		numberLenTop+= 1;	//also prepare for - 0
		item_NumberRange_LL& numberTo_LL= temp_itemAdd->Number.num_LL;
		if( NumberExists[0] ){
			tempStoreStr_min.ToLongLong( &numberTo_LL.r_from );
			if( numberTo_LL.r_from< 0 ) NumberNegative= true;
		}
		if( NumberExists[1] ){
			tempStoreStr_max.ToLongLong( &numberTo_LL.r_to );
			if( numberTo_LL.r_to< 0 ) NumberNegative= true;
		}
	}
	else if( retValTop> 1 ){	//its Float
		temp_itemAdd->NumberType= 2;	//set type to Double
		numberLenTop+= 1+8;	//also prepare for - 0.12345678
		item_NumberRange_D& numberTo_D= temp_itemAdd->Number.num_D;
		if( NumberExists[0] ){
			if(NumberExists[0]> 2)
				tempStoreStr_min.Replace( ",", ".", false );
			tempStoreStr_min.ToDouble( &numberTo_D.r_from );
			if( numberTo_D.r_from< 0 ) NumberNegative= true;
		}
		if( NumberExists[1] ){
			if(NumberExists[1]> 2)
				tempStoreStr_max.Replace( ",", ".", false );
			tempStoreStr_max.ToDouble( &numberTo_D.r_to );
			if( numberTo_D.r_to< 0 ) NumberNegative= true;
		}
		 //get specific dot-coma, float separator
		retVal= setPattern_advanced_findNext_separated( pattern_storeS );	//find ',;'
		if(!retVal){
			retVal= setPattern_advanced_findFloatSpecyfic( pattern_storeS );	//find what user sees as float separator
			if(retVal>= error_setPatternAdv_Error){
				pattern_storeS.reason= "Number range ended abruptly, without closure";
				return retVal;	//that was an syntax error (out of scope)
			}
			if( !retVal ){
				temp_itemAdd->pointSeparatorChar= advTemp_store.floatSepChar;	//set the found user char
			} else {
printf("* note! Number Range trailing separator wont take effect\n");
			}
		}
	}

	temp_itemAdd->predictedLength.min= 1;
	temp_itemAdd->predictedLength.max= numberLenTop;
	temp_itemAdd->_update();	//we need to make sure min<max, if not then we invert it to be like so
	temp_itemAdd->optionsMask.setBit(option_CaseSense, NumberNegative );

	if(temp_itemAdd->pointSeparatorChar=='-' && NumberNegative ){
		pattern_storeS.reason= "Number range wants '-' as floating point, but it already means Negative Numbers";
		return error_setPatternAdv_Error;
	} else if ( temp_itemAdd->pointSeparatorChar>= '0' && temp_itemAdd->pointSeparatorChar<= '9' ){
		pattern_storeS.reason= "Number range floating point separator cannot be a Digit";
		return error_setPatternAdv_Error;
	}
	regex_item.AddItem(temp_itemAdd);
	safeItem_based.disarm();

return 0;
}

int fafikLib_regex::setPattern_advanced_anyOneOfText(pattern_store_struct& pattern_storeS)
{
	 //so we want to gather strings that are either in "" or without
	pattern_storeAdvTemp_struct& advTemp_store= *pattern_storeS.advTemp_store;
	item_match_anyOneOfText* temp_itemAdd= new item_match_anyOneOfText;
	 //we wont safe guard it, if it fails its syntax error and that invalidates everything
	regex_item.AddItem( temp_itemAdd );
	 //set options
	temp_itemAdd->optionsMask= pattern_storeS.optionsMask;
	 ///string that gives us
	wxString outputedStr;

	while(pattern_storeS.iterAtPos_curr!= pattern_storeS.iterAtPos_end){
		const wxChar32& charAtPos= *pattern_storeS.iterAtPos_curr;
		const wxChar32& charAtPos_next= *(pattern_storeS.iterAtPos_curr +1);

		if(charAtPos== advTemp_store.advancedSeparatorChar ){	//so we have ,, ;;
			temp_itemAdd->add_string("");	//add an empty string
			setPattern_advanced_findNext_separated(pattern_storeS);
			continue;
		}
		else if(charAtPos== '}' ){	//so we exit
			temp_itemAdd->_updateFinal();
			return 0;
		}
		else if(charAtPos== ' ' && (charAtPos_next== '\"' || charAtPos_next== '\'') ){	//a space and then enter quotes
			++pattern_storeS.iterAtPos_curr;    //we need to go to next char
			if( extractQuotedText(pattern_storeS, outputedStr) ){
				return error_setPatternAdv_Error;
			}
			temp_itemAdd->add_string(outputedStr);
			setPattern_advanced_findNext_separated(pattern_storeS);
			continue;
		}
		else if( charAtPos== '\"' || charAtPos== '\'' ){	//enter quotes
			if( extractQuotedText(pattern_storeS, outputedStr) ){
				return error_setPatternAdv_Error;
			}
			temp_itemAdd->add_string(outputedStr);
			setPattern_advanced_findNext_separated(pattern_storeS);
			continue;
		}
		else {
			wxString::const_iterator iterText_start= pattern_storeS.iterAtPos_curr;
			wxString::const_iterator iterText_end= pattern_storeS.iterAtPos_curr;
			 //we add the text found to next separator
			if( setPattern_advanced_findNext_separated(pattern_storeS) )
				iterText_end= pattern_storeS.iterAtPos_curr;
			else
				iterText_end= pattern_storeS.iterAtPos_curr -1;
			outputedStr.clear();
			if( iterText_start < iterText_end )
				outputedStr.assign(iterText_start, iterText_end);
			temp_itemAdd->add_string( outputedStr );
			continue;
		}
		++pattern_storeS.iterAtPos_curr;	//if we somehow miss the if, then advance +1
	}

return error_setPatternAdv_Error;	//did not exit and we run out of chars
}

int fafikLib_regex::extractQuotedText(pattern_store_struct& pattern_storeS, wxString &outputStrFull)
{
	outputStrFull.clear();
	const wxChar32 quoteChar= *pattern_storeS.iterAtPos_curr;
	size_t onPos_q= pattern_storeS.iterAtPos_curr- pattern_storeS.iterAtPos_itemStart;
	++pattern_storeS.iterAtPos_curr;
	size_t backSlashCount= 0;
	bool retErr= 0;
	wxString::const_iterator iterStart= pattern_storeS.iterAtPos_curr;
	wxString outputStr_temp;

	while( pattern_storeS.iterAtPos_curr!= pattern_storeS.iterAtPos_end ){
		const wxChar32& charAtPos= *pattern_storeS.iterAtPos_curr;
		if( backSlashCount && charAtPos!= '\\' ){
			outputStrFull.append( iterStart, pattern_storeS.iterAtPos_curr -backSlashCount );
			retErr= processEscape(pattern_storeS.iterAtPos_curr, backSlashCount, outputStr_temp);
			outputStrFull.append(outputStr_temp);
			iterStart= pattern_storeS.iterAtPos_curr+1;
			backSlashCount= 0;	//clear
			if( !retErr ) {
				++pattern_storeS.iterAtPos_curr;
				continue;
			}
		}

		if( charAtPos== '\\' ){
			++backSlashCount;
		} else if( charAtPos== quoteChar ){	//we exit
			if( iterStart< pattern_storeS.iterAtPos_curr -backSlashCount ){
				outputStrFull.append( iterStart, pattern_storeS.iterAtPos_curr -backSlashCount );
			}
			++pattern_storeS.iterAtPos_curr;
			return error_setPatternAdv_ok;
		}

		++pattern_storeS.iterAtPos_curr;
	}

pattern_storeS.reason= "the Quote character from position ";
pattern_storeS.reason<< onPos_q << " is not closed.";
return error_setPatternAdv_Error;	//we did not exit
}


size_t fafikLib_regex::item_posIO::get_maxPosOfFind(bool r_find) const
{
	size_t retVal= (unsigned short) pos_start_begin;
	if(r_find)
		retVal-= (unsigned short) pos_start_maxOff;
	else
		retVal+= (unsigned short) pos_start_maxOff;
	if(retVal> (unsigned short)-1 ) retVal= -1;
	return retVal;
}
size_t fafikLib_regex::item_posIO::get_maxPosOfFind_full(bool r_find) const
{
	size_t retVal= (unsigned short) pos_start_begin;
	if(r_find)
		retVal-= (unsigned short) pos_off_maxPos;
	else
		retVal+= (unsigned short) pos_off_maxPos;
	if(retVal> (unsigned short)-1 ) retVal= -1;
	return retVal;
}
size_t fafikLib_regex::item_posIO::get_maxPosOfEnd(const unsigned short &DistanceToEnd, bool r_find) const
{
	size_t retVal= PathElement_str_p->size()-1;
	retVal-= DistanceToEnd;
	 //this check takes care of negative values and turns it into 0 instead (only for find), as in r_find this value should be > foundPosTo
	if( !r_find && retVal> PathElement_str_p->size() ) return 0;
	return retVal;
}

void fafikLib_regex::item_base_pathParts::calc_len_predicted()
{
	//PathElement_len_limited;	does nothing for now 2021-04-23

		//special case if'*' then just that && end
	if( pos_start== pos_end-1 && (*pos_start)->type_processes== item_processes_Star ){
		_matchesAny= true;
		return;
	}
		//special case if'/''/' then treat it as star '*' && end	(just so the user wont crash the program providing such a regex)
	if( pos_start== pos_end ){
		_matchesAny= true;
		return;
	}
		///u short (-1)
	const unsigned short m_short(-1);
		///size_t (m_short)
	const size_t m_short_size_t( m_short );

	unsigned short t_LenToFollow_min= 0;
		///difference between min, max
	size_t t_DistanceToFinish_max= 0;

	for(iterator iterRev= pos_end-1; iterRev!= pos_start-1; --iterRev ){	//r iterate from End->Begin
			///a better name for **iterRev
		item_base& iterRevTrans= **iterRev;
			//store the minimum amount of chars that need to follow
		iterRevTrans.Distanced_FromToEnd.min= t_LenToFollow_min;
			//store the min required len for previous entry
			//note that if that number manages to be >256 then no file name in Windows will be able to pass
		t_LenToFollow_min+= iterRevTrans.predictedLength.min;
			//store that max distance
		iterRevTrans.Distanced_FromToEnd.max= t_DistanceToFinish_max;
			//calc the max distance
		if( t_DistanceToFinish_max!= m_short_size_t ) {
			t_DistanceToFinish_max+= iterRevTrans.predictedLength.max;
				//overflow protection
			if( t_DistanceToFinish_max>= m_short_size_t ) t_DistanceToFinish_max= m_short_size_t;
		}
	}
		//great now store the overall minimum len of the filename
	PathElement_len_predicted.min= t_LenToFollow_min;
	PathElement_len_predicted.max= t_DistanceToFinish_max;

	t_LenToFollow_min= 0;
	t_DistanceToFinish_max= 0;
	for(iterator iterRev= pos_start; iterRev!= pos_end; ++iterRev ){	//iterate from Begin->End
			///a better name for **iterRev
		item_base& iterRevTrans= **iterRev;
			//store the minimum amount of chars that need to follow
		iterRevTrans.Distanced_FromToBegin.min= t_LenToFollow_min;
			//store the min required len for previous entry
			//note that if that number manages to be >256 then no file name in Windows will be able to pass
		t_LenToFollow_min+= iterRevTrans.predictedLength.min;
			//store that max distance
		iterRevTrans.Distanced_FromToBegin.max= t_DistanceToFinish_max;
			//calc the max distance
		if( t_DistanceToFinish_max!= m_short_size_t ) {
			t_DistanceToFinish_max+= iterRevTrans.predictedLength.max;
				//overflow protection
			if( t_DistanceToFinish_max>= m_short_size_t ) t_DistanceToFinish_max= m_short_size_t;
		}
	}
}

_FileType_MDFC_Checksum fafikLib_regex::full_regex_item::getHashType() const
{
	if(!hasHash()) return _FileType_MDFC_Checksum_none;
	for(auto iter: items_p){
		if(iter->HashType!= _FileType_MDFC_Checksum_none)
			return iter->HashType;
	}
	return _FileType_MDFC_Checksum_none;
}
int fafikLib_regex::full_regex_item::separatePathElement( full_regex_item_ParserTempStore& tempStore )
{
	 //set array size with reserve
	items_pathParts.reserve( tempStore.tempPathElements.size() );
	 //store PathElements amount
	amount_dirs= tempStore.tempPathElements.size();
	const wxVector<item_base*>::iterator startIIndex= items_p.begin();

	for( auto ats= tempStore.tempPathElements.begin(); ats!=tempStore.tempPathElements.end(); ++ats ){
			//translate index into iterator
		items_pathParts.push_back( item_base_pathParts(
			startIIndex+ ats->min,
			startIIndex+ ats->max
		) );
			//calculate some sizes
		items_pathParts.back().calc_len_predicted();
	}
return 0;
}


int fafikLib_regex::item_match_string::tryTest(item_posIO& item_posIO_store, item_posFoundO& item_returns) const
{
	const bool do_r_find= item_posIO_store.b_reverseCheck.getBit(option_rCheck_fromEnd);
	 ///stores the sub-function find result
	size_t ret_procVal;
	items_tempStore itemTempIO;
	itemTempIO.t_pos_start_maxOff= item_posIO_store.pos_start_maxOff;

	 ///for r_find
	size_t maxPosOfEnd= item_posIO_store.get_maxPosOfFind(do_r_find);
	 ///for r_find
	size_t maxMaxPosOfEnd= item_posIO_store.get_maxPosOfFind_full(do_r_find);
	if(maxPosOfEnd> item_posIO_store.pos_start_begin) maxPosOfEnd= 0;
	if(maxMaxPosOfEnd> item_posIO_store.PathElement_str_p->size()) maxMaxPosOfEnd= 0;

	item_posIO_store.pos_start_maxOff= 0;
	size_t Distanced_ToFin= Distanced_FromToEnd.max;

	if( do_r_find )
		Distanced_ToFin= Distanced_FromToBegin.max;

	while(true) {
		if( do_r_find ){	//find backward
			ret_procVal= tryTest_backward(item_posIO_store, item_returns, itemTempIO);
		}
		else	//find forward
			ret_procVal= tryTest_forward(item_posIO_store, item_returns, itemTempIO);

		if( ret_procVal== wxString::npos )
			return hitTest_failedMatch;
	  //calc distance to End
		if( do_r_find )
			itemTempIO.DistanceToFin_now= ret_procVal;	//backward
		else	//forward
			itemTempIO.DistanceToFin_now= item_posIO_store.PathElement_str_p->size()- (ret_procVal+ this->match_str.size());
	  //ok found, but where?
		if( itemTempIO.DistanceToFin_now<= Distanced_ToFin ) {
			item_posIO_store.pos_start_begin= itemTempIO.next_pos_start_begin;
			item_returns.found_pos_from= ret_procVal;
			item_returns.found_pos_to= ret_procVal+ this->match_str.size()-1;

			if( !do_r_find && itemTempIO.find_res > item_posIO_store.pos_off_maxPos ){
			  //found out of range, --error
				return hitTest_failedMatch;
			}
			else if( do_r_find && item_returns.found_pos_to < maxMaxPosOfEnd ){
			  //r_found out of range, --error
				return hitTest_failedMatch;
			}
			else if( !do_r_find && itemTempIO.find_res > itemTempIO.t_pos_start_maxOff ) {
				  //save that max pos of find
				item_posIO_store.reverseFind= ret_procVal;
				return hitTest_failedPartially_checkBackwards;
			}
			else if( do_r_find && item_returns.found_pos_to < maxPosOfEnd ) {
				  //save that max pos of find
				item_posIO_store.reverseFind= ret_procVal;
				return hitTest_failedPartially_checkBackwards;
			}
			  //found, at good pos, --ok
			return hitTest_ok;
		}
		else {
			 //misaligned
			continue;
		}
	}
return hitTest_failedMatch; //just so compiler will shut up
}
size_t fafikLib_regex::item_match_string::tryTest_forward(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& IO) const
{
	if(!IO.initiated_str) {
		IO.initiated_str= true;
		 ///this cuts down the char amount we will have to go through(copy)
		size_t CutToPos= IO.t_pos_start_maxOff;
		const size_t CutMaxPos= item_posIO_store.PathElement_str_p->size()-1 - Distanced_FromToEnd.min;
		if(item_posIO_store.pos_off_maxPos> CutToPos) CutToPos= item_posIO_store.pos_off_maxPos;

		if(CutToPos> CutMaxPos ) {
			CutToPos= CutMaxPos;
		} else {
			CutToPos= item_posIO_store.pos_start_begin +this->match_str.size()-1 +IO.t_pos_start_maxOff;
			if(CutToPos> CutMaxPos )
				CutToPos= CutMaxPos;
		}
		 //we want to bind that string in range (pos_start_begin  to  max pos that still allows further items to be present)
		IO.boundString= item_posIO_store.PathElement_str_p->SubString(item_posIO_store.pos_start_begin, CutToPos );
		if( optionsMask.getBit(option_CaseSense) )	//is case insensitive
			IO.boundString.LowerCase();
	}

	IO.find_res= IO.boundString.find( this->match_str, IO.findFrom );

	if( IO.find_res!= wxString::npos ) {
		IO.findFrom= IO.find_res +1;
		IO.find_res_plusBegin= item_posIO_store.pos_start_begin+ IO.find_res;
	  //save start pos for next find
		IO.next_pos_start_begin= IO.find_res_plusBegin+ this->match_str.size();
	  //return for now
		return IO.find_res_plusBegin;
	}
return -1;
}
size_t fafikLib_regex::item_match_string::tryTest_backward(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& IO) const
{
	if(!IO.initiated_str) {
		IO.initiated_str= true;
		const size_t CutToPos_Max= item_posIO_store.pos_start_begin- this->match_str.size()+1;
		 ///this cuts down the char amount we will have to go through
		size_t CutToPos= IO.t_pos_start_maxOff;
		if(item_posIO_store.pos_off_maxPos> CutToPos) CutToPos= item_posIO_store.pos_off_maxPos;

		CutToPos= CutToPos_Max- ((unsigned short)CutToPos);
		if( CutToPos> CutToPos_Max || CutToPos< Distanced_FromToBegin.min)
			CutToPos= Distanced_FromToBegin.min;
		 //we want to bind that string in range (pos_start_begin  to  max pos that still allows further items to be present)
		IO.boundString= item_posIO_store.PathElement_str_p->SubString(CutToPos, item_posIO_store.pos_start_begin );
		IO.boundStr_startPos= CutToPos;
		if( optionsMask.getBit(option_CaseSense) )	//is case insensitive
			IO.boundString.LowerCase();
		IO.findFrom= -1;
	}

	IO.find_res= IO.boundString.rfind( this->match_str, IO.findFrom );
	if( IO.find_res!= wxString::npos ) {
		IO.findFrom= IO.find_res-1;
		IO.find_res_plusBegin= IO.boundStr_startPos+ IO.find_res;
	  //save start pos for next find
		IO.next_pos_start_begin= IO.find_res_plusBegin-1;
	  //return for now
		return IO.find_res_plusBegin;
	}
return -1;
}

void fafikLib_regex::item_match_string::set(const wxString& str, const bool8 options )
{
  //set declarations
	type_processes= item_processes_String;
	predictedLength.min= predictedLength.max= str.size();
  //set user data
	match_str= str;
	optionsMask= options;
	if( optionsMask.getBit(option_CaseSense) )	//case insensitive
		match_str.LowerCase();
}
void fafikLib_regex::item_match_string::update()
{
  //set declarations
	type_processes= item_processes_String;
	predictedLength.min= predictedLength.max= match_str.size();
  //set user data
	if( optionsMask.getBit(option_CaseSense) )	//case insensitive
		match_str.LowerCase();
}

int fafikLib_regex::item_match_star::tryTest(item_posIO& item_posIO_store, item_posFoundO& item_returns) const
{
	item_posIO_store.pos_start_maxOff= -1;
	item_posIO_store.pos_off_maxPos= -1;
	item_returns.found_pos_from= item_posIO_store.pos_start_begin;
	item_returns.found_pos_to= item_posIO_store.pos_start_begin;
	item_returns.isWhatever= 3;
return hitTest_ok_cantQuestion;
}
void fafikLib_regex::item_match_star::set()
{
  //set declarations
	type_processes= item_processes_Star;
	predictedLength.min= 0;
	predictedLength.max= -1;
  //set user data
}

int fafikLib_regex::item_match_number::tryTest(item_posIO& item_posIO_store, item_posFoundO& item_returns) const
{
	const bool b_r_find= item_posIO_store.b_reverseCheck.getBit(option_rCheck_fromEnd);
	const bool b_invert= optionsMask.getBit(option_Inverted);
	size_t maxPosStart= item_posIO_store.get_maxPosOfFind( b_r_find );
	size_t maxMaxPosStart= item_posIO_store.get_maxPosOfFind_full( b_r_find );
	size_t maxPosFin= item_posIO_store.get_maxPosOfEnd( Distanced_FromToEnd.min, b_r_find );
	size_t DPosFromFin= item_posIO_store.get_maxPosOfEnd( Distanced_FromToEnd.max, b_r_find );
	if( b_r_find ){
		maxPosFin= Distanced_FromToBegin.min;
		DPosFromFin= Distanced_FromToBegin.max;
		if(maxPosStart> item_posIO_store.pos_start_begin) maxPosStart= 0;
		if(maxMaxPosStart> item_posIO_store.PathElement_str_p->size()) maxMaxPosStart= 0;
	}

	int retVal= 0;
	int ValToRet= hitTest_ok;

	items_tempStore itemTempIO;
	itemTempIO.t_pos_start_maxOff= item_posIO_store.pos_start_maxOff;	//copy this
	while(true){
		ValToRet= hitTest_ok;	//reset it just in case
		if( b_r_find ){	//backward
			retVal= tryTest_backward(item_posIO_store, item_returns, itemTempIO);
			//retVal= tryTest_backward(item_posIO_store, item_returns, itemTempIO);
		} else {	//forward
			retVal= tryTest_forward(item_posIO_store, item_returns, itemTempIO);
		}

		if( retVal== hitTest_failedMatch )	//not found, so exit
			return hitTest_failedMatch;
		if( b_r_find ){	//r_find
			if( itemTempIO.foundPos_to < maxMaxPosStart ){
				return hitTest_failedMatch;	//found out of scope
			} else if( itemTempIO.foundPos_to < maxPosStart ){
				ValToRet= hitTest_failedPartially_checkBackwards;	//found a bit further then expected
			}

			if( itemTempIO.foundPos_from < maxPosFin ){
				return hitTest_failedMatch;	//found out of scope
			} else if( itemTempIO.foundPos_from > DPosFromFin ){
				return hitTest_failedMatch;	//found out of scope
			}
		} else {		//find
			if( itemTempIO.foundPos_from > maxMaxPosStart ){
				return hitTest_failedMatch;	//found out of scope
			} else if( itemTempIO.foundPos_from > maxPosStart ){
				ValToRet= hitTest_failedPartially_checkBackwards;	//found a bit further then expected
			}

			if( itemTempIO.foundPos_to > maxPosFin ){
				return hitTest_failedMatch;	//found out of scope
			} else if( itemTempIO.foundPos_to < DPosFromFin ){
				return hitTest_failedMatch;	//found out of scope
			}
		}


		retVal= tryTest_numInRange(itemTempIO);	//check if our extracted number matches the <range>
		if( !b_invert && retVal== hitTest_failedMatch ){	//normal output
			continue;	//retry
		}
		else if( b_invert && retVal== hitTest_ok ){	//invert the output
			continue;	//retry
		}
		break;	//found good
	}
	item_returns.found_pos_from= itemTempIO.foundPos_from;
	item_returns.found_pos_to= itemTempIO.foundPos_to;

	item_posIO_store.pos_start_maxOff= 0;
	item_posIO_store.pos_start_begin= itemTempIO.next_pos_start_begin;
	return ValToRet;
}
int fafikLib_regex::item_match_number::tryTest_forward(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& it_IO ) const
{
	int retVal= 0;
	if( !it_IO.initiated_str ){	//initiate string
		it_IO.initiated_str= true;
		it_IO.findFrom= item_posIO_store.pos_start_begin;

		it_IO.char_iter= item_posIO_store.PathElement_str_p->begin()+ it_IO.findFrom;
		it_IO.char_iterEnd= item_posIO_store.PathElement_str_p->end();
		it_IO.iter_change= +1;
	}

	retVal= tryTest_findNumberStr( it_IO );	//find the number-string
	if( retVal== hitTest_ok ){	//found something
		wxString::const_iterator char_iter_O_foundFrom= it_IO.char_iter_O_foundFrom;
		wxString::const_iterator char_iter_O_foundTo= it_IO.char_iter_O_foundTo;
		if( optionsMask.getBit(option_CaseSense) && char_iter_O_foundFrom> item_posIO_store.PathElement_str_p->begin()+ it_IO.findFrom ){
			if( *(char_iter_O_foundFrom-1)== wxChar32('-') ){
				--char_iter_O_foundFrom;	//include minus sign '-'
			}
		}

		if( this->NumberType== 2 && it_IO.char_iter_O_foundTo != it_IO.char_iterEnd && *it_IO.char_iter_O_foundTo== pointSeparatorChar ){
			++it_IO.char_iter;	//advance the float point separator
			 //find the float part of number-string (has to be exactly there)
			tryTest_findNumberStr( it_IO, true );	//we dont need to capture the result as it will either succeed or do nothing

			it_IO.char_iter= char_iter_O_foundTo;	//revert the finder to a position after the last digit, (one before trying float find)
		}
		 //copy the number part from string so we can cast it into a Number.
		it_IO.boundString.assign(char_iter_O_foundFrom, it_IO.char_iter_O_foundTo);	//extract number-string using provided iters
		 //convert iter into pos based
		it_IO.foundPos_from= char_iter_O_foundFrom- item_posIO_store.PathElement_str_p->begin();
		it_IO.foundPos_to= it_IO.char_iter_O_foundTo-1 - item_posIO_store.PathElement_str_p->begin();
		it_IO.next_pos_start_begin= it_IO.foundPos_to +1;
		return hitTest_ok;
	}
	return hitTest_failedMatch;
}
int fafikLib_regex::item_match_number::tryTest_backward(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& it_IO ) const
{
	int retVal= 0;
	if( !it_IO.initiated_str ){	//initiate string
		it_IO.initiated_str= true;
		it_IO.findFrom= item_posIO_store.pos_start_begin;
		it_IO.char_iter= item_posIO_store.PathElement_str_p->begin()+ it_IO.findFrom;
		it_IO.char_iterEnd= (item_posIO_store.PathElement_str_p->begin() -1) +Distanced_FromToBegin.min;
		it_IO.iter_change= -1;
	}
	retVal= tryTest_findNumberStr( it_IO );	//find the number-string
	if( retVal== hitTest_ok ){	//found something
		wxString::const_iterator char_iter_O_foundFrom= it_IO.char_iter_O_foundFrom;
		wxString::const_iterator char_iter_O_foundTo= it_IO.char_iter_O_foundTo;
		 //found a number, here it would be a float part, so find the number
		if( this->NumberType== 2 && it_IO.char_iter_O_foundTo != it_IO.char_iterEnd && *it_IO.char_iter_O_foundTo== pointSeparatorChar ){
			--it_IO.char_iter;	//advance the float point separator
			 //find the float part of number-string (has to be exactly there)
			tryTest_findNumberStr( it_IO, true );	//we dont need to capture the result as it will either succeed or do nothing
			it_IO.char_iter= char_iter_O_foundTo;	//revert the finder to a position after the last digit, (one before trying float find)
		}
		if( optionsMask.getBit(option_CaseSense) && it_IO.char_iter_O_foundTo!= it_IO.char_iterEnd ){
			if( *(it_IO.char_iter_O_foundTo)== wxChar32('-') ){
				--it_IO.char_iter_O_foundTo;	//include minus sign '-'
			}
		}
		++it_IO.char_iter_O_foundTo;
		 //copy the number part from string so we can cast it into a Number.
		it_IO.boundString.assign(it_IO.char_iter_O_foundTo, char_iter_O_foundFrom+1);	//extract number-string using provided iters
		 //convert iter into pos based
		it_IO.foundPos_from= it_IO.char_iter_O_foundTo - item_posIO_store.PathElement_str_p->begin();
		it_IO.foundPos_to= char_iter_O_foundFrom- item_posIO_store.PathElement_str_p->begin();
		it_IO.next_pos_start_begin= it_IO.foundPos_from -1;
		return hitTest_ok;
	}
return hitTest_failedMatch;
}
int fafikLib_regex::item_match_number::tryTest_findNumberStr( items_tempStore& it_IO, const bool cantOffset ) const
{
	bool b_foundNumStart= false;
	for( ; it_IO.char_iter!= it_IO.char_iterEnd; it_IO.char_iter+= it_IO.iter_change ){
		const wxChar32& char_AtIter= *it_IO.char_iter;

		if( char_AtIter>= wxChar32('0') && char_AtIter<= wxChar32('9') ){	//char is a digit 0-9
			if( !b_foundNumStart ) it_IO.char_iter_O_foundFrom= it_IO.char_iter;	//save the first location
			b_foundNumStart= true;	//mark found a start
		}
		else if( b_foundNumStart ) {	//found a string of digits and ended
			it_IO.char_iter_O_foundTo= it_IO.char_iter;
			return hitTest_ok;
		}
		else if( cantOffset && !b_foundNumStart ) {
			return hitTest_failedMatch;
		}
	}
	if( b_foundNumStart ) {	//found a string of digits and EOL
		it_IO.char_iter_O_foundTo= it_IO.char_iter;
		return hitTest_ok;
	}

return hitTest_failedMatch;
}
int fafikLib_regex::item_match_number::tryTest_numInRange( items_tempStore& it_IO ) const
{
	if( this->NumberType== 2 ) {	//double
		double temp_out_D;
		if(pointSeparatorChar!= '.')	//do we need to change user char into dot '.'
			it_IO.boundString.Replace( wxUniChar(pointSeparatorChar), ".", false );
		 ///cast raw storage to c_D
		const item_NumberRange_D& casted_D= Number.num_D;
		it_IO.boundString.ToDouble( &temp_out_D );
		if(this->NumbersExists== 3 ){
			if( temp_out_D>= casted_D.r_from && temp_out_D<= casted_D.r_to )
				return hitTest_ok;
		} else if(this->NumbersExists== 2 ){
			if( temp_out_D<= casted_D.r_to )
				return hitTest_ok;
		} else if(this->NumbersExists== 1 ){
			if( temp_out_D>= casted_D.r_from )
				return hitTest_ok;
		}
		return hitTest_failedMatch;
	} else {	//long long
		long long temp_out_LL;
		 ///cast raw storage to c_LL
		const item_NumberRange_LL& casted_LL= Number.num_LL;
		it_IO.boundString.ToLongLong( &temp_out_LL );
		if(this->NumbersExists== 3 ){
			if( temp_out_LL>= casted_LL.r_from && temp_out_LL<= casted_LL.r_to )
				return hitTest_ok;
		} else if(this->NumbersExists== 2 ){
			if( temp_out_LL<= casted_LL.r_to )
				return hitTest_ok;
		} else if(this->NumbersExists== 1 ){
			if( temp_out_LL>= casted_LL.r_from )
				return hitTest_ok;
		}
		return hitTest_failedMatch;
	}
return hitTest_failedMatch; //just so compiler shuts up
}
bool fafikLib_regex::item_match_number::_update()
{
	if(NumbersExists & 3){	//if both<min,max> numbers exist
		if( NumberType== 1 ){
			item_NumberRange_LL& castedNumber= Number.num_LL;
			if(castedNumber.r_from> castedNumber.r_to){
				std::swap( castedNumber.r_from, castedNumber.r_to );	//swap
				optionsMask.xorBit(option_Inverted);
			}
		}
		else if( NumberType== 2 ){
			item_NumberRange_D& castedNumber= Number.num_D;
			if(castedNumber.r_from> castedNumber.r_to){
				std::swap( castedNumber.r_from, castedNumber.r_to );	//swap
				optionsMask.xorBit(option_Inverted);
			}
		}
	}
	if( optionsMask.getBit(option_Inverted) ){
		predictedLength.max+= 8;	//add some length
		return true;
	}
return false;
}

int fafikLib_regex::item_match_anyChar::tryTest(item_posIO& item_posIO_store, item_posFoundO& item_returns) const
{
	item_returns.isWhatever= 2;
	if( !item_posIO_store.b_reverseCheck.getBit(option_rCheck_fromEnd) ){	//find
		return tryTest_forward(item_posIO_store, item_returns);
	} else {	//r_find
		return tryTest_backward(item_posIO_store, item_returns);
	}
return hitTest_failedMatch;
}
int fafikLib_regex::item_match_anyChar::tryTest_forward(item_posIO& item_posIO_store, item_posFoundO& item_returns) const
{
	if( item_posIO_store.PathElement_str_p->size() < item_posIO_store.pos_start_begin+ predictedLength.min )	//check if min amount of chars are still present
		return hitTest_failedMatch;
	if( item_posIO_store.pos_start_begin+ predictedLength.max < item_posIO_store.get_maxPosOfEnd(Distanced_FromToEnd.max) )
		return hitTest_failedMatch;	//we cant get to the end
	 //found from
	item_returns.found_pos_from= item_posIO_store.pos_start_begin;
	 //next start
	item_posIO_store.pos_start_begin+= predictedLength.min;
	 //found to
	item_returns.found_pos_to= item_posIO_store.pos_start_begin -1;

	item_posIO_store.pos_start_maxOff= maxOffset;
	item_posIO_store.addSet_off_maxPos( item_posIO_store.pos_start_maxOff );
return hitTest_ok;
}
int fafikLib_regex::item_match_anyChar::tryTest_backward(item_posIO& item_posIO_store, item_posFoundO& item_returns) const
{
	int posEnd= item_posIO_store.pos_start_begin- predictedLength.min +1;
	 if(posEnd < 0) return hitTest_failedMatch; //left to check string is too short
	int posEndMax= item_posIO_store.pos_start_begin- predictedLength.max +1;
	 if(posEndMax < 0) posEndMax= 0;

	if( (size_t)posEnd < Distanced_FromToBegin.min )	//check if min amount of chars are still present
		return hitTest_failedMatch;
	if( (size_t)posEndMax > Distanced_FromToBegin.max )
		return hitTest_failedMatch;	//we cant reach the end (we are short)
	 //found from
	item_returns.found_pos_to= item_posIO_store.pos_start_begin;
	 //next start
	item_posIO_store.pos_start_begin-= predictedLength.min;
	 //found to
	item_returns.found_pos_from= item_posIO_store.pos_start_begin +1;

	item_posIO_store.pos_start_maxOff= maxOffset;
	item_posIO_store.addSet_off_maxPos( item_posIO_store.pos_start_maxOff );
return hitTest_ok;
}
void fafikLib_regex::item_match_anyChar::set( unsigned short am_min, unsigned short am_max )
{	//also created by inLine object `?` with 1,1 (in regexr that is 0-1 of previous object)
  //set declarations
	type_processes= item_processes_AnyChar;
  //& set user data
	if(am_min> am_max) std::swap(am_min, am_max);	//just in case
	predictedLength.min= am_min;
	predictedLength.max= am_max;
	maxOffset= am_max- am_min;
}

int fafikLib_regex::item_match_anyOfChars::tryTest(item_posIO& item_posIO_store, item_posFoundO& item_returns) const
{
	items_tempStore it_IO;
	unsigned short maxPosOfFind= item_posIO_store.get_maxPosOfFind();
	size_t ret_procVal= -1;
	item_returns.start_pos= item_posIO_store.pos_start_begin;

	if( item_posIO_store.b_reverseCheck.getBit(option_rCheck_fromEnd) ){	//r find
		ret_procVal= tryTest_backward(item_posIO_store, item_returns, it_IO);
	} else {	//find
		ret_procVal= tryTest_forward(item_posIO_store, item_returns, it_IO);
	}

	if( ret_procVal== wxString::npos)
		return hitTest_failedMatch;	//not found or error, return error
	if( item_posIO_store.get_maxPosOfEnd(Distanced_FromToEnd.max) > item_returns.found_pos_to )	//failed
		return hitTest_failedMatch;
	if( item_returns.found_pos_from> maxPosOfFind ){
		  //save that max pos of find
		item_posIO_store.reverseFind= item_returns.found_pos_from;
		return hitTest_failedPartially_checkBackwards;	//we went further then expected
	}
return hitTest_ok;
}
size_t fafikLib_regex::item_match_anyOfChars::tryTest_forward(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& it_IO) const
{ //has to be at least min, and matches up to max (does not have to be max), this item is quite specific and will not let next entry match from any other point
	wxString::const_iterator atPosIter= item_posIO_store.PathElement_str_p->begin()+ item_posIO_store.pos_start_begin;
	const unsigned short maxPosOfFind_full= item_posIO_store.get_maxPosOfFind_full();
	size_t atPosIndex= item_posIO_store.pos_start_begin;
	unsigned short LeftCharsToEnd= item_posIO_store.PathElement_str_p->size()-1- atPosIndex;
	const bool b_invert= optionsMask.getBit(option_Inverted);
	bool gotResult= 0;
	BYTE reachedMin= false;
	if(predictedLength.min== 0 && b_invert== 0) reachedMin= true;	//we need to consider that someone might want to match something or nothing

	for( ; atPosIter!= item_posIO_store.PathElement_str_p->end(); ++atPosIter ){
		if( reachedMin== true && it_IO.matchingAmount>= predictedLength.max)	//check if we are at max
			break;	//just break the loop
		if(LeftCharsToEnd< Distanced_FromToEnd.min)
			break;	//we have to end

		gotResult= tryTestChar( *atPosIter );	//try this char in ranges

		if( (!b_invert && gotResult) || (b_invert && !gotResult) ){	//check or invert check
			++it_IO.matchingAmount;
			if( !reachedMin && it_IO.matchingAmount>= predictedLength.min ){
				reachedMin= true;	//mark it as passed
			}
		}
		else {
			if(!reachedMin){	//we still needed more, redo
				it_IO.matchingAmount= 0;
				if( item_posIO_store.PathElement_str_p->size()-1- atPosIndex < predictedLength.min )
					return -1;	//we cant fit another redo
				if( atPosIndex>= maxPosOfFind_full )
					return -1;	//we cant go that far
			} else {	//we are done, we have min
				break;	//just break the loop
			}
		}
		++atPosIndex;
		--LeftCharsToEnd;
	}
	if(!reachedMin)
		return -1;

	atPosIndex= atPosIter- item_posIO_store.PathElement_str_p->begin();
	item_returns.found_pos_to= atPosIndex -1;
	item_returns.found_pos_from= atPosIndex- it_IO.matchingAmount;
	item_posIO_store.pos_start_begin= item_returns.found_pos_from + predictedLength.min;
	item_posIO_store.pos_start_maxOff= it_IO.matchingAmount- predictedLength.min;
	item_posIO_store.addSet_off_maxPos(item_posIO_store.pos_start_maxOff);
return atPosIndex;
}
size_t fafikLib_regex::item_match_anyOfChars::tryTest_backward(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& it_IO) const
{ //has to be at least min, and matches up to max (does not have to be max), this item is quite specific and will not let next entry match from any other point
	wxString::const_iterator atPosIter= item_posIO_store.PathElement_str_p->begin()+ item_posIO_store.pos_start_begin;
	size_t maxPosOfFind_full= item_posIO_store.pos_start_begin- ((unsigned short)item_posIO_store.pos_off_maxPos);
	if(maxPosOfFind_full> item_posIO_store.pos_start_begin) maxPosOfFind_full= 0;

	size_t atPosIndex= item_posIO_store.pos_start_begin;
	const bool b_invert= optionsMask.getBit(option_Inverted);
	bool gotResult= 0;
	BYTE reachedMin= false;
	if(predictedLength.min== 0 && b_invert== 0) reachedMin= true;	//we need to consider that someone might want to match something or nothing

	for( ; atPosIter!= item_posIO_store.PathElement_str_p->begin()-1; --atPosIter ){
		if( reachedMin== true && it_IO.matchingAmount>= predictedLength.max)	//check if we are at max
			break;	//just break the loop
		if(atPosIndex< Distanced_FromToBegin.min)
			break;	//we have to end

		gotResult= tryTestChar( *atPosIter );	//try this char in ranges

		if( (!b_invert && gotResult) || (b_invert && !gotResult) ){	//check or invert check
			++it_IO.matchingAmount;
			if( !reachedMin && it_IO.matchingAmount>= predictedLength.min ){
				reachedMin= true;	//mark it as passed
			}
		} else {
			if(!reachedMin){	//we still needed more, redo
				it_IO.matchingAmount= 0;
				if( atPosIndex < predictedLength.min )
					return -1;	//we cant fit another redo
				if( atPosIndex<= maxPosOfFind_full )
					return -1;	//we cant go that far
			} else {	//we are done, we have min
				break;	//just break the loop
			}
		}
		--atPosIndex;
	}
	if(!reachedMin)
		return -1;
	atPosIndex= atPosIter- item_posIO_store.PathElement_str_p->begin();
	++atPosIndex;

	item_returns.found_pos_from= atPosIndex;
	item_returns.found_pos_to= atPosIndex+ it_IO.matchingAmount-1;
	item_posIO_store.pos_start_begin= item_returns.found_pos_to- predictedLength.min;

	 //allow offset max-min
	item_posIO_store.pos_start_maxOff= it_IO.matchingAmount- predictedLength.min;
	item_posIO_store.addSet_off_maxPos( item_posIO_store.pos_start_maxOff );
return atPosIndex;
}
bool fafikLib_regex::item_match_anyOfChars::tryTestChar(const wxUniChar &atPosChar) const
{
	for( auto tryRange= charsRanges.begin(); tryRange!= charsRanges.end(); ++tryRange ){
		if( atPosChar>= tryRange->Char_from && atPosChar<= tryRange->Char_to ){	//check if char is in <range>
			return true;
		}
	}

	size_t foundOnPos= charsToMatch.find( atPosChar );	//check if char is one of single chars
	if( foundOnPos!= wxString::npos ) return true;

return false;
}

int fafikLib_regex::item_match_anyOneOfText::tryTest(item_posIO& item_posIO_store, item_posFoundO& item_returns) const
{
	const bool do_r_find= item_posIO_store.b_reverseCheck.getBit(option_rCheck_fromEnd);
	const bool b_reversed= optionsMask.getBit(option_Inverted);
	 ///stores the sub-function find result
	int ret_procVal;

	if( do_r_find ) {	//find backward
		ret_procVal= tryTest_backward( item_posIO_store, item_returns );
	} else	//find forward
		ret_procVal= tryTest_forward( item_posIO_store, item_returns );

	item_posIO_store.pos_off_maxPos= 0;
	item_posIO_store.pos_start_maxOff= 0;

	if( b_reversed ){	//reverse the query
		if( ret_procVal>= hitTest_failedMatch )
			return hitTest_ok;
		return hitTest_failedMatch;
	}
	return ret_procVal;
}
int fafikLib_regex::item_match_anyOneOfText::tryTest_forward(item_posIO& item_posIO_store, item_posFoundO& item_returns) const
{
	const bool b_reversed= optionsMask.getBit(option_Inverted);

	items_tempStore itemTempIO;
	itemTempIO.t_pos_start_maxOff= item_posIO_store.pos_start_maxOff;
	 //this works for find, and for r_find we just use =Distanced_FromToBegin.min
	size_t CutMaxPos= item_posIO_store.get_maxPosOfEnd(Distanced_FromToEnd.min, false);

	 //cut a string part
	itemTempIO.boundStr_startPos= item_posIO_store.pos_start_begin;
	itemTempIO.boundString= item_posIO_store.PathElement_str_p->SubString( item_posIO_store.pos_start_begin, CutMaxPos);
	itemTempIO.findFrom= 0;
	if(optionsMask.getBit(option_CaseSense))
		itemTempIO.boundString.LowerCase();	//case insensitive
	itemTempIO.initiated_str= true;

	int findRes= hitTest_failedMatch;
	int findRes_least= hitTest_failedMatch;

	for( const_iterator StrArrayIter= stringsList.begin(); StrArrayIter!=stringsList.end(); ++StrArrayIter ){
		itemTempIO.findFrom= 0;	//this needs to be reset every iter
		findRes= tryTest_forward_str( item_posIO_store, item_returns, itemTempIO, *StrArrayIter );	//try:find a single string
		if(findRes < findRes_least) findRes_least= findRes;

		if( !b_reversed && findRes == hitTest_match){	//if(normal && ok) then
			return findRes;
		} else if( b_reversed && findRes <= hitTest_failedMatch){	//if(invert && NOT ok) then
			return findRes;
		}
	}

return findRes_least;
}
int fafikLib_regex::item_match_anyOneOfText::tryTest_backward(item_posIO& item_posIO_store, item_posFoundO& item_returns) const
{
	const bool b_reversed= optionsMask.getBit(option_Inverted);
	item_posIO_store.pos_start_begin_cpy= item_posIO_store.pos_start_begin;

	items_tempStore itemTempIO;
	itemTempIO.t_pos_start_maxOff= item_posIO_store.pos_start_maxOff;
	 //this works for find, and for r_find we just use =Distanced_FromToBegin.min
	const size_t CutToPos_Max= item_posIO_store.pos_start_begin- this->predictedLength.max +1;
	 ///this cuts down the char amount we will have to go through
	size_t CutToPos= itemTempIO.t_pos_start_maxOff;
	if(item_posIO_store.pos_off_maxPos> CutToPos) CutToPos= item_posIO_store.pos_off_maxPos;

	CutToPos= CutToPos_Max- ((unsigned short)CutToPos);
	if( CutToPos> CutToPos_Max || CutToPos< Distanced_FromToBegin.min)
		CutToPos= Distanced_FromToBegin.min;

	 //cut a string part
	itemTempIO.boundStr_startPos= CutToPos;
	itemTempIO.boundString= item_posIO_store.PathElement_str_p->SubString( CutToPos, item_posIO_store.pos_start_begin);
	itemTempIO.findFrom= -1;
	if(optionsMask.getBit(option_CaseSense))
		itemTempIO.boundString.LowerCase();	//case insensitive
	itemTempIO.initiated_str= true;

	int findRes= hitTest_failedMatch;
	int findRes_least= hitTest_failedMatch;

	for( const_iterator StrArrayIter= stringsList.begin(); StrArrayIter!=stringsList.end(); ++StrArrayIter ){
		itemTempIO.findFrom= -1;	//this needs to be reset every iter
		findRes= tryTest_backward_str( item_posIO_store, item_returns, itemTempIO, *StrArrayIter );	//try:find a single string
		if(findRes < findRes_least) findRes_least= findRes;

		if( !b_reversed && findRes == hitTest_match){	//if(normal && ok) then
			return findRes;
		} else if( b_reversed && findRes <= hitTest_failedMatch){	//if(invert && NOT ok) then
			return findRes;
		}
	}

return findRes_least;
}
int fafikLib_regex::item_match_anyOneOfText::tryTest_forward_str(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& it_IO, const wxString& match_str) const
{
	size_t maxPosOfEnd= item_posIO_store.get_maxPosOfFind(false);
	size_t maxMaxPosOfEnd= item_posIO_store.get_maxPosOfFind_full(false);
	size_t maxPosFin= item_posIO_store.get_maxPosOfEnd( Distanced_FromToEnd.min, false );
	size_t DPosFromFin= item_posIO_store.get_maxPosOfEnd( Distanced_FromToEnd.max, false );

	while(true) {
		it_IO.find_res= it_IO.boundString.find( match_str, it_IO.findFrom );
		if( it_IO.find_res== wxString::npos )
			return hitTest_failedMatch;	//not found
	  //found
		it_IO.findFrom= it_IO.find_res +1;
		it_IO.find_res_plusBegin= it_IO.boundStr_startPos+ it_IO.find_res;
	  //save start pos for next find
		it_IO.next_pos_start_begin= it_IO.find_res_plusBegin+ match_str.size();

	  //not found at all (continue; above needed), if
		if( it_IO.find_res_plusBegin== wxString::npos )
			return hitTest_failedMatch;
		item_returns.found_pos_from= it_IO.find_res_plusBegin;
		item_returns.found_pos_to= it_IO.find_res_plusBegin+ match_str.size()-1;
	  //ok found, but where?
		if( item_returns.found_pos_to > maxPosFin ){
			return hitTest_failedMatch;	//found out of scope (too far)
		}
		else if( item_returns.found_pos_to < DPosFromFin ){
			continue;	//found too soon
		}
		 //found at somewhat good pos
		item_posIO_store.pos_start_begin= it_IO.next_pos_start_begin;
		if( item_returns.found_pos_from > maxMaxPosOfEnd ){	//found out of range, --error
			return hitTest_failedMatch;
		}
		else if( item_returns.found_pos_from > maxPosOfEnd ) {	//found but at bad? pos, --recheck
			  //save that max pos of find
			item_posIO_store.reverseFind= it_IO.find_res_plusBegin;
			return hitTest_failedPartially_checkBackwards;
		}
		  //found, at good pos, --ok
		return hitTest_ok;
	}
return hitTest_failedMatch;
}
int fafikLib_regex::item_match_anyOneOfText::tryTest_backward_str(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& it_IO, const wxString& match_str) const
{
	size_t maxPosStart= item_posIO_store.get_maxPosOfFind(true);
	size_t maxMaxPosStart= item_posIO_store.get_maxPosOfFind_full(true);
	size_t maxPosFin= Distanced_FromToBegin.min;
	size_t DPosFromFin= Distanced_FromToBegin.max;
	if(maxPosStart> item_posIO_store.pos_start_begin_cpy) maxPosStart= 0;
	if(maxMaxPosStart> item_posIO_store.PathElement_str_p->size()) maxMaxPosStart= 0;

	while(true) {
		it_IO.find_res= it_IO.boundString.rfind( match_str, it_IO.findFrom );
		if( it_IO.find_res== wxString::npos )
			return hitTest_failedMatch;	//not found
		it_IO.findFrom= it_IO.find_res -1;
		it_IO.find_res_plusBegin= it_IO.boundStr_startPos+ it_IO.find_res;
	  //save start pos for next find
		it_IO.next_pos_start_begin= it_IO.find_res_plusBegin-1;// match_str.size() +1;

		 //not found at all (continue; above needed), if
		if( it_IO.find_res_plusBegin== wxString::npos )
			return hitTest_failedMatch;
		item_returns.found_pos_from= it_IO.find_res_plusBegin;
		item_returns.found_pos_to= it_IO.find_res_plusBegin+ match_str.size()-1;
		if(match_str.empty())	//or no string at all
			item_returns.found_pos_to= item_returns.found_pos_from;
	  //ok found, but where?
		if( item_returns.found_pos_from < maxPosFin ){
			return hitTest_failedMatch;	//found out of scope (too far)
		}
		else if( item_returns.found_pos_from > DPosFromFin ){
			continue;	//found too soon
		}
		 //found at somewhat good pos
		item_posIO_store.pos_start_begin= it_IO.next_pos_start_begin;
		if( item_returns.found_pos_to < maxMaxPosStart ){	//found out of range, --error
			return hitTest_failedMatch;
		}
		else if( item_returns.found_pos_to < maxPosStart ) {	//found but at bad? pos, --recheck
			  //save that max pos of find
			item_posIO_store.reverseFind= it_IO.find_res_plusBegin;
			return hitTest_failedPartially_checkBackwards;
		}
		  //found, at good pos, --ok
		return hitTest_ok;
	}
return hitTest_failedMatch;
}
void fafikLib_regex::item_match_anyOneOfText::add_string( const wxString& stringToAdd )
{
	if(stringToAdd.empty()){	//optionsMask.getBit(option_Inverted) &&
		andEmptyString.setBit(0,1);
		return;	//we can NOT find a reverse of EmptyString
	}
	if( optionsMask.getBit(option_CaseSense) ){
		stringsList.Add( stringToAdd.Lower() );
	} else {
		stringsList.Add( stringToAdd );
	}
}
void fafikLib_regex::item_match_anyOneOfText::_updateFinal()
{
	 //sort the List Len>Len
	std::sort(stringsList.begin(), stringsList.end(), fafikLib_regex::item_match_anyOneOfText::_sortStrings );

	if(stringsList.size()){	//we already sorted it from longest to shortest, and now we need to set min max sizes
		predictedLength.max= stringsList.front().size();
		predictedLength.min= stringsList.back().size();
	}
	if(andEmptyString.getBit(0)){
		if( optionsMask.getBit(option_Inverted) ){
			return;	//we can NOT find a reverse of EmptyString
		}
		stringsList.Add("");	//add the empty string only once as last element
		predictedLength.min= 0;
	}
	andEmptyString.setBit(1,1);
}
bool fafikLib_regex::item_match_anyOneOfText::_sortStrings(const wxString& item_a, const wxString& item_b )
{
	return ( item_a.size() > item_b.size() );
}


 //class fafikLib_regexVector_file, from this point

int fafikLib_regexVector_file::testPath( const wxArrayString& PathInParts, const DWORD& fileAttribs, bool SamePath, canSkippDirs_tracker* O_canSkippDirs, fafikLib_regex::resultHashInfo* resultHashInfo_o) const
{
	if(this->empty())
		return fafikLib_regex::testReturns_noMatch;
	 //if is Include && has a fileType but does not match this filter
	if(type_isInclude && fileAttribs && !fafikLib_regex::isMatchingAttribFile_dir(fileAttribs, type_FileAttribs) ){
		//we got attrib.File but we were expecting ONLY attrib.Dir (or reverse)
		return fafikLib_regex::testReturns_include_INVALID;		//result= INVALID test (includes a different filter)
	}
	if(O_canSkippDirs) O_canSkippDirs->resize(this->size());
	int gotResTemp= 0;
	size_t tempAtPos= 0;
	fafikLib_regex::resultPosition resultPos;
	fafikLib_regex::resultPosition* resultPos_passOn= nullptr;
	bool canSkipp_thoseDirs= false;
	if( O_canSkippDirs== nullptr ) canSkipp_thoseDirs= SamePath;

	for(const_iterator tempIt= begin(); tempIt!= end(); ++tempIt ){	//iterate all Lines
		if(SamePath && O_canSkippDirs){
			canSkipp_thoseDirs= O_canSkippDirs->at(tempAtPos);
		}
		resultPos_passOn= nullptr;
		if(resultHashInfo_o && (*tempIt)->hasHash()) resultPos_passOn= &resultPos;
		gotResTemp= (*tempIt)->testPath(PathInParts, fileAttribs, canSkipp_thoseDirs, this->type_isInclude, O_canSkippDirs? &O_canSkippDirs->at(tempAtPos) : nullptr, resultPos_passOn);	//test against all Lines
		++tempAtPos;
		if(gotResTemp== fafikLib_regex::hitTest_match){	//if matches
		  //process #Hash
			if(resultHashInfo_o && resultPos_passOn){
				resultHashInfo_o->HashPart= (*tempIt)->getHashPart(PathInParts, resultPos_passOn, &(resultHashInfo_o->Hash_type) );
			}
		  //return a hit
			if(type_isInclude)
				return fafikLib_regex::testReturns_isIncluded;	//result= copy/include this file
			return fafikLib_regex::testReturns_isExcluded;		//result= skip this file
		}
	}
	return fafikLib_regex::testReturns_noMatch;
}
bool fafikLib_regexVector_file::testPath_copies( const wxArrayString& PathInParts, const DWORD& fileAttribs, bool SamePath, canSkippDirs_tracker* O_canSkippDirs) const
{
	int retResState= this->testPath(PathInParts, fileAttribs, SamePath, O_canSkippDirs);
	if(retResState== fafikLib_regex::testReturns_isExcluded) return false;	//matches & mode=exclude ==> skip
	if(retResState== fafikLib_regex::testReturns_noMatch && type_isInclude) return false;	//no match & mode=include ==> skip
	//if(retResState== fafikLib_regex::testReturns_isIncluded) return true;
	//if(retResState== fafikLib_regex::testReturns_include_INVALID) return true;
	return true;	//compacted all other cases into (true)
}


 //class fafikLib_regexMultipleFiles, from this point

bool fafikLib_regexMultipleFiles::testPath_doCopy( const wxArrayString& PathInParts, const DWORD& fileAttribs, bool SamePath, canSkippDirs_tr_PerPath* storageCanSkipp, fafikLib_regex::resultHashInfo* resultHashInfo_o) const
{
	 ///do treat non matching elements as not Included (that means Skip)
	bool treatThisAsInclude= false;
	 ///item is Include but it might get excluded by following rules (on 2022-10-30)
	bool treatThisAsInclude_Final= false;
	int retState_curr= 0;
	 ///keeping track of current item is faster then recalculating it from iter-offset-position
	size_t itemAtPos= 0;
	if(storageCanSkipp) storageCanSkipp->resize(filesToProcess.size());

	for(const_iterator itemIt= begin(); itemIt!= end(); ++itemIt){	//iterate through all files
		if( treatThisAsInclude && (*itemIt)->isInclude() ) //skip another layer of include(already included)
			continue;
		retState_curr= (*itemIt)->testPath(PathInParts, fileAttribs, SamePath, storageCanSkipp ? &storageCanSkipp->at(itemAtPos) : nullptr, resultHashInfo_o );	//test
		++itemAtPos;
		if(retState_curr){ //we have matched something
			if(retState_curr== fafikLib_regex::testReturns_include_INVALID){
				continue;	//thats INVALID result
			}
			if(retState_curr== fafikLib_regex::testReturns_isIncluded)
				treatThisAsInclude_Final= true;	//include ==> copy
			if(retState_curr== fafikLib_regex::testReturns_isExcluded)
				return false;					//exclude ==> skip
		}
		 //if we got to here check if this Rule file is include
		if( (*itemIt)->type_isInclude ) treatThisAsInclude= true;
	}

	if(treatThisAsInclude_Final) return true; //include ==> copy
	if(treatThisAsInclude) return false; //we did not match a result(and at least 1 Rule File is Including this type of fileAttribs) means ==> skip
	return true;
}
size_t fafikLib_regexMultipleFiles::DeleteAll()
{
	const size_t sizeTable= filesToProcess.size();
	for(const_iterator itemIt= this->begin(); itemIt!= this->end(); ++itemIt){
		delete (*itemIt);
	}
	filesToProcess.clear();
	return sizeTable;
}

