#! /usr/bin/awk
# index - generate the list of files

NF > 2 { gsub(/.txt/, "", $8); print ($8)}
