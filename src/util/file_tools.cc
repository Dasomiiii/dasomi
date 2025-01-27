#include "file_tools.hh"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "string_tools.hh"

size_t read_entire_file(const char *filepath, char **dest)
{
    FILE *f = fopen(filepath, "r");
    assert(f);

    fseek(f, 0L, SEEK_END);
    const size_t size = ftell(f);
    rewind(f);

    char *data = (char *)malloc(size + 1/* Null terminator */);
    
    if (fread(data, 1, size, f) < 1) {
        free(data);
        fprintf(stderr, "Error: Failed to load file data from '%s'!\n", filepath);
        return -1;
    }

    // Null terminator
    data[size] = '\0';

    fclose(f);

    *dest = data;

    return size;
}

bool clone_file(const char *source_path, const char *dest_path)
{
    char *filedata = NULL;
    const size_t filesize = read_entire_file(source_path, &filedata);

    FILE *f = fopen(dest_path, "w");
    if (!f) {
        fprintf(stderr, "Error: Failed clone file '%s' since writing the destination file '%s' failed!\n",
                source_path, dest_path);
        free(filedata);
        return false;
    }

    fwrite(filedata, filesize, 1, f);
    fclose(f);

    memset(filedata, 0, filesize);
    free(filedata);

    return true;
}

bool does_directory_exist(const char *path)
{
    DIR *dir = opendir(path);
    if (dir) {
        closedir(dir);
        return true;
    }
    return false;
}

bool create_directory(const char *path)
{
    if (!path[0]) return false;

    mkdir(path, 0777);

    return true;
}

void remove_directory(const char *path)
{
    struct dirent *dir_ptr;
    DIR *dir = opendir(path);
    if (!dir) return;

    // Check if 'path' has a slash char at the end of it
    bool path_has_slash_at_end;
    {
        const char *c = path;

        while (*c) ++c;
        --c;

        path_has_slash_at_end = (*c == '/');
    }

    char full_path[1024];

    while ((dir_ptr = readdir(dir))) {

        snprintf(full_path, sizeof(full_path), "%s%s%s",
                 path, (path_has_slash_at_end ? "" : "/"), dir_ptr->d_name);

        if (dir_ptr->d_type == DT_DIR) {
            if (!compare_string(dir_ptr->d_name, ".") &&
                !compare_string(dir_ptr->d_name, "..")) {
                remove_directory(full_path);
            }
        }
        else if (dir_ptr->d_type == DT_REG) {
            remove(full_path);
        }
    }

    closedir(dir);

    rmdir(path);
}

