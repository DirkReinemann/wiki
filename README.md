# wiki

This is my personal wiki that consists of markdown files. I run it as a local mongoose server on port 8001.

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
| make install | copies wiki.bin to /usr/bin/wiki and adds, starts, enables systemd service (change wikidir in wiki.bin first!) |
| make uninstall | removes wiki.bin from /usr/bin/wiki and systemd service  |
| make update | copies wiki.bin to /usr/bin/wiki |

## wiki

| command | description |
| - | - |
| ./wiki -s | start local server on port 8001 |
| ./wiki -c | convert markdown to html |

## wiki.sh

| command | description |
| - | - |
| ./wiki.sh -s [KEYWORD] | search for keyword in markdown files |
