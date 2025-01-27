#include "post.hh"

#include <assert.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <memory.h>
#include <ctype.h>
#include "util/file_tools.hh"
#include "util/string_tools.hh"
#include "util/list.hh"
#include "console_args.hh"
#include "util/logging.hh"

static List<Post> posts;

// A local html-file gets written to this
static char *page_template;

// This is for the html-file that gets outputed
static String post_html_file_buf;

// This is for the post contents
static String content_buf;

static String image_filepath_buf;
static String key_buf;
static String html_filepath_buf;

static bool does_post_already_exist(const char *title)
{
    for (size_t i = 0; i < posts.count; ++i) {
        if (compare_string(posts.items[i].title, title)) {
            return true;
        }
    }
    return false;
}

static bool get_html_from_post(String *dest, char *c)
{
    if (!*c) return false;

    dest->clear();
    char last_char = '\0';

    bool is_text_bold      = false;
    bool is_text_italic    = false;
    bool is_writing_p_text = false;

    int newlines_in_row = 0;

    size_t i;

    while (*c) {
        if (last_char != '\\') {
            // TEST!
            #define FUCK_FUCK_FUCK                      \
                {                                       \
                    if (is_writing_p_text) {            \
                        is_writing_p_text = false;      \
                        dest->append_string("</p>\n");  \
                    }                                   \
                }


            if (*c == '#') {
                FUCK_FUCK_FUCK
                ++c;
                if (isspace(*c)) {
                    eat_spaces(&c);
                    dest->append_string("<h2>");
                    while (*c && *c != '\n') {
                        dest->append(*c);
                        ++c;
                    }
                    dest->append_string("</h2>\n");
                    goto end;
                }
            }
            else if (*c == '[') {
                ++c;
                key_buf.clear();
                while (*c && (*c != ']')) {
                    key_buf.append(*c);
                    ++c;
                }
                ++c;
                eat_spaces(&c);

                if (compare_string(key_buf.buf, "img")) {
                    FUCK_FUCK_FUCK
                    if (*c != '"') {
                        LOG_WARNING("Warning: Missing '\"' after [img]!");
                        goto end;
                    }
                    ++c;

                    image_filepath_buf.clear();
                    while (*c && (*c != '"')) {
                        if (*c == '\n') {
                            /*
                            printf("Warning: Missing '\"' at end of image filepath in '%s'!\n",
                                   filepath);
                            */
                            LOG_WARNING("Warning: Missing '\"' at end of image filepath!");
                            ++c;
                            goto end;
                        }
                        image_filepath_buf.append(*c);
                        ++c;
                    }
                    ++c;
                    dest->append_string("<img src=\"");
                    dest->append_string("../");  // Since the image directory is located a directory below the post directory
                    dest->append_string(image_filepath_buf.buf);
                    dest->append_string("\"></img>\n");
                }
                else if (compare_string(key_buf.buf, "link")) {
                    if (*c != '"') {
                        /*
                        printf("Warning: Missing '\"' before link address in '%s'!\n",
                               filepath);
                        */
                        LOG_WARNING("Warning: Missing '\"' before link address!");
                        goto end;
                    }

                    dest->append_string("<a href=");
                    ++c;
                    while (*c && (*c != '"')) {
                        dest->append(*c);
                        ++c;
                    }
                    dest->append('>');

                    ++c;
                    eat_spaces(&c);

                    if (*c != '(') {
                        /*
                        printf("Warning: Missing left parentheses after [link] in '%s'!\n",
                               filepath);
                        */
                        LOG_WARNING("Warning: Missing left parentheses after [link]!");
                        goto end;
                    }
                    last_char = *c;
                    ++c;

                    while (true) {
                        if (last_char != '\\') {
                            if (*c == ')') {
                                goto end_parsing_clickable_text;
                            }
                        }
                        if (!*c) {
                            /*
                            printf("Warning: Missing right parentheses after hyperlink text in '%s'!\n",
                                   filepath);
                            */
                            LOG_WARNING("Warning: Missing right parentheses after hyperlink text!");
                            goto end;
                        }
                        dest->append(*c);
                        last_char = *c;
                        ++c;
                    }
                end_parsing_clickable_text:
                    dest->append_string("</a>");
                }
                else if (compare_string(key_buf.buf, "code")) {
                    FUCK_FUCK_FUCK
                    dest->append_string("<pre class=\"code-snippet\">\n");
                    dest->append_string("<code>");

                    const char end_key[] = "[code]";
                    while (*c) {
                        if (last_char != '\\') {
                            char const *c_before_looking_for_end_mark = c;
                            i = 0;
                            while (*c && end_key[i] && (*c == end_key[i])) {
                                ++c;
                                ++i;
                                if (i >= sizeof(end_key) - 1/*null terminator*/) {
                                    // We found the end of the code segment!
                                    while (isspace(dest->buf[dest->length - 1])) {
                                        dest->pop();
                                    }
                                    dest->append_string("</code>\n");
                                    dest->append_string("</pre>\n");
                                    goto end;
                                }
                            }
                            while (c_before_looking_for_end_mark < c) {
                                dest->append(*c_before_looking_for_end_mark);
                                ++c_before_looking_for_end_mark;
                            }
                        }
                        switch (*c) {
                            default:
                                dest->append(*c);
                                break;
                            case '<':
                                dest->append_string("&lt;");
                                break;
                            case '>':
                                dest->append_string("&gt;");
                                break;
                            case '\\':
                                if (*(c + 1) && (*(c + 1) != '[')) {
                                    dest->append('\\');
                                }
                                break;
                        }
                        last_char = *c;
                        ++c;
                    }
                    LOG_WARNING("Warning: Code segment didn't end!");
                }
                else {
                    /*
                    printf("Warning: Unrecognized post attribute '%s' in '%s'\n",
                           key_buf.buf, filepath);
                    */
                    LOG_WARNING("Warning: Unrecognized post attribute '%s'",
                                key_buf.buf);
                }
                goto end;
            }
            else if (*c == '*') {
                if (is_text_bold)
                    dest->append_string("</b>");
                else
                    dest->append_string("<b>");
                is_text_bold = !is_text_bold;
                goto end;
            }
            else if (*c == '_') {
                if (is_text_italic)
                    dest->append_string("</i>");
                else
                    dest->append_string("<i>");
                is_text_italic = !is_text_italic;
                goto end;
            }
        }

        if (!is_writing_p_text) {
            is_writing_p_text = true;
            dest->append_string("<p>");
        }

        if (*c == '\\') {
            if (last_char == '\\') {
                dest->append('\\');
            }
        }
        else {
            if (*c == '\n') {
                if ((++newlines_in_row >= 2) && is_writing_p_text) {
                    is_writing_p_text = true;
                    dest->append_string("<p>");
                }
            } else {
                newlines_in_row = 0;
            }
            dest->append(*c);
        }

    end:
        last_char = *c;
        ++c;
    }

    dest->append_string(c);

    return true;
}

// Note: This function alters the string source
static bool parse_date_from_string(Date *date_dest, char *string_src)
{
    if (!*string_src) return false;

    /*
        Accepted date formats in the .post file format:
        YYYY / MM / DD
        YYYY - MM - DD
        YYYY/MM/DD
        YYYY-MM-DD
        [any combination of separators listed above]
        YYYY MM DD
    */

    char *value;

    int i = 0;
    u16 * const value_ptrs[3] = {
        &date_dest->year,
        &date_dest->month,
        &date_dest->day
    };

    while (true) {
        eat_spaces(&string_src);
        if (!*string_src) return false;

        value = string_src;
        while (isnumber(*string_src)) {
            ++string_src;
        }
        if (*string_src) {
            *(string_src++) = '\0';
        }

        *value_ptrs[i++] = atoi(value);
        if (i >= (int)(sizeof(value_ptrs) / sizeof(*value_ptrs))) {
            break;
        }

        eat_spaces(&string_src);
        if (!*string_src || (*string_src == '\n')){
            LOG_ERROR("Error when parsing date: The date stopped after the %s!",
                      i == 1 ? "year" : (i == 2 ? "month" : "year"));
            return false;
        }

        if ((*string_src != '/') && (*string_src != '-') && !isnumber(*string_src)) {
            LOG_ERROR("Error when parsing date: Invalid date separator char '%c'!", *string_src);
            return false;
        }
        ++string_src;
    }

    return true;
}

static bool parse_tags_from_string(Post *post_dest, char *string_src)
{
    if (!*string_src) return false;

    u16  tag_count = 0;
    Tag *tag;
    size_t i;

    while (true) {
        eat_spaces(&string_src);
        if (!*string_src) break;

        tag = &post_dest->tags[tag_count++];

        i = 0;
        while (*string_src && (*string_src != ',')) {
            tag->name[i] = tolower(*string_src);
            ++string_src;
            ++i;
        }
        if (*string_src) ++string_src;

        // Remove trailing spaces
        while (isspace(tag->name[--i])) {
            tag->name[i] = '\0';
        }
    }

    return true;
}

// Note: This function returns a pointer to 'buffer', which is a static function
// variable, meaning that every time this function gets called, every string
// derived from this function will change (unless the returned string gets copied,
// of course)!
static const char *get_formatted_date(Date *date)
{
    static char buffer[100];

    char month_name[10];
    switch (date->month) {
        default:
            copy_string(month_name, "INVALID");
            break;
        case 1:
            copy_string(month_name, "January");
            break;
        case 2:
            copy_string(month_name, "February");
            break;
        case 3:
            copy_string(month_name, "Mars");
            break;
        case 4:
            copy_string(month_name, "April");
            break;
        case 5:
            copy_string(month_name, "May");
            break;
        case 6:
            copy_string(month_name, "June");
            break;
        case 7:
            copy_string(month_name, "July");
            break;
        case 8:
            copy_string(month_name, "August");
            break;
        case 9:
            copy_string(month_name, "September");
            break;
        case 10:
            copy_string(month_name, "October");
            break;
        case 11:
            copy_string(month_name, "November");
            break;
        case 12:
            copy_string(month_name, "December");
            break;
    }

    char day_ordinal_indicator[3];
    switch (date->day) {
        default:
            copy_string(day_ordinal_indicator, "th");
            break;
        case 1:
        case 21:
        case 31:
            copy_string(day_ordinal_indicator, "st");
            break;
        case 2:
        case 22:
            copy_string(day_ordinal_indicator, "nd");
            break;
        case 3:
        case 23:
            copy_string(day_ordinal_indicator, "rd");
            break;
    }

    snprintf(buffer, sizeof(buffer), "%s %d%s, %d",
             month_name, date->day, day_ordinal_indicator, date->year);

    return buffer;
}

// Note: Calling this function will change the string that every pointer received from
// this function points to
static void write_post_archive_entry(Post *post, String *buf)
{
    buf->append_string("<div class=\"post-archive-entry\">\n");
    {
        //
        // Write date and title
        //

        buf->append_string("<a href=\"");
        buf->append_string(post->site_address);
        buf->append_string("\">");
        {
            buf->append_string("<h4>");
            {
                buf->append_string(get_formatted_date(&post->date));
                buf->append_string(" • ");
                buf->append_string(post->title);
            }
            buf->append_string("</h4>\n");
        }
        buf->append_string("</a>\n");

        //
        // Write tags
        //

        buf->append_string("<p>Tags: ");
        {
            int i = 0;
            while (post->tags[i].name[0]) {
                //buf->append_string("<a href=\"tags/");
                //buf->append_string(post->tags[i].name);
                //buf->append_string(".html\">");
                buf->append_string(post->tags[i].name);
                //buf->append_string("</a>");

                if ((i < MAX_TAG_COUNT) && (post->tags[i + 1].name[0])) {
                    buf->append_string(", ");
                }

                ++i;
            }
        }
        buf->append_string("</p>\n");
    }
    buf->append_string("</div>\n");
}

bool write_post_from_file(const char *filepath)
{
    char *filedata = NULL;
    const size_t filesize = read_entire_file(filepath, &filedata);
    assert(filedata);

    char *c = filedata;
    char *line;

    size_t i;

    Post post_info;
    memset(&post_info, 0, sizeof(post_info));

    //
    // Parse post attributes
    //

    while (true) {
        eat_spaces(&c);
        if (!*c) {
            free(filedata);
            LOG_ERROR("Error: The post '%s' doesn't contain any content!", filepath);
            return false;
        }

        // Begin parsing the content instead of the attributes when reaching equal signs
        if (*c == '=') {
            eat_remaining_line(&c);
            break;
        }

        line = eat_remaining_line(&c);

        // Ignore comments
        if (line[0] == '#') {
            continue;
        }

        char *attribute = eat_until_find_char(&line, ':');
        cut_trailing_spaces(&attribute);

        eat_spaces(&line);
        char *value = eat_remaining_line(&line);
        cut_trailing_spaces(&value);

        if (compare_string(attribute, "title")) {
            if (does_post_already_exist(value)) {
                LOG_ERROR("Error: The post '%s' has already been written!\n", value);
                free(filedata);
                return false;
            }
            copy_string(post_info.title, value);
        }
        else if (compare_string(attribute, "date")) {
            parse_date_from_string(&post_info.date, value);
            if ((post_info.date.month < 1) || (post_info.date.month > 12)) {
                LOG_WARNING("Warning: The post '%s' has an invalid month index of '%d'",
                            filepath, post_info.date.month);
            }
        }
        else if (compare_string(attribute, "tags")) {
            parse_tags_from_string(&post_info, value);
        }
        else {
            LOG_WARNING("Warning: Unrecognized post attribute '%s' in '%s'",
                        attribute, filepath);
        }
    }

    //
    // Parse the content into html
    //

    get_html_from_post(&content_buf, c);

    //
    // Start writing the html buffer
    //

    post_html_file_buf.clear();
    c = page_template;

    while (*c && (c < (filedata + filesize))) {
        if (*c == '%') {
            // If there are two percentages in a row, that means we've reached a variable that can be replaced
            // by other text. The variable also end with two percentages.
            // For example: %%date%% would be replaced by the post's publication date.
            if (*(c + 1) == '%') {
                c += 2;
                key_buf.clear();
                int percentages_in_row = 0;
                i = 0;
                while (*c) {
                    if (*c == '%') {
                        if (++percentages_in_row >= 2) {
                            ++c;
                            goto while_end_1;
                        }
                    } else {
                        percentages_in_row = 0;
                    }
                    key_buf.append(*c);
                    ++c;
                }
            while_end_1:

                if (compare_string(key_buf.buf, "title")) {
                    post_html_file_buf.append_string(post_info.title);
                }
                else if (compare_string(key_buf.buf, "content")) {
                    post_html_file_buf.append_string(content_buf.buf);
                }
                else if (compare_string(key_buf.buf, "date")) {
                    //post_html_file_buf.append_string("IMAGINE THERES A FORMATTED DATE HERE");
                    post_html_file_buf.append_string(get_formatted_date(&post_info.date));
                }
                else {
                    LOG_WARNING("Warning: Unrecognized placeholder name '%%%s%%' in 'page_template.html'! Nothing was done.",
                                key_buf.buf);
                }
                continue;
            }
        }
        post_html_file_buf.append(*c);
        ++c;
    }

    free(filedata);

    //
    // Write the html buffer to a file
    //

    html_filepath_buf.clear();
    html_filepath_buf.append_string(filepath);

    // Remove the source directory at the start
    const size_t source_len = strlen(console_args.resource_path.buf);
    for (i = 0; i < source_len; ++i) {
        html_filepath_buf.pop_at(0);
    }

    html_filepath_buf.insert_string(console_args.output_path.buf, 0);
    html_filepath_buf.insert_string("posts/", console_args.output_path.length);

    // Trim the .post extension
    for (i = 0; i < sizeof(".post") - 1; ++i) {
        html_filepath_buf.pop();
    }
    html_filepath_buf.append_string(".html");

    FILE *f = fopen(html_filepath_buf.buf, "w");
    if (!f) {
        LOG_ERROR("Error: Failed to write file '%s'!", html_filepath_buf.buf);
        return false;
    }

    fwrite(post_html_file_buf.buf, post_html_file_buf.length, 1, f);
    fclose(f);

    LOG_SUCCESS("Wrote post '%s'", html_filepath_buf.buf);

    //
    // Save the post
    //

    // Remove the output path from the filepath
    for (i = 0; i < console_args.output_path.length; ++i) {
        html_filepath_buf.pop_at(0);
    }

    copy_string(post_info.site_address, html_filepath_buf.buf, sizeof(post_info.site_address));
    posts.append(post_info);

    return true;
}

bool write_homepage_file()
{
    size_t i, j;

    content_buf.clear();

    // Note to self: Hard-coding the entire html file here might be a temporary solution...
    const char *html_begin =
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
            "<head>\n"
                "<meta charset=\"UTF-8\">\n"
                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                "<title>Dasomi</title>\n"
                "<link href=\"style.css\" rel=\"stylesheet\" type=\"text/css\" media=\"all\">\n"
            "</head>\n"
            "<body>\n"
                "<div class=\"island\">\n"
                    "<div class=\"banner\">\n"
                        "<a>\n"
                            "<h1>Dasomi</h1>\n"
                        "</a>\n"
                    "</div>\n"
                    "<div class=\"main\">\n"
                        "<div class=\"content\">\n"
                            "<p>Dasomi loves mathematics, programming, art and music! They make our world spin!!</p>\n"
                            "<p>Contact: @locus_sacker on Discord</p>\n";
    const char *html_end =
                        "</div>\n"
                    "</div>\n"
                "</div>\n"
            "</body>\n"
        "</html>";

    // Sort posts by their date
    {
        Post  temp;
        Post *post_1, *post_2;

        #define SWAP                               \
            memcpy(&temp,  post_1, sizeof(Post));  \
            memcpy(post_1, post_2, sizeof(Post));  \
            memcpy(post_2, &temp,  sizeof(Post));

        for (i = 0; i < posts.count - 1; ++i) {
            for (j = 0; j < posts.count - i - 1; ++j) {
                post_1 = &posts.items[j + 1];
                post_2 = &posts.items[j];

                if (post_1->date.year > post_2->date.year) {
                    SWAP
                }
                else if ((post_1->date.year == post_2->date.year) &&
                        (post_1->date.month > post_2->date.month)) {
                    SWAP
                }
                else if ((post_1->date.month == post_2->date.month) &&
                        (post_1->date.day > post_2->date.day)) {
                    SWAP
                }
            }
        }

        #undef SWAP
    }

    //
    // Write html for the post registry
    //

    content_buf.append_string(html_begin);
    {
        content_buf.append_string("<h2>Post Registry</h2>\n");
        for (i = 0; i < posts.count; ++i) {
            write_post_archive_entry(&posts.items[i], &content_buf);
        }
    }
    content_buf.append_string(html_end);

    //
    // Write the file to disk
    //

    html_filepath_buf.clear();
    html_filepath_buf.append_string(console_args.output_path.buf);
    html_filepath_buf.append_string("index.html");

    FILE *f = fopen(html_filepath_buf.buf, "w");
    if (!f) {
        LOG_ERROR("Error: Failed to write homepage '%s'!", html_filepath_buf.buf);
        return false;
    }

    fwrite(content_buf.buf, content_buf.length, 1, f);
    fclose(f);

    LOG_SUCCESS("Wrote homepage '%s'", html_filepath_buf.buf);

    return true;
}

bool init_post_writer_system()
{
    {
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s%s", console_args.resource_path.buf, "page_template.html");
        read_entire_file(filepath, &page_template);

        if (!*filepath) {
            LOG_ERROR("You need to provide a 'page_template.html' file in the resource directory!");
            return false;
        }
    }

    posts             .init();
    post_html_file_buf.init();
    content_buf       .init();
    image_filepath_buf.init();
    key_buf           .init();
    html_filepath_buf .init();

    return true;
}

void destroy_post_writer_system()
{
    free(page_template);

    posts             .destroy();
    post_html_file_buf.destroy();
    content_buf       .destroy();
    image_filepath_buf.destroy();
    key_buf           .destroy();
    html_filepath_buf .destroy();
}
