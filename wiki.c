#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <regex.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "mongoose.h"

const char *PORT = "127.0.0.1:8001";
const char *SRCDIR = "markdown";
const char *OUTDIR = "html";
const char *STATICDIR = "static";
const char *PANDOC_COMMAND = "pandoc %s -o %s";
const char *CONTENT_TYPE_JSON = "application/json";
const char *CONTENT_TYPE_TEXT = "text/plain";
const char *HIGHLIGHT_FORMAT = "<span class='highlight'>%s</span>";
const char *LINK_MARKDOWN_PATTERN = "href=\"%s\"";
const char *LINK_MARKDOWN_REPLACE = "onclick=\"loadFile('%s')\"";

char workpath[512];

size_t sworkpath = 0;
size_t ssrcdir = 0;
size_t soutdir = 0;

struct mg_serve_http_opts opts;

typedef enum contenttype {
    JSON,
    TEXT
} contenttype;

typedef struct searchresult {
    char *filename;
    char *lines;
} searchresult;

typedef struct searchresults {
    int           size;
    int           skeyword;
    searchresult *data;
    char *        keyword;
} searchresults;

typedef struct fileentry {
    char *relname;
} fileentry;

typedef struct fileentries {
    int        size;
    fileentry *data;
} fileentries;

void *traverse_directory(const char *path, void *(*ondir)(const char *path, const char *dirname, void *data),
                         void *(*onfile)(const char *path, const char *filename, void *data), void *data)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path)))
        return data;
    if (!(entry = readdir(dir)))
        return data;

    do {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            if (ondir != NULL)
                data = ondir(path, entry->d_name, data);

            size_t size = strlen(path) + strlen(entry->d_name) + 2;
            char dirpath[size];
            snprintf(dirpath, size, "%s/%s", path, entry->d_name);
            data = traverse_directory(dirpath, ondir, onfile, data);
        } else {
            if (onfile != NULL)
                data = onfile(path, entry->d_name, data);
        }
    } while ((entry = readdir(dir)) != NULL);
    closedir(dir);
    return data;
}

char *replace_in_string(char *value, char *sequence, char *replacement)
{
    char *result = (char *)malloc(1 * sizeof(char));

    result[0] = '\0';
    if (value == NULL)
        return result;

    size_t svalue = strlen(value);
    if (svalue == 0)
        return result;

    char *match = value;
    size_t pos = 0;
    size_t rpos = 0;
    size_t sreplacement = strlen(replacement);
    size_t ssequence = strlen(sequence);
    while ((match = strcasestr(match, sequence)) != NULL) {
        size_t oldpos = pos;
        size_t smatch = strlen(match);
        pos = svalue - smatch;
        size_t length = pos - oldpos;
        result = (char *)realloc(result, (rpos + length + sreplacement) * sizeof(char));
        if (length > 0)
            strncpy(result + rpos, value + oldpos, length);
        strncpy(result + rpos + length, replacement, sreplacement);
        rpos = rpos + length + sreplacement;
        match++;
        pos += ssequence;
    }
    size_t rest = svalue - pos;
    result = (char *)realloc(result, (rpos + rest + 1) * sizeof(char));
    strncpy(result + rpos, value + pos, rest);
    rpos += rest;
    result[rpos] = '\0';
    return result;
}

void str_to_lower(char *value)
{
    size_t size = strlen(value);
    for (unsigned long i = 0; i < size - 1; i++) {
        value[i] = tolower(value[i]);
    }
}

fileentry *contains_filename(fileentries *fs, const char *line)
{
    fileentry *result = NULL;
    for (int i = 0; i < fs->size && result == NULL; i++) {
        fileentry *f = fs->data + i;
        if (strstr(line, f->relname) != NULL)
            result = f;
    }
    return result;
}

void *link_markdown_link_onfile(const char *path, const char *filename, void *data)
{
    fileentries *fs = (fileentries *)data;
    size_t ssrcpath = strlen(path) + strlen(filename) + 2;
    char srcpath[ssrcpath];
    snprintf(srcpath, ssrcpath, "%s/%s", path, filename);
    size_t soutpath = ssrcpath + 4;
    char outpath[soutpath];
    snprintf(outpath, soutpath, "%s.tmp", srcpath);
    FILE *src = fopen(srcpath, "r");
    FILE *out = fopen(outpath, "w");
    if (src != NULL && out != NULL) {
        size_t size = 0;
        ssize_t read = 0;
        char *line = NULL;
        while ((read = getline(&line, &size, src)) != -1) {
            fileentry *f = contains_filename(fs, line);
            if (f != NULL) {
                size_t srelname = strlen(f->relname);
                size_t snewname = srelname + 2;
                char newname[snewname + 1];
                strncpy(newname, f->relname, srelname - 2);
                strncpy(newname + srelname - 2, "html", 4);
                newname[snewname] = '\0';
                size_t spattern = strlen(LINK_MARKDOWN_PATTERN) + strlen(f->relname);
                char pattern[spattern];
                snprintf(pattern, spattern, LINK_MARKDOWN_PATTERN, f->relname);
                size_t sreplace = strlen(LINK_MARKDOWN_REPLACE) + strlen(newname);
                char replace[sreplace];
                snprintf(replace, sreplace, LINK_MARKDOWN_REPLACE, newname);
                char *newline = replace_in_string(line, pattern, replace);
                fputs(newline, out);
                free(newline);
            }
            else {
                fputs(line, out);
            }
        }
        if (line != NULL)
            free(line);
        fclose(src);
        fclose(out);
        if (remove(srcpath) == 0) {
            rename(outpath, srcpath);
            remove(outpath);
        }
    }
    return data;
}

void *link_markdown_collect_onfile(const char *path, const char *filename, void *data)
{
    fileentries *fs = (fileentries *)data;
    fs->data = (fileentry *)realloc(fs->data, (fs->size + 1) * sizeof(fileentry));
    const char *relpath = path + sworkpath + ssrcdir + 1;
    size_t srelpath = strlen(relpath);
    size_t sfilename = strlen(filename);
    fileentry *f = fs->data + fs->size;
    f->relname = (char *)malloc((sfilename + 1) * sizeof(char));
    if (srelpath > 0) {
        relpath++;
        f->relname = (char *)realloc(f->relname, (sfilename + srelpath) * sizeof(char));
        strncpy(f->relname, relpath, srelpath - 1);
        strncpy(f->relname + srelpath - 1, "/", 1);
    }
    strncpy(f->relname + srelpath, filename, sfilename);
    strncpy(f->relname + srelpath + sfilename, "\0", 1);
    fs->size++;
    return fs;
}

void link_markdown()
{
    fileentries fs;
    fs.size = 0;
    fs.data = (fileentry *)malloc(0 * sizeof(fileentry));
    size_t ssrcpath = sworkpath + ssrcdir + 2;
    char srcpath[ssrcpath];
    snprintf(srcpath, ssrcpath, "%s/%s", workpath, SRCDIR);
    size_t soutpath = sworkpath + soutdir + 2;
    char outpath[soutpath];
    snprintf(outpath, soutpath, "%s/%s", workpath, OUTDIR);
    traverse_directory(srcpath, NULL, link_markdown_collect_onfile, &fs);
    traverse_directory(outpath, NULL, link_markdown_link_onfile, &fs);
    for (int i = 0; i < fs.size; i++)
        free((fs.data + i)->relname);
    free(fs.data);
}

void write_markdown_toc(const char *filename, const char *toc)
{
    size_t sfilename = strlen(filename);
    char tempfile[sfilename + 5];
    snprintf(tempfile, sfilename + 5, "%s.tmp", filename);

    FILE *fsrc = fopen(filename, "r");
    if (fsrc != NULL) {
        FILE *fdst = fopen(tempfile, "w");
        if (fdst != NULL) {
            size_t size = 0;
            char *line = NULL;
            int ignore = 0;
            int empty = 0;
            ssize_t read = 0;
            regex_t regexcs;
            regcomp(&regexcs, "^<!-- toc -->", 0);

            fputs("<!-- toc -->\n", fdst);
            fputs(toc, fdst);
            fputs("<!-- toc -->\n", fdst);
            fputs("\n", fdst);
            while ((read = getline(&line, &size, fsrc)) != -1) {
                if (read == 1)
                    empty++;
                else
                    empty = 0;

                if (!regexec(&regexcs, line, 0, NULL, 0)) {
                    if (ignore == 0)
                        ignore = 1;
                    else
                        ignore = 0;
                } else {
                    if (ignore == 0 && empty < 2)
                        fputs(line, fdst);
                }
            }
            if (line != NULL)
                free(line);
            fclose(fdst);
        }
        fclose(fsrc);
    }

    if (remove(filename) == 0) {
        rename(tempfile, filename);
        remove(tempfile);
    }
}

void create_markdown_toc(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        char *line = NULL;
        size_t size = 0;
        regex_t regexhash;
        regcomp(&regexhash, "^#", 0);
        regex_t regexcode;
        regcomp(&regexcode, "^```", 0);
        int code = 0;
        char *toc = (char *)malloc(1 * sizeof(char));
        int pos = 0;
        const char *template = "* [%s](#%s)\n";
        size_t stemplate = strlen(template) - 4;
        while (getline(&line, &size, file) != -1) {
            if (!regexec(&regexcode, line, 0, NULL, 0)) {
                if (code == 0)
                    code = 1;
                else
                    code = 0;
            }
            if (!regexec(&regexhash, line, 0, NULL, 0) && code == 0) {
                size_t sline = strlen(line);
                for(unsigned long i = 0; i < sline; i++) { 
                    if (line[i] == '#') {
                        toc= (char *)realloc(toc, (pos + 3) * sizeof(char));
                        strncpy(toc + pos, "  ", 2);
                        pos += 2;
                    } else {
                        size_t sname = sline - i - 2;
                        char name[sname + 1];
                        strncpy(name, line + i + 1, sname);
                        name[sname] = '\0';
                        char *link = replace_in_string(name, " ", "-");
                        str_to_lower(link);
                        size_t slink = strlen(link);
                        size_t length = stemplate + sname + slink;
                        toc = (char *)realloc(toc, (pos + length + 1) * sizeof(char));
                        snprintf(toc + pos, length + 1, template, name, link);
                        pos += length;
                        free(link);
                        break;
                    }
                }
            }
        }
        if (line != NULL)
            free(line);
        fclose(file);
        toc[pos] = '\0';
        write_markdown_toc(filename, toc);
        free(toc);
    }
}

void *convert_ondir(const char *path, const char *dirname, void *data)
{
    size_t spath = strlen(path);
    size_t sdirname = strlen(dirname);
    size_t soutpath = spath - ssrcdir + soutdir + sdirname + 2;
    char outpath[soutpath];

    strncpy(outpath, path, spath - ssrcdir);
    strncpy(outpath + spath - ssrcdir, OUTDIR, soutdir);
    strncpy(outpath + spath - ssrcdir + soutdir, "/", 1);
    strncpy(outpath + spath - ssrcdir + soutdir + 1, dirname, sdirname);
    outpath[soutpath - 1] = '\0';

    mkdir(outpath, 0755);
    return data;
}

void *convert_onfile(const char *path, const char *filename, void *data)
{
    const char *ending = strrchr(filename, '.');

    if (ending && !strcmp(ending, ".md")) {
        char *token = strtok((char *)filename, ".");
        size_t stoken = strlen(token);

        const char *relpath = path + sworkpath + ssrcdir + 1;
        size_t srelpath = strlen(relpath);

        size_t ssrcfile = sworkpath + ssrcdir + srelpath + stoken + 6;
        char srcfile[ssrcfile];
        snprintf(srcfile, ssrcfile, "%s/%s%s/%s.md", workpath, SRCDIR, relpath, token);

        size_t soutfile = sworkpath + soutdir + srelpath + stoken + 8;
        char outfile[soutfile];
        snprintf(outfile, soutfile, "%s/%s%s/%s.html", workpath, OUTDIR, relpath, token);

        size_t scommand = ssrcfile + soutfile + strlen(PANDOC_COMMAND);
        char command[scommand];
        snprintf(command, scommand, PANDOC_COMMAND, srcfile, outfile);

        create_markdown_toc(srcfile);

        system(command);
    }
    return data;
}

void *delete_directory_onfile(const char *path, const char *filename, void *data)
{
    size_t sfilepath = strlen(path) + strlen(filename) + 2;
    char filepath[sfilepath];
    snprintf(filepath, sfilepath, "%s/%s", path, filename);
    remove(filepath);
    return data;
}

void *delete_directory_ondir(const char *path, const char *dirname, void *data)
{
    fileentries *fs = (fileentries *)data;
    fs->data = realloc(fs->data, (fs->size + 1) * sizeof(fileentry));
    size_t sdirpath = strlen(path) + strlen(dirname) + 2;
    fileentry *f = fs->data + fs->size;
    f->relname = (char *)malloc(sdirpath * sizeof(char));
    snprintf(f->relname, sdirpath, "%s/%s", path, dirname);
    fs->size++;
    return fs;
}

void delete_directory(const char* path)
{
    fileentries fs;
    fs.data = (fileentry *)malloc(0 * sizeof(fileentry));
    fs.size = 0;
    traverse_directory(path, delete_directory_ondir, delete_directory_onfile, &fs);
    for (int i = 0; i < fs.size; i++) {
        fileentry *f = fs.data + i;
        remove(f->relname);
    }
}

void convert()
{
    size_t ssrcpath = strlen(workpath) + ssrcdir + 2;
    char srcpath[ssrcpath];
    size_t soutpath = strlen(workpath) + soutdir + 2;
    char outpath[soutpath];

    snprintf(srcpath, ssrcpath, "%s/%s", workpath, SRCDIR);
    snprintf(outpath, soutpath, "%s/%s", workpath, OUTDIR);
    delete_directory(outpath);
    mkdir(outpath, 0755);
    traverse_directory(srcpath, convert_ondir, convert_onfile, NULL);
    link_markdown();
}

void set_default_header(struct mg_connection *connection, contenttype c)
{
    const char *headerfmt = "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nTransfer-Encoding: chunked\r\n\r\n";
    char *type = NULL;
    size_t size = 0;

    switch (c) {
    case JSON: {
        size = strlen(CONTENT_TYPE_JSON) + 1;
        type = (char *)malloc(size * sizeof(char));
        strncpy(type, CONTENT_TYPE_JSON, size);
        break;
    }
    case TEXT: {
        size = strlen(CONTENT_TYPE_TEXT) + 1;
        type = (char *)malloc(size * sizeof(char));
        strncpy(type, CONTENT_TYPE_TEXT, size);
        break;
    }
    }
    type[size - 1] = '\0';
    size_t sheader = strlen(headerfmt) + size;
    char header[sheader];
    snprintf(header, sheader, headerfmt, type);
    mg_printf(connection, "%s", header);
    free(type);
}

void set_error_header(struct mg_connection *connection)
{
    mg_printf(connection, "HTTP/1.1 400 Bad Request\r\nContent-Type: %s\r\nTransfer-Encoding: chunked\r\n\r\n", CONTENT_TYPE_TEXT);
}

void *handle_filelist_onfile(const char *path, const char *filename, void *data)
{
    const char *relpath = path + sworkpath + soutdir + 2;
    char *filelist = (char *)data;

    size_t srelpath = strlen(relpath);
    size_t sfilename = strlen(filename);
    size_t sfilelist = strlen(filelist);
    size_t size = srelpath + sfilename + sfilelist + 4;

    if (srelpath > 0)
        size++;

    filelist = (char *)realloc(data, size * sizeof(char));

    strncpy(filelist + sfilelist, "\"", 1);
    if (srelpath > 0) {
        strncpy(filelist + sfilelist + 1, relpath, srelpath);
        strncpy(filelist + sfilelist + srelpath + 1, "/", 1);
        strncpy(filelist + sfilelist + srelpath + 2, filename, sfilename);
        strncpy(filelist + sfilelist + srelpath + sfilename + 2, "\",", 2);
    } else {
        strncpy(filelist + sfilelist + 1, filename, sfilename);
        strncpy(filelist + sfilelist + sfilename + 1, "\",", 2);
    }
    filelist[size - 1] = '\0';
    return filelist;
}

void handle_filelist(struct mg_connection *connection)
{
    char *filelist = (char *)malloc(1 * sizeof(char));
    size_t spath = sworkpath + soutdir + 2;
    char path[spath];

    snprintf(path, spath, "%s/%s", workpath, OUTDIR);

    filelist[0] = '\0';
    filelist = (char *)traverse_directory(path, NULL, handle_filelist_onfile, filelist);
    size_t sfilelist = strlen(filelist);
    if (sfilelist > 0)
        filelist[sfilelist - 1] = '\0';

    set_default_header(connection, JSON);
    mg_printf_http_chunk(connection, "[%s]", filelist);
    mg_send_http_chunk(connection, "", 0);
    free(filelist);
}

void handle_file(struct mg_connection *connection, struct http_message *message)
{
    char name[512];

    mg_get_http_var(&message->body, "filename", name, 512);

    size_t sfilename = sworkpath + soutdir + strlen(name) + 3;
    char filename[sfilename];
    snprintf(filename, sfilename, "%s/%s/%s", workpath, OUTDIR, name);

    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        char *line = NULL;
        ssize_t read = 0;
        size_t len = 0;
        set_default_header(connection, TEXT);
        while ((read = getline(&line, &len, file)) != -1) {
            mg_printf_http_chunk(connection, "%s", line);
            if (line != NULL) {
                free(line);
                line = NULL;
            }
        }
        fclose(file);
    } else {
        set_error_header(connection);
    }
    mg_send_http_chunk(connection, "", 0);
}

void *handle_search_onfile(const char *path, const char *filename, void *data)
{
    const char *relpath = path + sworkpath + ssrcdir + 1;
    searchresults *ss = (searchresults *)data;
    size_t sfilename = strlen(filename);
    size_t sfilepath = strlen(path) + sfilename + 2;
    size_t srelpath = strlen(relpath);
    size_t srelname = srelpath + sfilename + 3;
    char filepath[sfilepath];
    char relname[srelname];

    snprintf(filepath, sfilepath, "%s/%s", path, filename);
    if (srelpath > 0)
        snprintf(relname, srelname, "%s/%s", relpath + 1, filename);
    else
        snprintf(relname, srelname, "%s", filename);

    strncpy(relname + srelname - 5, "html", 4);
    relname[srelname - 1] = '\0';

    FILE *file = fopen(filepath, "r");
    if (file != NULL) {
        char *line = NULL;
        ssize_t read = 0;
        size_t len = 0;
        int alloc = 0;
        while ((read = getline(&line, &len, file)) != -1) {
            char *pos = strcasestr(line, ss->keyword);
            if (pos != NULL) {
                char match[ss->skeyword + 1];
                strncpy(match, pos, ss->skeyword);
                match[ss->skeyword] = '\0';
                size_t sreplacement = strlen(HIGHLIGHT_FORMAT) + ss->skeyword;
                char replacement[sreplacement];
                snprintf(replacement, sreplacement, HIGHLIGHT_FORMAT, match);
                char *highlighted = replace_in_string(line, ss->keyword, replacement);
                char *escaped = replace_in_string(highlighted, (char *)"\"", (char *)"\\\"");
                size_t sescaped = strlen(escaped) - 1;
                if (alloc == 0) {
                    ss->data = (searchresult *)realloc(ss->data, (ss->size + 1) * sizeof(searchresult));
                    searchresult *s = ss->data + ss->size;
                    s->filename = (char *)malloc((srelname + 1) * sizeof(char));
                    strncpy(s->filename, relname, srelname);
                    s->filename[srelname] = '\0';
                    s->lines = (char *)malloc((sescaped + 4) * sizeof(char));
                    strncpy(s->lines, "[\"", 2);
                    strncpy(s->lines + 2, escaped, sescaped);
                    strncpy(s->lines + sescaped + 2, "\"", 1);
                    s->lines[sescaped + 3] = '\0';
                    alloc = 1;
                } else {
                    searchresult *s = ss->data + ss->size;
                    size_t slines = strlen(s->lines);
                    s->lines = (char *)realloc(s->lines, (slines + sescaped + 4) * sizeof(char));
                    strncpy(s->lines + slines, ",\"", 2);
                    strncpy(s->lines + slines + 2, escaped, sescaped);
                    strncpy(s->lines + slines + sescaped + 2, "\"", 1);
                    s->lines[slines + sescaped + 3] = '\0';
                }
                free(highlighted);
                free(escaped);
            }
            if (line != NULL) {
                free(line);
                line = NULL;
            }
        }
        if (alloc == 1) {
            searchresult *s = ss->data + ss->size;
            size_t slines = strlen(s->lines);
            s->lines = (char *)realloc(s->lines, (slines + 2) * sizeof(char));
            s->lines[slines] = ']';
            s->lines[slines + 1] = '\0';
            ss->size++;
        }
        fclose(file);
    }
    return ss;
}

void handle_search(struct mg_connection *connection, struct http_message *message)
{
    char keyword[512];
    size_t spath = sworkpath + ssrcdir + 2;
    char path[spath];

    snprintf(path, spath, "%s/%s", workpath, SRCDIR);

    mg_get_http_var(&message->body, "keyword", keyword, 512);

    searchresults ss;
    ss.size = 0;
    ss.data = (searchresult *)malloc(0 * sizeof(searchresult));
    size_t skeyword = strlen(keyword);
    ss.keyword = (char *)malloc((skeyword + 1) * sizeof(char));
    strncpy(ss.keyword, keyword, skeyword);
    ss.keyword[skeyword] = '\0';
    ss.skeyword = skeyword;

    traverse_directory(path, NULL, handle_search_onfile, &ss);
    set_default_header(connection, JSON);
    mg_printf_http_chunk(connection, "%s", "[");
    for (int i = 0; i < ss.size; i++) {
        searchresult *s = ss.data + i;
        if (i == ss.size - 1)
            mg_printf_http_chunk(connection, "{\"filename\":\"%s\",\"lines\":%s}", s->filename, s->lines);
        else
            mg_printf_http_chunk(connection, "{\"filename\":\"%s\",\"lines\":%s},", s->filename, s->lines);

        free(s->filename);
        free(s->lines);
    }
    mg_printf_http_chunk(connection, "%s", "]");
    mg_send_http_chunk(connection, "", 0);
    free(ss.data);
    free(ss.keyword);
}

void handle_convert(struct mg_connection *connection)
{
    convert();
    set_default_header(connection, TEXT);
    mg_send_http_chunk(connection, "", 0);
}

void request_handler(struct mg_connection *connection, int event, void *data)
{
    struct http_message *message = (struct http_message *)data;

    switch (event) {
    case MG_EV_HTTP_REQUEST:
        if (mg_vcmp(&message->uri, "/filelist") == 0)
            handle_filelist(connection);
        else if (mg_vcmp(&message->uri, "/file") == 0)
            handle_file(connection, message);
        else if (mg_vcmp(&message->uri, "/search") == 0)
            handle_search(connection, message);
        else if (mg_vcmp(&message->uri, "/convert") == 0)
            handle_convert(connection);
        else
            mg_serve_http(connection, message, opts);
        break;
    default:
        break;
    }
}

void server()
{
    struct mg_mgr mgr;
    struct mg_connection *connection;

    mg_mgr_init(&mgr, NULL);
    connection = mg_bind(&mgr, PORT, request_handler);
    if (connection == NULL) {
        fprintf(stderr, "Error while starting the wiki on '%s'.\n", PORT);
    } else {
        mg_set_protocol_http_websocket(connection);
        opts.document_root = ".";
        fprintf(stderr, "Starting wiki on '%s'.\n", PORT);

        for (;;)
            mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);
}

void print_usage(const char *name)
{
    printf("Usage: %s [Options]\n\n", name);
    printf("Options:\n");
    printf("  %-20s %s\n", "-s", "start server");
    printf("  %-20s %s\n", "-c", "convert markdown to html");
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        print_usage(argv[0]);
        exit(1);
    }

    char *option = argv[1];
    if (option == NULL) {
        print_usage(argv[1]);
        exit(1);
    }

    if (getcwd(workpath, 512) == NULL) {
        fprintf(stderr, "Error while reading the current work directory.");
        exit(1);
    }

    sworkpath = strlen(workpath);
    ssrcdir = strlen(SRCDIR);
    soutdir = strlen(OUTDIR);

    if (strcmp(option, "-s") == 0) {
        server();
    } else if (strcmp(option, "-c") == 0) {
        convert();
    } else {
        print_usage(argv[0]);
        exit(1);
    }
    return 0;
}
