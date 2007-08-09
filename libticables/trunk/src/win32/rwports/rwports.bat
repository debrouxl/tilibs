echo "Building driver..."
nmake /F NMAKEFILE
copy ./i386/dhahelper.sys .
echo "Build done!"