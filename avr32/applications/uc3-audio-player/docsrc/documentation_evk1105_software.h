/**
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*! \addtogroup group_getting_started
 *
 * \subsection evk1105_step4 Step4 : Use the application
 *
 * The audio player application is divided in 3 different views.
 * To switch between views, use the following:
 *
 * KEYS
 * - Slide from PLAY / PAUSE key to LEFT key
 *   \verbatim Switch to the previous view. \endverbatim
 * - Slide from PLAY / PAUSE key to RIGHT key
 *   \verbatim Switch to the next view. \endverbatim
 *
 * \b Disk \b Navigation \b view
 *
 * This view shows the content of the device plugged.
 * It shows basically a list of files and folders in an explorer-like view.
 * This view is the first one shown once the USB device is plugged.
 *
 * KEYS
 * - UP / DOWN
 *   \verbatim To select a file or a directory. \endverbatim
 * - LEFT
 *   \verbatim To go to parent directory. \endverbatim
 * - RIGHT
 *   \verbatim To enter a directory. \endverbatim
 * - PLAY / PAUSE
 *   \verbatim To play the selected file or to enter in the selected folder. \endverbatim
 *
 * \b Playback \b view
 *
 * Shows information of the track currently played.
 *
 * KEYS
 * - UP / DOWN
 *   \verbatim To increase/decrease the volume. \endverbatim
 * - LEFT / RIGHT
 *   \verbatim To select the previous/next song. \endverbatim
 * - LEFT / RIGHT pressed for at least 1s
 *   \verbatim Fast Forward or Fast Rewind the song. \endverbatim
 * - PLAY / PAUSE
 *   \verbatim  To switch between play and pause modes. \endverbatim
 *
 * \b Configuration \b view
 *
 * This view is used to display and edit configurable options of the audio
 * player.
 *
 * KEYS
 * - UP / DOWN
 *   \verbatim To select the previous/next option. \endverbatim
 * - PLAY / PAUSE
 *   \verbatim To edit the option. \endverbatim
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

