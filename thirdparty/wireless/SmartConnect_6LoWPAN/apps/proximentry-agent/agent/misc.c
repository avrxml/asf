/*******************************************************************************
 *
 * Copyright (c) 2015 Proximetry Inc. All rights reserved.
 * Authors: Proximetry
 * All Rights Reserved
 *
 ******************************************************************************/
/**
 * @file misc.c
 * @brief
 * */

#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <asf.h>
#include "misc.h"
/* #include "device_api/settings.h" */
#include "settings.h"
/* #include "winc1500_helper.h" */

/**
 * @brief remove leading white-space characters from the string
 *
 * @param[in] p   the string to be trimmed
 *
 * @return pointer to the first byte of the trimmed string
 */
char * _PURE_ LTRIM(char *p)
{
	if (NULL != p)
		while (isspace((int)*p))
			++p;
	return p;
}

/**
 * @brief Remove trailing white-space characters from the string
 *
 * @param[in] p   the string to be trimmed
 *
 * @return pointer to the first byte of the trimmed string
 */
char *RTRIM(char *p)
{
	if ((NULL != p) && ('\0' != *p))
	{
		char *rp = p + strlen(p) - 1;
		while (isspace((int)*rp) && (rp >= p))
			--rp;
		rp[1] = '\0';
	}
	return p;
}

/**
 * @brief Remove both leading and trailing white-space characters from the string
 *
 * @param[in] p   the string to be trimmed
 *
 * @return pointer to the first byte of the trimmed string
 */
char *TRIM(char *p)
{
	return RTRIM(LTRIM(p));
}


/**
 * @brief Pretty prints a binary buffer
 *
 * @param[in] data       buffer to be printed
 * @param[in] data_len   buffer size
 */
void hexdump(const void *data, size_t data_len)
{
	const uint8_t *d = data;
	unsigned int i=0, j=0;
	char num[8], addr[64], hex[256];
	unsigned char chars[16], c;

	LOG("buffer [%p]: length:[%d]" EOL, d, data_len);
	while (data_len > 0)
	{
		snprintf(addr, sizeof(addr), "[%04d]", i);
		hex[0] = '\0';
		memset(chars, ' ', sizeof(chars));
		for (j=0; j<data_len && j<16; j++, d++)
		{
			c = *d;
			chars[j] = (isalnum(c) || ispunct(c)) ? c : '.';
			snprintf(num, sizeof(num), "%02x ", c);
			strcat(hex, num);
			if (j % 8 == 7)
				strcat(hex, "  ");
		}
		LOG("%s  %-49.49s  | %.8s %.8s |" EOL, addr, hex, chars, chars+8);
		data_len = (data_len >= 16) ? data_len - 16 : 0;
		i += 16;
	}
}

/**
 * @brief Suspends the program indefinitely
 */
void panic(void)
{

	#if (SAMD20 || SAMD21 || SAMR21 )
	system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
	#endif
	LOG("PANIC !!!");
	while(1)
		;
	/* system_sleep(); */
}

/**
 * @brief Three blink
 */

void triple_blink(void)
{
	int i = 3;

	do
	{
		port_pin_set_output_level(LED_0_PIN,  LED_0_ACTIVE );
		delay_ms(200);
		port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);
		delay_ms(800);
	}while(--i);
}

/*
 * @brief get_device_name
 *
 * Should be issues after winc1500_setup_mac_addr()
 */
#define HALF_SIZE_OF_HALF_OF_MAC		7
static char device_name[MAX_DEVICE_NAME_LEN] = {0};

char *get_device_name(void)
{
	char half_mac[HALF_SIZE_OF_HALF_OF_MAC] = {0};

	agent_settings_t *settings = get_settings();
	const uint8_t *mac = eui64;
	if (NULL != mac)
	{
		sprintf(half_mac, "%02X%02X%02X", mac[5], mac[6], mac[7]);
	}

	if (0 == strlen(settings->device_name))
	{
		/*
		 * If no device_name specified then use mac address as device name.
		 * If there is on network interface use procesor serial no.
		 */
		if (NULL != mac)
		{
			sprintf(device_name, PRINT_EUI64_FORMAT_3, PRINT_EUI64_VALUES(mac));
		}
		else
		{
			const char *serial_no_bin = get_hw_sw_info()->device_serial_no;
			memcpy(device_name, serial_no_bin, sizeof(device_name));
		}
	}
	else
	{
		/* device_name = device_name defined in settings + three last mac digits
		 * Last three mac digits are mandatory so settings->device_name must be truncated if needed */
		if (NULL != mac && (NULL == strstr(settings->device_name, half_mac)))
		{
			if (MAX_DEVICE_NAME_LEN-1 >= strlen(settings->device_name) + HALF_SIZE_OF_HALF_OF_MAC)
			{
				strcpy(device_name, settings->device_name);
				strcat(device_name, "_");
				strcat(device_name, half_mac);
			}
		}
		else
		{
			strncpy(device_name, settings->device_name, sizeof(device_name));
		}
	}

	LOG("Device name: \"%s\" is set" EOL, device_name);

	return device_name;
}


static device_hw_info_t device_hw_info;

/**
 * @brief Get hardware identification (id and serial).
 */
static void fill_hw_sw_info(void)
{
	uint32_t device_id = 0;
	uint32_t serial_no[4];

	device_id = (uint32_t)system_get_device_id();
	serial_no[0] = *(uint32_t *)0x0080A00C;
	serial_no[1] = *(uint32_t *)0x0080A040;
	serial_no[2] = *(uint32_t *)0x0080A044;
	serial_no[3] = *(uint32_t *)0x0080A048;

	sprintf(device_hw_info.device_id, "%08x", (unsigned int)device_id);
	sprintf(device_hw_info.device_serial_no, "%08x%08x%08x%08x",
		(unsigned)serial_no[0], (unsigned)serial_no[1],
		(unsigned)serial_no[2], (unsigned)serial_no[3]);
	device_hw_info.device_id[8] = '\0';
	device_hw_info.device_serial_no[32] = '\0';

	LOG("Device ID: [%s]" EOL, device_hw_info.device_id);
	LOG("Device Serial NO: [%s]" EOL, device_hw_info.device_serial_no);
}

/**
 * @brief Provide hardware identification (id and serial).
 *
 * @return Pointer to hardware identification information
 */
const device_hw_info_t *get_hw_sw_info(void)
{
	static bool hwinf = false;

	if (!hwinf) fill_hw_sw_info();

	return &device_hw_info;
}

/**
 * @brief Converts the uip addr to string
 *
 *  The function does not guarding the destination buffer size !
 * */
char *ipv6_addr_to_str(char *ipv6_str,  uip_ipaddr_t *ipv6)
{
    int i = 0;
	char *s = ipv6_str;
	bool was_zero = false;


    for(i = 0; i < 7; ++i) {
	  if((ipv6->u8[i * 2] == 0) && (ipv6->u8[i * 2 + 1] == 0))
	  {
		  if (!was_zero)
		  {
			  was_zero = true;
			  sprintf(s,":");
			  s+=1;
		  }
	  }
	  else
	  {
	      sprintf(s,"%02x%02x:", ipv6->u8[i * 2], ipv6->u8[i * 2 + 1]);
	  	  s+=5;
		  was_zero = false;
	  }
    }
    sprintf(s,"%02x%02x",
           ipv6->u8[7 * 2], ipv6->u8[7 * 2 + 1]);

	return ipv6_str;
}

/**
 * @brief Converts the eui64  to string
 *
 *  The function does not guarding the destination buffer size !
 * */
char *eui64_to_str(char *eui64_str,  uint8_t *eui64)
{
    int i = 0;
	char *s = eui64_str;

    for(i = 0; i < 8; ++i) {
      sprintf(s,"%02x:",eui64[i]);
	  s+=3;
    }

	s[-1] = '\0'; // remove last :

	return eui64_str;
}


