/**
 * \file
 *
 * \brief Tiny DHCP/DNS service configuration.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#ifndef CONF_TINYSERVICES_H_INCLUDED
#define CONF_TINYSERVICES_H_INCLUDED

/* Access Point's address in provisioning mode. */
#define AP_ADDR						192, 168, 5, 1
#define AP_ADDR_IP					_tiny_ip4_addr(AP_ADDR)

/** Default subnet mask in provisioning mode */
#define SN_MASK						255, 255, 255, 0
#define SN_MASK_IP					_tiny_ip4_addr(SN_MASK)

/** Default gateway in provisioning mode. */
#define GW_ADDR						192, 168, 5, 1
#define GW_ADDR_IP					_tiny_ip4_addr(GW_ADDR)

/** Default Domain Name supplied to the client by Tiny DHCP Server in provisioning mode. */
#define DOMAIN_NAME					"iot.com"
#define DOMAIN_NAME_LEN				(8)

/** Default Name Server supplied to the client by Tiny DHCP Server in provisioning mode. */
#define NAME_SERVER_HOST_NAME		"ns"
#define NAME_SERVER_FQDN			NAME_SERVER_HOST_NAME "." DOMAIN_NAME
#define NAME_SERVER_FQDN_LEN		(11)
#define HTTP_SRV_LOCAL_NAME			"smartconnect"

/** Default Renewal Time supplied to the client by Tiny DHCP Server in provisioning mode. */
#define SANE_RENEWAL_TIME			(ADAY_DNS_TTL)

/** Default Rebinding Time supplied to the client by Tiny DHCP Server in provisioning mode. */
#define SANE_REBINDING_TIME			(ADAY_DNS_TTL)

/** Default Lease Time supplied to the client by Tiny DHCP Server in provisioning mode. */
#define SANE_LEASE_TIME				(ADAY_DNS_TTL)

/** Default DNS Time To Live supplied to the client by Tiny DHCP Server in provisioning mode. */
#define SANE_DNS_TTL				(600)
#define TWOHOUR_DNS_TTL				(7200)
#define ADAY_DNS_TTL				(86400)

#endif /* CONF_TINYSERVICES_H_INCLUDED */
