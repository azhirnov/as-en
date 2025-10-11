//// etcpack v2.74
//// 
//// NO WARRANTY 
//// 
//// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE THE PROGRAM IS PROVIDED
//// "AS IS". ERICSSON MAKES NO REPRESENTATIONS OF ANY KIND, EXTENDS NO
//// WARRANTIES OR CONDITIONS OF ANY KIND; EITHER EXPRESS, IMPLIED OR
//// STATUTORY; INCLUDING, BUT NOT LIMITED TO, EXPRESS, IMPLIED OR
//// STATUTORY WARRANTIES OR CONDITIONS OF TITLE, MERCHANTABILITY,
//// SATISFACTORY QUALITY, SUITABILITY AND FITNESS FOR A PARTICULAR
//// PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
//// PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME
//// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. ERICSSON
//// MAKES NO WARRANTY THAT THE MANUFACTURE, SALE, OFFERING FOR SALE,
//// DISTRIBUTION, LEASE, USE OR IMPORTATION UNDER THE LICENSE WILL BE FREE
//// FROM INFRINGEMENT OF PATENTS, COPYRIGHTS OR OTHER INTELLECTUAL
//// PROPERTY RIGHTS OF OTHERS, AND THE VALIDITY OF THE LICENSE IS SUBJECT
//// TO YOUR SOLE RESPONSIBILITY TO MAKE SUCH DETERMINATION AND ACQUIRE
//// SUCH LICENSES AS MAY BE NECESSARY WITH RESPECT TO PATENTS, COPYRIGHT
//// AND OTHER INTELLECTUAL PROPERTY OF THIRD PARTIES.
//// 
//// FOR THE AVOIDANCE OF DOUBT THE PROGRAM (I) IS NOT LICENSED FOR; (II)
//// IS NOT DESIGNED FOR OR INTENDED FOR; AND (III) MAY NOT BE USED FOR;
//// ANY MISSION CRITICAL APPLICATIONS SUCH AS, BUT NOT LIMITED TO
//// OPERATION OF NUCLEAR OR HEALTHCARE COMPUTER SYSTEMS AND/OR NETWORKS,
//// AIRCRAFT OR TRAIN CONTROL AND/OR COMMUNICATION SYSTEMS OR ANY OTHER
//// COMPUTER SYSTEMS AND/OR NETWORKS OR CONTROL AND/OR COMMUNICATION
//// SYSTEMS ALL IN WHICH CASE THE FAILURE OF THE PROGRAM COULD LEAD TO
//// DEATH, PERSONAL INJURY, OR SEVERE PHYSICAL, MATERIAL OR ENVIRONMENTAL
//// DAMAGE. YOUR RIGHTS UNDER THIS LICENSE WILL TERMINATE AUTOMATICALLY
//// AND IMMEDIATELY WITHOUT NOTICE IF YOU FAIL TO COMPLY WITH THIS
//// PARAGRAPH.
//// 
//// IN NO EVENT WILL ERICSSON, BE LIABLE FOR ANY DAMAGES WHATSOEVER,
//// INCLUDING BUT NOT LIMITED TO PERSONAL INJURY, ANY GENERAL, SPECIAL,
//// INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF OR IN
//// CONNECTION WITH THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT
//// NOT LIMITED TO LOSS OF PROFITS, BUSINESS INTERUPTIONS, OR ANY OTHER
//// COMMERCIAL DAMAGES OR LOSSES, LOSS OF DATA OR DATA BEING RENDERED
//// INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF
//// THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS) REGARDLESS OF THE
//// THEORY OF LIABILITY (CONTRACT, TORT OR OTHERWISE), EVEN IF SUCH HOLDER
//// OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//// 
//// (C) Ericsson AB 2005-2013. All Rights Reserved.
//// 

// from https://github.com/Ericsson/ETCPACK/blob/master/source/etcpack.cxx

static const int alphaBase[16][4] =
{	
	{-15,-9,-6,-3},
	{-13,-10,-7,-3},
	{-13,-8,-5,-2},
	{-13,-6,-4,-2},
	{-12,-8,-6,-3},
	{-11,-9,-7,-3},
	{-11,-8,-7,-4},
	{-11,-8,-5,-3},
	{ -10,-8,-6,-2},
	{ -10,-8,-5,-2},
	{ -10,-8,-4,-2},
	{ -10,-7,-5,-2},
	{ -10,-7,-4,-3},
	{ -10,-3,-2, -1},
	{ -9,-8,-6,-4},
	{ -9,-7,-5,-3}
};

// Table for fast implementationi of clamping to the interval [0,255]
static const int clamp_table[768] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
						255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};


// Does decompression and then immediately converts from 11 bit signed to a 16-bit format.
// 
// NO WARRANTY --- SEE STATEMENT IN TOP OF FILE (C) Ericsson AB 2005-2013. All Rights Reserved.
int16_t get16bits11signed(int base, int table, int mul, int index) 
{
	int elevenbase = base-128;
	if(elevenbase==-128)
		elevenbase=-127;
	elevenbase*=8;
	//i want the positive value here
	int tabVal = -alphaBase[table][3-index%4]-1;
	//and the sign, please
	int sign = 1-(index/4);
	
	if(sign)
		tabVal=tabVal+1;
	int elevenTabVal = tabVal*8;

	if(mul!=0)
		elevenTabVal*=mul;
	else
		elevenTabVal/=8;

	if(sign)
		elevenTabVal=-elevenTabVal;

	//calculate sum
	int elevenbits = elevenbase+elevenTabVal;

	//clamp..
	if(elevenbits>=1024)
		elevenbits=1023;
	else if(elevenbits<-1023)
		elevenbits=-1023;
	//this is the value we would actually output.. 
	//but there aren't any good 11-bit file or uncompressed GL formats
	//so we extend to 15 bits signed.
	sign = elevenbits<0;
	elevenbits=abs(elevenbits);
	int16_t fifteenbits = (elevenbits<<5)+(elevenbits>>5);
	int16_t sixteenbits=fifteenbits;

	if(sign)
		sixteenbits=-sixteenbits;
	
	return sixteenbits;
}

// Does decompression and then immediately converts from 11 bit signed to a 16-bit format 
// Calculates the 11 bit value represented by base, table, mul and index, and extends it to 16 bits.
// NO WARRANTY --- SEE STATEMENT IN TOP OF FILE (C) Ericsson AB 2005-2013. All Rights Reserved.
uint16_t get16bits11bits(int base, int table, int mul, int index) 
{
	int elevenbase = base*8+4;

	//i want the positive value here
	int tabVal = -alphaBase[table][3-index%4]-1;
	//and the sign, please
	int sign = 1-(index/4);
	
	if(sign)
		tabVal=tabVal+1;
	int elevenTabVal = tabVal*8;

	if(mul!=0)
		elevenTabVal*=mul;
	else
		elevenTabVal/=8;

	if(sign)
		elevenTabVal=-elevenTabVal;

	//calculate sum
	int elevenbits = elevenbase+elevenTabVal;

	//clamp..
	if(elevenbits>=256*8)
		elevenbits=256*8-1;
	else if(elevenbits<0)
		elevenbits=0;
	//elevenbits now contains the 11 bit alpha value as defined in the spec.

	//extend to 16 bits before returning, since we don't have any good 11-bit file formats.
	uint16_t sixteenbits = (elevenbits<<5)+(elevenbits>>6);

	return sixteenbits;
}

// Code used to create the valtab
// NO WARRANTY --- SEE STATEMENT IN TOP OF FILE (C) Ericsson AB 2005-2013. All Rights Reserved.
void setupAlphaTable (int (&alphaTable) [256][8]) 
{

	//read table used for alpha compression
	int buf;
	for(int i = 16; i<32; i++) 
	{
		for(int j=0; j<8; j++) 
		{
			buf=alphaBase[i-16][3-j%4];
			if(j<4)
				alphaTable[i][j]=buf;
			else
				alphaTable[i][j]=(-buf-1);
		}
	}
	
	//beyond the first 16 values, the rest of the table is implicit.. so calculate that!
	for(int i=0; i<256; i++) 
	{
		//fill remaining slots in table with multiples of the first ones.
		int mul = i/16;
		int old = 16+i%16;
		for(int j = 0; j<8; j++) 
		{
			alphaTable[i][j]=alphaTable[old][j]*mul;
			//note: we don't do clamping here, though we could, because we'll be clamped afterwards anyway.
		}
	}
}

void setupValtab (int (&valtab) [1024*512], bool formatSigned)
{
	//fix precomputation table..!
	int16_t val16;
	int count=0;
	for(int base=0; base<256; base++) 
	{
		for(int tab=0; tab<16; tab++) 
		{
			for(int mul=0; mul<16; mul++) 
			{
				for(int index=0; index<8; index++) 
				{
					if(formatSigned)
					{
						val16=get16bits11signed(base,tab,mul,index);
						valtab[count] = val16 + 256*128;
					}
					else
						valtab[count]=get16bits11bits(base,tab,mul,index);
					count++;
				}
			}
		}
	}
}

struct AlphaTableAndValtab
{
	int		alphaTable [256][8];
	int		valtabS [1024*512];
	int		valtabU [1024*512];

	AlphaTableAndValtab ()
	{
		setupAlphaTable( alphaTable );
		setupValtab( valtabS, true );
		setupValtab( valtabU, false );
	}

	static AlphaTableAndValtab&  Get ()
	{
		static AlphaTableAndValtab	s_Instance;
		return s_Instance;
	}
};


const int* getValueTable (bool formatSigned) {
	auto& d = AlphaTableAndValtab::Get();
	return formatSigned ? d.valtabS : d.valtabU;
}

using AlphaTable_t = const int (&) [256][8];
AlphaTable_t getAlphaTable () {
	return AlphaTableAndValtab::Get().alphaTable;
}


// Helper function for the below function
// NO WARRANTY --- SEE STATEMENT IN TOP OF FILE (C) Ericsson AB 2005-2013. All Rights Reserved.
int getPremulIndex (int base, int tab, int mul, int index) 
{
	return (base<<11)+(tab<<7)+(mul<<3)+index;
}

// Calculates the error used in compressBlockAlpha16()
// NO WARRANTY --- SEE STATEMENT IN TOP OF FILE (C) Ericsson AB 2005-2013. All Rights Reserved.
double calcError (uint8_t* data, int ix, int iy, int width, int height, int base, int tab, int mul, double prevbest, bool formatSigned) 
{
	const int *valtab = getValueTable(formatSigned);
	int offset = getPremulIndex(base,tab,mul,0);
	double error=0;
	for (int y=0; y<4; y++) 
	{
		for(int x=0; x<4; x++) 
		{
			double besthere = (1<<20);
			besthere*=besthere;
			uint8_t byte1 = data[2*(x+ix+(y+iy)*width)];
			uint8_t byte2 = data[2*(x+ix+(y+iy)*width)+1];
			int alpha = (byte1<<8)+byte2;
			for(int index=0; index<8; index++) 
			{
				double indexError;
				indexError = alpha-valtab[offset+index];
				indexError*=indexError;
				if(indexError<besthere)
					besthere=indexError;
			}
			error+=besthere;
			if(error>=prevbest)
				return prevbest+(1<<30);
		}
	}
	return error;
}

// bit number frompos is extracted from input, and moved to bit number topos in the return value.
// NO WARRANTY --- SEE STATEMENT IN TOP OF FILE (C) Ericsson AB 2005-2013. All Rights Reserved.
uint8_t getbit(uint8_t input, int frompos, int topos) 
{
	uint8_t output=0;
	if(frompos>topos)
		return ((1<<frompos)&input)>>(frompos-topos);
	return ((1<<frompos)&input)<<(topos-frompos);
}

// takes as input a value, returns the value clamped to the interval [0,255].
// NO WARRANTY --- SEE STATEMENT IN TOP OF FILE (C) Ericsson AB 2005-2013. All Rights Reserved.
int clamp(int val) 
{
	if(val<0)
		val=0;
	if(val>255)
		val=255;
	return val;
}

// compressBlockAlpha16
// 
// Compresses a block using the 11-bit EAC formats.
// Depends on the global variable formatSigned.
// 
// COMPRESSED_R11_EAC (if formatSigned = 0)
// This is an 11-bit unsigned format. Since we do not have a good 11-bit file format, we use 16-bit pgm instead.
// Here we assume that, in the input 16-bit pgm file, 0 represents 0.0 and 65535 represents 1.0. The function compressBlockAlpha16 
// will find the compressed block which best matches the data. In detail, it will find the compressed block, which 
// if decompressed, will generate an 11-bit block that after bit replication to 16-bits will generate the closest 
// block to the original 16-bit pgm block.
// 
// COMPRESSED_SIGNED_R11_EAC (if formatSigned = 1)
// This is an 11-bit signed format. Since we do not have any signed file formats, we use unsigned 16-bit pgm instead.
// Hence we assume that, in the input 16-bit pgm file, 1 represents -1.0, 32768 represents 0.0 and 65535 represents 1.0. 
// The function compresseBlockAlpha16 will find the compressed block, which if decompressed, will generate a signed
// 11-bit block that after bit replication to 16-bits and conversion to unsigned (1 equals -1.0, 32768 equals 0.0 and 
// 65535 equals 1.0) will generate the closest block to the original 16-bit pgm block. 
//
// COMPRESSED_RG11_EAC is compressed by calling the function twice, dito for COMPRESSED_SIGNED_RG11_EAC.
// 
// NO WARRANTY --- SEE STATEMENT IN TOP OF FILE (C) Ericsson AB 2005-2013. All Rights Reserved.
void compressBlockAlpha16 (uint8_t* data, int ix, int iy, int width, int height, uint8_t* returnData, bool formatSigned) 
{
	unsigned int bestbase, besttable, bestmul;
	{
		double besterror;
		besterror=1<<20;
		besterror*=besterror;
		for(int base=0; base<256; base++) 
		{
			for(int table=0; table<16; table++) 
			{
				for(int mul=0; mul<16; mul++) 
				{
					double e = calcError(data, ix, iy, width, height,base,table,mul,besterror, formatSigned);
					if(e<besterror) 
					{
						bestbase=base;
						besttable=table;
						bestmul=mul;
						besterror=e;
					}
				}
			}
		}
	}
	returnData[0]=bestbase;
	returnData[1]=(bestmul<<4)+besttable;
	if(formatSigned) 
	{
		//if we have a signed format, the base value should be given as a signed byte. 
		signed char signedbase = bestbase-128;
		returnData[0]=*((uint8_t*)(&signedbase));
	}
	
	for(int i=2; i<8; i++) 
	{
		returnData[i]=0;
	}

	int byte=2;
	int bit=0;
	for (int x=0; x<4; x++) 
	{
		for(int y=0; y<4; y++) 
		{
			double besterror=255*255;
			besterror*=besterror;
			int bestindex=99;
			uint8_t byte1 = data[2*(x+ix+(y+iy)*width)];
			uint8_t byte2 = data[2*(x+ix+(y+iy)*width)+1];
			int alpha = (byte1<<8)+byte2;
			for(unsigned int index=0; index<8; index++) 
			{
				double indexError;
				if(formatSigned)
				{
					int16_t val16;
					int val;
					val16 = get16bits11signed(bestbase,besttable,bestmul,index);
					val = val16 + 256*128;
					indexError = alpha-val;
				}
				else
					indexError = alpha-get16bits11bits(bestbase,besttable,bestmul,index);

				indexError*=indexError;
				if(indexError<besterror) 
				{
					besterror=indexError;
					bestindex=index;
				}
			}
			
			for(int numbit=0; numbit<3; numbit++) 
			{
				returnData[byte]|=getbit(bestindex,2-numbit,7-bit);
				bit++;
				if(bit>7) 
				{
					bit=0;
					byte++;
				}
			}
		}
	}
}

// Exhaustive compression of alpha compression in a GL_COMPRESSED_RGB8_ETC2 block
// NO WARRANTY --- SEE STATEMENT IN TOP OF FILE (C) Ericsson AB 2005-2013. All Rights Reserved.
void compressBlockAlphaSlow (uint8_t* data, int ix, int iy, int width, int height, uint8_t* returnData) 
{
	auto& alphaTable = getAlphaTable();

	//determine the best table and base alpha value for this block using MSE
	int alphasum=0;
	int maxdist=-2;
	for(int x=0; x<4; x++) 
	{
		for(int y=0; y<4; y++) 
		{
			alphasum+=data[ix+x+(iy+y)*width];
		}
	}
	int alpha = (int)( ((float)alphasum)/16.0f+0.5f); //average pixel value, used as guess for base value.

	int bestsum=1000000000;
	int besttable=-3; 
	int bestalpha=128;
	int prevalpha=alpha;

	//main loop: determine best base alpha value and offset table to use for compression
	//try some different alpha tables.
	for(int table = 0; table<256&&bestsum>0; table++)
	{
		int tablealpha=prevalpha;
		int tablebestsum=1000000000;
		//test some different alpha values, trying to find the best one for the given table.
		for(int alphascale=32; alphascale>0; alphascale/=8) 
		{
			
			int startalpha = clamp(tablealpha-alphascale*4);
			int endalpha = clamp(tablealpha+alphascale*4);
			
			for(alpha=startalpha; alpha<=endalpha; alpha+=alphascale) {
				int sum=0;
				int val,diff,bestdiff=10000000,index;
				for(int x=0; x<4; x++) 
				{
					for(int y=0; y<4; y++) 
					{
						//compute best offset here, add square difference to sum..
						val=data[ix+x+(iy+y)*width];
						bestdiff=1000000000;
						//the values are always ordered from small to large, with the first 4 being negative and the last 4 positive
						//search is therefore made in the order 0-1-2-3 or 7-6-5-4, stopping when error increases compared to the previous entry tested.
						if(val>alpha) 
						{ 
							for(index=7; index>3; index--) 
							{
								diff=clamp_table[alpha+(alphaTable[table][index])+255]-val;
								diff*=diff;
								if(diff<=bestdiff) 
								{
									bestdiff=diff;
								}
								else
									break;
							}
						}
						else 
						{
							for(index=0; index<5; index++) 
							{
								diff=clamp_table[alpha+(alphaTable[table][index])+255]-val;
								diff*=diff;
								if(diff<bestdiff) 
								{
									bestdiff=diff;
								}
								else
									break;
							}
						}

						//best diff here is bestdiff, add it to sum!
						sum+=bestdiff;
						//if the sum here is worse than previously best already, there's no use in continuing the count..
						if(sum>tablebestsum) 
						{ 
							x=9999; //just to make it large and get out of the x<4 loop
							break;
						}
					}
				}
				if(sum<tablebestsum) 
				{
					tablebestsum=sum;
					tablealpha=alpha;
				}
				if(sum<bestsum) 
				{
					bestsum=sum;
					besttable=table;
					bestalpha=alpha;
				}
			}
			if(alphascale==4)
				alphascale=8;
		}
	}

	alpha=bestalpha;	
	//the best alpha value and table are known!
	//store them, then loop through the pixels again and print indices.
	returnData[0]=alpha;
	returnData[1]=besttable;
	for(int pos=2; pos<8; pos++) 
	{
		returnData[pos]=0;
	}
	int byte=2;
	int bit=0;
	for(int x=0; x<4; x++) 
	{
		for(int y=0; y<4; y++) 
		{
			//find correct index
			int besterror=1000000;
			int bestindex=99;
			for(int index=0; index<8; index++) //no clever ordering this time, as this loop is only run once per block anyway
			{ 
				int error= (clamp(alpha +(int)(alphaTable[besttable][index]))-data[ix+x+(iy+y)*width])*(clamp(alpha +(int)(alphaTable[besttable][index]))-data[ix+x+(iy+y)*width]);
				if(error<besterror) 
				{
					besterror=error;
					bestindex=index;
				}
			}
			//best table index has been determined.
			//pack 3-bit index into compressed data, one bit at a time
			for(int numbit=0; numbit<3; numbit++) 
			{
				returnData[byte]|=getbit(bestindex,2-numbit,7-bit);

				bit++;
				if(bit>7) 
				{
					bit=0;
					byte++;
				}
			}
		}
	}
}


// Decodes tha alpha component in a block coded with GL_COMPRESSED_RGBA8_ETC2_EAC.
// Note that this decoding is slightly different from that of GL_COMPRESSED_R11_EAC.
// However, a hardware decoder can share gates between the two formats as explained
// in the specification under GL_COMPRESSED_R11_EAC.
// NO WARRANTY --- SEE STATEMENT IN TOP OF FILE (C) Ericsson AB 2005-2013. All Rights Reserved.
void decompressBlockAlphaC(uint8_t* data, uint8_t* img, int width, int height, int ix, int iy, int channels) 
{
	auto& alphaTable = getAlphaTable();

	int alpha = data[0];
	int table = data[1];
	
	int bit=0;
	int byte=2;
	//extract an alpha value for each pixel.
	for(int x=0; x<4; x++) 
	{
		for(int y=0; y<4; y++) 
		{
			//Extract table index
			int index=0;
			for(int bitpos=0; bitpos<3; bitpos++) 
			{
				index|=getbit(data[byte],7-bit,2-bitpos);
				bit++;
				if(bit>7) 
				{
					bit=0;
					byte++;
				}
			}
			img[(ix+x+(iy+y)*width)*channels]=clamp(alpha +alphaTable[table][index]);
		}
	}
}

// Decompresses a block using one of the GL_COMPRESSED_R11_EAC or GL_COMPRESSED_SIGNED_R11_EAC-formats
// NO WARRANTY --- SEE STATEMENT IN TOP OF FILE (C) Ericsson AB 2005-2013. All Rights Reserved.
void decompressBlockAlpha16bitC(uint8_t* data, uint8_t* img, int width, int height, int ix, int iy, int channels, bool formatSigned) 
{
	int alpha = data[0];
	int table = data[1];

	if(formatSigned) 
	{
		//if we have a signed format, the base value is given as a signed byte. We convert it to (0-255) here,
		//so more code can be shared with the unsigned mode.
		alpha = *((signed char*)(&data[0]));
		alpha = alpha+128;
	}

	int bit=0;
	int byte=2;
	//extract an alpha value for each pixel.
	for(int x=0; x<4; x++) 
	{
		for(int y=0; y<4; y++) 
		{
			//Extract table index
			int index=0;
			for(int bitpos=0; bitpos<3; bitpos++) 
			{
				index|=getbit(data[byte],7-bit,2-bitpos);
				bit++;
				if(bit>7) 
				{
					bit=0;
					byte++;
				}
			}
			int windex = channels*(2*(ix+x+(iy+y)*width));
#if !PGMOUT
			if(formatSigned)
			{
				*(int16_t *)&img[windex] = get16bits11signed(alpha,(table%16),(table/16),index);
			}
			else
			{
				*(uint16_t *)&img[windex] = get16bits11bits(alpha,(table%16),(table/16),index);
			}
#else
			//make data compatible with the .pgm format. See the comment in compressBlockAlpha16() for details.
			uint16 uSixteen;
			if (formatSigned)
			{
				//the pgm-format only allows unsigned images,
				//so we add 2^15 to get a 16-bit value.
				uSixteen = get16bits11signed(alpha,(table%16),(table/16),index) + 256*128;
			}
			else
			{
				uSixteen = get16bits11bits(alpha,(table%16),(table/16),index);
			}
			//byte swap for pgm
			img[windex] = uSixteen/256;
			img[windex+1] = uSixteen%256;
#endif

		}
	}			
}


