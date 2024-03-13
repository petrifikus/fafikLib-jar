#ifndef FAFIKLIB_REGEX_H
#define FAFIKLIB_REGEX_H

#include <wx/defs.h>
#include "wxStringInputStream.h"
#include "Byte_LL.h"
#include "../FileType_MDFC.h"	//_FileType_MDFC_Checksum
#include <algorithm>

 ///RegPath the advanced regex for File Paths
class fafikLib_regex
{
 public:
	 //new idea on 2020-01-06
	 //update idea on 2020-04-22, of idea(2020-01-06)
	 ///@return 0= matching, non 0= See for errors
	enum hitTest_returns {
		hitTest_ok,
		hitTest_match= hitTest_ok,

		 ///problem: found the item but further then expected, re-check to see if its still valid
		hitTest_failedPartially_checkBackwards,

		hitTest_failedMatch,
		hitTest_failedCompletely= hitTest_failedMatch,
		 ///file type is wrong
		hitTest_failed_fileType,
		 ///amount of sub dirs is wrong
		hitTest_failed_subDirs,

		hitTest_ok_cantQuestion= 256,
	};
	enum testReturns {
		 ///test ended without any match
		testReturns_noMatch,
		 ///test returns a match {translate to type_isInclude}
		testReturns_isIncluded,
		 ///test returns a match {translate to type_isInclude}
		testReturns_isExcluded,

		 ///test is INVALID (we were testing for include but tested fileType cant be considered)
		testReturns_include_INVALID= -1,
	};

  //declarations & protected:storage for regex, on 2021-04-21 [[
	enum item_processes {
		item_processes_none= 0,
		 ///simple "*file*.ext"
		item_processes_String,
		 ///simple "*" of the previous one
		item_processes_Star,
		 ///simple "/file.ext",	@this one should not exist in Vector --might be used as `return` Value
		item_processes_DirSeparator,
		 ///advanced "{Symbol, Params... }", --useful in if(questioned item > item_processes_Advanced) then is advanced
		item_processes_Advanced,
		 ///advanced "{0,1.0,100}" Number ==regex ???
		item_processes_Number,
		 ///advanced "{. or *,1}" any char ==regex .?*
		item_processes_AnyChar,
		 ///advanced "{![A-Zabcd]}" any in range ==regex [A-Z], only specified Case
		item_processes_AnyOfChars,
		 ///advanced "{ ?, "":i or :s, ... }" any of exact strings ==regex (text|txt)
		item_processes_AnyOneOfText,

		 ///Hash mask
		item_processes_xHashMask= 256,
		 ///advanced "{[0-9A-Fa-f],8,8, #crc32}" any in range ==regex [A-Z], only specified Case, #Hash included //2023-09-08
		item_processes_AnyOfChars_Hash= 256| item_processes_AnyOfChars,
	};
	 ///max 8 entries
	enum e_options_bit {
		 ///invert the question result eg: Not(A-Z)
		option_Inverted,
		 ///specify case Sense (def. 0- sensitive)
		option_CaseSense,
	};
	 ///max 8 entries
	enum e_reverseCheck_bit {
		 ///go from End to Begin
		option_rCheck_fromEnd,
		 ///use hints to find that item
		option_rCheck_hints,
	};
	 ///@return & ANY file, or & directory
	 ///as dumb as Windows is this function tests if given attribs match the mask being either: a folder or ANY file
	static bool isMatchingAttribFile_dir(const DWORD &fileAttribs, const DWORD &mask_testAttribs);
	struct resultHashInfo {
		wxString HashPart;
		_FileType_MDFC_Checksum Hash_type= _FileType_MDFC_Checksum_none;
	};
	struct resultPosition_it {
		unsigned short pos_from= 0;
		unsigned short pos_to= -1;
		 ///@see item_processes for info like: is star, is literal
		unsigned short type_processes= 0;
		 ///@see _FileType_MDFC_Checksum
		unsigned short Hash_type= _FileType_MDFC_Checksum_none;
		resultPosition_it() {}
		resultPosition_it(unsigned short pos_from, unsigned short pos_to): pos_from(pos_from), pos_to(pos_to) {}
		unsigned short size() const {if(pos_to== (unsigned short)-1)return -1; return (pos_to- pos_from) +1;}
		unsigned short length() const {if(pos_to== (unsigned short)-1)return -1; return (pos_to- pos_from) +1;}
		bool isAnyChar()const {return type_processes==item_processes_Star || type_processes==item_processes_AnyChar;}
		bool isLiteral()const {return type_processes==item_processes_String || type_processes==item_processes_AnyOneOfText; }
		bool isText()const {return isLiteral();}
		bool isHash()const {return type_processes & item_processes_xHashMask;}
	};
	typedef wxVector<resultPosition_it> resultPosition_part;
	 ///pos_from and pos_to are both inclusive <b>[from, to]</b> not [from, to)
	typedef wxVector<resultPosition_part> resultPosition;
	struct PathElement_len_limiter{
		unsigned short min= 0;
		unsigned short max= -1;
		 ///empty ctor
		PathElement_len_limiter() {}
		 ///new ctor
		PathElement_len_limiter(unsigned short _min, unsigned short _max ) :
			min(_min),
			max(_max)
		{}
		 ///copy ctor
		PathElement_len_limiter(const PathElement_len_limiter &other){
			min= other.min;
			max= other.max;
		}
		 //copy operator
		PathElement_len_limiter& operator = (const PathElement_len_limiter &other){
			min= other.min;
			max= other.max;
			return *this;
		}
	};
	struct item_CharsRange_FT {
		wxUniChar Char_from= 0;
		wxUniChar Char_to= 0;
		item_CharsRange_FT(wxUniChar char_from, wxUniChar char_to):
			Char_from(char_from),
			Char_to(char_to)
		{}
	};
	struct item_NumberRange_LL {
		long long r_from;
		long long r_to;
	};
	struct item_NumberRange_D {
		double r_from;
		double r_to;
	};
	 ///yes its named DL so it wont be before LL, but after D
	struct item_NumberRange_DL {
		long double r_from;
		long double r_to;
	};
	 ///this struct keeps the position of successful finds
	struct item_posFoundO {
		 ///copy `pos_start_begin`
		unsigned short start_pos= 0;
		 ///store pos found from pos
		unsigned short found_pos_from= 0;
		 ///store pos found to pos
		unsigned short found_pos_to= 0;
		 ///how far `found_pos_from` can be from `start_pos`
		unsigned short start_max_offset= 0;
		 ///how far `found_pos_from` can be over all items
		unsigned short max_max_offset= 0;

		unsigned short reFind_pos_backward= 0;

		 ///item tryTest() return state
		BYTE returnState= 0;
		BYTE isWhatever= false;
		 ///ask: can this match be at any further pos (when running a rechecking)
		inline bool canFindFurther() const {
			unsigned short alreadyUsedOff= found_pos_from- start_pos;
			return( max_max_offset> alreadyUsedOff );
		}
	};
	 ///this item is passed to Derived<-(item_base)
	struct item_posIO {
		 ///PathArr.at( @index ), increments PathElement when it succeeds
		size_t index_inPathArr= 0;
		 ///an access pointer to above PathElement(index), we wont have access to the Array itself so we are grabbing 1 Element
		const wxString* PathElement_str_p= nullptr;

		 ///pos in str PathElement to (r_)begin searching from
		size_t pos_start_begin= 0;
		 ///a copy of `pos_start_begin` for rec.iterators that need to check the start value of it while changing
		size_t pos_start_begin_cpy= 0;
		 ///maximum offset for starting the next search (always =set)
		size_t pos_start_maxOff= 0;
		 ///maximum Pos for re-starting the search (always ++)
		size_t pos_off_maxPos= 0;

		 ///non 0 if reverse finding @(specifies nextItem.found_pos-1 )
		unsigned short reverseFind= 0;

		 ///if string find failed, or can still exist?? @see hitTest_returns
		BYTE returnState= 0;
		 ///is set to true do reverse_find. @see e_reverseCheck_bit
		bool8 b_reverseCheck= false;

		wxVector<item_posFoundO> item_returns;
		wxVector<item_posFoundO> item_returns_rBack;
		wxVector<item_posFoundO> item_returns_hinted;
		unsigned short item_returns_index= 0;

		size_t get_maxPosOfFind(bool r_find=false) const;
		size_t get_maxPosOfFind_full(bool r_find=false) const;
		size_t get_maxPosOfEnd(const unsigned short &DistanceToEnd, bool r_find= false) const;
		 ///safely add the value
		bool addSet_off_maxPos(unsigned short amount){
			if(pos_off_maxPos> (unsigned short)-1) return false;
			pos_off_maxPos+= amount;
			return true;
		}
		 ///safely remove the value
		bool removeSet_off_maxPos(const size_t& amount){
			if(!amount || !pos_off_maxPos) return false;
			if(pos_off_maxPos<amount){ pos_off_maxPos= 0; return true;}
			pos_off_maxPos-= amount;
			return true;
		}
		 ///safely remove the value
		bool removeSet_off_maxPos(const size_t& amount1, const size_t& amount2){
			if( amount1> amount2 )
				return removeSet_off_maxPos(amount1- amount2);
			return removeSet_off_maxPos(amount2- amount1);
		}
	};
	struct items_tempStore {
		wxString boundString;
		bool initiated_str= false;

		size_t findFrom= 0;
		size_t find_res= -1;
		size_t DistanceToFin_now= 0;
		size_t find_res_plusBegin= 0;
		size_t t_pos_start_maxOff= 0;
		size_t next_pos_start_begin= 0;
		size_t boundStr_startPos= 0;
		unsigned short matchingAmount= 0;

		wxString::const_iterator char_iter_O_foundFrom;
		wxString::const_iterator char_iter_O_foundTo;
		 ///this is the current pos (r_)iter to be scanned
		wxString::const_iterator char_iter;
		wxString::const_iterator char_iterEnd;
		wxString::size_type iter_change= +1;

		size_t foundPos_from= 0;
		size_t foundPos_to= 0;
	};
	struct itemPath_tempStore {
		int errReturned= 0;
		item_posIO tempPosIO;
	};
	 ///a very basic(virtual) item to store in vector, its a base that answers tryTest()
	struct item_base {
		item_base(){}
		virtual ~item_base(){}

		 ///this stores the type to be processed
		unsigned short type_processes= item_processes_none;
		 ///stores type of Hash
		_FileType_MDFC_Checksum HashType= _FileType_MDFC_Checksum_none;
		 ///store how many wxChars we need at least, "file*name"= 8, and at most = -1
		 ///@derivedClass has to fill this in
		PathElement_len_limiter predictedLength;
		 ///@param min= store the minimum length continuation of pathElement ( for =+(next parts in same Element).min )
		 ///@param max= store how far from the end item can finish, if its 0 it has to end now, if not then there is no pressure
		PathElement_len_limiter Distanced_FromToEnd;
		 ///@param min= store the minimum length to beginning
		 ///@param max= store how far from the beginning item can finish, if its 0 it has to end now
		PathElement_len_limiter Distanced_FromToBegin;
	  //abstract functions section
		 ///@return hitTest_(state) @see hitTest_returns
		virtual int tryTest(item_posIO& item_posIO_store, item_posFoundO& item_returns ) const =0;
	};

  //Derived Classes of `item_base` [[
	struct item_match_string: public item_base {
		 ///override dtr
		~item_match_string() {}
		 ///store string to match
		wxString match_str;
		 ///CaseSense, reversed, @see e_options_bit
		bool8 optionsMask= 0;
	  //abstract-less functions section
		int tryTest(item_posIO& item_posIO_store, item_posFoundO& item_returns) const;
		item_match_string() {}
		item_match_string(const wxString& str, const bool8 options=0 ) { set(str, options); }
		void set(const wxString& str, const bool8 options=0 );
		void update();
	 protected:
	 	 ///@return pos found at (-1 for error)
		size_t tryTest_forward(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& IO) const;
		 ///@return pos rfound at (-1 for error)
		size_t tryTest_backward(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& IO) const;
	};
	struct item_match_star: public item_base {
		 ///override dtr
		~item_match_star() {}
	  //abstract-less functions section
		int tryTest(item_posIO& item_posIO_store, item_posFoundO& item_returns) const;
		item_match_star() { set(); }
		void set();
	};
	struct item_match_number: public item_base {
		 ///override dtr
		~item_match_number() {}
		 ///store raw number to match, (not for #long Double=12). @see item_NumberRange_LL
		union _numT{
			 ///a cast to `Byte` if someone needs it
			unsigned char rawData;
			 ///an Byte array that ensures min size= 16
			unsigned char rawData_P[16];
			item_NumberRange_LL num_LL;
			item_NumberRange_D num_D;
		} Number;
		 ///CaseSense-->see if negative '-', reversed, @see e_options_bit
		bool8 optionsMask= 0;
		 ///1 int(LL), 2 double, cast using @see item_NumberRange_LL
		BYTE NumberType= 1;
		 ///0= none error, 1= min, 2= max, 1+2=3= both
		BYTE NumbersExists= 0;
		 ///specify which char acts as '.' or ',' in floating numbers, if 0 then its not float
		wxChar32 pointSeparatorChar= wxChar32('.');
	  //abstract-less functions section
		int tryTest(item_posIO& item_posIO_store, item_posFoundO& item_returns) const;
		 ///ctor specifies that we are handling number
		item_match_number() { memset(Number.rawData_P, 0x00, sizeof(Number)); type_processes= item_processes_Number; }
		 ///@return true= did adjust, false= nothing
		bool _update();
	 protected:
		int tryTest_forward(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& it_IO ) const;
		int tryTest_backward(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& it_IO ) const;
		int tryTest_findNumberStr( items_tempStore& it_IO, const bool cantOffset=false ) const;
		int tryTest_numInRange( items_tempStore& it_IO ) const;
	};
	 ///this is a bit more special then `anyOfChars`, as it skips min chars and treats rest as optional
	struct item_match_anyChar: public item_base {
		 ///override dtr
		~item_match_anyChar() {}
	  //abstract-less functions section
		int tryTest(item_posIO& item_posIO_store, item_posFoundO& item_returns) const;
		item_match_anyChar() {}
		item_match_anyChar( unsigned short am_min, unsigned short am_max ) { set(am_min, am_max); }
		item_match_anyChar( unsigned short amount ) { set(amount); }
		void set( unsigned short am_min, unsigned short am_max );
		inline void set( unsigned short amount ) { set(amount,amount); }
	 protected:
		 ///calculated max-min
		unsigned short maxOffset= 0;
		int tryTest_forward(item_posIO& item_posIO_store, item_posFoundO& item_returns) const;
		int tryTest_backward(item_posIO& item_posIO_store, item_posFoundO& item_returns) const;
	};
	 ///this will match at least min chars, up to max chars and the rest passed further
	struct item_match_anyOfChars: public item_base {
		 ///override dtr
		~item_match_anyOfChars() {}
		 ///store ranges to match
		wxVector<item_CharsRange_FT> charsRanges;
		 ///store single chars to match
		wxString charsToMatch;
		 ///reversed, @see e_options_bit
		bool8 optionsMask= 0;
	  //abstract-less functions section
		int tryTest(item_posIO& item_posIO_store, item_posFoundO& item_returns) const;
		 ///empty ctor
		item_match_anyOfChars() { type_processes= item_processes_AnyOfChars; }
		 ///@return false= set success, true= error
		bool setAmount(unsigned short am_min, unsigned short am_max ) {if(am_min>am_max)return true; predictedLength.min= am_min; predictedLength.max= am_max; return false; }
		inline void setAmount(unsigned short amount ) {setAmount(amount, amount);}
		 ///check if its empty (does not contain anything)
		bool empty() const { return !(charsRanges.size() || charsToMatch.size() ); }

		 ///alias to add range
		inline void add_range( const item_CharsRange_FT& adds ) { charsRanges.push_back(adds); }
		 ///alias to add char
		inline void add_char( const wxUniChar& c_char ) { charsToMatch.Append(c_char); }
		 ///alias to add chars
		inline void add_char( const wxString::const_iterator& it_from, const wxString::const_iterator& it_to ){
			if( it_from< it_to )
				charsToMatch.append(it_from, it_to);
		}
	 protected:
		size_t tryTest_forward(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& it_IO) const;
		size_t tryTest_backward(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& it_IO) const;
		 ///@return true if current char is in range
		inline bool tryTestChar(const wxUniChar &atPosChar) const;
	};
	 ///this will match any of specified Text, from longest to shortest
	struct item_match_anyOneOfText: public item_base {
		 ///override dtr
		~item_match_anyOneOfText(){};
		 ///storage for string
		wxStringVector stringsList;
		 ///CaseSense, reversed, @see e_options_bit
		bool8 optionsMask= 0;
	  //def
		typedef wxStringVector::iterator iterator;
		typedef wxStringVector::const_iterator const_iterator;
		const_iterator begin() const {return stringsList.begin();}
		const_iterator end() const {return stringsList.end();}
	  //abstract-less functions section
		int tryTest(item_posIO& item_posIO_store, item_posFoundO& item_returns) const;
		 ///basic ctor
		item_match_anyOneOfText(){type_processes= item_processes_AnyOneOfText;}
		 ///alias to add string
		inline void add_string( const wxString& stringToAdd );
		 ///this function finalizes the string collection(by adding the empty string last)
		inline void _updateFinal();
	 protected:
	 	bool8 andEmptyString= 0;
	 	static bool _sortStrings(const wxString& item_a, const wxString& item_b );
		int tryTest_forward(item_posIO& item_posIO_store, item_posFoundO& item_returns) const;
		int tryTest_backward(item_posIO& item_posIO_store, item_posFoundO& item_returns) const;
		 int tryTest_forward_str(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& it_IO, const wxString& match_str) const;
		 int tryTest_backward_str(item_posIO& item_posIO_store, item_posFoundO& item_returns, items_tempStore& it_IO, const wxString& match_str) const;
	};
  //end ]] Derived Classes of `item_base`

	 ///this stores ranges where items begin & end (both inclusive)
	struct item_base_pathParts {
		 ///empty destructor
		~item_base_pathParts() {}
		typedef wxVector<item_base*> pathParts;
		typedef pathParts::iterator iterator;
		typedef pathParts::const_iterator const_iterator;
		iterator pos_start;
		iterator pos_end;
		 ///store the <min,max> length of PathElement @(useful in quick search)
		PathElement_len_limiter PathElement_len_predicted;
		 ///extremely quick confirmation if this PathElement looks for /'*'/ ps. will not work with '**', {*,0,-1}
		bool _matchesAny= false;

		item_base_pathParts( iterator _start, iterator _end ):
			pos_start(_start),
			pos_end(_end)
		{}
	  //functions section
		void calc_len_predicted();
		inline size_t getSize() const {return pos_end- pos_start; }
		inline bool canTest(unsigned short strLen) const { return ( strLen>= PathElement_len_predicted.min && strLen<= PathElement_len_predicted.max ); }
		inline const_iterator begin() const {return pos_start;}
		inline const_iterator end() const {return pos_end;}
		inline const_iterator rbegin() const {return pos_start-1;}
		inline const_iterator rend() const {return pos_end-1;}
	};

	 ///this structure stores Parser data temporarily
	struct full_regex_item_ParserTempStore {
		unsigned short lastPathElIndex= 0;
		 ///this vector stores `items_p`.indexes
		wxVector<PathElement_len_limiter> tempPathElements;

	};
	 ///storage of 1 Line worth of RegPath
	struct full_regex_item {
	 //questions
		inline bool beginsAtRoot() const {return is_rooted;}
		inline unsigned short getDepthMin() const {return amount_dirs;}
		inline unsigned short getDepthMax() const {if(is_rooted)return amount_dirs; return -1;}
		inline unsigned short getDepthStart(unsigned short dirsIn) const { if(is_rooted)return 0; if(dirsIn<amount_dirs)return 0; return dirsIn- amount_dirs; } //we cant go negative dirs, so always make sure its a positive
		inline bool canTest( unsigned short dirsIn ) const {if(is_rooted && dirsIn== amount_dirs) return true; if(!is_rooted && dirsIn>= amount_dirs) return true; return false; }
		inline bool canTest_includeFolder( unsigned short dirsIn ) const {if(is_rooted && dirsIn> amount_dirs) return false; return true; }
		inline bool hasHash() const { return has_hash; }
		inline bool hasChecksum() const { return has_hash; }
		_FileType_MDFC_Checksum getHashType() const;
	 //main functions, like start, and Try
		 ///main function to parse input string version into machines
		int setPattern( const wxString& strPattern );
		 ///automatic helper for marking PathElement
		int separatePathElement( full_regex_item_ParserTempStore& tempStore );
		 ///@return amount of items
		 ///alias items_p.push_back
		inline size_t AddItem( item_base* item ){
			items_p.push_back( item );
			return items_p.size();
		}
	 protected:
	 	friend class fafikLib_regex;
		 ///stores all items
		wxVector<item_base*> items_p;	// "*/*/file*{0,1,10}.{?,mkv,mp4}"	accepts any file "file<1,10>.(mkv|mp4)" under 2+ layers of folders
		 ///stores iterators(start,end) that form a part of path (Dir or File name), do NOT destroy()
		wxVector<item_base_pathParts> items_pathParts;	//splits above into path parts "*/", "*/", "file..."
		 ///whether or not the item has to be at precise depth
		bool is_rooted= false;
		 ///if #Hash like #crc32 is present
		bool has_hash= false;
		 ///this determines CaseSense, include/exclude on found
		bool8 optionsMask= 0;
		 ///how many levels of Dirs does this item need (file counts too)
		unsigned short amount_dirs= 0;
		 ///we want to work on Folder or File ?
		DWORD FileType= FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_DIRECTORY;

		void FreeItems(){
			for( wxVector<item_base*>::iterator tempIt= items_p.begin(); tempIt!= items_p.end(); ++tempIt ){
				delete *tempIt;	//free RAM
			}
			items_p.clear();	//just in case we trigger it ourself
		}
	 public:
	 	 ///dtor that frees up everything(pointers) when deleted
		~full_regex_item() {
			FreeItems();	//free RAM
		};
	};

 protected:
 	enum e_setPattern_advanced_errors {
		error_setPatternAdv_ok,
		error_setPatternAdv_slight,
		error_setPatternAdv_preemptiveExit,
		 ///this is the point at which we stop the regex compiler. whether its EOL, syntax error, or anything that would break it that hard
		error_setPatternAdv_Error,
 	};
	struct pattern_storeAdvTemp_struct{
		 ///used in advanced
		bool forcedFindMinus= false;
		 ///used in advanced
		bool8 optionsOverride;
		 ///a char (or not defined yet) that specifies ',' or ';'
		wxChar32 advancedSeparatorChar= 0;
		wxChar32 floatSepChar= 0;

		 ///0= false, 1= LL, 2= Double
		BYTE NumberStrFound= false;
		wxString::const_iterator found_start;
		wxString::const_iterator found_end;
 	};
 	struct pattern_store_struct {
		const wxString* strPattern_p= nullptr;
		wxString::const_iterator iterAtPos_curr;
		wxString::const_iterator iterAtPos_end;
		wxString::const_iterator iterAtPos_itemStart;
//		wxString::const_iterator iterAtPos_advancedStart;
		bool begun_itemStr= false;
		bool8 optionsMask;

		item_match_string* itemStrToAdd= nullptr;

		int backslashCount= 0;
		 ///only an easy access
		pattern_storeAdvTemp_struct* advTemp_store= nullptr;
		wxString reason;
 	};
 	class _safeItem_based {
	 public:
		_safeItem_based(item_base* itemBase){_itemBase= itemBase;}
		~_safeItem_based() { if(_itemBase){delete _itemBase; _itemBase= nullptr; } }
		void disarm(){_itemBase= nullptr;}
	 private:
		item_base* _itemBase= nullptr;
 	};
 // ]]end 2021-04-21, declarations & protected:storage

 // functions used to compile, test the RegPath item [[
 	 ///@return false= string & cancel special, true= enter special, match string was created
 	bool setPattern_advancedOrString(pattern_store_struct& pattern_storeS, const bool dontFinalize= false);
 	 ///@return false= string & cancel special, true= enter special, match string was created
 	bool processEscape(const wxString::const_iterator& currPos, const size_t backslashCount, wxString& outputEscaped);
 	 ///@return e_setPattern_advanced_errors
 	int setPattern_advanced_enter(pattern_store_struct& pattern_storeS);
	   ///finds the next ',;', the pos is just after that char(not it) @return e_setPattern_advanced_errors
 	  int setPattern_advanced_findNext_separated(pattern_store_struct& pattern_storeS);
	   ///finds the next '#', the pos is just after that char(not it) @return e_setPattern_advanced_errors
	   ///@return 0= ok found can extract Hash info, 1-3= error not found
 	  int setPattern_advanced_findNext_hash(pattern_store_struct& pattern_storeS);
 	   _FileType_MDFC_Checksum setPattern_advanced_findNext_hashProcess(const pattern_store_struct& pattern_storeS);
 	   ///@return 0= not found(up to separator char, or EOL), 1= LL, 2= Double with dot, 3= Double with coma
 	  int setPattern_advanced_findNumberStr(pattern_store_struct& pattern_storeS, BYTE find_dot_forDouble= false);
 	   ///@return e_setPattern_advanced_errors
 	  int setPattern_advanced_findFloatSpecyfic(pattern_store_struct& pattern_storeS);
 	 ///@return e_setPattern_advanced_errors
 	int setPattern_advanced_anyChar(pattern_store_struct& pattern_storeS);
 	 ///@return e_setPattern_advanced_errors
 	int setPattern_advanced_anyOfChars(pattern_store_struct& pattern_storeS);
 	  ///@return e_setPattern_advanced_errors
 	 int setPattern_advanced_anyOfChars_predefined(pattern_store_struct& pattern_storeS);
 	 ///@return e_setPattern_advanced_errors
 	int setPattern_advanced_number(pattern_store_struct& pattern_storeS);
 	 ///@return e_setPattern_advanced_errors
 	int setPattern_advanced_anyOneOfText(pattern_store_struct& pattern_storeS);
 	   //
 	  int extractQuotedText(pattern_store_struct& pattern_storeS, wxString &outputStrFull);
	int copyPos_StartEnd_toResult(const wxVector<item_posFoundO>& inputVec, resultPosition_part& outputResult, const item_base_pathParts& context_itemBasePP)const;

	int testPathPart( const item_base_pathParts& namedP, itemPath_tempStore& itPath_IO, resultPosition_part* resultPos_part=nullptr ) const;
	int testPath_forward( const item_base_pathParts& namedP, itemPath_tempStore& itPath_IO ) const;
	int testPath_backward( const item_base_pathParts& namedP, itemPath_tempStore& itPath_IO ) const;
	int testPath_withHints( const item_base_pathParts& namedP, itemPath_tempStore& itPath_IO ) const;
  //end]] functions

  //store data
 protected:
	 ///data storage
	full_regex_item regex_item;
	BYTE b_compiled= false;
	 ///@see fafikLib_regex::hitTest_returns
	int i_lastErrorCode= 0;

	  ///debug only ctor(don't use)
	wxDEPRECATED( fafikLib_regex() ) {
		printf("@@ Using Debug version of `fafikLib_regex`\n");
	}
	 ///parse the line(max len= 2000) to compile a file-regex then used to test files against it
	int setPattern( const wxString& strPattern, bool caseInsensitive, DWORD FileAttribs_type= -1 );

  //public accessible part of RegPath
 public:
	virtual ~fafikLib_regex();
  //def
	 ///@return unsigned short
	 ///here we are using uShort(0-65k) as its exactly the upper limit of ELLong Path (in Windows) so it can store exactly which dir
	typedef unsigned short canSkippSomeDirs;
  //functions and calls
	fafikLib_regex( const wxString& strPattern, bool caseInsensitive= true, DWORD FileAttribs_type= -1 );
	static size_t separatePath( const wxString& PathAsString, wxStringVector& pathInParts );
		///@return hitTest_returns:: @see fafikLib_regex::hitTest_returns
		///@param const wxStringVector& PathInParts -takes in path relative to the source.root
		///@param fileAttribs -provide file attributes whether its a file/folder
		///@param bool SamePath -set this to skip processing of parentDirectories of item
		///@param bool isIncludeMode -it will report back include error instead of exclude error
		///@param (canSkippSomeDirs* O_canSkippDirs) -keep this to skip processing of already matched path part
		///@param (resultPosition* <b>resultPos</b>) -retrieve result of match, size based on Template not on provided path
		///the main part of the regex	--array separated parts of path edition
	int testPath( const wxStringVector& PathInParts, const DWORD &fileAttribs, const bool& SamePath=false, const bool& isIncludeMode=false, canSkippSomeDirs* O_canSkippDirs=nullptr, resultPosition* resultPos=nullptr) const;
		///@return hitTest_returns:: @see fafikLib_regex::hitTest_returns
	inline int testPath( const wxString& PathAsString, const DWORD &fileAttribs, const bool& isIncludeMode=false, resultPosition* resultPos=nullptr ) const {
		wxStringVector pathInParts;
		separatePath(PathAsString, pathInParts);
		return testPath( pathInParts, fileAttribs, false, isIncludeMode, nullptr, resultPos );
	}

	 ///gets Hash string from file name (eg. "The Movie [01AB]" => "01AB".crc16) that was specified in regpath	//2023-09-08
	 ///@return @b wxString of Hash part AND @b HashType_o info of Hash Type = @see _FileType_MDFC_Checksum
	static wxString getHashPart(const wxStringVector& PathInParts, const resultPosition* resultPos, _FileType_MDFC_Checksum* HashType_o );

	inline bool hasHash() const {return regex_item.hasHash();}
	inline _FileType_MDFC_Checksum getHashType() const {return regex_item.getHashType();}
	inline DWORD Get_File_Attribs() const {return regex_item.FileType;}
	 ///@return @see fafikLib_regex::hitTest_returns
	inline int Get_errorCode() const {return i_lastErrorCode;}
	 ///@return @see fafikLib_regex::hitTest_returns
	inline int getError() const {return i_lastErrorCode;}

  //we operate on complex derived pointers(class) here, we cant just copy this item as we would end up copying pointers to memory that was just freed up
  wxDECLARE_NO_COPY_CLASS(fafikLib_regex);
};

class fafikLib_regexVector_file: public wxVector<fafikLib_regex*>
{
 protected:
 	void FreeItems(){
 		for( iterator tempIt= begin(); tempIt!= end(); ++tempIt ){
			delete *tempIt;
		}
		wxVector::clear();
 	}
 	void clear() {
 		FreeItems();
	}
 public:
 //storage
 	bool type_isInclude= false;
 	bool type_caseSensitive= false;
 	 ///auto gathered info of attribs file/dir,
 	DWORD type_FileAttribs= 0;
	 ///optional
 	wxString fileName;

 //defines
	typedef wxVector<fafikLib_regex::canSkippSomeDirs> canSkippDirs_tracker;

 //functions & calls
	~fafikLib_regexVector_file(){
		FreeItems();
	};
	fafikLib_regexVector_file(bool includeMode=false, bool CaseSensitiveMode=false): type_isInclude(includeMode), type_caseSensitive(CaseSensitiveMode) {}
	 ///@return errorCode
	 ///@param silenceWarns: &1 silence error, &2 silence #Hash type info
	int AddItem( const wxString& strPattern, DWORD FileAttribs_type, BYTE override_caseSensitive= -1, BYTE silenceWarns= -1 ){
		type_FileAttribs|= FileAttribs_type;
		if( override_caseSensitive== BYTE(-1) )
			override_caseSensitive= type_caseSensitive;
		fafikLib_regex* tempItem= new fafikLib_regex(strPattern, !override_caseSensitive, FileAttribs_type);
		push_back( tempItem );
		if(tempItem->hasHash() && !(silenceWarns&2) ){
			_FileType_MDFC_Checksum type= tempItem->getHashType();
			switch (type)
			{
			 case _FileType_MDFC_Checksum_MD2: wxLogMessage("Hash Type: MD2"); break;
			 case _FileType_MDFC_Checksum_MD4: wxLogMessage("Hash Type: MD4"); break;
			 case _FileType_MDFC_Checksum_MD6: wxLogMessage("Hash Type: MD6"); break;
			 case _FileType_MDFC_Checksum_MD5: wxLogMessage("Hash Type: MD5"); break;
			 case _FileType_MDFC_Checksum_SHA1: wxLogMessage("Hash Type: SHA1"); break;
			 case _FileType_MDFC_Checksum_SHA2_224: wxLogMessage("Hash Type: SHA2 224"); break;
			 case _FileType_MDFC_Checksum_SHA2_256: wxLogMessage("Hash Type: SHA2 256"); break;
			 case _FileType_MDFC_Checksum_SHA2_384: wxLogMessage("Hash Type: SHA2 384"); break;
			 case _FileType_MDFC_Checksum_SHA2_512: wxLogMessage("Hash Type: SHA2 512"); break;
			 case _FileType_MDFC_Checksum_crc16: wxLogMessage("Hash Type: CRC 16"); break;
			 case _FileType_MDFC_Checksum_crc32: wxLogMessage("Hash Type: CRC 32"); break;
			 case _FileType_MDFC_Checksum_crc64: wxLogMessage("Hash Type: CRC 64"); break;
			 default:
				wxLogWarning("!Unknown Hash Type: %u", (unsigned)type );
			 break;
			}
		}
		if( tempItem->Get_errorCode() && !(silenceWarns&1) ){
			wxLogError("Error Compiling Rule Line[%u] of File \"%s\"", this->size()-1, fileName );
		}
		return tempItem->Get_errorCode();
	}
	 ///@return errorCode
	 ///@warning ! this item will be now managed by this class (and its ~destruction)
	int AddItem(fafikLib_regex* byItem){
		if(!byItem) return -1;
		type_FileAttribs|= byItem->Get_File_Attribs();
		push_back(byItem);
		return byItem->Get_errorCode();
	}
	 ///@return @see fafikLib_regex::testReturns
	 ///@param SamePath & O_canSkippDirs are linked together if there is one only one will act, if both are set then they will act together
	 ///processed result call to each item
	int testPath( const wxStringVector& PathInParts, const DWORD& fileAttribs, bool SamePath=false, canSkippDirs_tracker* O_canSkippDirs=nullptr, fafikLib_regex::resultHashInfo* resultHashInfo_o=nullptr) const;

  //easy access
	 ///@return false =Skip, true =Copy
	 ///gives a bool response: Skip or Copy (probably not as exact as from `fafikLib_regexMultipleFiles`->testPath )
	bool testPath_copies( const wxStringVector& PathInParts, const DWORD& fileAttribs, bool SamePath=false, canSkippDirs_tracker* O_canSkippDirs=nullptr) const;

	inline void Set_CaseSensitive(bool CaseSensitiveMode) { type_caseSensitive= CaseSensitiveMode; }
	inline void Set_Include(bool includeMode) { type_isInclude= includeMode; }
	bool isInclude() const {return type_isInclude;}
	bool isExclude() const {return !type_isInclude;}
	bool isCaseSensitive() const {return type_caseSensitive;}
	bool isCaseInSensitive() const {return !type_caseSensitive;}
};

	///this class is to be used only for massive checking
class fafikLib_regexMultipleFiles {
 public:
 //def
	typedef wxVector<const fafikLib_regexVector_file*>::const_iterator const_iterator;

	typedef wxVector<fafikLib_regexVector_file::canSkippDirs_tracker> canSkippDirs_tr_PerPath;

 //storage
	 ///this storage will only read data without changing anything
	wxVector<const fafikLib_regexVector_file*> filesToProcess;
	bool runCleanupOnDelete= false;

 //functions and calls
	~fafikLib_regexMultipleFiles(){ if(runCleanupOnDelete)DeleteAll(); }
	fafikLib_regexMultipleFiles(bool selfDelete=false): runCleanupOnDelete(selfDelete) {}

	inline const_iterator begin() const {return filesToProcess.begin();}
	inline const_iterator end() const {return filesToProcess.end();}
	inline size_t size() const {return filesToProcess.size();}
	inline void Add_RuleFile_entry(const fafikLib_regexVector_file* ruleFileEntry) {filesToProcess.push_back(ruleFileEntry);}
	 ///@return false =Skip, true =Copy
	 ///gives a bool response: Skip or Copy
	bool testPath_doCopy( const wxStringVector& PathInParts, const DWORD& fileAttribs, bool SamePath=false, canSkippDirs_tr_PerPath* storageCanSkipp= nullptr, fafikLib_regex::resultHashInfo* resultHashInfo_o=nullptr) const;

	 ///its not automatic, you have to call it, or request it when creating the class
	size_t DeleteAll();
	inline size_t RemoveAll(){return DeleteAll();}
};




#endif // FAFIKLIB_REGEX_H
