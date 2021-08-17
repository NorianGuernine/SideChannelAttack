/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h>
#include "xil_printf.h"
#include "AES.h"
#include "PmodGPIO.h"
#include "sleep.h"
#include "xil_cache.h"

PmodGPIO myDevice;

void DemoInitialize();
void DemoRun();
void DemoCleanup();
void EnableCaches();
void DisableCaches();

void DemoInitialize() {
   EnableCaches();
   GPIO_begin(&myDevice, XPAR_PMODGPIO_0_AXI_LITE_GPIO_BASEADDR, 0x00);
}
/*
* AES ShiftRows
*/
void ShiftRows(unsigned char* myplaintext)
{
    int i=0;

    // ShiftRows
    unsigned char temp[NB_WORD_DATA] = {myplaintext[0], myplaintext[5], myplaintext[10], myplaintext[15],
                                  myplaintext[4], myplaintext[9], myplaintext[14], myplaintext[3],
                                  myplaintext[8], myplaintext[13], myplaintext[2], myplaintext[7],
                                  myplaintext[12], myplaintext[1], myplaintext[6], myplaintext[11]};
    for(i=0;i<NB_WORD_DATA;i++)
        myplaintext[i]  = temp[i];
}

/*
* GF Mult by 2:0
* if MSB = 0 => Left Shift
* if MSB = 1 => Left Shift xor 0x1B
*/
unsigned char GF_Mult_2(unsigned char input)
{

    if (input & 0x80)
        return (input<< 1) ^ 0x1B;
    else
        return input<< 1;
}

/*
* GF Mult by 3
* GF_Mult_2 xor input
*/
unsigned char GF_Mult_3(unsigned char input)
{
    return GF_Mult_2(input) ^ input;
}

/*
* AES MixColumns
*/
void MixColumns(unsigned char* myplaintext)
{
    int i=0;

    // Copy plaintext
    unsigned char temp[NB_WORD_DATA] = {myplaintext[0], myplaintext[1], myplaintext[2], myplaintext[3],
                                  myplaintext[4], myplaintext[5], myplaintext[6], myplaintext[7],
                                  myplaintext[8], myplaintext[9], myplaintext[10], myplaintext[11],
                                  myplaintext[12], myplaintext[13], myplaintext[14], myplaintext[15]};

    for(i=0;i<NB_WORD_DATA;i=i+4)
    {
        // Row 1 : 02 03 01 01
        myplaintext[i] = GF_Mult_2(temp[i]) ^ GF_Mult_3(temp[i+1]) ^ temp[i+2] ^ temp[i+3];

        // Row 2 : 01 02 03 01
        myplaintext[i+1] = temp[i] ^ GF_Mult_2(temp[i+1]) ^ GF_Mult_3(temp[i+2]) ^ temp[i+3];

        // Row 3 : 01 01 02 03
        myplaintext[i+2] = temp[i] ^ temp[i+1] ^ GF_Mult_2(temp[i+2]) ^ GF_Mult_3(temp[i+3]);

        // Row 4 : 03 01 01 02
        myplaintext[i+3] = GF_Mult_3(temp[i]) ^ temp[i+1] ^ temp[i+2] ^ GF_Mult_2(temp[i+3]);

    }
}

/*
* AES KeySchedule
*/
void KeySchedule(unsigned char* mycipherkey, unsigned char round)
{
    int i=0;

    // Copy cipherkey & RotWord (last line) & SBOX
    unsigned char temp[NB_WORD_DATA] = {SBOX[mycipherkey[0]], SBOX[mycipherkey[1]], SBOX[mycipherkey[2]], SBOX[mycipherkey[3]],
                                  SBOX[mycipherkey[4]], SBOX[mycipherkey[5]], SBOX[mycipherkey[6]], SBOX[mycipherkey[7]],
                                  SBOX[mycipherkey[8]], SBOX[mycipherkey[9]], SBOX[mycipherkey[10]], SBOX[mycipherkey[11]],
                                  SBOX[mycipherkey[13]], SBOX[mycipherkey[14]], SBOX[mycipherkey[15]], SBOX[mycipherkey[12]]};

    // New key column 0
    for(i=0;i<4;i++)
        mycipherkey[i] ^= RCON[round][i] ^ temp[12+i];

    // Other columns (1 to 3)
    for(i=4;i<NB_WORD_DATA;i++)
        mycipherkey[i] ^= mycipherkey[i-4];
}

/*
* AES cipher
*/
void AES_Cipher(unsigned char* PlainText, unsigned char* CipherKey)
{
    int i=0;
    int round = 0;

    // Test :  CipherText = 0x3925841D 02DC09FB DC118597 196A0B32
	//unsigned char PlainText[NB_WORD_DATA] = {0x32, 0x43, 0xF6, 0xA8, 0x88, 0x5A, 0x30, 0x8D, 0x31, 0x31, 0x98, 0xA2, 0xE0, 0x37, 0x07, 0x34};
	//unsigned char CipherKey[NB_WORD_DATA] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};

    // Initial round
    for (i = 0; i < NB_WORD_DATA; i++)
    {
        PlainText[i] ^= CipherKey[i];
    }

    // Nine Main Rounds
    for (round = 0; round < NB_ROUND-1; round++)
    {
        // 1 - SubBytes
        for (i = 0; i < NB_WORD_DATA; i++)
        {
            PlainText[i] = SBOX[PlainText[i]];
        }

        // 2 - ShiftRows
        ShiftRows(PlainText);

        // 3 - MixColumn
        MixColumns(PlainText);

        // 4 - Add Round Key
        KeySchedule(CipherKey, round);
        for (i = 0; i < NB_WORD_DATA; i++)
        {
            PlainText[i] ^= CipherKey[i];
        }
    }

    // Final Round

    // 1 - SubBytes
    for (i = 0; i < NB_WORD_DATA; i++)
    {
        PlainText[i] = SBOX[PlainText[i]];
    }

    // 2 - ShiftRows
    ShiftRows(PlainText);

    // 3 - Add Round Key
    KeySchedule(CipherKey, NB_ROUND-1);
    for (i = 0; i < NB_WORD_DATA; i++)
    {
        PlainText[i] ^= CipherKey[i];
    }
}

void print_hex8(unsigned char num)
{
   int i;
   unsigned char j;
   for(i = 4; i >= 0; i -= 4)
   {
      j = (num >> i) & 0xf;
      if(j < 10)
         putchar('0' + j);
      else
         putchar('a' - 10 + j);
   }
}

void print_128(unsigned char *CT)
{
   int j=0;
   for(j=0;j<NB_WORD_DATA;j++)
        {
            print_hex8(CT[j]);
        }
    puts("\n");
}

int main()
{
	unsigned char PlainText[NB_WORD_DATA] = {0x32, 0x43, 0xF6, 0xA8, 0x88, 0x5A, 0x30, 0x8D, 0x31, 0x31, 0x98, 0xA2, 0xE0, 0x37, 0x07, 0x34};
	unsigned char CipherKey[NB_WORD_DATA] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
	unsigned char PlainTextTab[DATA][NB_WORD_DATA];
	uint16_t i,n = 0;
	DemoInitialize();
    for (n=0;n<DATA;n++){
		for(i=0;i<NB_WORD_DATA; i++){
			PlainText[i]=rand();
			PlainTextTab[n][i];
		}
    }
    while(1)
    {
		for (n=0;n<DATA;n++)
		{
			//inbyte();
			GPIO_setPins(&myDevice, 0xFF);
			AES_Cipher(PlainTextTab[n], CipherKey);
			GPIO_setPins(&myDevice, 0x00);
			print_128(PlainTextTab[n]);
		}
    }
    DemoCleanup();
    return 0;
}

void DemoCleanup() {
   DisableCaches();
}

void EnableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheDisable();
#endif
#endif
}
