#pragma once

#include "types.hh"

size_t read_entire_file       (const char *filepath, char **dest);
size_t read_entire_binary_file(const char *filepath, u8   **dest);

bool clone_file(const char *source_path, const char *dest_path);

bool does_directory_exist(const char *path);

bool create_directory(const char *path);

void remove_directory(const char *path);
