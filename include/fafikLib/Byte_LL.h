#ifndef BYTE_LL_H
#define BYTE_LL_H

typedef unsigned char BYTE;

#include <wx/string.h>

///a rip-out of class
class FafikEC
{
 public:
	 ///plain static_cast
	 ///convert LL into Byte*[ 8? ],
	static BYTE* BytesP_fromLL( long long &from_LL );

		///get min required bytes to write this value
//	static size_t Bytes_fromLL_getSize( BYTE* byte_arr );
	static size_t Bytes_getSize_fromBp( BYTE* byte_arr );
		///auto Bytes_fromLL_getSize( BytesP_fromLL() )
	static size_t Bytes_getSize_fromLL( long long &from_LL ){
		return Bytes_getSize_fromBp( BytesP_fromLL(from_LL) );
	}

		///insert bytes*[ len ] into LL
	static long long Bytes_ToLL( BYTE *from_Byte, size_t len=8 );
		///plain static_cast
		///convert Byte*[ 8 ] into LL
	static long long* Bytes_ToPLL( BYTE *from_Byte );

		///@return string as if char[len]
	static wxString Bytes_ToCharString( BYTE *from_Byte, size_t len=8 );
		///@return Bytes_ToCharString( auto ... )
		///be warned that this is not safe to write normally
		///overload for auto
	static wxString Bytes_ToCharString( long long &from_LL ){
		BYTE *temp_B= BytesP_fromLL( from_LL );
		return Bytes_ToCharString( temp_B, Bytes_getSize_fromBp(temp_B) );
	}
		///@param get LL from string as if char[len]
	static long long Bytes_FromCharString_LL( wxString &str_ofLLChars );

};


struct bool8;
struct bool8b;
	///8 bool in 1Byte for space saving 8x
struct bool8 //: protected bool8b
{
	 ///set state of pos bit to state
	void setBit(BYTE pos, bool state=1){
		pos&= 7;		//trim it to 7 max
		BYTE iter= 1;
		iter<<= pos;	//shift that 1 to pos
		if(state) store|= iter;
		else store&= ~iter;
	}
	 ///set state of pos bit to state from the other.bit
	inline void copyBit(BYTE pos, const bool8& other){
		setBit(pos, other.getBit(pos) );
	}
	 ///alias setBit()
	inline void changeBit(BYTE pos=0, bool state=0){setBit(pos,state);}
	 ///get state of bit on pos
	bool getBit(BYTE pos=0)const{
		pos&= 7;
		BYTE iter= 1;
		iter<<= pos;
		return store& iter;
	}
	BYTE getValue()const{return store;}
	BYTE& getValueRef(){return store;}
	 ///reverse state of pos bit
	void xorBit(BYTE pos=0){
		pos&= 7;	//trim it to 7 max
		BYTE iter= 1;
		iter<<= pos;
		store^= iter;
	}
		///alias xorBit()
	inline void reverseBit(BYTE pos=0){xorBit(pos);}
		///sets state of pos bit to 0. alias setBit()
	inline void clearBit(BYTE pos=0){setBit(pos, 0);}
		///reset everything
	void clear(){store= 0;}

  ///constructor
	bool8(){}
	virtual ~bool8(){}
	bool8(const bool8& other):store(other.store){
//		store= other.store;
	};
	bool8(const bool8* other_p):store(other_p->store){
//		store= other_p->store;
	};
	bool8(const int& otherB):store(otherB){
//		store= otherB;
	};
	bool8(const BYTE& pos, bool state){
		setBit( pos, state );
	};
  ///forwarded operator
	bool8 operator>>(int pos)const{
		bool8 ret_val(this);
		ret_val.store>>= pos;
		return ret_val;
	}
	bool8 operator<<(int pos)const{
		bool8 ret_val(this);
		ret_val.store<<= pos;
		return ret_val;
	}
	bool operator==(const bool8& other)const{
		return store== other.store;
	}
	bool operator==(unsigned other)const{
		return store== other;
	}
	bool operator==(int other)const{
		return store== (unsigned)other;
	}
	bool operator!=(const bool8& other)const{
		return store!= other.store;
	}
	bool operator!=(unsigned other)const{
		return store!= other;
	}

	operator BYTE() const{ return (store);}	//This is the conversion operator
//	operator unsigned int() const{ return (store);}	//This is the conversion operator
  ///ease of access
	bool8& operator=(const bool8& other){
		store= other.store;
		return *this;
	}
	bool8& operator=( const int& other){
		store= other;
		return *this;
	}

	BYTE operator&(int hitMask)const{
		return store& hitMask;
	}
	bool8& operator&=(BYTE hitMask){
		store&= hitMask;
		return *this;
	}
	BYTE operator|(int hitMask)const{
		return store| hitMask;
	}
	bool8& operator|=(BYTE hitMask){
		store|= hitMask;
		return *this;
	}
	BYTE operator^(int hitMask)const{
		return store^ hitMask;
	}
	bool8& operator^=(BYTE hitMask){
		store^= hitMask;
		return *this;
	}

	///new on 2020-02-05
bool8b at(BYTE pos);
bool8b operator[](BYTE pos);
#ifndef _MSC_VER
const bool8b at(BYTE pos)const;
const bool8b operator[](BYTE pos) const;
#endif

 protected:
	BYTE store= 0;
//	friend class bool8b;
};

	///new on 2020-02-05
	///this is only a temporary class and gets invalid after changing parent
struct bool8b //: protected bool8
{
 public:
	bool8b (bool8* Ptr, BYTE Pos): storePtr(Ptr), storePos(Pos) {}
	~bool8b() {}
  ///basic calls R:S
	bool get() const {return storePtr->getBit(storePos);}
	bool8b& set(bool state) {storePtr->setBit(storePos, state); return *this; }
  ///ease of access
	operator bool() const{ return get();}	//This is the conversion operator
	bool8b& operator=(bool state){
		return set(state);
	}
	bool operator==(bool other)const{
		return get()== other;
	}

 protected:
		///this points to parent
 	bool8* storePtr= nullptr;
		///this specifies bit pos
	BYTE storePos= 0;
};


	///a very small implementation of bool8 array
struct bool8_arr{
	bool8* store= nullptr;
	virtual ~bool8_arr(){
		Free();
	}

	bool8_arr(){}
	bool8_arr(const size_t& newSize):store(new bool8[newSize]),arr_len(newSize){}

		///drops any allocated array, used when ctor didnt create one
	void sizeChange(const size_t& newSize){
		Free();
		arr_len= newSize;
		if(newSize) store= new bool8[newSize];	//0 means Free() only
	}
	size_t size()const{return arr_len;}

  ///operator
	bool8& operator[](const size_t& atPos){
		return at(atPos);
	}
	bool8& at(const size_t& atPos){
		return store[atPos];
	}



		///set state of pos bit to state
	void setBit(size_t pos=0, bool state=1){
		store[ pos/8 ].setBit( pos );	//will be trimmed to BYTE(7) inside
	}
		///alias setBit()
	inline void changeBit(size_t pos=0, bool state=0){setBit(pos,state);}
		///get state of bit on pos
	bool getBit(size_t pos=0)const{
		return store[ pos/8 ].getBit(pos);
	}
	bool8* getValue()const{return store;}
		///reverse state of pos bit
	void xorBit(size_t pos=0){
		store[ pos/8 ].xorBit(pos);
	}
		///alias xorBit()
	inline void reverseBit(size_t pos=0){xorBit(pos);}
		///sets state of pos bit to 0. alias setBit()
	inline void clearBit(size_t pos=0){setBit(pos, 0);}
		///reset everything
	void clear(){sizeChange(0);}



 protected:
	size_t arr_len= 0;
		///use sizeChange(0)
	void Free(){
		if(store) delete[] store;
		store= nullptr;
	}
};






#endif // BYTE_LL_H
