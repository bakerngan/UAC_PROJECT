/*
 * test.h
 *
 *  Created on: May 26, 2012
 *      Author: pyang
 */
#include "diskio.h"
#include "ff.h"
#include "monitor.h"
#include <stdio.h>
void put_rc(FRESULT);
void initSDCard(void);
void writeToSDCard(int);
void closeFile(void);
void SDOpenSnapshot(void);
void SDOpenVideo(void);
