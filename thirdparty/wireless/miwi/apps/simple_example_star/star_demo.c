/**
* \file  star_demo.c
*
* \brief Demo Application for MiWi Star Implementation
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

/************************ HEADERS ****************************************/
#include "miwi_api.h"
#include "miwi_p2p.h"
#include "task.h"
#include "star_demo.h"
#include "mimem.h"
#include "asf.h"
#if defined(ENABLE_SLEEP_FEATURE)
#include "sleep_mgr.h"
#endif

#if defined(PROTOCOL_STAR)
    uint8_t i;
    uint8_t TxSynCount = 0;    // Maintain the Count on TX's Done
    uint8_t TxSynCount2 = 0; // Maintain the Count on TX's Done
    uint8_t TxNum = 0;         // Maintain the Count on TX's Done
    uint8_t RxNum = 0;         // Maintain the Count on RX's Done
    // check for selections made by USER
    bool chk_sel_status;
    uint8_t NumOfActiveScanResponse;
	bool update_ed;
	uint8_t select_ed;
	uint8_t msghandledemo = 0;
	extern uint8_t myChannel;
    
    bool display_connections;
    MIWI_TICK t1 , t2;

    static void dataConfcb(uint8_t handle, miwi_status_t status, uint8_t* msgPointer)
    {
        //printf("\nData Confirm: Handle: %d handle:%d", handle, status);
		MiMem_Free(msgPointer);
    }
	
	static void Connection_Confirm(miwi_status_t status)
	{
		//printf("\r\nConnect Operation Status: ");
		//printf("%d\n",status);
	}
    void run_star_demo(void)
    {
        t1.Val = MiWi_TickGet ();
        display_connections = true;
        while(display_connections)
        {
            {   
                P2PTasks();
#if defined(ENABLE_SLEEP_FEATURE)
				if (!role && Total_Connections())
				{
					if (P2PStatus.bits.Sleeping)
					{
						MiApp_TransceiverPowerState(POWER_STATE_WAKEUP_DR);
						//printf("\r\nDevice wokeup");
					}
					else
					{
						if(!(P2PStatus.bits.DataRequesting || P2PStatus.bits.RxHasUserData))
						{
							MiApp_TransceiverPowerState(POWER_STATE_SLEEP);
							sm_sleep((RFD_WAKEUP_INTERVAL - 2));
							//printf("\r\nDevice is sleeping");
						}
					}
				}
#endif
                /*******************************************************************/
                // If no packet received, now we can check if we want to send out
                // any information.
                // Function ButtonPressed will return if any of the two buttons
                // has been pushed.
                /*******************************************************************/
#if defined (CONF_BOARD_JOYSTICK)
            if(update_ed == true)
			{
				#if defined (ENABLE_LCD)
				LCD_Erase();
				if (myConnectionIndex_in_PanCo  == select_ed)
				{   // if END_device displays itself , "me" is added in display to denote itself
					snprintf(LCDText, sizeof(LCDText), "UP:%02d-%02x%02x%02x-me \nDOWN: Change node", END_DEVICES_Short_Address[select_ed].connection_slot,END_DEVICES_Short_Address[select_ed].Address[0],
					END_DEVICES_Short_Address[select_ed].Address[1],END_DEVICES_Short_Address[select_ed].Address[2] );
					LCD_Update();
				}
				else
				{
					snprintf(LCDText, sizeof(LCDText), "UP:%02d-%02x%02x%02x \nDOWN: Change node", END_DEVICES_Short_Address[select_ed].connection_slot,END_DEVICES_Short_Address[select_ed].Address[0],
					END_DEVICES_Short_Address[select_ed].Address[1],END_DEVICES_Short_Address[select_ed].Address[2] );
					LCD_Update();
				}
				snprintf(LCDText, sizeof(LCDText), "UP:%02d-%02x%02x%02x \nDOWN: Change node", END_DEVICES_Short_Address[select_ed].connection_slot,END_DEVICES_Short_Address[select_ed].Address[0],
				END_DEVICES_Short_Address[select_ed].Address[1],END_DEVICES_Short_Address[select_ed].Address[2]  );
				LCD_Update();
				#endif
			}
            uint8_t JoyStickAction = JoystickPressed();
            switch( JoyStickAction )
            {
	            case JOYSTICK_CENTER:
				{
                    chk_sel_status = true;
					select_ed = 0;
                    update_ed = true;
                    //User Selected Change end device
                    LCD_Erase();
                    if (myConnectionIndex_in_PanCo  == select_ed)
                    {   // if END_device displays itself , "me" is added in display to denote itself 
					    snprintf(LCDText, sizeof(LCDText), "UP:%02d-%02x%02x%02x-me \nDOWN: Change node", END_DEVICES_Short_Address[select_ed].connection_slot,END_DEVICES_Short_Address[select_ed].Address[0],
                        END_DEVICES_Short_Address[select_ed].Address[1],END_DEVICES_Short_Address[select_ed].Address[2] );
						LCD_Update();
                    }
                    else
                    {
					    snprintf(LCDText, sizeof(LCDText), "UP:%02d-%02x%02x%02x \nDOWN: Change node", END_DEVICES_Short_Address[select_ed].connection_slot,END_DEVICES_Short_Address[select_ed].Address[0],
                        END_DEVICES_Short_Address[select_ed].Address[1],END_DEVICES_Short_Address[select_ed].Address[2] );
						LCD_Update();
                    }
                    // Display another Peer Device Address
	                chk_sel_status = true;
				}
				break;

	            case JOYSTICK_UP:
				{
	                if(chk_sel_status)
	                {
                        bool sw_layer_ack_status , mac_ack_status;
                        uint8_t* dataPtr = NULL;
                        uint8_t dataLen = 0;
						update_ed = false;    // No Display of peer Device Info 
                        chk_sel_status = false;   // Selection Mode off

                        if (myConnectionIndex_in_PanCo == select_ed)
                        {
                            dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(MIWI_TEXT_LEN));
						    if (NULL == dataPtr)
						        return;
                            for (i = 0 ; i < 21 ; i++)
                            {
                                dataPtr[dataLen++] = (MiWi[(TxSynCount2%6)][i]);
                            }
							TxSynCount2++;

                            // IF on the demo , a END_Device displays its own Connection Detail
                            // We unicast data packet to just PAN COR , No forwarding
                            mac_ack_status = MiApp_SendData(LONG_ADDR_LEN, ConnectionTable[0].Address, 
							dataLen, dataPtr, msghandledemo++, true, dataConfcb);
                            if (mac_ack_status)
                              TxNum++;
                        }
						else
                        { 
                            // Data can be sent at a time from one END_DEVICE_TO_ANOTHER 
                            // Edx --> Pan CO --> EDy
                            // To forward a Packet from one ED to another ED , the first 4 bytes should holding
                            // a CMD and end dest device short address (3 bytes)
                            dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(4 + MIWI_TEXT_LEN));
						    if (NULL == dataPtr)
						        return;
                            dataPtr[dataLen++] = CMD_FORWRD_PACKET;
                            dataPtr[dataLen++] = (END_DEVICES_Short_Address[select_ed].Address[0]);// sending the first byte payload as the destination nodes
                            dataPtr[dataLen++] = (END_DEVICES_Short_Address[select_ed].Address[1]);// sending the first byte payload as the destination nodes
                            dataPtr[dataLen++] = (END_DEVICES_Short_Address[select_ed].Address[2]);// sending the first byte payload as the destination nodes
                            for (i = 4 ; i < 25 ; i++)
                            {
                                dataPtr[dataLen++] = (MiWi[(TxSynCount%6)][i-4]);
                            }
							TxSynCount++;
                            #if defined(ENABLE_SECURITY)
                                sw_layer_ack_status = MiApp_UnicastStar (dataLen, dataPtr, true);
                            #else
                                sw_layer_ack_status = MiApp_UnicastStar (dataLen, dataPtr, false);
                            #endif

                            #if defined(ENABLE_APP_LAYER_ACK)
                            if (sw_layer_ack_status)
                            {
                                TxNum++;     // Tx was successful
                            }
                            else
                            {
                                LCDDisplay((char *)"Data_Sending_Fail!!", 0, false); 
                            }
                            #else    
                            TxNum++;
                            #endif
                        }
                        DemoOutput_UpdateTxRx(TxNum, RxNum);
                        delay_ms(100);  // Delay for Display
                        STAR_DEMO_OPTIONS_MESSAGE (role);
	                }
				}

	            break;

	            case JOYSTICK_DOWN:
				{
				    if(chk_sel_status)
                    {
                    	if (select_ed > end_nodes-1)  // Reset Peer Device Info
                        {    
                        	// If end of Peer Device Info reset the count
                        	select_ed = 0;    
                        }
                        else
                        {
                        	// New device Information 
                         	select_ed = select_ed+1;
                        }
					}
				}
	            break;

				default:
				break;
			}
#endif
                uint8_t PressedButton = ButtonPressed();
                if ( PressedButton == 1 || PressedButton == 2)
                {

                    if (role)
                    {   
                        /*******************************************************************/ 
                        // Button 1 pressed. We need to send out the bitmap of word "MiWi".
                        // First call MiApp_FlushTx to reset the Transmit buffer. Then fill 
                        // the buffer one byte by one byte by calling function 
                        // MiApp_WriteData
                        /*******************************************************************/

                        uint8_t* dataPtr = NULL;
                        uint8_t dataLen = 0;
                        uint16_t broadcastAddress = 0xFFFF;
                        dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(MIWI_TEXT_LEN));
						if (NULL == dataPtr)
						    return;
                        for(i = 0; i < 21; i++)
                        { //Tx_Buffer User Data
                            dataPtr[dataLen++] = (MiWi[(TxSynCount%6)][i]);
                        }
                        TxSynCount++;


                        /*******************************************************************/
                        // Function MiApp_BroadcastPacket is used to broadcast a message
                        //    The only parameter is the boolean to indicate if we need to
                        //       secure the frame
                        /*******************************************************************/
                        MiApp_SendData(SHORT_ADDR_LEN, (uint8_t *)&broadcastAddress, 
						dataLen, dataPtr, msghandledemo++, true, dataConfcb);

                        DemoOutput_UpdateTxRx(++TxNum, RxNum);

                    }
                    else
                    {

                        select_ed = 0;
                        chk_sel_status = true;
                        update_ed = true;
                        while(update_ed == true)
                        {
                            //User Selected Change end device
#if defined (ENABLE_LCD)
                            LCD_Erase();
                            if (myConnectionIndex_in_PanCo  == select_ed)
                            {   // if END_device displays itself , "me" is added in display to denote itself 
						            snprintf(LCDText, sizeof(LCDText), "SW0:%02d-%02x%02x%02x-me \nBUTTON1: Change node", END_DEVICES_Short_Address[select_ed].connection_slot,END_DEVICES_Short_Address[select_ed].Address[0],
                                        END_DEVICES_Short_Address[select_ed].Address[1],END_DEVICES_Short_Address[select_ed].Address[2] );
						            LCD_Update();
                            }
                            else
                            {
						            snprintf(LCDText, sizeof(LCDText), "SW0:%02d-%02x%02x%02x \nBUTTON1: Change node", END_DEVICES_Short_Address[select_ed].connection_slot,END_DEVICES_Short_Address[select_ed].Address[0],
                                        END_DEVICES_Short_Address[select_ed].Address[1],END_DEVICES_Short_Address[select_ed].Address[2] );
						            LCD_Update();
                            }
						    //snprintf(LCDText, sizeof(LCDText), "SW0:%02d-%02x%02x%02x \nBUTTON1: Change node", END_DEVICES_Short_Address[select_ed].connection_slot,END_DEVICES_Short_Address[select_ed].Address[0],
                              //          END_DEVICES_Short_Address[select_ed].Address[1],END_DEVICES_Short_Address[select_ed].Address[2]  );
						            LCD_Update();
#endif									
                            chk_sel_status = true;

                            while(chk_sel_status)
                                {
                                    // Check for Button Press on Dev Kits
                                    uint8_t switch_val = ButtonPressed();
                                    // While waiting in TX , RX will process if any message was available
                                    P2PTasks ();   //MATEMP
                                    bool sw_layer_ack_status , mac_ack_status;
                                    if(switch_val == 1)   // Send Packet
                                    {
                                        uint8_t* dataPtr = NULL;
                                        uint8_t dataLen = 0;
                                        update_ed = false;    // No Display of peer Device Info 
                                        chk_sel_status = false;   // Selection Mode off
                                        
                                        if (myConnectionIndex_in_PanCo == select_ed)
                                        {
                                            dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(21));
							                if (NULL == dataPtr)
								                return;
                                            for (i = 0 ; i < 21 ; i++)
                                            {
                                                dataPtr[dataLen++] = (MiWi[(TxSynCount%6)][i]);
                                            }
											TxSynCount++;

                                            // IF on the demo , a END_Device displays its own Connection Detail
                                            // We unicast data packet to just PAN COR , No forwarding
                                            mac_ack_status = MiApp_SendData(LONG_ADDR_LEN, ConnectionTable[0].Address, 
											dataLen, dataPtr, msghandledemo++, true, dataConfcb);
                                            if (mac_ack_status)
                                              TxNum++;
                                        }

                                        else
                                        { 
                                            // Data can be sent at a time from one END_DEVICE_TO_ANOTHER 
                                            // Edx --> Pan CO --> EDy
                                            // To forward a Packet from one ED to another ED , the first 4 bytes should holding
                                            // a CMD and end dest device short address (3 bytes)
                                            dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(4 + MIWI_TEXT_LEN));
							                if (NULL == dataPtr)
								                return;
                                            dataPtr[dataLen++] = (CMD_FORWRD_PACKET);
                                            dataPtr[dataLen++] = (END_DEVICES_Short_Address[select_ed].Address[0]);// sending the first byte payload as the destination nodes
                                            dataPtr[dataLen++] = (END_DEVICES_Short_Address[select_ed].Address[1]);// sending the first byte payload as the destination nodes
                                            dataPtr[dataLen++] = (END_DEVICES_Short_Address[select_ed].Address[2]);// sending the first byte payload as the destination nodes
                                            for (i = 4 ; i < 25 ; i++)
                                            {
                                                dataPtr[dataLen++] = (MiWi[(TxSynCount%6)][i-4]);
                                            }
											TxSynCount++;
                                            #if defined(ENABLE_SECURITY)
                                                sw_layer_ack_status = MiApp_UnicastStar (dataLen, dataPtr, true);
                                            #else
                                                sw_layer_ack_status = MiApp_UnicastStar (dataLen, dataPtr, false);
                                            #endif


                                            #if defined(ENABLE_APP_LAYER_ACK)
                                                if (sw_layer_ack_status)
                                                {
                                                    TxNum++;     // Tx was successful
                                                }
                                                else
                                                {
                                                    #if defined (ENABLE_LCD)
													LCDDisplay((char *)"Data_Sending_Fail!!", 0, false); 
													#endif
                                                }
                                            #else    
                                                TxNum++;
                                            #endif

                                        }
                                        DemoOutput_UpdateTxRx(TxNum, RxNum);
                                        delay_ms(1000);  // Delay for Display
                                        STAR_DEMO_OPTIONS_MESSAGE (role);
                                    }
                                    else if(switch_val == 2)   // SWITCH to new peer Device Info
                                    {
                                        if (select_ed > end_nodes-1)  // Reset Peer Device Info
                                        {    
                                            // If end of Peer Device Info reset the count
                                            select_ed = 0;    
                                        }
                                        else
                                        {
                                            // New device Information 
                                            select_ed = select_ed+1;
                                        }
                                        chk_sel_status = false;

                                    }  // end of SWITCH to new peer Device INFO
                                } // end of Selection of Peer Device 

                        } // END of Display of PEER DEVICE Info         

                    }// end of END_DEVICE send packet option
                } // end of options on button press
            } // end of TX DEMO for PAN CO and END DEVICE

            // If a End devices looses connection with PAN Co

            if (lost_connection && !role)
            {
                MiApp_EstablishConnection(myChannel, 0, NULL, 0, Connection_Confirm);
                lost_connection = false;

            }
            
            t2.Val = MiWi_TickGet ();
            // Display the no of End devices in Network
            if( MiWi_TickGetDiff(t2, t1) > (SHARE_PEER_DEVICE_INFO_TIMEOUT) && role)
            {
#if defined (ENABLE_LCD)
                LCD_Erase();
				snprintf(LCDText, sizeof(LCDText), "END_DEVICES :%02d",conn_size);
				LCD_Update();
#endif
                delay_ms(1000);
                display_connections = false;
                STAR_DEMO_OPTIONS_MESSAGE (true);  // Pan Co

            }

        }
    }
#endif

/*********************************************************************
* Function: void ReceivedDataIndication (RECEIVED_MESSAGE *ind)
*
* Overview: Process a Received Message
*
* PreCondition: MiApp_ProtocolInit
*
* Input:  None
*
* Output: None
*
********************************************************************/
void ReceivedDataIndication (RECEIVED_MESSAGE *ind)
{
    /*******************************************************************/
    // If a packet has been received, handle the information available 
    // in rxMessage.
    /*******************************************************************/
    #if defined(ENABLE_CONSOLE)
        DemoOutput_HandleMessage();
    #endif

    DemoOutput_UpdateTxRx(TxNum, ++RxNum);
    delay_ms(1000);
    // Toggle LED2 to indicate receiving a packet.
	LED_Toggle(LED0);
    // Display the Src End Device Info on reception msg
    // We do not display if it is a PAN CO or if the message
    // received was a broadcast packet
    if (!role && !rxMessage.flags.bits.broadcast)
    {
        Source_END_DEVICE_INFO(rxMessage.Payload);
    }
    STAR_DEMO_OPTIONS_MESSAGE (role);
}