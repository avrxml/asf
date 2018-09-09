/**
* \file  miwi_config_p2p.h
*
* \brief Configuration for MiWi P2P Protocol
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
*
* \asf_license_start
*
* \page License
*
* Subject to your compliance with these terms, you may use Microchip
* software and any derivatives exclusively with Microchip products. 
* It is your responsibility to comply with third party license terms applicable 
* to your use of third party software (including open source software) that 
* may accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
* WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, 
* INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, 
* AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE 
* LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL 
* LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE 
* SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE 
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY 
* RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
* \asf_license_stop
*
*/
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

#ifndef __CONFIGURE_P2P_H

    #define __CONFIGURE_P2P_H
    
    #include "miwi_config.h"
    #include "sysTimer.h"

        /*********************************************************************/
        // ENABLE_DUMP will enable the stack to be able to print out the 
        // content of the P2P connection entry. It is useful in the debugging
        // process
        /*********************************************************************/
        #define ENABLE_DUMP
        
        
        /*********************************************************************/
        // RFD_DATA_WAIT is the timeout defined for sleeping device to receive 
        // a message from the associate device after Data Request. After this
        // timeout, the RFD device can continue to operate and then go to 
        // sleep to conserve battery power.
        /*********************************************************************/
        #define RFD_DATA_WAIT                   0x00003FFF
        
        
        /*********************************************************************/
        // CONNECTION_RETRY_TIMES is the maximum time that the wireless node
        // can try to establish a connection. Once the retry times are exhausted
        // control will be return to application layer to decide what to do next
        /*********************************************************************/
        #define CONNECTION_RETRY_TIMES          3
    
    
        /*********************************************************************/
        // CONNECTION_INTERVAL defines the interval in second between two 
        // connection request. 
        /*********************************************************************/
        #define CONNECTION_INTERVAL             2


        /*********************************************************************/
        // FA_BROADCAST_TIME defines the total number of times to broadcast
        // the channel hopping message to the rest of PAN, before the 
        // Frequency Agility initiator jump to the new channel
        /*********************************************************************/
        #define FA_BROADCAST_TIME           0x03
    
    
        /*********************************************************************/
        // RESYNC_TIMES defines the maximum number of times to try resynchronization
        // in all available channels before hand over the control to the application
        // layer
        /*********************************************************************/
        #define RESYNC_TIMES                0x03


        /*********************************************************************/
        // ACTIVE_SCAN_RESULT_SIZE defines the maximum active scan result
        // that the stack can hold. If active scan responses received exceed
        // the definition of ACTIVE_SCAN_RESULT_SIZE, those later active scan
        // responses will be discarded
        /*********************************************************************/
        #define ACTIVE_SCAN_RESULT_SIZE     4
    
    
        /*********************************************************************/
        // INDIRECT_MESSAGE_SIZE defines the maximum number of packets that
        // the device can store for the sleeping device(s)
        /*********************************************************************/
        #define INDIRECT_MESSAGE_SIZE   2
            
            
        /*********************************************************************/
        // INDIRECT_MESSAGE_TIMEOUT defines the timeout interval in seconds
        // for the stored packets for sleeping devices
        /*********************************************************************/
        #define INDIRECT_MESSAGE_TIMEOUT (ONE_SECOND * RFD_WAKEUP_INTERVAL * (INDIRECT_MESSAGE_SIZE + 1))
        
        
        /*********************************************************************/
        // ENABLE_ENHANCED_DATA_REQUEST enables the Enhanced Data Request 
        // feature of P2P stack. It combines the message that is send from
        // the sleeping device with Data Request command upon wakeup, to save
        // 20% - 30% active time for sleeping device, thus prolong the battery
        // life.
        /*********************************************************************/
        //#define ENABLE_ENHANCED_DATA_REQUEST
        
        
        /*********************************************************************/
        // ENABLE_TIME_SYNC enables the Time Synchronizaiton feature of P2P
        // stack. It allows the FFD to coordinate the check-in interval of
        // sleeping device, thus allow one FFD to connect to many sleeping
        // device. Once Time Synchronization feature is enabled, following
        // parameters are also required to be defined:
        //      TIME_SYNC_SLOTS
        //      COUNTER_CRYSTAL_FREQ
        /*********************************************************************/
        //#define ENABLE_TIME_SYNC
        
        
        /*********************************************************************/
        // TIME_SYNC_SLOTS defines the total number of time slot available 
        // within one duty cycle. As a rule, the number of time slot must be
        // equal or higher than the total number of sleeping devices that are
        // connected to the FFD, so that each sleeping device can be assigned
        // to a time slot. The time slot period is calcualted by following 
        // formula:
        //      Time Slot Period = RFD_WAKEUP_INTERVAL / TIME_SYNC_SLOTS
        // The length of time slot period depends on the primary oscillator
        // accuracy on the FFD as well as the 32KHz crystal accuracy on sleeping
        // devices. 
        // The definition of TIME_SYNC_SLOTS is only valid if ENABLE_TIME_SYNC
        // is defined
        /*********************************************************************/
        #define TIME_SYNC_SLOTS         10


        /*********************************************************************/
        // COUNTER_CRYSTAL_FREQ defines the frequency of the crystal that 
        // is connected to the MCU counter to perform timing functionality
        // when MCU is in sleep. 
        /*********************************************************************/
        #define COUNTER_CRYSTAL_FREQ    32768
    
    
#endif

