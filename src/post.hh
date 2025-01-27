#pragma once

#include "util/types.hh"
//#include "tag.hh"

#define MAX_TAG_COUNT    10
#define TAG_NAME_LENGTH  50

struct Date {
    u16 year, month, day;
};

struct Tag {
    char name[TAG_NAME_LENGTH];
};

struct Post {
    char title       [100];
    char site_address[150];

    Date date;

    Tag tags[MAX_TAG_COUNT];
};

bool init_post_writer_system();
void destroy_post_writer_system();

bool write_post_from_file(const char *filepath);

bool write_homepage_file();
