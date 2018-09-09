/**
* \file  p2p_demo.c
*
* \brief Demo Application for MiWi P2P Implementation
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
#include "p2p_demo.h"
#include "mimem.h"
#include "asf.h"
#if defined(ENABLE_SLEEP_FEATURE)
#include "sleep_mgr.h"
#endif

#if defined(PROTOCOL_P2P)

    uint8_t i;
    uint8_t TxSynCount = 0;
    uint8_t TxSynCount2 = 0;
    uint8_t TxNum = 0;
    uint8_t RxNum = 0;
    bool chk_sel_status = true;  // Check _select_Status
    uint8_t NumOfActiveScanResponse;
    bool update_ed;
    uint8_t select_ed;
    uint8_t msghandledemo = 0;

    static void dataConfcb(uint8_t handle, miwi_status_t status, uint8_t* msgPointer);

    static void dataConfcb(uint8_t handle, miwi_status_t status, uint8_t* msgPointer)
    {
        //printf("\nData Confirm: Handle: %d handle:%d", handle, status); //MATEMP
		MiMem_Free(msgPointer);
    }
    void run_p2p_demo(void)
    {
#if defined(ENABLE_SLEEP_FEATURE)
        if (Total_Connections())
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
        {
            /*******************************************************************/
            // If no packet received, now we can check if we want to send out
            // any information.
            // Function ButtonPressed will return if any of the two buttons
            // has been pushed.
            /*******************************************************************/
#if defined (CONF_BOARD_JOYSTICK)
            uint8_t JoyStickAction = JoystickPressed();
            switch( JoyStickAction )
            {
	            case JOYSTICK_CENTER:
                    chk_sel_status = true;
					select_ed = 0;
                    update_ed = true;
	                //Peer Device Info
	                #if defined (ENABLE_LCD)
	                LCD_Erase();
					snprintf(LCDText, sizeof(LCDText), "UP  : %02d-%02x%02x%02x \nDOWN: Change node", select_ed,ConnectionTable[select_ed].Address[0],
					ConnectionTable[select_ed].Address[1],ConnectionTable[select_ed].Address[2]);
					LCD_Update();
	                #endif
	                // Display another Peer Device Address
	                chk_sel_status = true;
				break;

	            case JOYSTICK_UP:
	                if(chk_sel_status)
	                {
                        uint8_t* dataPtr = NULL;
                        uint8_t dataLen = 0;
                        update_ed = false;
                        chk_sel_status = false;

                        dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(DE_LEN));
						if (NULL == dataPtr)
						    return;
                        for(i = 0; i < 11; i++)
                        {
                            // Tx Buffer User Data
                            dataPtr[dataLen++] = DE[(TxSynCount2%6)][i];
                        }
                        TxSynCount2++;
                        if( MiApp_SendData(LONG_ADDR_LEN, ConnectionTable[select_ed].Address, dataLen, dataPtr, msghandledemo++, true, dataConfcb) == false )
                        {
                           DemoOutput_UnicastFail();
                        }
                        else
                        {
				            // Successful Transmission
				            TxNum++;
			            }
		                select_ed = 0;
			            // Update the LCD
			            DemoOutput_UpdateTxRx(TxNum, RxNum);

			            DemoOutput_Instruction();
		            }
	            break;

	            case JOYSTICK_DOWN:
				    if(chk_sel_status)
                    {
			            if (select_ed > conn_size-2)
			            {
				            // Last Peer Device
				            select_ed = 0;
			            }
			            else
			            {
				            // Update the Display
				            select_ed = select_ed+1;
			            }
						#if defined (ENABLE_LCD)
	                    LCD_Erase();
						snprintf(LCDText, sizeof(LCDText), "UP  : %02d-%02x%02x%02x \nDOWN: Change node", select_ed,ConnectionTable[select_ed].Address[0],
						ConnectionTable[select_ed].Address[1],ConnectionTable[select_ed].Address[2]);
						LCD_Update();
	                    #endif
					}
	            break;

				default:
				break;
			}
#endif

			uint8_t PressedButton = ButtonPressed();
            switch( PressedButton )
            {
                case 1:
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
                    {
                        // TX buffer User DATA
                        dataPtr[dataLen++] = (MiWi[(TxSynCount%6)][i]);
                    }
                    TxSynCount++;
                    /*******************************************************************/
                    // Function MiApp_BroadcastPacket is used to broadcast a message
                    //    The only parameter is the boolean to indicate if we need to
                    //       secure the frame
                    /*******************************************************************/
                     MiApp_SendData(SHORT_ADDR_LEN, (uint8_t *)&broadcastAddress, dataLen, dataPtr, msghandledemo++, true, dataConfcb);
                    DemoOutput_UpdateTxRx(++TxNum, RxNum);

                    DemoOutput_Instruction();
				}
                break;

#if !defined (CONF_BOARD_JOYSTICK)
                case 2:
                    chk_sel_status = true;
                    select_ed =0;
                    update_ed = true;
                    while(update_ed == true)
                    {

                        //Peer Device Info
#if defined (ENABLE_LCD)
                       LCD_Erase();
						snprintf(LCDText, sizeof(LCDText),(char*)"SW:%02d-%02x%02x%02x \nBUTTON1: Change node",select_ed,ConnectionTable[select_ed].Address[0],
                               ConnectionTable[select_ed].Address[1],ConnectionTable[select_ed].Address[2]);
                        LCD_Update();
#endif
                        // Display another Peer Device Address
                        chk_sel_status = true;

                        while(chk_sel_status)
                        {
                            uint8_t switch_val = ButtonPressed();
                            //// While waiting in TX , RX will process if any message was available
                            if(switch_val == 1)
                            {
                                uint8_t* dataPtr = NULL;
                                uint8_t dataLen = 0;
                                update_ed = false;
                                chk_sel_status = false;

                                dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(DE_LEN));
							    if (NULL == dataPtr)
								    return;
                                for(i = 0; i < 11; i++)
                                {
                                    // Tx Buffer User Data
                                    dataPtr[dataLen++] = (DE[(TxSynCount2%6)][i]);
                                }
                                TxSynCount2++;
                                if( MiApp_SendData(LONG_ADDR_LEN, ConnectionTable[select_ed].Address, dataLen, dataPtr, msghandledemo++, 1, dataConfcb) == false)
                                {
                                    DemoOutput_UnicastFail();
                                }
                                else
                                {
                                    // Successful Transmission
                                    TxNum++;
                                }
                                // Update the LCD
                                DemoOutput_UpdateTxRx(TxNum, RxNum);

                                DemoOutput_Instruction();

                                break;
                            }   // end switch_val = 1

                            else if(switch_val == 2)
                            {
                                if (select_ed > conn_size-2)
                                {
                                    // Last Peer Device
                                    select_ed = 0;
                                }
                                else
                                {
                                    // Update the Display
                                    select_ed = select_ed+1;
                                }
                                chk_sel_status = false;
                            }   // end switch_val = 2
                        }  // end of Peer Device selection

                    } // End of Display


                    break;
#endif
                default:
                    break;
            }

        }

    }

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
    // Toggle LED2 to indicate receiving a packet.
    LED_Toggle(LED0);
    DemoOutput_Instruction();
}
#endif