/*
 * isoparser.cpp
 *
 *  Created on: Dec 6, 2020
 *      Author: Sara
 */

#include"includes.h"

/*
 * in: input, unsigned char contains BCD Bytes that are 0x00 to 0x99
 * len: number of bytes in the array called in
 * assume the in passed is 0x00 0x10 ..> it is equivalent to 10
 * 0x12 0x54 ..>1254
 * 1 = 1*1000
 * 2 = 2*100
 * 5 = 5*10
 * 4 = 4*1
 * take the sum of all , it will be the decimal number
 */

long BcdToDecimal(unsigned char*in, unsigned char len)
{
     long sum = 0;
	 long mul = pow(10,((2*len)-1));//1 byte is two digits

	 for(int i = 0; i < len; i++)
	 {
	    sum += (0x0f&(in[i]>>4))*mul;
		mul /= 10;
		sum += (0x0f&in[i])*mul;
		mul /= 10;
	 }

	 return sum;
}

/*
 * here we convert BCD to ASCII like ..>0x22 0x43 ..> ASCII ==>"2243"
 * BCD..0x00 to 0x99
 */
void BcdToAscii(unsigned char*in, unsigned char *out, unsigned char len)
{

	 for(int i = 0; i < len; i++)
	 {
	    out[2*i] = (0x0f&(in[i]>>4))+'0';
	    out[(2*i)+1] = (0x0f&in[i])+'0';
	 }

}


short reverseShortBytes(short in)
{
	short out = 0;
	char*pIn = (char*)(&in);
	char*pOut = (char*)(&out);

	pOut[0] = pIn[1];
	pOut[1] = pIn[0];

	return out;
}

long reverseLongBytes(long in)
{
	long out = 0;
	char*pIn = (char*)(&in);
	char*pOut = (char*)(&out);

	pOut[0] = pIn[3];
	pOut[1] = pIn[2];
	pOut[2] = pIn[1];
	pOut[3] = pIn[0];

	return out;
}

//convert number like 72 to 0x72
unsigned char convertByteBcdToHex(unsigned char in)
{
	unsigned char out = 0;
	unsigned char firstDigit = in%10;
	unsigned char secondDigit = in/10;

	out = ((0xf0 &(secondDigit << 4)) | (0x0f&firstDigit));

	return out;

}
