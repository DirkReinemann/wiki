#!/bin/bash

usage()
{
    echo "wiki helper script"
    echo
    echo "Usage: $0 [Options]"
    echo
    echo "Options:"
    printf "  %-20s %s\n" "-s [KEYWORD]" "search for keyword"
    exit 1
}

search()
{
    grep --color -rniE "$1" markdown
}

if [ $# == 0 ]; then
    usage
fi

while getopts "s:" OPT; do
    case $OPT in
        s)
            search $OPTARG
        ;;
        \?)
            usage
        ;;
        :)
            usage
        ;;
    esac
done

exit 0
