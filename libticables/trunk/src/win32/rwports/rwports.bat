echo "Building driver..."
nmake /F NMAKEFILE
copy ./i386/rwports.sys .
echo "Build done!"