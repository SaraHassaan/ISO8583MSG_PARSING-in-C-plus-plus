/*
 * isoParser8583.cpp
 *
 *  Created on: Dec 16, 2020
 *      Author: Sara
 */

#include"includes.h"

//Act as look up table
std::map<unsigned char, Iso8583_DE_Definition_t> DefinationFieldsMap =
{
    { 1,  Iso8583_DE_Definition_t(DE_TYPE_B,  64) },
    { 2,  Iso8583_DE_Definition_t(DE_TYPE_N_LLVAR,  19) },
    { 3,  Iso8583_DE_Definition_t(DE_TYPE_N,  6) },
	{ 4,  Iso8583_DE_Definition_t(DE_TYPE_N,  12) },
	{ 7,  Iso8583_DE_Definition_t(DE_TYPE_N,  10) },
	{ 11,  Iso8583_DE_Definition_t(DE_TYPE_N,  6) },
	{ 22,  Iso8583_DE_Definition_t(DE_TYPE_N,  3) },
	{ 41, Iso8583_DE_Definition_t(DE_TYPE_ANS,  8) },
	{ 60, Iso8583_DE_Definition_t(DE_TYPE_ANS_LLLVAR,  999) },
	{ 70, Iso8583_DE_Definition_t(DE_TYPE_N,  3) }

};


IsoPasrser8583:: IsoPasrser8583()
{
	tid ="";
	stan = 0;
	mti = 0;
	processingCode = 0;

}

void IsoPasrser8583:: isoParser(unsigned char*isoMsg){

	memset(bitMapIndexSet, 0, sizeof(bitMapIndexSet));
	DE.resize(128);

	mti = (unsigned short)BcdToDecimal(isoMsg, 2);

	parseBitMap(isoMsg, bitMapIndexSet);
	parseDataElements(isoMsg, bitMapIndexSet);
	loadDataElements();
}

void IsoPasrser8583:: parseBitMap(unsigned char*isoMsg, unsigned char *bitMapIndexSet){

	int i ;
	int k = 0;
	unsigned char mask = 0x80;
	//unsigned char byte = 0;

	//the first bitmap starts from byte3 in isoMSg (index in isoMsg = 2)
	for(i = 2; i <= 9; i++)//8bytes
	{
		//byte = convertByteBcdToHex(isoStr[i]);
		for(int j = 0; j < 8; j++)
		{
			if(isoMsg[i] & (mask >> j))
			{
				bitMapIndexSet[k++] = ((8*(i-2))+(j+1)); //push the enabled data element(number of bit field that exist)
			}
		}
	}

	//check for second bitmap
	if(bitMapIndexSet[0] == 1)
	{
		for(; i <= 17; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				if(isoMsg[i] & (mask >> j))
				{
					bitMapIndexSet[k++] = ((8*(i-2))+(j+1)); //push the enabled data element(number of bit field that exist)

				}
			}
		}
	}

}

void IsoPasrser8583:: parseDataElements(unsigned char*isoMsg, unsigned char *bitMapIndexSet){

	int indexMsg;
	int j = 1;//start from 1 to skip secondary bitmap bit
	int length;
	std::map<unsigned char, Iso8583_DE_Definition_t>::iterator it;
	unsigned char DeTemp[129] = {0};
	//unsigned char DeTemp[129][999] = {{0}};
	unsigned char Bytes[128] = {0};

	if(bitMapIndexSet[0] == 1)
	{
		indexMsg = 18; //2bytes MTI+first bitmap + second bitmap
	}
	else
	{
		indexMsg = 10; //2bytes MTI+first bitmap
	}

	while(bitMapIndexSet[j] != 0)
	{
		it = DefinationFieldsMap.find(bitMapIndexSet[j]);
		switch (it->second.DeType)
		{
			case DE_TYPE_N:

				//odd digits that means there is padding in ISO MSG for this data element
				length = (it->second.length%2 == 0)?it->second.length/2:((unsigned short)it->second.length/2)+1;
				memset(DeTemp, 0, sizeof(DeTemp));
				BcdToAscii(&isoMsg[indexMsg], DeTemp, length);
				copy(DeTemp, DeTemp+(2*length), back_inserter(DE[bitMapIndexSet[j]]));
				indexMsg += length;

				break;

			case DE_TYPE_N_LLVAR:

				length = BcdToDecimal(&isoMsg[indexMsg], 1);
				length /= 2;
				indexMsg++;
				memset(DeTemp, 0, sizeof(DeTemp));
				BcdToAscii(&isoMsg[indexMsg], DeTemp, length);//length by bytes
				//copy(DeTemp, DeTemp+(2*length), DE[bitMapIndexSet[j]].begin());
				copy(DeTemp, DeTemp+(2*length), back_inserter(DE[bitMapIndexSet[j]]));


				indexMsg += length;

				break;

			case DE_TYPE_N_LLLVAR:

				length = BcdToDecimal(&isoMsg[indexMsg], 2);
				length /= 2;
				indexMsg += 2;

				memset(DeTemp, 0, sizeof(DeTemp));
				BcdToAscii(&isoMsg[indexMsg], DeTemp, length);
				//copy(DeTemp, DeTemp+(2*length), DE[bitMapIndexSet[j]].begin());
				copy(DeTemp, DeTemp+(2*length), back_inserter(DE[bitMapIndexSet[j]]));


				indexMsg += length;

				break;

			case DE_TYPE_ANS:

				memset(Bytes, 0, sizeof(Bytes));
				length = it->second.length;

				memset(DeTemp, 0, sizeof(DeTemp));
				memcpy(DeTemp,&isoMsg[indexMsg], length);
				//copy(DeTemp, DeTemp+(2*length), DE[bitMapIndexSet[j]].begin());
				copy(DeTemp, DeTemp+(length), back_inserter(DE[bitMapIndexSet[j]]));


				indexMsg += length;


				break;

			case DE_TYPE_ANS_LLLVAR:

				length = BcdToDecimal(&isoMsg[indexMsg], 2);
				indexMsg += 2;

				memset(DeTemp, 0, sizeof(DeTemp));
				memcpy(DeTemp,&isoMsg[indexMsg], length);
				//copy(DeTemp, DeTemp+(2*length), DE[bitMapIndexSet[j]].begin());
				copy(DeTemp, DeTemp+(length), back_inserter(DE[bitMapIndexSet[j]]));


				indexMsg += length;

				break;

			case DE_TYPE_B:

				length = (it->second.length)/8;
				memset(DeTemp, 0, sizeof(DeTemp));
				memcpy(DeTemp,&isoMsg[indexMsg], length);
				//copy(DeTemp, DeTemp+(2*length), DE[bitMapIndexSet[j]].begin());
				copy(DeTemp, DeTemp+(length), back_inserter(DE[bitMapIndexSet[j]]));


				indexMsg += length;

				break;

		}

		j++;
	}
}

void IsoPasrser8583:: loadDataElements(){

	std::ostringstream oss;

	std::copy(DE[DE_TID_INDEX].begin(), DE[DE_TID_INDEX].end(), std::ostream_iterator<unsigned char>(oss,""));
	tid = oss.str();

	oss.str("");
	std::copy(DE[DE_PRIVATE_USE_INDEX].begin(), DE[DE_PRIVATE_USE_INDEX].end(), std::ostream_iterator<unsigned char>(oss,""));
	privateUse = oss.str();

	oss.str("");
	std::copy(DE[DE_PROCESS_CODE_INDEX].begin(), DE[DE_PROCESS_CODE_INDEX].end(), std::ostream_iterator<unsigned char>(oss,""));
	processingCode = stol(oss.str());

	oss.str("");
	std::copy(DE[DE_STAN_INDEX].begin(), DE[DE_STAN_INDEX].end(), std::ostream_iterator<unsigned char>(oss,""));
	stan = stol(oss.str());

	oss.str("");
	std::copy(DE[DE_NET_MANG_CODE_INDEX].begin(), DE[DE_NET_MANG_CODE_INDEX].end(), std::ostream_iterator<unsigned char>(oss,""));
	netMangCode = (unsigned short)stol(oss.str());


}




