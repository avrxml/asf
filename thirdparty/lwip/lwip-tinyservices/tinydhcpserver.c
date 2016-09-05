/**
 *
 * \file
 *
 * \brief Tiny DHCP Server.
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

#include "conf_tinyservices.h"
#include "tinyservices.h"
#include "wifi_private.h"
#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include <string.h>

#if LWIP_UDP && LWIP_DHCP

static struct udp_pcb * dhcp_pcb = 0;

/* This is the broadcast address used by concurrent mode: 192.168.5.255 */
static ip_addr_t ip_addr_c_broadcast = { 0xff05a8c0 };

#define LWIP_DHCP_MAX_CLIENT 7
static uint8_t lwip_dhcp_known_mac[LWIP_DHCP_MAX_CLIENT][6];

void lwip_dhcp_register_mac(uint8_t *mac)
{
	/* Make sure there is no previous occurrence of MAC in the list. */
	lwip_dhcp_unregister_mac(mac);

	for (uint8_t i = 0; i < LWIP_DHCP_MAX_CLIENT; ++i) {
		if (lwip_dhcp_known_mac[i][0] == 0 && lwip_dhcp_known_mac[i][1] == 0 &&
				lwip_dhcp_known_mac[i][2] == 0 && lwip_dhcp_known_mac[i][3] == 0 && 
				lwip_dhcp_known_mac[i][4] == 0 && lwip_dhcp_known_mac[i][5] == 0) {
			lwip_dhcp_known_mac[i][0] = mac[0];
			lwip_dhcp_known_mac[i][1] = mac[1];
			lwip_dhcp_known_mac[i][2] = mac[2];
			lwip_dhcp_known_mac[i][3] = mac[3];
			lwip_dhcp_known_mac[i][4] = mac[4];
			lwip_dhcp_known_mac[i][5] = mac[5];
			break;
		}
	}
}

void lwip_dhcp_unregister_mac(uint8_t *mac)
{
	for (uint32_t i = 0; i < LWIP_DHCP_MAX_CLIENT; ++i) {
		if (lwip_dhcp_known_mac[i][0] == mac[0] && lwip_dhcp_known_mac[i][1] == mac[1] &&
				lwip_dhcp_known_mac[i][2] == mac[2] && lwip_dhcp_known_mac[i][3] == mac[3] && 
				lwip_dhcp_known_mac[i][4] == mac[4] && lwip_dhcp_known_mac[i][5] == mac[5]) {
			memset(lwip_dhcp_known_mac[i], 0, 6);
		}
	}
}

/**
 * \brief Find a IP address for MAC.
 *
 * \return IP address if available, NULL if not.
 */
static ip_addr_t lwip_dhcp_get_ip_address(uint8_t *mac)
{
	ip_addr_t offered_ip_base = { 0 };

	for (uint32_t i = 0; i < LWIP_DHCP_MAX_CLIENT; ++i) {
		if (lwip_dhcp_known_mac[i][0] == mac[0] && lwip_dhcp_known_mac[i][1] == mac[1] &&
				lwip_dhcp_known_mac[i][2] == mac[2] && lwip_dhcp_known_mac[i][3] == mac[3] && 
				lwip_dhcp_known_mac[i][4] == mac[4] && lwip_dhcp_known_mac[i][5] == mac[5]) {

			/* Use index to track free IP address. */
			offered_ip_base.addr = ((10 + i) << 24) + 0x0005a8c0;
			return offered_ip_base;
		}
	}

	return offered_ip_base;
}

/**
 * \brief Find an option in a DHCP packet.
 *
 * \param pbuf_in the pbuf containing the a DHCP packet.
 * \param option an unsigned char representing the option to search for.
 * \param opt_val a pointer to place the value of the option located.
 * \param opt_size a pointer to the size of the option located.
 * \return 1 if option found, 0 if option not found.
 */
static uint16_t lwip_dhcp_find_option(struct pbuf * pbuf_in, uint8_t option, uint8_t * opt_val, uint8_t* opt_size)
{
    uint32_t offset = DHCP_OPTIONS_OFS;
    uint16_t opt_found = 0;
    uint8_t byte = 0;

    memset(opt_val, 0, *opt_size);

    while((byte != 0xff) && (offset < pbuf_in->tot_len)) {
        byte = pbuf_get_at(pbuf_in, offset);
        if(byte == option) {
            offset++;
            *opt_size = pbuf_get_at(pbuf_in, offset);
            offset++;
            pbuf_copy_partial(pbuf_in, opt_val, *opt_size, offset);
            offset += *opt_size;
            opt_found = 1;
        }
        offset++;
        offset += ((pbuf_get_at(pbuf_in, offset)) + 1);
    }

    return opt_found;
}

/**
 * \brief Receive DHCP discover, request and inform packets and respond to 
 * them by supplying one IP Address for the client and setting the DNS server
 * address.
 *
 * \param arg unused.
 * \param dhcp_pcb a pointer to the pcb that DHCP packets are received on.
 * \param pbuf_in a pointer to the pbuf containing the DHCP packet.
 * \param client_addr the IP Address (if any) that originated the DHCP traffic.
 * \param port unused.
 */
static void lwip_dhcp_server_fn(void *arg, struct udp_pcb * dhcp_pcb_recv, struct pbuf * pbuf_in, struct ip_addr * client_addr, uint16_t port)
{
    struct pbuf * pbuf_out = 0;
    uint32_t dhcp_data_in_size = 0;
    uint32_t dhcp_data_out_size = 0;
    uint32_t options_offset = 0;
    err_t err = ERR_OK;
    uint32_t client_requested_addr = 0;
    uint8_t addr_size = 0;
    uint8_t val = 0;
	ip_addr_t avail_ip;
	uint8_t mac[6];

    UNUSED(arg);
    UNUSED(port);

    Assert(pbuf_in);

    if ((NULL == pbuf_in) || ((dhcp_data_in_size = pbuf_in->tot_len) <= DHCP_OPTIONS_OFS)) {
	    goto free1_and_return;
	}

    pbuf_out = pbuf_alloc(PBUF_TRANSPORT, DHCP_RESPONSE_DEFAULT_SIZE, PBUF_POOL);
    Assert(pbuf_out);

    if ((NULL == pbuf_out) || ((dhcp_data_out_size = pbuf_out->tot_len) < DHCP_OPTIONS_OFS)) {
	    goto free1_and_return;
	}

    pbuf_copy(pbuf_out, pbuf_in);

	/* Read client MAC address. */
	for (uint32_t i = 0; i < 6; ++i) {
		pbuf_read8(pbuf_in, DHCP_CHADDR_OFS+i, &val);
		mac[i] = val;
	}

	/* Try register MAC address and get IP address. */
	lwip_dhcp_register_mac(mac);
	avail_ip = lwip_dhcp_get_ip_address(mac);
	if (0 == avail_ip.addr) {
		goto free1_and_return;
	}
	
	/* Read request type. */
    err = pbuf_read8(pbuf_in, DHCP_OPTIONS_OFS+2, &val);

    switch (val) {
        case DHCP_DISCOVER:
            err += pbuf_write8(pbuf_out, DHCP_OP_OFS, DHCP_BOOTREPLY);
            err += pbuf_write8(pbuf_out, DHCP_SECS_OFS, 0);
            err += pbuf_write8(pbuf_out, DHCP_FLAGS_OFS, 0x80);

            /* set up the default address. */
            err += pbuf_write_ip(pbuf_out, DHCP_YIADDR_OFS, avail_ip);

            /* set up the default next server address. */
            err += pbuf_write_ip(pbuf_out, DHCP_SIADDR_OFS, AP_ADDR_IP);

            /* supply the cookie. */
            err += pbuf_write32be(pbuf_out, DHCP_COOKIE_OFS, DHCP_MAGIC_COOKIE);

            options_offset = DHCP_OPTIONS_OFS;

            /* Construct a DHCP Offer Packet. */
            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_MESSAGE_TYPE);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 1 /* len */);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, DHCP_OFFER);
            options_offset++;

            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_SUBNET_MASK);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4 /* len */);
            options_offset++;
            err += pbuf_write_ip(pbuf_out, options_offset, SN_MASK_IP);
            options_offset += 4;

            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_DOMAIN_NAME);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, DOMAIN_NAME_LEN);
            options_offset++;
            err += pbuf_take_partial(pbuf_out, options_offset, DOMAIN_NAME, DOMAIN_NAME_LEN);
            options_offset += DOMAIN_NAME_LEN;

            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_ROUTER);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4);
            options_offset++;
            err += pbuf_write_ip(pbuf_out, options_offset, AP_ADDR_IP);
            options_offset += 4;

            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_DNS_SERVER);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4);
            options_offset++;
            err += pbuf_write_ip(pbuf_out, options_offset, AP_ADDR_IP);
            options_offset += 4;

            /* set a sane renewal time. */
            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_T1);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4 /* option len */);
            options_offset++;
            err += pbuf_write32be(pbuf_out, options_offset, SANE_RENEWAL_TIME);
            options_offset += 4;

            /* set a rebinding time. */
            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_T2);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4 /* option len */);
            options_offset++;
            err += pbuf_write32be(pbuf_out, options_offset, SANE_REBINDING_TIME);
            options_offset += 4;

            /* set a lease time. */
            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_LEASE_TIME);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4);
            options_offset++;
            err += pbuf_write32be(pbuf_out, options_offset, SANE_LEASE_TIME);
            options_offset += 4;

            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_SERVER_ID);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4);
            options_offset++;
            err += pbuf_write_ip(pbuf_out, options_offset, AP_ADDR_IP);
            options_offset += 4;

            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_VENDOR_SPECIFIC_INFO);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 0);
            options_offset++;

            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_END);
            options_offset++;

            if (err == 0) {
                pbuf_realloc(pbuf_out, options_offset);
                err = udp_sendto(dhcp_pcb, pbuf_out, &ip_addr_c_broadcast, DHCP_CLIENT_PORT);
                if (ERR_OK != err) {
				    goto free1_and_return;
				}
            } 
			else {
                goto free1_and_return;
            }
        break;

        case DHCP_REQUEST:
            err += pbuf_write8(pbuf_out, DHCP_OP_OFS, DHCP_BOOTREPLY);
            err += pbuf_write8(pbuf_out, DHCP_SECS_OFS, 0);

            /* Set the broadcast flag on the reply if it was set in the request. */
            uint8_t flags = 0;
            err += pbuf_read8(pbuf_in, DHCP_FLAGS_OFS, &flags);
            if(flags & 0x80) {
                err += pbuf_read8(pbuf_out, DHCP_FLAGS_OFS, &flags);
                flags |= 0x80;
                err += pbuf_write8(pbuf_out, DHCP_FLAGS_OFS, flags);
            }
            else {
                err += pbuf_read8(pbuf_out, DHCP_FLAGS_OFS, &flags);
                flags &= ~0x80;
                err += pbuf_write8(pbuf_out, DHCP_FLAGS_OFS, flags);
            }

            /* Need to figure out if the client is requesting the address it was assigned by us or by another
                DHCP server.  PCs appear to initially ask for their "old" IP addr when they first connect to
                our network - NAK them to initiate DORA in a timely fashion */
            if (0 == lwip_dhcp_find_option(pbuf_in, DHCP_OPTION_REQUESTED_IP, (uint8_t *)&client_requested_addr, &addr_size)) {
                goto free1_and_return;
            }

            if (client_requested_addr == avail_ip.addr) {
                /* set up the default address. */
                err += pbuf_write_ip(pbuf_out, DHCP_YIADDR_OFS, avail_ip);

                /* set up the default next server address. */
                err += pbuf_write_ip(pbuf_out, DHCP_SIADDR_OFS, AP_ADDR_IP);

                /* supply the cookie. */
                err += pbuf_write32be(pbuf_out, DHCP_COOKIE_OFS, DHCP_MAGIC_COOKIE);

                options_offset = DHCP_OPTIONS_OFS; /* Past the magic cookie. */

                err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_MESSAGE_TYPE);
                options_offset++;
                err += pbuf_write8(pbuf_out, options_offset, 1 /* option len */);
                options_offset++;
                err += pbuf_write8(pbuf_out, options_offset, DHCP_ACK);
                options_offset++;

                /* Set a Sane Subnet Mask. */
                err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_SUBNET_MASK);
                options_offset++;
                err += pbuf_write8(pbuf_out, options_offset, 4);
                options_offset++;
                err += pbuf_write_ip(pbuf_out, options_offset, SN_MASK_IP);
                options_offset += 4;

                err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_DOMAIN_NAME);
                options_offset++;
                err += pbuf_write8(pbuf_out, options_offset, DOMAIN_NAME_LEN);
                options_offset++;
                pbuf_take_partial(pbuf_out, options_offset, DOMAIN_NAME, DOMAIN_NAME_LEN);
                options_offset += DOMAIN_NAME_LEN;

                err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_ROUTER);
                options_offset++;
                err += pbuf_write8(pbuf_out, options_offset, 4);
                options_offset++;
                err += pbuf_write_ip(pbuf_out, options_offset, AP_ADDR_IP);
                options_offset += 4;

                err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_DNS_SERVER);
                options_offset++;
                err += pbuf_write8(pbuf_out, options_offset, 4);
                options_offset++;
                err += pbuf_write_ip(pbuf_out, options_offset, AP_ADDR_IP);
                options_offset += 4;

                /* set a sane renewal time. */
                err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_T1);
                options_offset++;
                err += pbuf_write8(pbuf_out, options_offset, 4);
                options_offset++;
                err += pbuf_write32be(pbuf_out, options_offset, SANE_RENEWAL_TIME);
                options_offset += 4;

                /* set a rebinding time. */
                err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_T2);
                options_offset++;
                err += pbuf_write8(pbuf_out, options_offset, 4);
                options_offset++;
                err += pbuf_write32be(pbuf_out, options_offset, SANE_REBINDING_TIME);
                options_offset += 4;

                /* set a lease time. */
                err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_LEASE_TIME);
                options_offset++;
                err += pbuf_write8(pbuf_out, options_offset, 4);
                options_offset++;
                err += pbuf_write32be(pbuf_out, options_offset, SANE_LEASE_TIME);
                options_offset += 4;

                /* Identify ourselves as the DHCP Server. */
                err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_SERVER_ID);
                options_offset++;
                err += pbuf_write8(pbuf_out, options_offset, 4);
                options_offset++;
                err += pbuf_write_ip(pbuf_out, options_offset, AP_ADDR_IP);
                options_offset += 4;

                err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_VENDOR_SPECIFIC_INFO);
                options_offset++;
                err += pbuf_write8(pbuf_out, options_offset, 0);
                options_offset++;

                /* Set the end option. */
                err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_END);
                options_offset++;
            }
            else {
                /* Supply the cookie. */
                err += pbuf_write32be(pbuf_out, DHCP_COOKIE_OFS, DHCP_MAGIC_COOKIE);

                options_offset = DHCP_OPTIONS_OFS; /* Past the magic cookie. */

                err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_MESSAGE_TYPE);
                options_offset++;
                err += pbuf_write8(pbuf_out, options_offset, 1 /* option len */);
                options_offset++;
                err += pbuf_write8(pbuf_out, options_offset, DHCP_NAK);
                options_offset++;

                /* Set the end option. */
                err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_END);
                options_offset++;
            }

            if (err == 0) {
                pbuf_realloc(pbuf_out, options_offset);
                err = udp_sendto(dhcp_pcb, pbuf_out, &ip_addr_c_broadcast, DHCP_CLIENT_PORT);
                if (ERR_OK != err) {
				    goto free1_and_return;
				}
            }
			else {
                goto free1_and_return;
            }
        break;

        case DHCP_INFORM:
            err += pbuf_write8(pbuf_out, DHCP_OP_OFS, DHCP_BOOTREPLY);
            err += pbuf_write8(pbuf_out, DHCP_SECS_OFS, 0);
            err += pbuf_write8(pbuf_out, DHCP_FLAGS_OFS, 0);

            /* Set up the default address. */
            err += pbuf_write_ip(pbuf_out, DHCP_YIADDR_OFS, avail_ip);

            /* Set up the default next server address. */
            err += pbuf_write_ip(pbuf_out, DHCP_SIADDR_OFS, AP_ADDR_IP);

            /* Supply the cookie. */
            err += pbuf_write32be(pbuf_out, DHCP_COOKIE_OFS, DHCP_MAGIC_COOKIE);

            options_offset = DHCP_OPTIONS_OFS; /* Past the magic cookie. */

            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_MESSAGE_TYPE);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 1 /* option len */);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, DHCP_ACK);
            options_offset++;

            /* Set a Sane Subnet Mask. */
            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_SUBNET_MASK);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4);
            options_offset++;
            err += pbuf_write_ip(pbuf_out, options_offset, SN_MASK_IP);
            options_offset += 4;

            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_DOMAIN_NAME);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, DOMAIN_NAME_LEN);
            options_offset++;
            pbuf_take_partial(pbuf_out, options_offset, DOMAIN_NAME, DOMAIN_NAME_LEN);
            options_offset += DOMAIN_NAME_LEN;

            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_ROUTER);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4);
            options_offset++;
            err += pbuf_write_ip(pbuf_out, options_offset, AP_ADDR_IP);
            options_offset += 4;

            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_DNS_SERVER);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4);
            options_offset++;
            err += pbuf_write_ip(pbuf_out, options_offset, AP_ADDR_IP);
            options_offset += 4;

            /* Set a sane renewal time. */
            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_T1);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4);
            options_offset++;
            err += pbuf_write32be(pbuf_out, options_offset, SANE_RENEWAL_TIME);
            options_offset += 4;

            /* Set a rebinding time. */
            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_T2);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4);
            options_offset++;
            err += pbuf_write32be(pbuf_out, options_offset, SANE_REBINDING_TIME);
            options_offset += 4;

            /* Set a lease time. */
            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_LEASE_TIME);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4);
            options_offset++;
            err += pbuf_write32be(pbuf_out, options_offset, SANE_LEASE_TIME);
            options_offset += 4;

            /* Identify ourselves as the DHCP Server. */
            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_SERVER_ID);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 4);
            options_offset++;
            err += pbuf_write_ip(pbuf_out, options_offset, AP_ADDR_IP);
            options_offset += 4;

            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_VENDOR_SPECIFIC_INFO);
            options_offset++;
            err += pbuf_write8(pbuf_out, options_offset, 0);
            options_offset++;

            /* Set the end option. */
            err += pbuf_write8(pbuf_out, options_offset, DHCP_OPTION_END);
            options_offset++;

            if (err == 0) {
                pbuf_realloc(pbuf_out, options_offset);
                err = udp_sendto(dhcp_pcb, pbuf_out, client_addr, DHCP_CLIENT_PORT);
                if (ERR_OK != err) {
				    goto free1_and_return;
				}
            } 
			else {
                goto free1_and_return;
            }
        break;

		/* Only handling discover and request messages. */
		default:
            break;
	}

free1_and_return:
	if (pbuf_out) {
		pbuf_free(pbuf_out);
	}
	pbuf_free(pbuf_in);
}


/**
 * \brief Start the Tiny DNS Server. Create a new UDP PCB, bind that PCB
 * to the DHCP_SERVER_PORT and set up a receiver for that PCB.
 *
 * This variant of the start function must only be called from within LWIP's main thread.
 *
 * This function must be invoked via lwip_tiny_dhcpserver_start
 */
static void lwip_dhcpserver_start_threadsafe(void * arg)
{
    UNUSED(arg);

	/* Clean known mac array. */
	for (uint32_t i = 0; i < LWIP_DHCP_MAX_CLIENT; ++i) {
		memset(lwip_dhcp_known_mac[i], 0, 6);
	}

    if(0 == dhcp_pcb) {
        dhcp_pcb = udp_new();
        Assert(dhcp_pcb);
        if(0 == dhcp_pcb) {
            return;
        }

        udp_bind(dhcp_pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);
		udp_recv(dhcp_pcb, lwip_dhcp_server_fn, 0);
    }
}

/**
 * \brief Start the Tiny DHCP Server. Create a new UDP PCB, bind that PCB
 * to the DHCP_SERVER_PORT and set up a receiver for that PCB.
 *
 * Thread-safe variant of this function. This function posts
 * the LWIP-thread-only variant of this function (with dummy
 * argument) to the LWIP thread's mailbox and returns.
 *
 * NOTE: This function cannot wait. It may be called
 * from within the LWIP main thread of execution.
 */
void lwip_tiny_dhcpserver_start(void)
{
    tcpip_callback(lwip_dhcpserver_start_threadsafe, 0);
}

/**
 * \brief Stop a Tiny DHCP Server. Disconnect and remove the UDP PCB.
 *
 * This function must only be called from within LWIP's main thread.
 * It must be invoked via lwip_tiny_dhcp_server_stop.
 */
static void lwip_tiny_dhcpserver_stop_threadsafe(void * arg)
{
    UNUSED(arg);

    if(dhcp_pcb != 0) {
        udp_disconnect(dhcp_pcb);
        udp_remove(dhcp_pcb);
        dhcp_pcb = 0;
    }
}

/**
 * \brief Stop a Tiny DHCP Server. Disconnect and remove the UDP PCB.
 *
 * Thread-safe variant of this function. This function posts
 * the lwip-main-thread only function and a dummy argument to
 * the main lwip thread.
 *
 * NOTE: This function cannot wait. It may be called
 * from within the LWIP main thread of execution.
 */
void lwip_tiny_dhcpserver_stop(void)
{
    tcpip_callback(lwip_tiny_dhcpserver_stop_threadsafe, 0);
}

#endif
