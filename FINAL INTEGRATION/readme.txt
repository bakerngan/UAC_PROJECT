
LPC2000 SD/MMC (MCI) interface and ChaN's FAT-module
----------------------------------------------------
done by Martin Thomas, Kaiserslautern, Germany
http://www.siwawi.arubi.uni-kl.de/avr_projects/arm_projects


--- This is a test-release, no warranties, use on you own risk ---

Based on ChaN's ffat, using some of NXP's example code "bundle" LPC23xx/24xx.

The code is prepared to be build with:

- arm-elf GNU Toolchain using a Makefile (tested with my WinARM 5/2007-testing,
  please visit my web-pages for futher information on WinARM)

- arm-elf GNU Toolchain called by uVision (tested with WinARM 5/2007, the 
  arm-glue-package and RV-MDK 3.11, please visit my web-pages for 
  further information on WinARM and the arm-glue package and keil.com
  for information on RV-MDK/uVision)
  Version 20080914 tested with uVision 3.23a and Codesourcery G++ lite Q1/2008
  without "arm-glue".

- uVision and RealView as in Keil's RV-MDK. This has only been tested
  with a "stripped down" code since I only have the evaluation-version 
  of the RV-MDK which has a 16kB size-limit. The project file fat_mci.Uv2 
  should be prepared to build the complete example but I could not test this.
  Update 20080914: uVisions limit has been raised to 32kB, but I have not
   done any tests with the Realview tools with the updated sources

TODO: use DMA-memory for fat- and file-buffers to avoid copying.

Version 20070914 Tested with NXP LPC2368 on Keil MCB2300 board (Vers.3) and
  a SanDisk 256MB SD-Card.
Version 20080914 Tested with NXP LPC2378 on IAR/Olimex LPC-2378-STK


Version History
---------------
20070914 - initial release "ChaN 0.05 / NXP 1.4 / mt 0.3"
20080716 - drivers/nxp/mci.h SPI prescale-values from hex to dez
20080914 - added Multi Block Write by Lorenz Aebi 2008
           update to Chan's ffat 0.06
           modifications for uVision 3.32a with CS G++ lite Q1/2008 (no "arm-glue")