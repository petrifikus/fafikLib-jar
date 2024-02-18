
2018-08-04
{
	specialEntry {
		link
		auto_key
		...
		comment{}
	}
	assign{ list of assign chars}
	table{
		enter{}
		exit{}
	}
	key_val{}	/	lineTable{ not yet inplemented 2018-08-04}
	bytes[in table: enter/exit/key_val ]{ "%n [s/l]" where s-0/l-1 sets (underwrite)overwrite_byte_type}
	recognitionPriority{ exit, enter, key_val }	//by default
	elementMaxDistanceApart= 1	//by default



	opensFiles={	///auto use * when opening file with * .extension
		///is caseInsensitive
		fafikLib_PRON	///only value matters, key can store type name
		json
	}
	||
	opensFiles=	""	///for `{}` or `""` works
}




"%#"	///means further processed table
described in DB as
[[
 pattern="<%k %#>"
 %#={
  pattern="%k=%u"	//%k -key, %u -unQuoteString
  ||	pattern="%k=%s"	//%k -key, %s -string
 }
 ends="</%k>"
]]

<Item name="Enable HASH" type="numeric">0</Item>


///for now wait until "Extract From Quotation mark"
XML	// https://www.w3schools.com/xml/tryit.asp?filename=tryxml_parsertest2
"<%k %#>"
	"%#"
"</%k>"
where
"%#" =
	"%k=%v ..."

///CSS without multi Line
CSS	// https://www.w3schools.com/xml/cd_catalog.txt
"%k{"
	"%k" ":" "%v" void";"
"}"

JSON	// http://json.org/example.html
"%k:{"
	"\"%k\"" ":" "%v" void","
"}"
||
"%k:["
	"\"%k\"" ":" "%v" void","
"]"

my	//	my format
"[%k]={"
	"%k" "=" "%v"
	||
	"[%k]={" "*am Bytes"
		"%v"
"}[%k]"


by default after 'table.enter' search will continue until finds 'table.leave'
specialEntry={
	///after such line, pointer to such object will be inserted to the next definition
!!!	!!!	///warning trying to delete by pointer non existing object
	copy="%copy%"	///memcopy previous defined entry
///	pointer="%pointer%"	///link	pointer to any field
	link="%link%"		///link	pointer to table
$	auto_key="%auto_key%"	///assign key = wxArray.pos
	link_file="%link_file%"	///like type_stores == 3
	link_DB && entry = "%link_DB_entry%"	///interpret other DB.entry

	into="%into%" ///2018-08-02 using [][][]finds append following object to specified itemEntry(yes it can not be a Table)
	into="%insertTo%"	///multiple names under one key

2018-07-21
$	vector.comment ="//"C || "#"Java || "--"LUA ///non interpretable line of lore
2018-08-09
//	at= "%at%" ///launches sub "interpretNextLine()" to edit requested entry
	at ///will work same as KeyNoRepeat=true
// TODO (main#9#08/13/18): implement KeyNoRepeat
	options.KeyNoRepeat	///check if in table exists such name, if so "itemEntry_getExistOrNew()"
}
table={	/// XML
 enter="<%k%#>"
 leave="</%k>"
// assign="="
}



make double assign for amount of Bytes..
..={*[1-8 short call bytes][Bytes unsigned long long]
binary content
}
[[	///binary assign
wxULongLong temp_ULL=-1;	///iterates from end
unsigned long long temp_ull=-2;	///iterates from beg
//BYTE *temp_char=static_cast<BYTE*>(temp_ULL.asArray());
BYTE *temp_char=static_cast<BYTE*>(static_cast<void*>(&temp_ull) );
for(int temp_i=0;temp_i<8;temp_i++)
{
	temp_char[temp_i] = temp_i;
	printf("%i,",temp_char[temp_i]);
}
printf("\n-------- that was ULL\n");
printf("ULL = %u\n",temp_ull);
]]
"%%"+... 	///removes 1st %, and skips reinterpretation
"%a 0" 		///%a followed by [number] stringsArr index	2018-06-12-19.07
"%u \" \" ..."	///char:(double or single quote __'\"' '\''), unused## char:(unQuote ? from __''), BYTE:(list of acceptable errors__(all by deff)) 0 1 2
"%S 0 -1"	///no long is used ## ///use int to subs string (from by/-to) __(0,-1)
//"%k %a \" \" \"\t\" "	///stringArr list of break chars escape() :: where "%a" changes to assign	__(%a)
"%k %a 0 space tab { }"	///stringArr list of break chars escape() :: where "%a" changes to assign	__(%a)
"%#"	2018-06-11-19.07
table.enter.{anywhere inside, use pointer}.bytes."*"."%n short/long"	///mode: short's' type assign(numbers >>ULL), long'l' assign short to assign long
 table.exit.{anywhere inside, use pointer}.bytes
where "%n s/l" is number(by sstrream>>string>>long) && mode:s/l
	{n_count=8}from end test if(BYTE[n] != 0){n_count--;}else break;
$	"%;"	///test if [table][exit] is at EOL
elementMaxDistanceApart= 1;		//after end1 +elementMaxDistanceApart to start2

v ?		void	is exact	(voids element)
e		element	is exact	(matches element)
key
value
bytes	optional
auto_key	allows for itemEntry.key = parent->subTable.size()
EOL "max size difference arg"		confirm End Of Line to accept

if(bigDB){
	create ".new" temp file when saving
}else{
	???
}

commentLine_begginsWith = {
	"//"
	"/*"
}

assign={	///possible table of strings
 "="
 using "%a 0" can specify stringsArr index
// unQuoteErr=0	///look at fafikLib_Q::extractNext()
}
key_value={
 "%u \" "	///char:(double or single quote '\"' '\''), char:(unQuote ? 'u'), BYTE:(list of acceptable errors(all by deff)) 0 1 2
 "%S 0 -0"	///use size_t to subs string (from by/-to) 0,0
// unQuoteErr=0
}
/// XML specific
 lineTable={
	"<"
	"%k"
	"%#"
	"/>";	///%k -unbroken string, %# -hash table
	%link%
	///for each dot "." before first "Quote" do up(to parent)
	."table"."#"
 }
table={	/// JSON 1
/// JSON 1
// key="%u"
// key="%S"	//will catch everything before %a
// unQuoteErr=0;
 enter={ ///collection of AND entries
// 	%auto_key%
 	key"%u"	///%u unQuoted
 	e"%a"	///%a -assign_sign	/// in e="". e -element
 	e{	///collection of OR subEntries in AND_entry[n]
 		bytes={///new idea location, 2018-07-23, unsure 2018-08-03
			e="[["
			e="%n"
			e="]]"
		}

 		e"{"	/// {}
 		auto_key"["
 	}
 	///declares to search for "*<%n>" at end, 0 -use direct number(ULL), 1 -use number of [1-8] bytes to make ULL number of bytes
	bytes {
 		e"*"
		  bytes {"[s/l]"} specify read method 'on the fly'
 		bytes"%n [s/l]"	specify read method 'constant'
 	}
 	?"%;"	///test if [table][exit] is at EOL

 }
 exit={
 	e{
	 e"}"
	 auto_key"]"
	}
	bytes={
		///2018-07-23-20.31 this way one can have file info and content in one
	}
	EOL	///enforce line check for full declaration
//	v
 }
// using %a{} means anything not matching the search goes to void, e.g "key = { " ... "}"


// exit=;
/// fafikLib_PRON
 enter={
	 e"["	/// in e="". e -element
	 key"%u"
	 //key"%S"	MFC
	 //key"\\"	MFC
	 e"]"
	 e"%a
	 e{
	 	e"{";	///%u unQuoted, %a -assign, {}
	 	auto_key"["
	 }
	bytes {
 		e"*"
 		e"%n 0"
	}
 }
 exit={
	//e""		MFC
	e{
		e"}["
		auto_key"]["
	}
	key"%u"
	e"]";
 }
/// XML
// enter="<%k%#>%S";	///%k -unbroken string, %# -hash table
 enter={///one match(to whole) per line
 	e"<"
 	key"%k"	///any declaration beginning with "%" is special
 	hash"%#"
 	e">"
 	value"%S";	///%k -unbroken string, %# -hash table
 }
  "#"={
  	///"#" will insert into subtable("#")
   key="%k"	///%k -unbroken string ( till blank char || EndOfLine && NOT assign char(reason = its a string) )
   unQuoteErr = ?
   assign="="
   value="%u 1"	///needs version of ["extract unescaped"] ...
   value="%u 0"	///and get content ["extract without start,end(\") "]
   unQuoteErr=0
  }
 exit=
 e"</"
 key"%k"
 e">";
// findEndFirst= 1;

// assign="="

}
opensFiles={	///auto use * when opening file with * .extension
	fafikLib_PRON
	json
}


//table=
//{	/// JSON 2
// enter="["
// exit="]"
//// assign="="
//}


{//MFC
	assign={
		"|"
	}

	table={
		enter={
			key"%S"		//folder
			key"/"		//folder end"/"
			value"$S"	//new: exclude"e", or include"i"
		}
		exit={
			e{
				""
				"."
				".."
			}
		}
	}

	key_value={
		key"%S"		//file
		e"\\"
		value"$S"	//new: exclude"e", or include"i"
	}


//[[ 2018-07-24-00.04
	priority=[///define priority
		(specialEntry)
		exit
		key
		enter
	]

	SpaceChar=" "	///depth * SpaceChar at lineBegin, note valid are { ""," ", "\t" }
	SpaceConvertToTab="4"	///amount of SpaceChar to change into "\t"
	uninterpretableLineEndsAs= "null", "lore",

//]]
}

file type "ini"{
[table key]
values=%S
!!((EOL))

comment={ ";" }
}


//auto_key{/// bad idea,: not automatic
// key_value={
//
// }
//
// table{
//
// }
//}

search normal if(fail && auto_key) redo scan from after "%a <i>"

