/*******************************************************************************
 *
 * Configures logging function
 *
 * Copyright (c) 2015 Proximetry Inc. All rights reserved.
 * Authors: Proximetry
 * All Rights Reserved
 *
 ******************************************************************************/


#ifndef CONF_LOGGING_H
#define CONF_LOGGING_H

#define EOL "\r\n"

#define LOG(...) printf(__VA_ARGS__)

//#define LOG_LEVEL_DEBUG
#ifdef LOG_LEVEL_DEBUG
#define LOG_DEBUG(...) LOG(__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif


#endif /* CONF_LOGGING_H */
