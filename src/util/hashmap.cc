#include "hashmap.hh"

size_t gen_hash(const void *key, const size_t key_size, const size_t table_size)
{
    size_t hash = 0;

    const u8 *byte = (u8 *)key;

    for (size_t i = 0; i < key_size; ++i) {
        hash += *byte;
        hash = (hash * *byte) % table_size;

        ++byte;
    }

    return hash;
}
