#!/bin/sh
#
# Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
#
# Copyright (C) 2006-2018 Microchip Technology Inc. and its subsidiaries.
#
#
# Subject to your compliance with these terms, you may use Microchip
# software and any derivatives exclusively with Microchip products. 
# It is your responsibility to comply with third party license terms applicable 
# to your use of third party software (including open source software) that 
# may accompany Microchip software.
#
# THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
# WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, 
# INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, 
# AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE 
# LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL 
# LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE 
# SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE 
# POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
# ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY 
# RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
# THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

#********************* Configuration *********************
#********* Arguments *******************
COM_PORT='COM1'
COM_BAUDRATE='57600'
#********* Software path **************
DATAGET="`pwd`/../../PROGRAMS/WINDOWS/DATA_GET/DataGet.exe"
DATAEXTRACT="`pwd`/../../PROGRAMS/WINDOWS/DATA_EXTRACT/DataExtract.exe"
DATAPRINT="`pwd`/../../PROGRAMS/WINDOWS/DATA_PRINT/DataPrint.exe"
#*********************************************************

TEMP1_FILE="./temp1"
TEMP2_FILE="./temp2"

# To retreive data from the COM port
$DATAGET -f $TEMP1_FILE $COM_PORT $COM_BAUDRATE
# To extract them
$DATAEXTRACT $TEMP1_FILE "%i %f" 2 > $TEMP2_FILE
# And finaly print them
$DATAPRINT $TEMP2_FILE

rm $TEMP1_FILE 2> /dev/null
rm $TEMP2_FILE 2> /dev/null
