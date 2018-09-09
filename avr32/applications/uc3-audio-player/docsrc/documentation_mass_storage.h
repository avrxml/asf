/**
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
