/*
 * isoparser8583.h
 *
 *  Created on: Dec 16, 2020
 *      Author: Sara
 */

#ifndef ISOPARSER8583_H_
#define ISOPARSER8583_H_

#include"includes.h"

#define DE_TYPE_N              1
#define DE_TYPE_N_LLVAR        2
#define DE_TYPE_N_LLLVAR       3
#define DE_TYPE_ANS_LLVAR      4
#define DE_TYPE_ANS            5
#define DE_TYPE_ANS_LLLVAR     6
#define DE_TYPE_B              7


#define DE_PROCESS_CODE_INDEX              3
#define DE_STAN_INDEX                      11
#define DE_TID_INDEX                       41
#define DE_PRIVATE_USE_INDEX               60
#define DE_NET_MANG_CODE_INDEX             70



//this STR contains the type of data element numeric, or ASCII or alpha..etc and the length of this data element
typedef struct DE_Def{

    unsigned short DeType;
	unsigned short length;

    DE_Def(unsigned short DeType, unsigned short length)
    {
		this->DeType = DeType;
		this->length = length;
    }
}Iso8583_DE_Definition_t;


class IsoPasrser8583{

public:

	unsigned short mti;
	unsigned short netMangCode;
	unsigned long stan;
	unsigned long processingCode;
	std::string tid;
	std::string privateUse;

	IsoPasrser8583();
    void isoParser(unsigned char*isoMsg);

private:
    unsigned char bitMapIndexSet[129];
    std::vector<std::vector<unsigned char>>DE;

    void parseBitMap(unsigned char*isoMsg, unsigned char *bitMapIndexSet);//isoMsg..>i/p, bitMapIndexSet..>o/p
    void parseDataElements(unsigned char*isoMsg, unsigned char *bitMapIndexSet);
    void loadDataElements();
};



#endif /* ISOPARSER8583_H_ */
