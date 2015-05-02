/*
 * test.c
 *
 *  Created on: May 26, 2012
 *      Author: pyang
 */
#include "FAT.h"
FATFS fatfs[_DRIVES];
FIL file1, file2;		/* File objects */
UINT s2;				/* not used */
void put_rc (FRESULT rc)
{
	const char *p;
	static const char str[] =
		"OK\0" "NOT_READY\0" "NO_FILE\0" "FR_NO_PATH\0" "INVALID_NAME\0" "INVALID_DRIVE\0"
		"DENIED\0" "EXIST\0" "RW_ERROR\0" "WRITE_PROTECTED\0" "NOT_ENABLED\0"
		"NO_FILESYSTEM\0" "INVALID_OBJECT\0" "MKFS_ABORTED\0";
	FRESULT i;

	for (p = str, i = 0; i != rc && *p; i++) {
		while(*p++);
	}
	xprintf("\nrc=%u FR_%s", (UINT)rc, p);
}
void initSDCard()//fi 0 (force initialize 0)
{
	long p1=0;
	put_rc(f_mount((BYTE)p1, &fatfs[p1]));

}
void SDOpenSnapshot()
{
	put_rc(f_unlink("snapshot.jpg"));
	long p1=7;				/* read(1) write(2) create(4)*/
	put_rc(f_open(&file2, "snapshot.jpg", (BYTE)p1));
}
void SDOpenVideo()
{
	put_rc(f_unlink("size.bin"));
	put_rc(f_unlink("video.bin"));
	long p1=7;				/* read(1) write(2) create(4)*/
	put_rc(f_open(&file1, "size.bin", (BYTE)p1));
	put_rc(f_open(&file2, "video.bin", (BYTE)p1));
}
void writeToSDCard(int size)
{
	//BYTE *data = (BYTE*)0x80000000;
	BYTE *data = (BYTE*)0x40004000;
	f_write(&file1, &size, sizeof(int), &s2);
	while (size>512)	{
		f_write(&file2, data, 512, &s2);
		data+=512;
		size=size-512;
	}
	f_write(&file2, data, size, &s2);

}
void closeFile()
{
	put_rc(f_close(&file1));
	put_rc(f_close(&file2));
}

