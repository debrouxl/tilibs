#! /bin/sh

SRC=./text
TMP=/tmp/doc
DST=./html
TPL=./tmpl
TBL=./index

# Scan the 'text' folder and list keywords

echo "Create list of keywords..."
ls -l --color=never ./text | awk -f index.awk | sort > $TBL
rm -f $TMP/*.*
rm -f $DST/*.*

# Copy template files and hand-written files

cp $TPL/*.* $DST

# Generate an html file starting at a text file

echo "Pass #1: text -> html conversion..."
mkdir $TMP >/dev/null 2>&1
for I in $SRC/*.txt ; do \
#    echo $I
    cat $I | awk -f pass1.awk src=$I dst=$TMP
done

# Re-parse html file for cross-references
# to do with an awk script again

echo "Pass #2: html -> html cross-referencing..."
for I in $TMP/*.html ; do \
#    echo $I
    cat $I | awk -f pass2.awk src=$I tbl=$TBL dst=$DST
done

echo "Done !"