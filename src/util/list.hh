#pragma once

#include "types.hh"
#include <stdlib.h>

template<typename T>
struct List {
    void init()
    {
        count    = 0;
        capacity = 20;
        items    = (T *)malloc(capacity * sizeof(T));
    }

    void destroy()
    {
        free(items);
        count    = 0;
        capacity = 0;
    }

    void append(T &item)
    {
        if (count >= capacity) {
            capacity *= 2;
            items = (T *)realloc(items, capacity * sizeof(T));
        }
        items[count++] = item;
    }

    void clear()
    {
        memset(items, 0, count * sizeof(T));
        count = 0;
    }

    bool contains(T &item)
    {
        for (size_t i = 0; i < count; ++i) {
            if (memcmp(&items[i], &item, sizeof(T))) return true;
        }
        return false;
    }

    T     *items;
    size_t count, capacity;
};
