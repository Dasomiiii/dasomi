#include "console_args.hh"

#include <stdio.h>
#include <stdlib.h>
#include "util/file_tools.hh"
#include "util/logging.hh"
#include "util/string_tools.hh"

Console_Args console_args;

static const char help_msg[] =
"Dasomi is a static site generator for https://dasomi.neocities.org.\n"
"\n"
"Usage:\n"
    "\tdasomi [arguments]\n"
"\n"
"Arguments:\n"
    "\t-resources <string>\n"
    "\t\tSets the resource directory for post, image, font and css files.\n"
    "\t-output <string>\n"
    "\t\tSets the output directory. If the directory doesn't exist, it gets created. If the directory already exists, it gets replaced.";

static inline void print_help_message()
{
    printf("%s\n", help_msg);
    exit(EXIT_SUCCESS);
}

static inline void ensure_slash_at_end_of_path(String *path)
{
    if (path->buf[path->length - 1] != '/') {
        path->append('/');
    }
}

void parse_cmdline_arguments(const int argc, const char *argv[])
{
    if (argc <= 1) print_help_message();

    console_args.resource_path.init();
    console_args.output_path  .init();

    int i = 0;
    while (++i < argc) {

        if (compare_string(argv[i], "-resources")) {
            if (console_args.resource_path.buf[0]) {
                LOG_ERROR("Error: The source directory was set more than once");
                exit(EXIT_FAILURE);
            }
            if ((++i >= argc) || (argv[i][0] == '-')) {
                LOG_ERROR("Error: Expected directory after -r");
                exit(EXIT_FAILURE);
            }
            if (!does_directory_exist(argv[i])) {
                LOG_ERROR("Error: The resource directory doesn't exist");
                exit(EXIT_FAILURE);
            }

            console_args.resource_path.append_string(argv[i]);
            ensure_slash_at_end_of_path(&console_args.resource_path);
        }
        else if (compare_string(argv[i], "-output")) {

            if (console_args.output_path.buf[0]) {
                LOG_ERROR("Error: The output directory was set more than once");
                exit(EXIT_FAILURE);
            }
            if ((++i >= argc) || (argv[i][0] == '-')) {
                LOG_ERROR("Error: Expected directory after -r");
                exit(EXIT_FAILURE);
            }

            console_args.output_path.append_string(argv[i]);
            ensure_slash_at_end_of_path(&console_args.output_path);
        }
        else {
            print_help_message();
        }
    }

    if (!console_args.resource_path.buf[0]) {
        LOG_ERROR("Error: The source directory wasn't set");
        exit(EXIT_FAILURE);
    }
    if (!console_args.output_path.buf[0]) {
        LOG_ERROR("Error: The output directory wasn't set");
        exit(EXIT_FAILURE);
    }
}
