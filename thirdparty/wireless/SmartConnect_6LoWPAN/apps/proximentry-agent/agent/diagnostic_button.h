/*******************************************************************************
 *
 * Copyright (c) 2015 Proximetry Inc. All rights reserved.
 * Authors: Proximetry
 * All Rights Reserved
 *
 ******************************************************************************/
/**
 * @file diagnostic_button.h
 * @brief
 * */
#ifndef __DIAGNOSTIC_BUTTON__
#define __DIAGNOSTIC_BUTTON__

#include <stdbool.h>
#include <common.h>

bool diagnostic_button_service_is_active(void) _PURE_;
void diagnostic_button_led_service(void);
void diagnostic_button_service(void);

#endif /* __DIAGNOSTIC_BUTTON__ */
