#! /usr/bin/awk
# scan - scan source files for exported functions
# Copyright (C) Romain Lievin 2003
# Contact: roms@tilp.info
#

BEGIN {
    if (ARGC < 2) {
	print "usage: cat <src_file> | awk -f scan.awk dst=<file>";
	exit;
    }
}

/TICALL/ {
    nf = split($0, array, /TICALL/);
#    print array[nf] >> dst;
    nf2 = split(array[nf], array2, /\(/);
    print array2[1] >> dst;
}

END { 
}
