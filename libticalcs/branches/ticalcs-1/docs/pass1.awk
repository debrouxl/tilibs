#! /usr/bin/awk
# pass1 - generate an html file starting at a text file
# Copyright (C) Romain Lievin 2002
# Contact: roms@tilp.info
#

function write(s) {
    print s > dest;
}

BEGIN {
    if (ARGC < 2) {
	print "usage: cat <src_file> | pass1.awk src=<file> dst=<folder>";
	exit;
    }

    Keywords["Name"] = "";
    Keywords["Type"] = "";
    Keywords["Header"] = "";
    Keywords["Definition"] = "";

    Types["Structure"] = "";
    Types["Function"] = "";
    Types["Define"] = "";
    Types["Enum"] = ""

    Main = "";

    Name = "";
    Type = "";
    Header = "";
    Definition = "";
}

# Get destination filename
/\[Main\]/ {
    Main = "Main";

    nf = split(src, array, /\//); 
    sub(/.txt/, "", array[nf]);
    dest = dst "/" array[nf] ".html";
}

# Get name and check
/Name=/ {
    nf = split($0, expr, /=/);	
    if ((nf < 2) || (!(expr[1] in Keywords))) {
	print "Illegal keyword !"; print $0; exit;
    }

    Name = expr[2];
}

# Get type and check
/Type=/ {
    nf = split($0, expr, /=/);
    if ((nf < 2) || (!(expr[1] in Keywords))) {
        print "Illegal keyword !"; print $0; exit;
    }

    if(!(expr[2] in Types)) {
	print "Illegal type !"; print $0; exit;
    }

    Type = expr[2];
}

# Get header and check
/Header=/ {
    nf = split($0, expr, /=/);
    if ((nf < 2) || (!(expr[1] in Keywords))) {
        print "Illegal keyword !"; print $0; exit;
    }

    Header = expr[2];
}

# Get definition and write html header
/Definition=/ {
    nf = split($0, expr, /=/);
    if ((nf < 2) || (!(expr[1] in Keywords))) {
        print "Illegal keyword !"; print $0; exit;
    }

    Definition = expr[2];

    write("<html>");
    print "<head>" > dest;
    print "<title>" Name "</title>" > dest;
    print "<link rel=\"stylesheet\" type=\"text/css\" HREF=\"style.css\">" > dest;
    print "</head>" > dest;
    print "<body bgcolor=\"#FFFFF8\">" > dest;
    print "<table class=\"INVTABLE\" width=\"100%\">" > dest;
    print "<tr>" > dest;
    print "<td class=\"NOBORDER\" width=\"40\"><img src=\"function.gif\" width=\"32\" height=\"32\" border=\"0\"></td>" > dest;
    print "<td class=\"TITLE\">" Name "</td>" > dest;
    print "<td class=\"DESCRIPTION\">" Type "</td>" > dest;
    print "</tr>" > dest;
    print "</table>" > dest;
    print "<hr>" > dest;
    print "<table class=\"NOBORDER\" width=\"100%\"><tr>" > dest;
    print "<td class=\"HEADER\" align=\"right\">" Header > dest;
    print "</td>" > dest;
    print "</tr></table>" > dest;
    print "<p><table class=\"DEFTABLE\"><tr><td class=\"DEFINITION\">" Definition "</td></tr></table>" > dest;
}

# Parse parameters and put them into a table
/\[Parameters\]/ ||  /\[Fields\]/ {
    if ($0 ~ /Parameters/)
	print "<p class=\"ITEMDESC\">" "Parameters" "<p>" > dest;
    else
	print "<p class=\"ITEMDESC\">" "Fields"     "<p>" > dest;

    print "<table class=\"NOBORDER\" width=\"100%\"><tr>" > dest;
    
    while ((getline > 0) && !(($0 ~ /\[/) && ($0 ~ /\]/))) {
        nf = split($0, expr, / : /);
	
	print "<tr>" > dest;
	print "<td valign=\"top\" align=\"left\">" expr[1] "<br>" > dest;
	print "</td>" > dest;
	print "<td valign=\"top\">" expr[2] "<br>" > dest;
	print "</td>" > dest;
	print "</tr>" > dest;	    
    }

    print "</table>" > dest;
}

# Copies summary
/\[Summary\]/ { 
    print "<p class=\"ITEMDESC\"> Summary <p>" > dest;

    while ((getline > 0) && !(($0 ~ /\[/) && ($0 ~ /\]/))) {
	print $0 > dest; 
    }
}

# Copies description
/\[Description\]/ { 
    print "<p class=\"ITEMDESC\"> Description <p>" > dest;

    while ((getline > 0) && !(($0 ~ /\[/) && ($0 ~ /\]/))) {
        print $0 > dest;
    }
}    

# Copies misc
/\[See also\]/ {
    print "<p class=\"ITEMDESC\"> See also <p>" > dest;

    while ((getline > 0) && !(($0 ~ /\[/) && ($0 ~ /\]/))) {
        print $0 > dest;
    }
}

# Write end of file
END { 
    print "<hr>" > dest;
    print "<h3> <a href=\"apiindex.html\"> Return to the API index </a></h3>" > dest;
    print "</body>" > dest;
    print "</html>" > dest;
}
