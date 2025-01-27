#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "types.hh"

size_t gen_hash(const void *key, const size_t key_size, const size_t table_size);

template<typename Key, typename Value>
struct Hashmap {
    struct Entry {
        Key   key;
        Value value;
        bool  is_occupied;
    };

    void init()
    {
        table_size  = 100;
        entry_count = 0;
        entries     = (Entry *)malloc(table_size * sizeof(Entry));
    }

    void destroy()
    {
        free(entries);
    }

    void clear()
    {
        for (size_t i = 0; i < table_size; ++i) {
            entries[i].is_occupied = false;
        }

        entry_count = 0;
    }

    void insert(const Key &key, const Value &value)
    {
        expand_table_if_necessary();

        size_t hash = gen_hash(&key, sizeof(Key), table_size);

        // Resolve collisions
        for (size_t i = 0; i < table_size; ++i) {
            if (!entries[hash].is_occupied)
                goto found_available_entry;

            if (++hash >= table_size)
                hash = 0;
        }

        fprintf(stderr, "Error: Couldn't insert value into hashmap; there weren't any available entry slots!\n");
        return;

    found_available_entry:

        Entry *entry = &entries[hash];

        memcpy(&entry->value, &value, sizeof(Value));
        memcpy(&entry->key,   &key,   sizeof(Key));
        entry->is_occupied = true;

        ++entry_count;
    }

    void remove(const Key &key)
    {
        size_t hash = gen_hash(&key, sizeof(Key), table_size);

        // Resolve collisions
        for (size_t i = 0; i < table_size; ++i) {
            if (entries[hash].is_occupied &&
                !memcmp(&key, &entries[hash].key, sizeof(Key)))
            {
                entries[hash].is_occupied = false;
                --entry_count;
            }

            if (++hash >= table_size) hash = 0;
        }
    }

    Entry *get(const Key &key)
    {
        size_t hash = gen_hash(&key, sizeof(Key), table_size);

        // Resolve collisions
        for (size_t i = 0; i < table_size; ++i) {
            if (entries[hash].is_occupied &&
                !memcmp(&key, &entries[hash].key, sizeof(Key)))
            {
                return &entries[hash];
            }

            if (++hash >= table_size) hash = 0;
        }

        return NULL;
    }

private:

    inline void expand_table_if_necessary()
    {
        if (entry_count >= table_size) {
            table_size *= 2;
            entries = (Entry *)realloc(entries, table_size * sizeof(Entry));
        }
    }

    size_t entry_count, table_size;

    Entry *entries;
};

