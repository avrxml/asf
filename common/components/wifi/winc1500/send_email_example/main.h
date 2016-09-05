/**
 * \file
 *
 * \brief MAIN configuration.
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

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/** Wi-Fi Settings */
#define MAIN_WLAN_SSID                  "DEMO_AP" /**< Destination SSID */
#define MAIN_WLAN_AUTH                  M2M_WIFI_SEC_WPA_PSK /**< Security manner */
#define MAIN_WLAN_PSK                   "12345678" /**< Password for Destination SSID */

/** Using IP address. */
#define IPV4_BYTE(val, index)           ((val >> (index * 8)) & 0xFF)

/** All SMTP defines */
#define MAIN_SMTP_BUF_LEN               1024
#define MAIN_GMAIL_HOST_NAME            "smtp.gmail.com"
#define MAIN_GMAIL_HOST_PORT            465
#define MAIN_SENDER_RFC                 "<sender@gmail.com>" /* Set Sender Email Address */
#define MAIN_RECIPIENT_RFC              "<recipient@gmail.com>"  /* Set Recipient Email Address */
#define MAIN_EMAIL_SUBJECT              "Hello from WINC1500!"
#define MAIN_TO_ADDRESS                 "recipient@gmail.com" /* Set To Email Address */
#define MAIN_FROM_ADDRESS               "sender@gmail.com" /* Set From Email Address */
#define MAIN_FROM_PASSWORD              "12345678"              /* Set Sender Email Password */
#define MAIN_EMAIL_MSG                  "This mail is sent from Send Email Example."
#define MAIN_WAITING_TIME               30000
#define MAIN_RETRY_COUNT                3

typedef enum {
	SocketInit = 0,
	SocketConnect,
	SocketWaiting,
	SocketComplete,
	SocketError
} eSocketStatus;

typedef enum {
	SMTP_INACTIVE = 0,
	SMTP_INIT,
	SMTP_HELO,
	SMTP_AUTH,
	SMTP_AUTH_USERNAME,
	SMTP_AUTH_PASSWORD,
	SMTP_FROM,
	SMTP_RCPT,
	SMTP_DATA,
	SMTP_MESSAGE_SUBJECT,
	SMTP_MESSAGE_TO,
	SMTP_MESSAGE_FROM,
	SMTP_MESSAGE_CRLF,
	SMTP_MESSAGE_BODY,
	SMTP_MESSAGE_DATAEND,
	SMTP_QUIT,
	SMTP_ERROR
} eSMTPStatus;

/* Initialize error handling flags for smtp state machine */
typedef enum {
	MAIN_EMAIL_ERROR_FAILED = -1,
	MAIN_EMAIL_ERROR_NONE = 0,
	MAIN_EMAIL_ERROR_INIT,
	MAIN_EMAIL_ERROR_HELO,
	MAIN_EMAIL_ERROR_AUTH,
	MAIN_EMAIL_ERROR_AUTH_USERNAME,
	MAIN_EMAIL_ERROR_AUTH_PASSWORD,
	MAIN_EMAIL_ERROR_FROM,
	MAIN_EMAIL_ERROR_RCPT,
	MAIN_EMAIL_ERROR_DATA,
	MAIN_EMAIL_ERROR_MESSAGE,
	MAIN_EMAIL_ERROR_QUIT
} eMainEmailError;

/** Return Codes */
const char cSmtpCodeReady[] = {'2', '2', '0', '\0'};
const char cSmtpCodeOkReply[] = {'2', '5', '0', '\0'};
const char cSmtpCodeIntermedReply[] = {'3', '5', '4', '\0'};
const char cSmtpCodeAuthReply[] = {'3', '3', '4', '\0'};
const char cSmtpCodeAuthSuccess[] = {'2', '3', '5', '\0'};

/** Send Codes */
const char cSmtpHelo[] = {'H', 'E', 'L', 'O', '\0'};
const char cSmtpMailFrom[] = {'M', 'A', 'I', 'L', ' ', 'F', 'R', 'O', 'M', ':', ' ', '\0'};
const char cSmtpRcpt[] = {'R', 'C', 'P', 'T', ' ', 'T', 'O', ':', ' ', '\0'};
const char cSmtpData[] = "DATA";
const char cSmtpCrlf[] = "\r\n";
const char cSmtpSubject[] = "Subject: ";
const char cSmtpTo[] = "To: ";
const char cSmtpFrom[] = "From: ";
const char cSmtpDataEnd[] = {'\r', '\n', '.', '\r', '\n', '\0'};
const char cSmtpQuit[] = {'Q', 'U', 'I', 'T', '\r', '\n', '\0'};

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H_INCLUDED */
