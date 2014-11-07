#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* map of GBK to IBM-1388, basic ASCII to EBCDIC part */
/* only 126 ASCII chars valid */
char A2E[129] =
	"\x00\x01\x02\x03\x37\x2D\x2E\x2F\x16\x05\x25\x0B\x0C\x0D\x00\x00"
	"\x10\x11\x12\x13\x3C\x3D\x32\x26\x18\x19\x3F\x27\x1C\x1D\x1E\x1F"
	"\x40\x5A\x7F\x7B\xE0\x6C\x50\x7D\x4D\x5D\x5C\x4E\x6B\x60\x4B\x61"
	"\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\x7A\x5E\x4C\x7E\x6E\x6F"
	"\x7C\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xD1\xD2\xD3\xD4\xD5\xD6"
	"\xD7\xD8\xD9\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xBA\xB2\xBB\xB0\x6D"
	"\x79\x81\x82\x83\x84\x85\x86\x87\x88\x89\x91\x92\x93\x94\x95\x96"
	"\x97\x98\x99\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xC0\x4F\xD0\xA0\x07";

/* map char array of GBK to IBM-1388 */
unsigned char *c_mgi;

/*-----------------------------------------------------------------------------+
| gbk_to_ibm1388                                                               |
+------------------------------------------------------------------------------+
|   Function to convert GBK encoded string to IBM-1388                         |
|                                                                              |
|   Function arguments                                                         |
|     unsigned char *gbk - pre-allocated GBK encoded source string             |
|     unsigned char *ibm - pre-allocated IBM-1388 encoded output string        |
|                                                                              |
|   Return code                                                                |
|     >= 0 - Output string length                                              |
|       -1 - Invalid source string                                             |
+-----------------------------------------------------------------------------*/
int gbk_to_ibm1388(unsigned char *gbk, unsigned char *ibm)
{
	size_t offset;
	size_t cnt = 0;
	int is_last_single = 1; /* Yes(1), No(0) */
	unsigned char *c = gbk;

	while (*c)
	{
		if (!(*c & 0x80))
		{
			/* End of double byte */
			if (!is_last_single)
			{
				is_last_single = 1; /* Yes */
				ibm[cnt++] = 0x0F;
			}

			if (*c == 0x0E || *c == 0x0F)
			{
				/* Invalid GBK string */
				return -1;
			}

			ibm[cnt++] = A2E[*c++];
		}
		else
		{
			/* check high byte */
			if (*c < 0x81 || *c > 0xFE)
			{
				/* Invalid GBK string */
				return -1;
			}

			/* calculate base offset by high byte */
			offset = (*c++ - 0x81) * 0xBF;

			/* check low byte */
			if (*c < 0x40 || *c == 0x7F || *c > 0xFE)
			{
				/* Invalid GBK string */
				return -1;
			}

			/* calculate exact offset by low byte */
			offset += *c++ - 0x40;
			offset *= 2;

			/* Start of double byte */
			if (is_last_single)
			{
				is_last_single = 0;  /* No */
				ibm[cnt++] = 0x0E;
			}

			ibm[cnt++] = c_mgi[offset++];
			ibm[cnt++] = c_mgi[offset];
		}
	}

	/* End of double byte */
	if (!is_last_single) ibm[cnt++] = 0x0F;

	/* Finish string */
	ibm[cnt] = '\0';

	return cnt;
}

int test_valid()
{
	int length;
	unsigned char result[100];

	length = gbk_to_ibm1388((unsigned char *)"1234567890", result);
	if (length > 0) printf("%s\x0D", result);

	length = gbk_to_ibm1388((unsigned char *)"~!@#$%^&*()_+`-=[];',./{}:<>?|", result);
	if (length > 0) printf("%s\x0D", result);

	length = gbk_to_ibm1388((unsigned char *)"abcdefghijklmnopqrstuvwxyz", result);
	if (length > 0) printf("%s\x0D", result);

	length = gbk_to_ibm1388((unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZ", result);
	if (length > 0) printf("%s\x0D", result);

	length = gbk_to_ibm1388((unsigned char *)"≤‚ ‘≤‚ ‘≤‚ ‘", result);
	if (length > 0) printf("%s\x0D", result);

	length = gbk_to_ibm1388((unsigned char *)"*≤‚* ‘*≤‚* ‘*≤‚* ‘*", result);
	if (length > 0) printf("%s\x0D", result);

	length = gbk_to_ibm1388((unsigned char *)"≤‚ ‘***≤‚ ‘***≤‚ ‘", result);
	if (length > 0) printf("%s\x0D", result);

	length = gbk_to_ibm1388((unsigned char *)"***≤‚ ‘≤‚ ‘≤‚ ‘***", result);
	if (length > 0) printf("%s\x0D", result);

	length = gbk_to_ibm1388((unsigned char *)"***≤‚ ‘≤‚ ‘≤‚ ‘", result);
	if (length > 0) printf("%s\x0D", result);

	length = gbk_to_ibm1388((unsigned char *)"≤‚ ‘≤‚ ‘≤‚ ‘***", result);
	if (length > 0) printf("%s\x0D", result);

	length = gbk_to_ibm1388((unsigned char *)"***≤‚ ‘***≤‚ ‘***≤‚ ‘***", result);
	if (length > 0) printf("%s\x0D", result);

	length = gbk_to_ibm1388((unsigned char *)"***≤‚*** ‘≤‚ ‘≤‚*** ‘***", result);
	if (length > 0) printf("%s\x0D", result);

	return 0;
}

int test_invalid()
{
	int length;
	unsigned char result[100];

	length = gbk_to_ibm1388((unsigned char *)"≤", result);
	if (length < 0) printf("1 Invalid!\n");

	length = gbk_to_ibm1388((unsigned char *)"≤***", result);
	if (length < 0) printf("2 Invalid!\n");

	length = gbk_to_ibm1388((unsigned char *)"***≤", result);
	if (length < 0) printf("3 Invalid!\n");

	length = gbk_to_ibm1388((unsigned char *)"≤‚ ‘≤‚ ‘≤‚ ", result);
	if (length < 0) printf("4 Invalid!\n");

	length = gbk_to_ibm1388((unsigned char *)"≤‚ ‘≤‚ ‘ ‚ ‘", result);
	if (length < 0) printf("5 Invalid!\n");

	length = gbk_to_ibm1388((unsigned char *)"≤‚ ‘≤‚ ‘‚ ‘", result);
	if (length < 0) printf("6 Invalid!\n");

	length = gbk_to_ibm1388((unsigned char *)"Ä‚ ‘≤‚ ‘≤‚ ‘", result);
	if (length < 0) printf("7 Invalid!\n");

	length = gbk_to_ibm1388((unsigned char *)"≤9 ‘≤‚ ‘≤‚ ‘", result);
	if (length < 0) printf("8 Invalid!\n");

	length = gbk_to_ibm1388((unsigned char *)"≤ ‘≤‚ ‘≤‚ ‘", result);
	if (length < 0) printf("9 Invalid!\n");

	return 0;
}

int test_perf()
{
	int i;
	unsigned char result[200];

	for (i = 0; i < 10000000; i++)
	{
		gbk_to_ibm1388
		(
			(unsigned char *)
				"~!@#$%^&*()_+`-=[];',./{}:<>?|1234567890-=][';/."
				"≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘"
				"≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘",
			result
		);
	}

	return 0;
}

int main()
{
	/* map file handle of GBK to IBM-1388 */
	FILE *f_mgi;

	/* Open GBK to IBM-1388 map */
	f_mgi = fopen("gbkto1388.map", "rb");
	if (f_mgi == NULL) exit(1);

	/* Allocate buffer for map */
	c_mgi = (unsigned char *)malloc(sizeof(unsigned char) * 48132);
	if (c_mgi == NULL) exit(1);

	/* Read map to memory */
	size_t cnt = fread(c_mgi, sizeof(unsigned char), 48132, f_mgi);
	if (cnt != 48132) exit(1);

	/* Close GBK to IBM-1388 map */
	fclose(f_mgi);

	test_valid();
	/* test_invalid(); */
	/* test_perf(); */

	/* Free map memory */
	free(c_mgi);

	return 0;
}
