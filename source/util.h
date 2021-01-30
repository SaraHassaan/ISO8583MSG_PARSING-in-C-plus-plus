/*
 * isoparser.h
 *
 *  Created on: Dec 6, 2020
 *      Author: Sara
 */

#ifndef UTIL_H_
#define UTIL_H_


#include"includes.h"

void BcdToAscii(unsigned char*in, unsigned char *out, unsigned char len);
long BcdToDecimal(unsigned char*in, unsigned char len);
unsigned char convertByteBcdToHex(unsigned char in);
short reverseShortBytes(short in);
long reverseLongBytes(long in);





#endif /* UTIL_H_ */
