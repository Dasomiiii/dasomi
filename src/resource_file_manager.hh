#pragma once

#include "util/string_tools.hh"

enum Resource_File_Type {
    RESOURCE_FILE_TYPE_INVALID = 0,
    RESOURCE_FILE_TYPE_POST,
    RESOURCE_FILE_TYPE_IMAGE,
    RESOURCE_FILE_TYPE_FONT,
    RESOURCE_FILE_TYPE_CSS
};

struct Resource_File {
    void init(const char *filepath);

    void destroy();

    String name, filepath;
    Resource_File_Type filetype;
};

void init_resource_file_list();
void destroy_resource_file_list();

void collect_resource_files_from_dir(const char *directory_path);

void handle_resource_files();
