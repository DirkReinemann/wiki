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
const char *PANDOC_COMMAND = "pandoc %s -s --highlight-style pygments -o %s";

char workpath[512];

size_t sworkpath = 0;
size_t ssrcdir = 0;
size_t soutdir = 0;

struct mg_serve_http_opts opts;

void render_file(const char *relpath, const size_t srelpath)
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

    system(command);
}

void create_outdir(const char *relpath, const size_t srelpath)
{
    size_t soutpath = sworkpath + soutdir + srelpath + 2;
    char outpath[soutpath];

    snprintf(outpath, soutpath, "%s/%s%s", workpath, OUTDIR, relpath);
    mkdir(outpath, 0755);
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
    if (getcwd(workpath, 512) == NULL) {
        printf("Error while reading the current work directory.");
        exit(1);
    }

    sworkpath = strlen(workpath);
    ssrcdir = strlen(SRCDIR);
    soutdir = strlen(OUTDIR);
    create_outdir("", 0);
    read_directory("", 0);
}
void request_handler(struct mg_connection *connection, int event, void *data)
{
    struct http_message *message = (struct http_message *)data;

    switch (event) {
    case MG_EV_HTTP_REQUEST:
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
    opts.document_root = OUTDIR;
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
