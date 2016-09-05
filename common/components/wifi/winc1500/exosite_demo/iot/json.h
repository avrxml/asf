/**
 * \file
 *
 * \brief JSON parser
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

#ifndef JSON_H_INCLUDED
#define JSON_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" 
{
#endif

/** Max token size of JSON element. Token means that pair of key and value. */
#define JSON_MAX_TOKEN_SIZE 64
/** Max size of token name. */
#define JSON_MAX_NAME_SIZE 16

/** 
 * \brief JSON type. 
 */
enum json_type
{
	JSON_TYPE_NULL = 0,
	JSON_TYPE_OBJECT,
	JSON_TYPE_STRING,
	JSON_TYPE_BOOLEAN,
	JSON_TYPE_INTEGER,
	JSON_TYPE_REAL,
	JSON_TYPE_ARRAY,
	JSON_TYPE_MAX
};

/** \brief JSON data structure. */
struct json_obj
{
	/** Type of this data. */
	enum json_type type;
	/** Name of this data. */
	char name[JSON_MAX_NAME_SIZE];
	/** End pointer of JSON buffer. */
	char *end_ptr;
	
	/** Value of this JSON token. */
	union
	{
		/* String data. */
		char s[JSON_MAX_TOKEN_SIZE - JSON_MAX_NAME_SIZE];
		/* Boolean data. */
		int b;
		/* Fixed number data. */
		int i;
		/* Real number data. */
		double d;
		/* Object or Array data. */ 
		char *o; /* Start point of object. */
	} value;
};

/**
 * \brief Create the JSON data from the string buffer.
 *
 * \param[out] obj             Pointer of JSON token which will be stored json informations.
 * \param[in]  data            JSON data represented as a string.
 * \param[in]  data_len        JSON data length.
 *
 * \return     0               Success.
 * \return     otherwise       Failed to create.
 */
int json_create(struct json_obj *obj, const char *data, int data_len);

/**
 * \brief Get child count in the JSON object.
 *
 * The input should be an object or array.
 *
 * \param[in]  obj             Pointer of the parent JSON data.
 *
 * \return     0               Success.
 * \return     otherwise       Failed to create.
 */
int json_get_child_count(struct json_obj *obj);

/**
 * \brief Get child data in the JSON object.
 *
 * The input should be an object or array.
 *
 * \param[in]  obj             Pointer of the parent JSON data.
 * \param[in]  index           Index which is located in the parent object.
 * \param[in]  out             Pointer of JSON token which will be stored child JSON informations.
 *
 * \return     0               Success.
 * \return     otherwise       Failed to create.
 */
int json_get_child(struct json_obj *obj, int index, struct json_obj *out);

/**
 * \brief Find data from the JSON object
 *
 * The input should be an object or array.
 * This function supported colon separated search.
 *
 * If JSON data is as follows
 * {"obj1":{"data1":"value1","data2":"value2","obj2":{"data3":"value3"}}}
 * You can be found value of data3 using the following name variable.
 * "obj1:obj2:data3"
 *
 * \param[in]  obj             Pointer of the parent JSON data.
 * \param[in]  name            The name of the item you are looking for.
 * \param[in]  out             Pointer of JSON token which will be stored child JSON informations.
 *
 * \return     0               Success.
 * \return     otherwise       Failed to create.
 */
int json_find(struct json_obj *obj, const char *name, struct json_obj *out);


#ifdef __cplusplus
}
#endif

#endif /* JSON_H_INCLUDED */

