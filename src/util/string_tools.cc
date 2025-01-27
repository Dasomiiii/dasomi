#include "string_tools.hh"

#include <ctype.h>

void String::init()
{
    length   = 0;
    capacity = 50;
    buf      = (char *)malloc(capacity * sizeof(char));
}

void String::destroy()
{
    free(buf);
    length   = 0;
    capacity = 0;
}

void String::append_string(const char *str)
{
    while (*str) {
        this->append(*str);
        ++str;
    }
}

void String::insert_string(const char *str, size_t index)
{
    while (*str) {
        this->insert(*str, index);
        ++str;
        ++index;
    }
}

void String::append(const char chr)
{
    expand_if_necessary();

    buf[length++] = chr;
}

void String::insert(const char chr, size_t index)
{
    expand_if_necessary();

    for (size_t i = length; i > index; --i) {
        buf[i] = buf[i - 1];
    }
    buf[index] = chr;

    ++length;
}

void String::clear()
{
    memset(buf, 0, capacity);
    length = 0;
}

void String::pop()
{
    buf[--length] = '\0';
}

void String::pop_at(size_t index)
{
    memmove(buf + index, buf + index + 1,
            (length - index - 1) * sizeof(char));

    buf[--length] = '\0';
}

void String::trim_from_right_until_find_char(const char chr)
{
    while ((buf[length - 1] != chr) && buf[length - 1]) this->pop();
}

// ==================================================

void eat_spaces(char **text_ptr)
{
    while (isspace(**text_ptr)) {
        ++(*text_ptr);
    }
}

char *eat_word(char **text_ptr)
{
    if (!**text_ptr) return NULL;

    char *s = *text_ptr;

    while (**text_ptr && !isspace(**text_ptr)) {
        ++(*text_ptr);
    }
    if (**text_ptr) {
        **text_ptr = '\0';
        ++(*text_ptr);
    }
    return s;
}

char *eat_until_find_char(char **text_ptr, const char chr)
{
    if (!**text_ptr) return NULL;

    char *s = *text_ptr;

    while ((**text_ptr) && (**text_ptr != chr)) {
        ++(*text_ptr);
    }
    if (**text_ptr) {
        **text_ptr = '\0';
        ++(*text_ptr);
    }
    return s;
}

char *eat_word_or_until_find_char(char **text_ptr, const char chr)
{
    if (!**text_ptr) return NULL;

    char *s = *text_ptr;

    while ((**text_ptr) && (!isspace(**text_ptr)) && (**text_ptr != chr)) {
        ++(*text_ptr);
    }
    if (**text_ptr) {
        **text_ptr = '\0';
        ++(*text_ptr);
    }
    return s;
}

void cut_trailing_spaces(char **text_ptr)
{
    if (!**text_ptr) return;

    char *c = *text_ptr;
    while (*c) ++c;
    --c;

    while (isspace(*c)) {
        *c = '\0';
        --c;
    }
}

bool compare_string(const char *s, const char *other)
{
    while (*s && *other) {
        if (*s != *other) return false;
        ++s;
        ++other;
    }
    return true;
}

bool does_string_end_with(char *s, const char *str)
{
    if (!*s) return false;

    const size_t len = strlen(str);

    while (*s) ++s;
    s -= len;

    size_t i = 0;
    while (*s && (i < len)) {
        if (*s != str[i]) {
            return false;
        }
        ++i;
        ++s;
    }
    return true;
}

void trim_from_right_until_find_char(char **text_ptr, const char chr)
{
    if (!**text_ptr) return;

    char *c = *text_ptr;
    while (*c) ++c;
    --c;

    while (*c != **text_ptr) {
        --c;
        if (*c == chr) {
            while (*c) {
                *c = '\0';
                ++c;
            }
            return;
        }
    }
}

void copy_string(char *dest, const char *other)
{
    while ((*dest = *other)) {
        ++dest;
        ++other;
    }
}

void copy_string(char *dest, const char *other, size_t len)
{
    while ((*dest = *other) && (len > 0)) {
        ++dest;
        ++other;
        --len;
    }
}

const char *eat_until_find_char_from_right(const char **text_ptr, const char chr)
{
    if (!*text_ptr) return NULL;

    const char *s = *text_ptr;  // Return value
    const char *c = *text_ptr;  // Iterator

    while (*c) {
        if (*c == chr) s = c;

        ++c;
    }

    return s;
}

char *eat_until_find_char_from_right(char **text_ptr, const char chr)
{
    if (!*text_ptr) return NULL;

    char *s = *text_ptr;  // Return value
    char *c = *text_ptr;  // Iterator

    while (*c) {
        if (*c == chr) s = c;

        ++c;
    }

    return s;
}
