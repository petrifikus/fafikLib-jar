#ifndef FAFIKLIB_CRYPTO_H
#define FAFIKLIB_CRYPTO_H

#include "fafikLib_readLineByte.h"
#include "../FileType_MDFC.h"

#include <cstdio>
#include <string>
#include <windows.h>

enum encrFile_7_storesType{
	encrFile_7_stores_null,
		///if IO() works then this should be enough
	encrFile_7_stores_winLink_file,
	encrFile_7_stores_winLink_dir,
		///if IO() does NOT work, then this has to be used
	encrFile_7_stores_winSymLink_file,
	encrFile_7_stores_winHardLink_file,
	encrFile_7_stores_winSymLink_dir,
	encrFile_7_stores_winJunction_dir,
		///unimplemented
	encrFile_7_stores_win_NetworkShare,
};
enum encrFile_7_keySupplier: unsigned short{
	 //not predefined key, user enters it
	encrFile_7_keyNull,
	 //not predefined key, user enters it
	encrFile_7_keyNotProvided= encrFile_7_keyNull,
	 //not predefined key, user enters it
	encrFile_7_keyUserProvided= encrFile_7_keyNull,
  //predefined key managers
	encrFile_7_key_MDFC_symLink,
};


struct encrFile_7_struct_Header: public _FileType_MDFC_Header, _FileType_MDFC_additionalHeader{
  //data store
	struct struct_store{	//we want all that data as single continuous object(but just in case of compiler errors name the struct)
		 ///if key is auto provided this field should be non 0, @see encrFile_7_keySupplier
		encrFile_7_keySupplier keySupplier= encrFile_7_keyNull;
		 ///the hash
		BYTE checksum_F77sha_32_v1[4]= {0};
		BYTE keyValidator[8]= {0};

		 ///just an ease of access to set the data of `keyValidator`
		inline void set_keyValidator(const std::string& str8chars){if(str8chars.size()>=8){memcpy(keyValidator+0, &str8chars.at(0), 8);} }
		inline bool isKeySameAs(const BYTE* asOtherKey)const {return !memcmp(keyValidator+0, asOtherKey+0, 8);}
		inline bool isKeySameAs(const struct_store& other)const {return !memcmp(keyValidator+0, other.keyValidator+0, 8);}
		inline bool isKeySameAs(const std::string& str8chars)const {if(str8chars.size()<8)return false; return !memcmp(keyValidator+0, &str8chars.at(0), 8);}
		inline size_t keyValidator_size()const {return sizeof(keyValidator);}
	} store_preEncData;
  //ctor, dtor
	 encrFile_7_struct_Header(){init();}
	~encrFile_7_struct_Header(){}

  //functions
	void init(){
		 //head
		_FileType_MDFC_Header::storesType= _FileType_MDFC_st_encrypted_7;
		_FileType_MDFC_Header::checksumType= _FileType_MDFC_Checksum_F77sha_32_v1;
		_FileType_MDFC_Header::storesType_version= 2;
		 //head +
        _FileType_MDFC_additionalHeader::addHeadType= _FileType_MDFC_addHeadType_keyMDFCSymlink;
        store_preEncData.keySupplier= encrFile_7_key_MDFC_symLink;
		 //add head size
        additionalHeaderSize= sizeof(store_preEncData) + sizeof(_FileType_MDFC_additionalHeader);
	}
  //operators
	bool operator == (const encrFile_7_struct_Header& other){
		return ( _FileType_MDFC_Header::operator==(other) && addHeadType== other.addHeadType && 0== memcmp(this->__get_store_preEncData(), other.__get_store_preEncData(), sizeof(store_preEncData)) );
	}
  //ease of access functions
	 ///@return pos to first Byte of actual data (also size of complete Header+Hash)
//	inline size_t getDataStartPos()const { return sizeof(_FileType_MDFC_Header)+ additionalHeaderSize; }
	inline size_t getDataStartPos()const { return sizeof(encrFile_7_struct_Header); }
	inline BYTE* get_checksumP() {return store_preEncData.checksum_F77sha_32_v1 +0; }
	inline const BYTE* get_checksumP()const {return store_preEncData.checksum_F77sha_32_v1 +0; }
	inline unsigned int get_checksum()const { unsigned int retVal; memcpy(&retVal, this->get_checksumP(), 4); return retVal;}
	inline size_t get_checksum_size()const {return sizeof(store_preEncData.checksum_F77sha_32_v1); }
	inline BYTE* get_keyValidatorP() {return store_preEncData.keyValidator +0; }
	inline const BYTE* get_keyValidatorP()const {return store_preEncData.keyValidator +0; }
//	inline std::string get_CKeyValidator()const {return "fafikEnc";}
	inline size_t get_keyValidator_size()const {return sizeof(store_preEncData.keyValidator); }

	inline void set_checksum(const unsigned int& fromInt) { memcpy(this->get_checksumP(), &fromInt, 4 ); }
	inline void set_checksum(const BYTE* from4Bytes) { if(from4Bytes)memcpy(this->get_checksumP(), from4Bytes, 4 ); }
	inline void set_keyValidator(const BYTE* from8Bytes) { if(from8Bytes)memcpy(this->get_keyValidatorP(), from8Bytes, 8 ); }

 private:
 	BYTE* __get_store_preEncData() {return reinterpret_cast<BYTE*>(&store_preEncData);}
 	const BYTE* __get_store_preEncData()const {return reinterpret_cast<const BYTE*>(&store_preEncData);}
};
struct encrFile_7_struct_In{
	 ///this pieace of data is here to prevent reversing the encrypt effects
	unsigned short RandomDataSeed[4]= {0};
	 ///@see encrFile_7_storesType
	 ///determines what is stored
	unsigned int storesType= 0;
	 ///store the link times in order: C,A,W
	FILETIME fileTimes[3];
	 ///from start to +size
	size_t data_size= 0;
	 ///data start pos
	size_t data_startPos= 0;
};


class fafikLib_crypto {
 public:
		///TODO remove init()
	 fafikLib_crypto(){ _init(); }
	~fafikLib_crypto(){}

		///Encrypt data string (32-bit max 512MB)
		///@return 0 ok, 1 fail
	bool encryptData_7(LPCVOID dataIn, LPVOID dataOut, const size_t &dataSize, LPCVOID KeyIn, const size_t &KeySize);
	inline bool encryptData_7(LPCVOID dataIn, LPVOID dataOut, const size_t &dataSize, const std::string& KeyIn){
		return encryptData_7(dataIn, dataOut, dataSize, KeyIn.data(), KeyIn.size() );
	}
	inline bool encryptData_7(const std::string& dataIn, std::string& dataOut, const std::string& KeyIn){
		if(dataOut.size() > dataIn.size()){	//set over sized data to 0x00
			memset( (&dataOut.at(dataOut.size())), BYTE(0x00), (dataOut.size() -dataIn.size())-1 );
		}
		try{	//try to resize out buffer (as this can easily fail in 32-bit Exe)
			dataOut.resize(dataIn.size());
		} catch (...) {}
		if( dataOut.size()!= dataIn.size() ) return 1;
		return encryptData_7(dataIn.data(), &dataOut.at(0), dataIn.size(), KeyIn.data(), KeyIn.size() );
	}
		///Decrypt data string (32-bit max 512MB)
		///warning dataIn is NOT const, and will be partially changed.
		///@return 0 ok, 1 fail
	bool decryptData_7(LPCVOID dataIn, LPVOID dataOut, const size_t &dataSize, LPCVOID KeyIn, const size_t &KeySize);
	inline bool decryptData_7(LPCVOID dataIn, LPVOID dataOut, const size_t &dataSize, const std::string& KeyIn){
		return decryptData_7(dataIn, dataOut, dataSize, KeyIn.data(), KeyIn.size() );
	}
	inline bool decryptData_7(const std::string& dataIn, std::string& dataOut, const std::string& KeyIn){
		if(dataOut.size() > dataIn.size()){	//set over sized data to 0x00
			memset( (&dataOut.at(dataOut.size())), 0x00, (dataOut.size() -dataIn.size())-1 );
		}
		try{	//try to resize out buffer (as this can easily fail in 32-bit Exe)
			dataOut.resize(dataIn.size());
		} catch (...) {}
		if( dataOut.size()!= dataIn.size() ) return 1;
		return decryptData_7(&dataIn.at(0), &dataOut.at(0), dataIn.size(), KeyIn.data(), KeyIn.size() );
	}
 #ifdef FAFIKLIB_READLINEBYTE_H	//re-implement those functions using "fafikLib_readLineByte_base" streams for 64-bit && file support
		///warning output file will be slightly larger(up to 512 Bytes), but it might require a new cluster on disk
	bool encryptData_7(const fafikLib_readLineByte_base& StreamIn);
 #endif // FAFIKLIB_READLINEBYTE_H

		///F77sha_32_v1 = SHA256 based hasher by fafik77, 32-bit, v1
 	struct _v7_encodeKey_clas{
	 protected:
			///unteachable 2 positions
 		static const BYTE constPredef[16];
 		BYTE dataReshufler[4]= { 0x48, 0x87, 0x79, 0xFA };

 		wxFileOffset _totalDataSize= 0;
 		BYTE dataBuff_left[16]= {0};	//initialize the damn thing YOU IDIOT
 		size_t dataBuff_left_size= 0;
 		bool _b_finished= false;

 		inline BYTE ShiftR(const BYTE& inB, const BYTE& by) const;
		inline BYTE RotR(const BYTE& inB, const BYTE& by) const;
		inline BYTE CH(const BYTE& x, const BYTE& y, const BYTE& z) const;
		inline BYTE MAJ(const BYTE& x, const BYTE& y, const BYTE& z) const;
		inline BYTE F1(const BYTE& x) const;
		inline BYTE F2(const BYTE& x) const;
		inline BYTE F3(const BYTE& x) const;
		inline BYTE F4(const BYTE& x) const;

		void encode_inBlocks(const BYTE* dataIn, const size_t &BlocksAmount);
	 public:
	 	_v7_encodeKey_clas(){}
	 	~_v7_encodeKey_clas(){}

		void update(const BYTE* data_in, const size_t &dataSize);
		inline void update(const char* data_in, const size_t &dataSize){
			update(reinterpret_cast<const BYTE*>(data_in), dataSize);
		}
		void finish();
		inline bool isFinished()const{return _b_finished;}
//		void prep(const BYTE* data_in, const size_t &dataSize);
//		inline void prep(const char* data_in, const size_t &dataSize){
//			prep(reinterpret_cast<const BYTE*>(data_in), dataSize);
//		}
		inline size_t getResult()const{
			size_t retVal= 0;
			memcpy( &retVal, dataReshufler, 4);
			return retVal;
		}
		inline const BYTE* getResult_P()const{
			return dataReshufler;
		}
		inline bool compareChecksum(const size_t dataIn)const{
			return this->getResult()== dataIn;
		}
		inline bool compareChecksum(const BYTE* dataIn)const{
			return !memcmp(dataReshufler, dataIn, 4);
		}
 	};
 protected:
	 ///this struct stores Bytes of pass, also it has a safe dtor
 	struct encodedKey{
	  //def
		typedef BYTE* iterator;
		typedef const BYTE* const_iterator;
	  //data store
 		 ///an array that stores encryption pass bytes
		BYTE key[32]= {0};
	  //functions
		inline size_t size()const {return sizeof(key);}
		inline iterator begin() {return key+ 0; }
		inline iterator end() {return key+ size(); }
		BYTE& operator [] (const size_t& pos) {if(pos>=size())return key[31]; return key[pos]; }
		const BYTE& operator [] (const size_t& pos)const {if(pos>=size())return key[31]; return key[pos]; }
		~encodedKey(){ memset(key+0, 0x00, size() ); }
 	};
	void _init();
		//encode 32bit to 8bit
	BYTE shorten_32to8(const size_t &dataIn)const;

	void _v7_encodeKey(const BYTE* data_in, encodedKey& data_out, const size_t &dataSize, size_t &HashedOut);
	BYTE _v7_encodeKey_Sum(const BYTE* data_in, const size_t &dataSize);
	void _v7_shuffleKey(BYTE* dataIO_LP, const size_t &dataSize, const size_t &dataShuffle);


	size_t _v7_SuperSecretKey_gen(const size_t &dataSize, const BYTE* KeyEncodedIn, const size_t &KeySize, const BYTE& KeyEncodedSume);
	//size_t _v7_SuperSecretKey_gen(const size_t &dataSize, const BYTE* KeyEncodedIn, const size_t &KeySize, const size_t& KeyEncodedSume);
		///crypt part shuffle
	void _v7_shuffleData(BYTE* dataIO, const size_t &dataSize, const size_t& SuperSecret_key, bool reverseOp);
		///n+1<=>n		=0 should be most performant
	inline void _DataOp_MozaicData(BYTE* dataIO, const size_t &dataPosStart, const size_t &dataPosEnd);
		///last<=>first =1 deacent as first+n,half+n
	inline void _DataOp_ReverseData(BYTE* dataIO, const size_t &dataPosStart, const size_t &dataPosEnd);
		///each+char	=2 + and - is slow
	inline void _DataOp_AddData(BYTE* dataIO, const size_t &dataPosStart, const size_t &dataPosEnd, const BYTE& conAddData);
		///half<=>first, mirror effect	=3 jumping so much is super slow, as first=last
	inline void _DataOp_RotateData(BYTE* dataIO, const size_t &dataPosStart, const size_t &dataPosEnd);
//	inline void _DataOp_XorData(const BYTE& conXorData);

//		///shift<< as closed loop
//	inline void _DataOp_ShiftBitsData(bool side);

		///en-crypt part 1
	void _v7_encrData_p1(const BYTE* dataIn, BYTE* dataOut, const size_t &dataSize, encodedKey& KeyEncodedIn, const BYTE &SSKeyHashShort);
		///de-crypt part 1
	void _v7_decrData_p1(const BYTE* dataIn, BYTE* dataOut, const size_t &dataSize, encodedKey& KeyEncodedIn, const BYTE &SSKeyHashShort);
 #ifdef FAFIKLIB_READLINEBYTE_H	//re-implement those functions using "fafikLib_readLineByte_base" streams
 #endif // FAFIKLIB_READLINEBYTE_H

};

#endif // FAFIKLIB_CRYPTO_H
