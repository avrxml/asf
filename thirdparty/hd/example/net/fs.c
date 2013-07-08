/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#include <string.h>

#include "lwip/def.h"
#include <net/fs.h>
#include <net/fsdata_file.h>

static const struct fsdata_file *fs_static_root;
static const struct fsdata_file *fs_dyn_root;

void fs_register_root(const struct fsdata_file *static_root,
                      const struct fsdata_file *dyn_root) {
        fs_static_root = static_root;
        fs_dyn_root = dyn_root;
}

/**
 * Assumes that name is NUL-terminated.
 */
int
fs_open(char *name, struct fs_file *file)
{
        const struct fsdata_file *f;

        for (f = fs_static_root; f != NULL; f = f->next) {
                if (!strcmp(name, f->name)) {
                        f->cb((const char**) &file->data,
                              &file->len,
                              NULL);
                        return 1;
                }
        }
        /* Partial matches for text input forms */
        for (f = fs_dyn_root; f != NULL; f = f->next) {
                if (!strncmp(name, f->name, strlen(f->name))) {
                        f->cb((const char**) &file->data,
                              &file->len,
                              name + strlen(f->name));
                        return 1;
                }
        }
        return 0;
}
