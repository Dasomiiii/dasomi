#include "resource_file_manager.hh"

#include <dirent.h>
#include <memory.h>
#include <sys/stat.h>
#include "console_args.hh"
#include "post.hh"
#include "util/file_tools.hh"
#include "util/list.hh"
#include "util/logging.hh"

static List<Resource_File> resource_files;

void Resource_File::init(const char *filepath)
{
    this->filepath.init();
    this->filepath.append_string(filepath);

    const char *name = eat_until_find_char_from_right(&filepath, '/');
    ++name;  // So that the first char isn't '/'

    this->name.init();
    this->name.append_string(name);

    //
    // Get the file type
    //

    const char *extension = eat_until_find_char_from_right(&name, '.');
    ++extension;  // So that the first char isn't '.'

    if (compare_string(extension, "post"))
        filetype = RESOURCE_FILE_TYPE_POST;

    else if (compare_string(extension, "jpg"))
        filetype = RESOURCE_FILE_TYPE_IMAGE;
    else if (compare_string(extension, "jpeg"))
        filetype = RESOURCE_FILE_TYPE_IMAGE;
    else if (compare_string(extension, "png"))
        filetype = RESOURCE_FILE_TYPE_IMAGE;
    else if (compare_string(extension, "gif"))
        filetype = RESOURCE_FILE_TYPE_IMAGE;
    else if (compare_string(extension, "svg"))
        filetype = RESOURCE_FILE_TYPE_IMAGE;
    else if (compare_string(extension, "ico"))
        filetype = RESOURCE_FILE_TYPE_IMAGE;

    else if (compare_string(extension, "ttf"))
        filetype = RESOURCE_FILE_TYPE_FONT;
    else if (compare_string(extension, "otf"))
        filetype = RESOURCE_FILE_TYPE_FONT;
    else if (compare_string(extension, "woff"))
        filetype = RESOURCE_FILE_TYPE_FONT;
    else if (compare_string(extension, "woff2"))
        filetype = RESOURCE_FILE_TYPE_FONT;
    else if (compare_string(extension, "eot"))
        filetype = RESOURCE_FILE_TYPE_FONT;

    else if (compare_string(extension, "css"))
        filetype = RESOURCE_FILE_TYPE_CSS;

    else {
        LOG_WARNING("'%s' files are ignored", extension);
        filetype = RESOURCE_FILE_TYPE_INVALID;
    }
}

void Resource_File::destroy()
{
    name.destroy();
    filepath.destroy();
}

// ==================================================

void init_resource_file_list()
{
    resource_files.init();
}

void destroy_resource_file_list()
{
    resource_files.destroy();
}

void collect_resource_files_from_dir(const char *directory_path)
{
    DIR *dir = opendir(directory_path);
    if (!dir) return;

    struct dirent *dir_ptr;

    char filepath[1024];

    while ((dir_ptr = readdir(dir))) {
        if (dir_ptr->d_type == DT_DIR) {
            if (!compare_string(dir_ptr->d_name, ".") &&
                !compare_string(dir_ptr->d_name, "..")) {

                snprintf(filepath, sizeof(filepath),
                         "%s%s/", directory_path, dir_ptr->d_name);

                collect_resource_files_from_dir(filepath);
            }
        }
        else if (dir_ptr->d_type == DT_REG) {

            if (compare_string(dir_ptr->d_name, ".DS_Store") ||
                compare_string(dir_ptr->d_name, "page_template.html")) {
                goto end;
            }

            snprintf(filepath, sizeof(filepath), "%s%s",
                     directory_path, dir_ptr->d_name);

            Resource_File res_file;
            res_file.init(filepath);

            // Don't include invalid file types
            if (res_file.filetype == RESOURCE_FILE_TYPE_INVALID) {
                res_file.destroy();
                goto end;
            }

            // Make sure there aren't any other resource files with the same name
            for (size_t i = 0; i < resource_files.count; ++i) {
                if (compare_string(res_file.name.buf, resource_files.items[i].name.buf)) {
                    LOG_WARNING("A resource file called '%s' already exists! It doesn't matter if the files are in different directories!",
                                res_file.name.buf);
                    res_file.destroy();
                    goto end;
                }
            }

            resource_files.append(res_file);
        }

    end: ;
    }

    closedir(dir);
}

void handle_resource_files()
{
    size_t i, j;

    // Create directories for the output
    {
        remove_directory(console_args.output_path.buf);
        create_directory(console_args.output_path.buf);

        char directory_name[1024];

        snprintf(directory_name, sizeof(directory_name), "%sposts", console_args.output_path.buf);
        create_directory(directory_name);

        snprintf(directory_name, sizeof(directory_name), "%simages", console_args.output_path.buf);
        create_directory(directory_name);

        snprintf(directory_name, sizeof(directory_name), "%sfonts", console_args.output_path.buf);
        create_directory(directory_name);
    }

    // Sort the resource files by their filetype (the console output becomes a lot cleaner this way)
    {
        Resource_File *res_file_1, *res_file_2;
        Resource_File  temp;

        for (i = 0; i < resource_files.count - 1; ++i) {
            for (j = 0; j < resource_files.count - i - 1; ++j) {
                res_file_1 = &resource_files.items[j + 1];
                res_file_2 = &resource_files.items[j];
                if (res_file_1->filetype > res_file_2->filetype) {
                    memcpy(&temp,      res_file_1, sizeof(Resource_File));
                    memcpy(res_file_1, res_file_2, sizeof(Resource_File));
                    memcpy(res_file_2, &temp,      sizeof(Resource_File));
                }
            }
        }
    }

    Resource_File *res_file;

    char filepath[1024];

    for (i = 0; i < resource_files.count; ++i) {
        res_file = &resource_files.items[i];

        switch (res_file->filetype) {
            default:
                goto end;

            case RESOURCE_FILE_TYPE_POST:
            {
                write_post_from_file(res_file->filepath.buf);
                goto end;
            }

            case RESOURCE_FILE_TYPE_IMAGE:
            {
                snprintf(filepath, sizeof(filepath), "%simages/%s",
                         console_args.output_path.buf, res_file->name.buf);
                clone_file(res_file->filepath.buf, filepath);

                LOG_SUCCESS("Copied image '%s' to '%s'",
                            res_file->filepath.buf, filepath);
                goto end;
            }

            case RESOURCE_FILE_TYPE_FONT:
            {
                snprintf(filepath, sizeof(filepath), "%sfonts/%s",
                         console_args.output_path.buf, res_file->name.buf);
                clone_file(res_file->filepath.buf, filepath);

                LOG_SUCCESS("Copied font '%s' to '%s'",
                            res_file->filepath.buf, filepath);
                goto end;
            }

            case RESOURCE_FILE_TYPE_CSS:
            {
                snprintf(filepath, sizeof(filepath), "%s%s",
                         console_args.output_path.buf, res_file->name.buf);
                clone_file(res_file->filepath.buf, filepath);

                LOG_SUCCESS("Copied stylesheet '%s' to '%s'",
                            res_file->filepath.buf, filepath);
                goto end;
            }
        }
    end: ;
    }
}
