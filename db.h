/*
 * Copyright (c) 2024, Junior Rantila
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

void db_sync(void);
void* db_state(char const* id, size_t);
#define db_table(T) ((T*)db_state(#T, sizeof(T)))

#ifdef DB_IMPLEMENTATION

#include <errno.h>
#include <sys/xattr.h>
#include <stdlib.h>

void db_sync(void);

struct DB_State {
    char const* id;
    void const* addr;
    size_t size;
};
static struct DB_State g_db_state[1024];
static size_t g_db_state_count = 0;

void* db_state(char const* id, size_t size)
{
    if (g_db_state_count >= sizeof(g_db_state) / sizeof(g_db_state[0])) {
        errno = EOVERFLOW;
        return NULL;
    }

    static int ran_before;
    if (!ran_before) {
        ran_before = 1;
        atexit(db_sync);
    }

    char const* path = getenv("_");
    if (path == NULL)
        return NULL;

    void* addr = calloc(1, size);
    if (addr == NULL)
        return NULL;

#ifdef __APPLE__
    getxattr(path, id, addr, size, 0, 0);
#else 
    getxattr(path, id, addr, size);
#endif

    g_db_state[g_db_state_count++] = (struct DB_State) {
        .id = id,
        .addr = addr,
        .size = size
    };

    return addr;
}

void db_sync(void)
{
    char const* path = getenv("_");
    for (size_t i = 0; i < g_db_state_count; i++) {
        struct DB_State state = g_db_state[i];
#ifdef __APPLE__
        setxattr(path, state.id, state.addr, state.size, 0, 0);
#else
        setxattr(path, state.id, state.addr, state.size);
#endif
    }
}

#endif

#ifdef __cplusplus
}
#endif
