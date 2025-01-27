#include "console_args.hh"
#include "post.hh"
#include "resource_file_manager.hh"

int main(const int argc, const char *argv[])
{
    parse_cmdline_arguments(argc, argv);

    if (!init_post_writer_system()) {
        exit(EXIT_FAILURE);
    }

    init_resource_file_list();
    collect_resource_files_from_dir(console_args.resource_path.buf);

    handle_resource_files();
    write_homepage_file();

    destroy_resource_file_list();
    destroy_post_writer_system();
}
