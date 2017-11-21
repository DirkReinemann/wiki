# wiki

This is my personal wiki that consists of markdown files. I run it as a local server with mongoose
on port 8000.

# commands

## makefile

| command | description |
| - | - |
| make | compiles source files to wiki executable |
| make compile | same as above |
| make clean | clean compiled and temp files |
| make start | start local server |
| make build | convert markdown to html and create other content for server |
| make clean-outdir | delete html directory |

## wiki.sh

| command | description |
| - | - |
| ./wiki.sh -s [KEYWORD] | search fopr keyword in markdown files |
