#pragma once

#include "util/string_tools.hh"
#include "util/types.hh"

extern struct Console_Args {
    String resource_path;
    String output_path;
} console_args;  // This variable gets declared in console_args.cc

void parse_cmdline_arguments(const int argc, const char *argv[]);
