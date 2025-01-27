#pragma once

#include "types.hh"
#include <memory>

struct String {
    void init();

    void destroy();

    void append_string(const char *str);
    void insert_string(const char *str, size_t index);

    void append(const char chr);
    void insert(const char chr, size_t index);

    void clear();

    void pop();
    void pop_at(size_t index);

    void trim_from_right_until_find_char(const char chr);

    char  *buf;
    size_t length;

private:

    inline void expand_if_necessary()
    {
        if (length >= capacity - 1) {
            capacity *= 2;
            buf = (char *)realloc(buf, capacity);
            memset(buf + length, 0, capacity - length);
        }
    }

    size_t capacity;
};

void eat_spaces(char **text_ptr);

char *eat_word(char **text_ptr);

char *eat_until_find_char(char **text_ptr, const char chr);

inline char *eat_remaining_line(char **text_ptr)
{
    return eat_until_find_char(text_ptr, '\n');
}

char *eat_word_or_until_find_char(char **text_ptr, const char chr);

void cut_trailing_spaces(char **text_ptr);

void trim_from_right_until_find_char(char **text_ptr, const char chr);

bool compare_string(const char *s, const char *other);

bool does_string_end_with(char *s, const char *str);

void copy_string(char *dest, const char *other);
void copy_string(char *dest, const char *other, size_t len);

const char *eat_until_find_char_from_right(const char **text_ptr, const char chr);
//char       *eat_until_find_char_from_right(char       **text_ptr, const char chr);
