#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "mongoose.h"

const char *PORT = "8000";
const char *SRCDIR = "markdown";
const char *OUTDIR = "html";
const char *STATICDIR = "static";
const char *PANDOC_COMMAND = "pandoc %s -o %s";
const char *CONTENT_TYPE_JSON = "application/json";
const char *CONTENT_TYPE_TEXT = "text/plain";

char workpath[512];

size_t sworkpath = 0;
size_t ssrcdir = 0;
size_t soutdir = 0;

struct mg_serve_http_opts opts;

typedef enum {
    JSON,
    TEXT
} contenttype;

int render_file(const char *relpath, const size_t srelpath)
{
    size_t ssrcfile, soutfile, scommand;

    ssrcfile = sworkpath + ssrcdir + srelpath + 4;
    char srcfile[ssrcfile];
    snprintf(srcfile, ssrcfile, "%s/%s%s.md", workpath, SRCDIR, relpath);

    soutfile = sworkpath + soutdir + srelpath + 6;
    char outfile[soutfile];
    snprintf(outfile, soutfile, "%s/%s%s.html", workpath, OUTDIR, relpath);

    scommand = ssrcfile + soutfile + strlen(PANDOC_COMMAND);
    char command[scommand];
    snprintf(command, scommand, PANDOC_COMMAND, srcfile, outfile);

    return system(command);
}

void create_outdir(const char *relpath, const size_t srelpath)
{
    size_t soutpath = sworkpath + soutdir + srelpath + 2;
    char outpath[soutpath];

    snprintf(outpath, soutpath, "%s/%s%s", workpath, OUTDIR, relpath);
    mkdir(outpath, 0755);
}

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

void read_directory(const char *path, const size_t spath)
{
    DIR *dir;
    struct dirent *entry;
    size_t size = 0;

    size = sworkpath + ssrcdir + spath + 2;
    char basepath[size];
    snprintf(basepath, size, "%s/%s/%s", workpath, SRCDIR, path);

    if (!(dir = opendir(basepath)))
        return;
    if (!(entry = readdir(dir)))
        return;

    do {
        size = spath + strlen(entry->d_name) + 2;
        char relpath[size];
        snprintf(relpath, size, "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            create_outdir(relpath, size);
            read_directory(relpath, size);
        } else {
            char *ending = strrchr(entry->d_name, '.');
            if (ending && !strcmp(ending, ".md")) {
                char *token = strtok(relpath, ".");
                render_file(token, size - 3);
            }
        }
    } while ((entry = readdir(dir)) != NULL);
    closedir(dir);
}

void render()
{
    create_outdir("", 0);
    read_directory("", 0);
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
    set_default_header(connection, TEXT);

    size_t sfilename = sworkpath + soutdir + strlen(name) + 3;
    char filename[sfilename];
    snprintf(filename, sfilename, "%s/%s/%s", workpath, OUTDIR, name);

    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        char *line = NULL;
        ssize_t read = 0;
        size_t len = 0;
        while ((read = getline(&line, &len, file)) != -1)
            mg_printf_http_chunk(connection, "%s", line);
        if (line != NULL) {
            free(line);
            line = NULL;
        }
        fclose(file);
    }
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
    mg_set_protocol_http_websocket(connection);
    opts.document_root = ".";
    printf("Starting wiki on port %s.\n", PORT);

    for (;;)
        mg_mgr_poll(&mgr, 1000);

    mg_mgr_free(&mgr);
}

void print_usage(const char *name)
{
    printf("Usage: %s [Options]\n\n", name);
    printf("Options:\n");
    printf("  %-20s %s\n", "-s", "start server");
    printf("  %-20s %s\n", "-r", "render markdown");
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
        printf("Error while reading the current work directory.");
        exit(1);
    }

    sworkpath = strlen(workpath);
    ssrcdir = strlen(SRCDIR);
    soutdir = strlen(OUTDIR);

    if (strcmp(option, "-s") == 0) {
        server();
    } else if (strcmp(option, "-r") == 0) {
        render();
    } else {
        print_usage(argv[0]);
        exit(1);
    }
    return 0;
}
