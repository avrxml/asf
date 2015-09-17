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
/*! \addtogroup group_main_files
 *
 * \section mass_storage Mass Storage
 * - ai_usb_ms.c : The USB mass storage module to be plugged into the generic audio interface.
 * - host_mem.c : USB host memory manager.
 * - scsi_decoder.c : Set of functions to parse SCSI packets.
 * - host_mass_storage_task.c : Detects the presence of a USB mass storage device.
 *
 * \section fat FAT
 * - \b Low \b level \b functions
 *  - fat.c : Set of all basic low level functions (get_cluster_list...).
 *  - fat_unusual.c : Extended set of low level functions (mount...).
 * - \b Abstract \b interface
 *  - ctrl_access.c : Abstract layer for memory management.
 * - \b Navigation
 *  - navigation.c : Functions to navigate in the FAT file system (cd...).
 *  - nav_automatic.c : Plug-in to automatic navigation (next, previous...).
 *  - nav_filterlist.c : Plug-in to handle file filtering during navigation.
 * - \b File
 *  - file.c : Basic functions to handle a file (open, read...).
 *  - reader_txt.c : Set of function to handle text files (read_line...).
 *  - unicode.c : Plug-in to handle Unicode in text files.
 *  - play_list.c : Set of functions to parse and read a playlist file.
 *
 */

/*! \addtogroup group_device_supported
 *
 * \section ds_ms USB Mass Storage Devices
 *
 * - Any USB mass storage devices, which includes USB memory sticks and external
 *   USB hard drives.
 *   \verbatim Make sure the USB device does not drain to much current. \endverbatim
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
