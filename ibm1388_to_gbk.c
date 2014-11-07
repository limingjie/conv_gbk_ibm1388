#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* map of IBM-1388 to GBK, basic EBCDIC to ASCII part */
/* only 126 EBCDIC chars valid */
char E2A[257] =
	"\x00\x01\x02\x03\x00\x09\x00\x7F\x00\x00\x00\x0B\x0C\x0D\x00\x00"
	"\x10\x11\x12\x13\x00\x00\x08\x00\x18\x19\x00\x00\x1C\x1D\x1E\x1F"
	"\x00\x00\x00\x00\x00\x0A\x17\x1B\x00\x00\x00\x00\x00\x05\x06\x07"
	"\x00\x00\x16\x00\x00\x00\x00\x04\x00\x00\x00\x00\x14\x15\x00\x1A"
	"\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2E\x3C\x28\x2B\x7C"
	"\x26\x00\x00\x00\x00\x00\x00\x00\x00\x00\x21\x00\x2A\x29\x3B\x00"
	"\x2D\x2F\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2C\x25\x5F\x3E\x3F"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x60\x3A\x23\x40\x27\x3D\x22"
	"\x00\x61\x62\x63\x64\x65\x66\x67\x68\x69\x00\x00\x00\x00\x00\x00"
	"\x00\x6A\x6B\x6C\x6D\x6E\x6F\x70\x71\x72\x00\x00\x00\x00\x00\x00"
	"\x7E\x00\x73\x74\x75\x76\x77\x78\x79\x7A\x00\x00\x00\x00\x00\x00"
	"\x5E\x00\x5C\x00\x00\x00\x00\x00\x00\x00\x5B\x5D\x00\x00\x00\x00"
	"\x7B\x41\x42\x43\x44\x45\x46\x47\x48\x49\x00\x00\x00\x00\x00\x00"
	"\x7D\x4A\x4B\x4C\x4D\x4E\x4F\x50\x51\x52\x00\x00\x00\x00\x00\x00"
	"\x24\x00\x53\x54\x55\x56\x57\x58\x59\x5A\x00\x00\x00\x00\x00\x00"
	"\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x00\x00\x00\x00\x00\x00";

/* map char array of IBM-1388 to GBK */
unsigned char *c_mig;

/*-----------------------------------------------------------------------------+
| ibm1388_to_gbk                                                               |
+------------------------------------------------------------------------------+
|   Function to convert IBM-1388 encoded string to GBK                         |
|                                                                              |
|   Function arguments                                                         |
|     unsigned char *ibm - pre-allocated IBM-1388 encoded source string        |
|     unsigned char *gbk - pre-allocated GBK encoded output string             |
|                                                                              |
|   Return code                                                                |
|     >= 0 - Output string length                                              |
|       -1 - Invalid source string                                             |
+-----------------------------------------------------------------------------*/
int ibm1388_to_gbk(unsigned char *ibm, unsigned char *gbk)
{
	size_t offset;
	size_t g_cnt = 0;
	int is_single = 1; /* Yes(1), No(0) */
	unsigned char *c = ibm;

	while (*c)
	{
		if (is_single)
		{
			/* start of double byte */
			if (*c == 0x0E)
			{
				is_single = 0;
				c++;
				continue;
			}

			/* check current char */
			if (E2A[*c] == 0x00 && *c != 0x00)
			{
				/* Invalid IBM-1388 string */
				return -1;
			}

			gbk[g_cnt++] = E2A[*c++];
		}
		else
		{
			/* end of double byte */
			if (*c == 0x0F)
			{
				is_single = 1;
				c++;
				continue;
			}

			/* check high byte */
			if (*c < 0x40 || *c > 0xFF)
			{
				/* Invalid IBM-1388 string */
				return -1;
			}

			/* calculate base offset by high byte */
			offset = (*c++ - 0x40) * 0xc0;

			/* check low byte */
			if (*c < 0x40 || *c > 0xFF)
			{
				/* Invalid IBM-1388 string */
				return -1;
			}

			/* calculate exact offset by low byte */
			offset += *c++ - 0x40;
			offset *= 2;

			gbk[g_cnt++] = c_mig[offset++];
			gbk[g_cnt++] = c_mig[offset];
		}
	}

	/* Error end of string, missed 0x0F */
	if (!is_single) return -1;

	gbk[g_cnt] = '\0';

	return g_cnt;
}

int test_valid()
{
	int rc;
	unsigned char gbk[200];

	rc = ibm1388_to_gbk((unsigned char *)"\x0E\x0F", gbk);
	printf("%d - %s\n", rc, gbk);
	rc = ibm1388_to_gbk((unsigned char *)"\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9", gbk);
	printf("%d - %s\n", rc, gbk);
	rc = ibm1388_to_gbk((unsigned char *)"\xa0\x5a\x7c\x7b\xe0\x6c\xb0\x50\x5c\x4d\x5d\x6d\x4e\x79\x60\x7e\xba\xbb\x5e\x7d\x6b\x4b\x61\xc0\xd0\x7a\x4c\x6e\x6f\x4f", gbk);
	printf("%d - %s\n", rc, gbk);
	rc = ibm1388_to_gbk((unsigned char *)"\x81\x82\x83\x84\x85\x86\x87\x88\x89\x91\x92\x93\x94\x95\x96\x97\x98\x99\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9", gbk);
	printf("%d - %s\n", rc, gbk);
	rc = ibm1388_to_gbk((unsigned char *)"\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9", gbk);
	printf("%d - %s\n", rc, gbk);
	rc = ibm1388_to_gbk((unsigned char *)"\x0e\x49\xe1\x55\xd3\x49\xe1\x55\xd3\x49\xe1\x55\xd3\x0f", gbk);
	printf("%d - %s\n", rc, gbk);
	rc = ibm1388_to_gbk((unsigned char *)"\x5c\x0e\x49\xe1\x0f\x5c\x0e\x55\xd3\x0f\x5c\x0e\x49\xe1\x0f\x5c\x0e\x55\xd3\x0f\x5c\x0e\x49\xe1\x0f\x5c\x0e\x55\xd3\x0f\x5c", gbk);
	printf("%d - %s\n", rc, gbk);
	rc = ibm1388_to_gbk((unsigned char *)"\x0e\x49\xe1\x55\xd3\x0f\x5c\x5c\x5c\x0e\x49\xe1\x55\xd3\x0f\x5c\x5c\x5c\x0e\x49\xe1\x55\xd3\x0f", gbk);
	printf("%d - %s\n", rc, gbk);
	rc = ibm1388_to_gbk((unsigned char *)"\x5c\x5c\x5c\x0e\x49\xe1\x55\xd3\x49\xe1\x55\xd3\x49\xe1\x55\xd3\x0f\x5c\x5c\x5c", gbk);
	printf("%d - %s\n", rc, gbk);
	rc = ibm1388_to_gbk((unsigned char *)"\x5c\x5c\x5c\x0e\x49\xe1\x55\xd3\x49\xe1\x55\xd3\x49\xe1\x55\xd3\x0f", gbk);
	printf("%d - %s\n", rc, gbk);
	rc = ibm1388_to_gbk((unsigned char *)"\x0e\x49\xe1\x55\xd3\x49\xe1\x55\xd3\x49\xe1\x55\xd3\x0f\x5c\x5c\x5c", gbk);
	printf("%d - %s\n", rc, gbk);
	rc = ibm1388_to_gbk((unsigned char *)"\x5c\x5c\x5c\x0e\x49\xe1\x55\xd3\x0f\x5c\x5c\x5c\x0e\x49\xe1\x55\xd3\x0f\x5c\x5c\x5c\x0e\x49\xe1\x55\xd3\x0f\x5c\x5c\x5c", gbk);
	printf("%d - %s\n", rc, gbk);
	rc = ibm1388_to_gbk((unsigned char *)"\x5c\x5c\x5c\x0e\x49\xe1\x0f\x5c\x5c\x5c\x0e\x55\xd3\x49\xe1\x55\xd3\x49\xe1\x0f\x5c\x5c\x5c\x0e\x55\xd3\x0f\x5c\x5c\x5c", gbk);
	printf("%d - %s\n", rc, gbk);

	return 0;
}

int test_invalid()
{
	int rc;
	unsigned char gbk[200];

	rc = ibm1388_to_gbk((unsigned char *)"\x0E", gbk);
	if (rc < 0) printf("1 Invalid!\n");

	rc = ibm1388_to_gbk((unsigned char *)"\x0E\xF0\xF1\xF2", gbk);
	if (rc < 0) printf("2 Invalid!\n");

	rc = ibm1388_to_gbk((unsigned char *)"\x0E\x0E", gbk);
	if (rc < 0) printf("3 Invalid!\n");

	rc = ibm1388_to_gbk((unsigned char *)"\x0F", gbk);
	if (rc < 0) printf("4 Invalid!\n");

	rc = ibm1388_to_gbk((unsigned char *)"\xF0\xF1\xF2\x0F", gbk);
	if (rc < 0) printf("5 Invalid!\n");

	rc = ibm1388_to_gbk((unsigned char *)"\x41", gbk);
	if (rc < 0) printf("6 Invalid!\n");

	rc = ibm1388_to_gbk((unsigned char *)"\x0e\x39\x81\x0f", gbk);
	if (rc < 0) printf("7 Invalid!\n");

	rc = ibm1388_to_gbk((unsigned char *)"\x0e\x49\xe1\x55\x0f", gbk);
	if (rc < 0) printf("8 Invalid!\n");

	rc = ibm1388_to_gbk((unsigned char *)"\x0e\x39\xe1\x55\xd3\x0f", gbk);
	if (rc < 0) printf("9 Invalid!\n");

	rc = ibm1388_to_gbk((unsigned char *)"\x0e\x49\x21\x55\xd3\x0f", gbk);
	if (rc < 0) printf("10 Invalid!\n");

	return 0;
}

int main()
{
	/* map file handle of IBM-1388 to GBK */
	FILE *f_mig;

	/* Open IBM-1388 to GBK map */
	f_mig = fopen("1388togbk.map", "rb");
	if (f_mig == NULL) exit(1);

	/* Allocate buffer for map */
	c_mig = (unsigned char *)malloc(sizeof(unsigned char) * 73728);
	if (c_mig == NULL) exit(1);

	/* Read map to memory */
	size_t cnt = fread(c_mig, sizeof(unsigned char), 73728, f_mig);
	if (cnt != 73728) exit(1);

	/* Close IBM-1388 to GBK map */
	fclose(f_mig);

	test_valid();
	test_invalid();

	free(c_mig);

	return 0;
}
