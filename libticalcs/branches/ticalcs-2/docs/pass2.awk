#! /usr/bin/awk
# extract - create cross-links starting at index and html files
# Copyright (C) Romain Lievin 2002
# Contact: roms@tilp.info
#

BEGIN {
    if (ARGC < 3) {
	print "usage: cat <src_file> | extract.awk src=<file> tbl=<index> dst=<folder>";
	exit;
    }
}

# Load the 'index' file into an array such as: 
# array[ticablelinkcable.put] = <a href="ticablelinkcable.put.html">put</a>
/<html>/ {
    while ((getline line < tbl) > 0) {
	nf = split(line, array, /\./);
	entry = "@" line "@";
	table[entry] = "<a href=\"" line ".html" "\">" array[nf] "</a>";
    }

    # build filename
    nf = split(src, array, /\//);
    sub(/.html/, "", array[nf]);
    dest = dst "/" array[nf] ".html";
}

# Search & replace by using the array
{ 
#    print $0;
    for (s in table) {
	sub(s, table[s], $0);
    } 
    print $0 > dest;
}

END { 
}
