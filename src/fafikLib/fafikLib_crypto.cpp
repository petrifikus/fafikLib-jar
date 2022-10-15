#include "fafikLib_crypto.h"

bool fafikLib_crypto::encryptData_7(LPCVOID dataIn, LPVOID dataOut, const size_t &dataSize, LPCVOID KeyIn, const size_t &KeySize)
{
		//dont accept errors, also dont accept 1 char (could be exploited)
	if(!dataSize || dataSize== size_t(-1) || dataSize== size_t(1)) return 1;
	if(!KeySize || KeySize== size_t(-1)) return 1;
	if( dataIn==NULL || dataOut==NULL || KeyIn==NULL ) return 1;
		//encode key to pass further
	size_t keyEncoded_hash;
	BYTE keyEncoded_hashShort;
	 ///the actual key to (de)en-code with
	encodedKey keyEncoded_Const;
//	BYTE keyEncoded_Const[KeySize];
	_v7_encodeKey(static_cast<const BYTE*>(KeyIn), keyEncoded_Const, KeySize, keyEncoded_hash);
	keyEncoded_hashShort= shorten_32to8(keyEncoded_hash);
//printf("keyEncoded_hash= %u\n", keyEncoded_hash);

	//use keyEncoded_hashShort instead
//	BYTE keyEncodedSum_Const= _v7_encodeKey_Sum(keyEncoded_Const, KeySize);
//printf("keyEncodedSum_Const = %u\n", keyEncodedSum_Const);
//printf("keyEncoded_Const = %s\n", keyEncoded_Const);
		//make some number
	size_t keyEncoded_SSKey= _v7_SuperSecretKey_gen(dataSize, keyEncoded_Const.key, keyEncoded_Const.size(), keyEncoded_hashShort);
	BYTE keyEncoded_SSKeyShort= shorten_32to8( keyEncoded_hash+keyEncoded_hashShort+ keyEncoded_SSKey);
//printf("encrypt keyEncoded_SSKey = %u, %02x, hs=%02x\n",keyEncoded_SSKey,keyEncoded_SSKey,keyEncoded_hashShort);
//	printf("Lib KeySS= %u\t %02x\n", keyEncoded_SS, keyEncoded_SS);
		//encrypt
	_v7_encrData_p1(
		static_cast<const BYTE*>(dataIn), //data in
		static_cast<BYTE*>(dataOut), //data out
		dataSize, //data size
		keyEncoded_Const, //Key Encoded
//		KeySize, //Key Encoded size
		keyEncoded_SSKeyShort //SS Key Hash Short
	);
	_v7_shuffleData(static_cast<BYTE*>(dataOut), dataSize, keyEncoded_SSKey, false);

  //make sure to destroy sensitive data
	keyEncoded_hash= 0;
	keyEncoded_hashShort= 0x00;
	keyEncoded_SSKey= 0;
	keyEncoded_SSKeyShort= 0x00;
 return 0;
}
bool fafikLib_crypto::decryptData_7(LPCVOID dataIn, LPVOID dataOut, const size_t &dataSize, LPCVOID KeyIn, const size_t &KeySize)
{
		//dont accept errors
	if(!dataSize || dataSize== size_t(-1)) return 1;
	if(!KeySize || KeySize== size_t(-1)) return 1;
	if( dataIn==NULL || dataOut==NULL || KeyIn==NULL ) return 1;
		//encode key to pass further
	size_t keyEncoded_hash;
	BYTE keyEncoded_hashShort;
	 ///the actual key to (de)en-code with
	encodedKey keyEncoded_Const;
//	BYTE keyEncoded_Const[KeySize];
	_v7_encodeKey(static_cast<const BYTE*>(KeyIn), keyEncoded_Const, KeySize, keyEncoded_hash);
	keyEncoded_hashShort= shorten_32to8(keyEncoded_hash);
//printf("keyEncodedSum_Const = %u\n", keyEncodedSum_Const);
//printf("keyEncoded_Const = %s\n", keyEncoded_Const);
		//make some number
	size_t keyEncoded_SSKey= _v7_SuperSecretKey_gen(dataSize, keyEncoded_Const.key, keyEncoded_Const.size(), keyEncoded_hashShort);
	BYTE keyEncoded_SSKeyShort= shorten_32to8( keyEncoded_hash+keyEncoded_hashShort+ keyEncoded_SSKey);
//printf("decrypt keyEncoded_SSKey = %u, %02x, hs=%02x\n",keyEncoded_SSKey,keyEncoded_SSKey,keyEncoded_hashShort);
		// decrypt
		//copy In to Out buffer
	memcpy(dataOut, dataIn, dataSize);
//printf("comp in==out? =%u\n", memcmp(dataOut, dataIn, dataSize) );

		//
	_v7_shuffleData(static_cast<BYTE*>(dataOut), dataSize, keyEncoded_SSKey, true);

	_v7_decrData_p1(
		static_cast<const BYTE*>(dataOut), //data in
		static_cast<BYTE*>(dataOut), //data out
		dataSize, //data size
		keyEncoded_Const, //Key Encoded
//		KeySize, //Key Encoded size
		keyEncoded_SSKeyShort //SS Key Hash Short
	);
		//make sure to destroy sensitive data
	keyEncoded_hash= 0;
	keyEncoded_hashShort= 0x00;
	keyEncoded_SSKey= 0;
	keyEncoded_SSKeyShort= 0x00;
 return 0;
}


const BYTE fafikLib_crypto::_v7_encodeKey_clas::constPredef[16]={
	0x1a, 0x23, 0x05, 0x73,
	0xf2, 0xfa, 0x3c, 0x5b,
	0x4a, 0x76, 0xf4, 0x68,
	0x92, 0x81, 0x41, 0xe5
};
BYTE fafikLib_crypto::_v7_encodeKey_clas::ShiftR(const BYTE& inB, const BYTE& by) const {
	return inB>> by;
}
BYTE fafikLib_crypto::_v7_encodeKey_clas::RotR(const BYTE& inB, const BYTE& by) const {
	return (inB>> by) | (inB<< (8- by));
}
BYTE fafikLib_crypto::_v7_encodeKey_clas::CH(const BYTE& x, const BYTE& y, const BYTE& z) const {
	return (x & y) ^ (~x & z);
}
BYTE fafikLib_crypto::_v7_encodeKey_clas::MAJ(const BYTE& x, const BYTE& y, const BYTE& z) const {
	return (x & y) ^ (x & z) ^ (y & z);
}
BYTE fafikLib_crypto::_v7_encodeKey_clas::F1(const BYTE& x) const {
	return (RotR(x, 2) ^ RotR(x, 5) ^ RotR(x, 6));
}
BYTE fafikLib_crypto::_v7_encodeKey_clas::F2(const BYTE& x) const {
	return (RotR(x, 6) ^ RotR(x, 3) ^ RotR(x, 1));
}
BYTE fafikLib_crypto::_v7_encodeKey_clas::F3(const BYTE& x) const {
	return (RotR(x, 7) ^ RotR(x, 2) ^ RotR(x, 3));
//	return (RotR(x, 1) ^ RotR(x, 4) ^ RotR(x, 3));
}
BYTE fafikLib_crypto::_v7_encodeKey_clas::F4(const BYTE& x) const {
	return (RotR(x, 1) ^ RotR(x, 3) ^ RotR(x, 2));
}

	///1 Block = 16 Bytes
void fafikLib_crypto::_v7_encodeKey_clas::encode_inBlocks(const BYTE* dataIn, const size_t &BlocksAmount)
{
	BYTE dataWrite[48];
	BYTE dataReshuflerBlock[4];
	size_t temp1, temp2;
		//a pointer to data start in block
	const BYTE *dataPortionP= dataIn+ 0;

	for(size_t loopBlockIter= 0; loopBlockIter!= BlocksAmount; ++loopBlockIter){
		memcpy(dataWrite, dataPortionP, 16);	//we need writable data
		for(size_t iterByte= 16; iterByte!= 48; ++iterByte){	//loop through each byte in block
			dataWrite[iterByte]= F4( dataWrite[iterByte- 2]+ dataWrite[iterByte- 7]+ F3(dataWrite[iterByte- 15]) )+ dataWrite[iterByte- 16] ;
		}
		for(size_t sss=0; sss!=4; ++sss){	//copy inter block data to this block
			dataReshuflerBlock[sss]= dataReshufler[sss];
		}

		for(size_t iterByte= 0; iterByte!= 48; ++iterByte){	//do some expensive calculations
			temp1= dataReshuflerBlock[3]+ F2(dataReshuflerBlock[2])+
					CH(dataReshuflerBlock[1], dataReshuflerBlock[2], dataReshuflerBlock[3])+
					constPredef[iterByte&15]+ dataWrite[iterByte];
			temp2= F1(dataReshuflerBlock[0])+ MAJ( dataReshuflerBlock[0], dataReshuflerBlock[1], dataReshuflerBlock[2] );

			dataReshuflerBlock[3]= dataReshuflerBlock[2];
			dataReshuflerBlock[2]= dataReshuflerBlock[1]+ temp1;
			dataReshuflerBlock[1]= dataReshuflerBlock[0];
			dataReshuflerBlock[0]= temp1+ temp2;
		}

		for(size_t sss=0; sss!=4; ++sss){	//copy(+add) this block data to inter block
			dataReshufler[sss]+= dataReshuflerBlock[sss];
		}
		dataPortionP+= 16;	//increase data pointer to next block
	}
		//make sure to destroy sensitive data
	memset(dataWrite, 0x00, sizeof(dataWrite));
	memset(dataReshuflerBlock, 0x00, 4);
	temp1= temp2= 0;
	dataPortionP= NULL;
}
void fafikLib_crypto::_v7_encodeKey_clas::update(const BYTE* dataIn, const size_t &dataSize)
{
	if(_b_finished) return;
	if(dataIn==NULL || dataSize==0) return;

	_totalDataSize+= dataSize;
		//calc data left to copy
	size_t dataSize_toCopy= 16- dataBuff_left_size;
	size_t dataSize_lastPos= 0;
		//check if data curr size can support that
	if(dataSize_toCopy> dataSize) dataSize_toCopy= dataSize;
		//not that it matters but ok (limit the memcpy)
	if(dataBuff_left_size== 0 && dataSize>= 16) dataSize_toCopy= 0;
	if(dataSize_toCopy!= 0){
			//copy that tiny part of data to buffer
		memcpy(dataBuff_left+ dataBuff_left_size, dataIn+ dataSize_lastPos, dataSize_toCopy);
		dataSize_lastPos+= dataSize_toCopy;
		dataBuff_left_size+= dataSize_toCopy;
	}
		//encode that part of data
	if(dataBuff_left_size== 16){
		encode_inBlocks(dataBuff_left, 1);
		dataBuff_left_size= 0;
	}
	size_t dataSize_leftToEncode= dataSize- dataSize_toCopy;
	size_t dataSize_Blocks= dataSize_leftToEncode/ 16 ;
		//encode proper data
	if(dataSize_Blocks){
		encode_inBlocks(dataIn+ dataSize_lastPos, dataSize_Blocks);
		dataSize_lastPos+= dataSize_Blocks* 16;
	}
		//we have some data left, copy to buffer
	if(dataSize_lastPos!= dataSize){
		size_t dataSize_leftToStore= dataSize- dataSize_lastPos;
		memcpy(dataBuff_left+ dataBuff_left_size, dataIn+ dataSize_lastPos, dataSize_leftToStore);
	}
}
void fafikLib_crypto::_v7_encodeKey_clas::finish()
{
	if(_b_finished) return;
	_b_finished= true;

//	size_t dataBuff_left_size

	if(dataBuff_left_size){ //if we have some unfinished data
		for(size_t tempIterPos= dataBuff_left_size; tempIterPos!= 16; ++tempIterPos){
			dataBuff_left[tempIterPos]= 0x10;
		}
		if(dataBuff_left_size<16){
			dataBuff_left[dataBuff_left_size]= 0x5D;	//set the char just after the proper data
			dataBuff_left[15]= 0x5D; //set the last char
		}
		encode_inBlocks(dataBuff_left, 1);
	}
		//make sure to destroy sensitive data
	memset(dataBuff_left, 0x00, sizeof(dataBuff_left));
	dataBuff_left_size= 0;
}
void fafikLib_crypto::_init(){
}
BYTE fafikLib_crypto::shorten_32to8(const size_t &dataIn)const
{
//	const BYTE *Data_B= static_cast<const BYTE*>(static_cast<const void*>(&dataIn));
	const BYTE *Data_B= reinterpret_cast<const BYTE*>(&dataIn);
	BYTE retVal= dataIn;
	for(size_t iterPos= 0; iterPos!=2; ++iterPos){
		retVal+= Data_B[iterPos]^ (Data_B[iterPos+2]<<3)| (Data_B[iterPos+1]>>3);
	}

return retVal;
}
void fafikLib_crypto::_v7_encodeKey(const BYTE* data_in, encodedKey& data_out, const size_t& dataSize, size_t& HashedOut){
	size_t LenMost= dataSize;
	if(LenMost> data_out.size()) LenMost= data_out.size();
	 //copy key from in to out
	memcpy(data_out.key+0, data_in, LenMost);
	 // [start the transformation
	_v7_encodeKey_clas keyEncoder;
	keyEncoder.update(data_in, dataSize);
	keyEncoder.finish();
	HashedOut= keyEncoder.getResult();
	const BYTE* encoded_hash_B= keyEncoder.getResult_P();
	const BYTE encoded_hashShort= shorten_32to8(HashedOut);
	BYTE addSeed= encoded_hash_B[dataSize&3];

	for(size_t tempU= LenMost; tempU!= (LenMost+4); ++tempU){
		data_out[tempU% data_out.size()] += encoded_hash_B[(tempU-LenMost)&3]; //add some hash into the mix (after the input key looped back)
	}
//	for(size_t dataAtPos= 0; dataAtPos!= dataSize; ++dataAtPos){
//		data_out[dataAtPos% data_out.size()] ^= (data_in[dataAtPos]^ encoded_hash_B[dataAtPos&3])+ (data_in[dataAtPos]+ ~encoded_hashShort)+ encoded_hash_B[(dataSize-dataAtPos)&3]+ addSeed;
//		addSeed+= data_out[dataAtPos% data_out.size()];
//	}
	size_t dataAt_rev= data_out.size();
	for(size_t dataAtPos= 0; dataAtPos!= data_out.size(); ++dataAtPos){
		--dataAt_rev; //update the reverse position
		 ///assign name to `data_out[dataAtPos]`
		const BYTE& kCMap= data_out[dataAtPos];
		const BYTE& krCMap= data_out[dataAt_rev];
		data_out[dataAtPos]=
			((kCMap^ encoded_hash_B[dataAtPos&3])^ ((krCMap^(kCMap&addSeed))+ ~(encoded_hashShort+addSeed) )) ^ (encoded_hash_B[(dataAtPos)&3]+ (addSeed&0xBA));
		addSeed+= data_out[dataAtPos];
	}
  // ]finish the transformation
	_v7_encodeKey_clas keyEncoder_fin;
	keyEncoder_fin.update(data_out.key+ 0, data_out.size() );
	keyEncoder_fin.finish();
	HashedOut= keyEncoder_fin.getResult();
  //make sure to destroy sensitive data
	encoded_hash_B= NULL;
	addSeed= 0;
}

BYTE fafikLib_crypto::_v7_encodeKey_Sum(const BYTE* dataIn, const size_t &dataSize)
{
	if(!dataSize || dataSize== size_t(-1) ) return 0;
		//cast to type
	const BYTE *dataInLast= dataIn+ dataSize-1;
	BYTE retSum= dataSize * dataIn[0];
	for(const BYTE *dataInIter= dataIn; dataInIter!=dataInLast; ++dataInIter){
		retSum= ((retSum^(*dataInIter)) &~(*dataInIter)) ^ ~(retSum+(*dataInIter));
	}
		//make sure to destroy sensitive data
	dataInLast= NULL;
return retSum;
}
	///rotate the key (somewhat unpredictable)
void fafikLib_crypto::_v7_shuffleKey(BYTE* dataIO, const size_t &dataSize, const size_t &dataShuffle)
{
	if(!dataSize || dataSize== size_t(-1) || dataSize== size_t(1)) return;
		//cast to type
		//make
	BYTE FirstChar= dataIO[1];
	BYTE dataIO_Copy[dataSize];
	memcpy(dataIO_Copy, dataIO, dataSize);

	const size_t reofset= (dataShuffle +1 +bool(FirstChar&3)) %dataSize;
	size_t reinterpretPos= 0;
	for(size_t startPos=0; startPos!=dataSize; ++startPos){
		reinterpretPos= reofset + startPos;
		if(reinterpretPos>=dataSize) reinterpretPos-= dataSize;
		dataIO[startPos]= dataIO_Copy[reinterpretPos];
	}
		//make sure to destroy sensitive data
	FirstChar= 0x00;
	memset(dataIO_Copy, 0x00, dataSize );
	reinterpretPos= 0;
}
	///do some magic to make the data even more unpredictable
size_t fafikLib_crypto::_v7_SuperSecretKey_gen(const size_t &dataSize, const BYTE* KeyEncodedIn, const size_t &KeySize, const BYTE& KeyEncodedSume)
{
	size_t generatedKeySpliter= 17272;
		//cast to type
//	BYTE* generatedKeySpliter_Byte= static_cast<BYTE*>(static_cast<void*>(&generatedKeySpliter));
	BYTE* generatedKeySpliter_Byte= reinterpret_cast<BYTE*>(&generatedKeySpliter);
		//make data
	const BYTE *c_iter= KeyEncodedIn;
	const BYTE *c_last= c_iter+ KeySize;

	while(c_iter!= c_last){
		generatedKeySpliter+= *c_iter;
		generatedKeySpliter_Byte[0]^= (*c_iter)&(dataSize&59);
		if(dataSize>12 && dataSize<61) generatedKeySpliter_Byte[(*c_iter)&3]+= (*c_iter)&(dataSize&84);
		if(dataSize>15 && dataSize<54) generatedKeySpliter_Byte[1]^= (*c_iter)&(dataSize&145);
		if(dataSize>58) generatedKeySpliter_Byte[(*c_iter)&3]|= dataSize&231;
		if(generatedKeySpliter_Byte[(dataSize&0xAD)&3] &16){
			generatedKeySpliter_Byte[generatedKeySpliter&3]+= dataSize;
			generatedKeySpliter_Byte[generatedKeySpliter&3]^= ~(*c_iter)&(dataSize&111);
		}
		generatedKeySpliter_Byte[generatedKeySpliter&3]+= KeyEncodedSume;
		++c_iter;
	}

	generatedKeySpliter<<= dataSize& 2;
	generatedKeySpliter>>= bool(generatedKeySpliter& 12);
	generatedKeySpliter|= dataSize;
		//make sure to destroy sensitive data
	generatedKeySpliter_Byte= NULL;
	c_iter= NULL;
	c_last= NULL;
return generatedKeySpliter;
}
void fafikLib_crypto::_v7_shuffleData(BYTE* dataIO, const size_t &dataSize, const size_t& SuperSecret_key, bool reverseOp)
{
	if(!dataSize || dataSize== size_t(-1)) return;
		//cast to type
//	size_t SuperSecret_keyCopy= SuperSecret_key;
//	const BYTE* SuperSecret_key_BA= static_cast<const BYTE*>(static_cast<const void*>(&SuperSecret_keyCopy));
	const BYTE* SuperSecret_key_BA= reinterpret_cast<const BYTE*>(&SuperSecret_key);
		//make data
	size_t onozero_counter= SuperSecret_key;
	size_t onozero_counter_count= 0;
	size_t onozero_counter_lastAt= 0;
	size_t operationList_size1[4]= {(size_t) -1};
	size_t operationList_size2[4]= {(size_t) -1};
	BYTE operationList_data[4]= {(BYTE) -1};
	BYTE operationList[4]= {(BYTE) -1};
	BYTE operationList_size= 0;
	bool shuffleHitsPos0= false;
	size_t shuffleHits_smallest_index= 0;
	size_t shuffleHits_smallestDstc= (size_t) -1;
	size_t shuffleHits_smallestDstc_temp= 0;

	for(size_t CountPos=0; CountPos!=32; ++CountPos){	//count 1s or 0s
		if(onozero_counter&1){
			++onozero_counter_count;
			onozero_counter_lastAt= CountPos;	//save highest pos of 1|0 bit
		}
		onozero_counter>>= 1;	//move to the next bit
		if(!onozero_counter) break; //fast exit
	}
	operationList_size= onozero_counter_lastAt/8;

	onozero_counter= SuperSecret_key & dataSize;	//maybe we can interpolate some size
	if(!onozero_counter)	//i rely need some size
		onozero_counter= onozero_counter_count% dataSize;
	for(BYTE operAssIter= 0; operAssIter!= operationList_size; ++operAssIter){
//		printf("operationList_size= %u\n", operationList_size);
		operationList[operAssIter]= (SuperSecret_key_BA[operAssIter] ^ onozero_counter) &3;	//extract 3 as there are 3 methods
		operationList_size1[operAssIter]= (onozero_counter ^ SuperSecret_key_BA[operAssIter]) %dataSize;
		operationList_size2[operAssIter]= ~operationList_size1[operAssIter] + (dataSize^ (SuperSecret_key_BA[operAssIter]));
		operationList_data[operAssIter]= operationList_size1[operAssIter]- (SuperSecret_key_BA[operAssIter] ^ onozero_counter);	//extract 3 as there are 3 methods

		if(operationList_size2[operAssIter]> dataSize) operationList_size2[operAssIter]= dataSize-1; //cant point outside of dataSize
		if(operationList_size1[operAssIter]> operationList_size2[operAssIter]) std::swap(operationList_size1[operAssIter], operationList_size2[operAssIter]); //cant be 2 1, has to be 1 2
		if(!operationList_size1[operAssIter]) shuffleHitsPos0= true;
		shuffleHits_smallestDstc_temp= operationList_size2[operAssIter]- operationList_size1[operAssIter];
		if(shuffleHits_smallestDstc_temp< shuffleHits_smallestDstc){
			shuffleHits_smallestDstc= shuffleHits_smallestDstc_temp;
			shuffleHits_smallest_index= operAssIter;
		}
	}
	if(!shuffleHitsPos0){ //make sure that its shuffled from 0
		operationList_size1[shuffleHits_smallest_index]= 0;
	}

//	operationList[2] = 3;
//	for(BYTE operAssIter= 0; operAssIter!= operationList_size; ++operAssIter){
//		printf(" Oper(%u, %u, %u, + %u)\n", size_t(operationList[operAssIter]), operationList_size1[operAssIter], operationList_size2[operAssIter], operationList_data[operAssIter]);
//	}

	BYTE truePosOn= 0;
	for(BYTE operAssIter= 0; operAssIter!= operationList_size; ++operAssIter){
		if(reverseOp)	truePosOn= operationList_size- operAssIter-1;
		else			truePosOn= operAssIter;

		switch (operationList[truePosOn])
		{
		case BYTE(0):
			_DataOp_MozaicData(dataIO, operationList_size1[truePosOn], operationList_size2[truePosOn]);
		break;
		case BYTE(1):
			_DataOp_ReverseData(dataIO, operationList_size1[truePosOn], operationList_size2[truePosOn] );
		break;
		case BYTE(2):	///I need Better KeyEncode (this one treats 'Yy' as almost the same)
			_DataOp_AddData(dataIO, operationList_size1[truePosOn], operationList_size2[truePosOn], operationList_data[truePosOn]* (reverseOp? BYTE(-1): 1) );
		break;
		case BYTE(3):
			_DataOp_RotateData(dataIO, operationList_size1[truePosOn], operationList_size2[truePosOn] );
		break;

		default:
		break;
		}
	}
		//make sure to destroy sensitive data
	SuperSecret_key_BA= NULL;
	onozero_counter= 0;
	onozero_counter_count= 0;
	onozero_counter_lastAt= 0;
	for(int tempReseter= 0; tempReseter!= 4; ++tempReseter){
		operationList_size1[tempReseter]= 0;
		operationList_size2[tempReseter]= 0;
		operationList_data[tempReseter]= 0;
		operationList[tempReseter]= 0;
	}
	operationList_size= 0;
	shuffleHitsPos0= 0;
	shuffleHits_smallest_index= 0;
	shuffleHits_smallestDstc= 0;
	shuffleHits_smallestDstc_temp= 0;
	truePosOn= 0;
}
void fafikLib_crypto::_DataOp_MozaicData(BYTE* dataIO, const size_t &dataPosStart, const size_t &dataPosEnd) //0
{
	size_t dataDiffSize= dataPosEnd- dataPosStart;
	dataDiffSize&= ~size_t(1); //loose the 1, num&(~1)
	if(!dataDiffSize || dataDiffSize== 1) return;

	BYTE* dataN= dataIO+ dataPosStart;
	BYTE* dataN_end= dataN+ dataDiffSize;

	BYTE dataN_saved= 0;
	while(dataN != dataN_end){
		dataN_saved= (*dataN);
		(*dataN)= *(dataN+1);
		(*(dataN+1))= dataN_saved;
//		if(reversed){	//Xor
//			dataN_saved= (*dataN+1);
//			(*(dataN+1))^= *(dataN);
//			(*dataN)^= dataN_saved;
//		} else {
//			dataN_saved= (*dataN);
//			(*dataN)^= *(dataN+1);
//			(*(dataN+1))^= dataN_saved;
//		}
		dataN+= 2;
	}
		//make sure to destroy sensitive data
	dataDiffSize= 0;
	dataN= NULL;
	dataN_end= NULL;
	dataN_saved= 0;
}
void fafikLib_crypto::_DataOp_ReverseData(BYTE* dataIO, const size_t &dataPosStart, const size_t &dataPosEnd) //1
{
	size_t dataDiffSize= dataPosEnd- dataPosStart;
	dataDiffSize&= ~size_t(1); //loose the 1, num&(~1)
	if(!dataDiffSize || dataDiffSize== 1) return;

	BYTE* dataN= dataIO+ dataPosStart;
	BYTE* dataNH= dataN+ dataDiffSize/2;
	BYTE* dataN_end= dataN+ dataDiffSize;

	BYTE dataN_saved= 0;
	while(dataNH!= dataN_end){
		dataN_saved= *dataN;
		*dataN= *dataNH;
		*dataNH= dataN_saved;

		++dataN;
		++dataNH;
	}
		//make sure to destroy sensitive data
	dataDiffSize= 0;
	dataN= NULL;
	dataNH= NULL;
	dataN_end= NULL;
	dataN_saved= 0;
}
void fafikLib_crypto::_DataOp_AddData(BYTE* dataIO, const size_t &dataPosStart, const size_t &dataPosEnd, const BYTE& conAddData) //2
{
	BYTE* dataIterPos= dataIO+ dataPosStart;
	BYTE* dataIter_end= dataIO+ dataPosEnd;

	while(dataIterPos!= dataIter_end){
		(*dataIterPos++)+= conAddData;
	}
		//make sure to destroy sensitive data
	dataIterPos= NULL;
	dataIter_end= NULL;
}
void fafikLib_crypto::_DataOp_RotateData(BYTE* dataIO, const size_t &dataPosStart, const size_t &dataPosEnd) //3
{
	size_t dataDiffSize= dataPosEnd- dataPosStart;
	if(!dataDiffSize || dataDiffSize== 1) return;

	BYTE* dataF= dataIO+ dataPosStart;
	BYTE* dataL= dataIO+ dataPosEnd;

	BYTE dataN_saved= 0;
	while(dataDiffSize> 1){
		dataN_saved= *dataF;
		*dataF= *dataL;
		*dataL= dataN_saved;

		++dataF;
		--dataL;
		dataDiffSize-= 2;
	}
		//make sure to destroy sensitive data
	dataDiffSize= 0;
	dataF= NULL;
	dataL= NULL;
	dataN_saved= 0;
}



	///@param dataIn.size == dataOut.size, otherwise error
void fafikLib_crypto::_v7_encrData_p1(const BYTE *dataIn_B, BYTE *dataOut_B, const size_t &dataSize, encodedKey& KeyEncodedIn, const BYTE &SSKeyHashShort)
{
	if(!dataSize || dataSize== size_t(-1)) return;
		//give it a type(unsigned), otherwise any operation is invalid
		//key
	size_t dataCurrKey_shufS= 0;
//	encodedKey dataCurrKeyShuffler= KeyEncodedIn;
	size_t KeySize= KeyEncodedIn.size();
//	BYTE dataCurrKeyShuffler[KeySize];
//	memcpy(dataCurrKeyShuffler, dataCurrKey, KeySize);
	BYTE* dataCurrKey= KeyEncodedIn.begin();
	_v7_shuffleKey(KeyEncodedIn.begin(), KeySize, dataSize&1);
		//more self keys
	BYTE dataCurrKey_TopAds= SSKeyHashShort;
	BYTE dataCurrKey_ads= _v7_encodeKey_Sum(KeyEncodedIn.begin(), KeySize);
	size_t dataCurrKey_pos= 0;
		//encrypt
	size_t dataAtPos= 0;
	for(; dataAtPos!=dataSize; ++dataAtPos){	///crypt data
//		dataOut_B[dataAtPos]= (dataIn_B[dataAtPos] + *dataCurrKey) + (*dataCurrKey_ads ^ *dataCurrKey);
//		dataOut_B[dataAtPos]= dataIn_B[dataAtPos] + (*dataCurrKey ^ *dataCurrKey_ads ^ *dataCurrKey_preAds);
		dataOut_B[dataAtPos]= ((dataIn_B[dataAtPos] ^ *dataCurrKey) ^~dataCurrKey_TopAds) ;

//		dataCurrKey_ads^= (dataCurrKey_TopAds)+ 0xCF;
//		dataCurrKey_TopAds= ((~dataOut_B[dataAtPos])& dataCurrKey_ads)^ dataCurrKey_TopAds;
		dataCurrKey_ads+= 0x5f;
		dataCurrKey_TopAds= (dataOut_B[dataAtPos]& dataCurrKey_TopAds)^(dataOut_B[dataAtPos]& ~dataCurrKey_TopAds) +dataCurrKey_ads ;
		++dataCurrKey;
		++dataCurrKey_pos;
		if(dataCurrKey_pos>= KeySize){
			_v7_shuffleKey(KeyEncodedIn.begin(), KeySize, dataCurrKey_shufS++ +dataSize);
			dataCurrKey= KeyEncodedIn.begin();
			dataCurrKey_pos= 0;
		}
	}
		//make sure to destroy sensitive data
//	dataIn_B= NULL;
//	dataOut_B= NULL;
	KeySize= 0;
	dataCurrKey= NULL;
	dataCurrKey_shufS= 0;
	dataCurrKey_TopAds= 0x00;
	dataCurrKey_ads= 0x00;
	dataCurrKey_pos= 0;
	dataAtPos= 0;
}

	///@param dataIn.size == dataOut.size, otherwise error
void fafikLib_crypto::_v7_decrData_p1(const BYTE *dataIn_B, BYTE *dataOut_B, const size_t &dataSize, encodedKey& KeyEncodedIn, const BYTE &SSKeyHashShort)
{
	if(!dataSize || dataSize== size_t(-1)) return;
		//give it a type(unsigned), otherwise any operation is invalid
		//key
	size_t dataCurrKey_shufS= 0;
	size_t KeySize= KeyEncodedIn.size();
//	BYTE dataCurrKeyShuffler[KeySize];
//	memcpy(dataCurrKeyShuffler, dataCurrKeyIn, KeySize);
	BYTE* dataCurrKey= KeyEncodedIn.begin();
//	dataCurrKey= dataCurrKeyShuffler;
	_v7_shuffleKey(KeyEncodedIn.begin(), KeySize, dataSize&1);
		//more self keys
	BYTE dataCurrKey_TopAds= SSKeyHashShort;
	BYTE dataCurrKey_ads= _v7_encodeKey_Sum(KeyEncodedIn.begin(), KeySize);
		///a copy of input
	BYTE dataCurrIn_B= 0;
	size_t dataCurrKey_pos= 0;
		//decrypt reverse encrypt
	size_t dataAtPos= 0;
	for(; dataAtPos!=dataSize; ++dataAtPos){	///decrypt data
		dataCurrIn_B= dataIn_B[dataAtPos];
		dataOut_B[dataAtPos]= ((dataCurrIn_B ^ *dataCurrKey) ^~dataCurrKey_TopAds);
			//next key
//		dataCurrKey_ads^= (~dataCurrKey_TopAds)+ 0x54;
//		dataCurrKey_TopAds= (dataCurrKey_TopAds ^ dataCurrKey_ads) + ~dataIn_B[dataAtPos];
		dataCurrKey_ads+= 0x5f;
		dataCurrKey_TopAds= (dataCurrIn_B& dataCurrKey_TopAds)^(dataCurrIn_B& ~dataCurrKey_TopAds) +dataCurrKey_ads ;
		++dataCurrKey;
		++dataCurrKey_pos;
		if(dataCurrKey_pos>= KeySize){
			_v7_shuffleKey(KeyEncodedIn.begin(), KeySize, dataCurrKey_shufS++ +dataSize);
			dataCurrKey= KeyEncodedIn.begin();
			dataCurrKey_pos= 0;
		}
	}
		//make sure to destroy sensitive data
//	dataIn_B= NULL;
//	dataOut_B= NULL;
	KeySize= 0;
	dataCurrKey= NULL;
	dataCurrKey_shufS= 0;
	dataCurrKey_TopAds= 0x00;
	dataCurrKey_ads= 0x00;
	dataCurrIn_B= 0x00;
	dataCurrKey_pos= 0;
	dataAtPos= 0;
}





