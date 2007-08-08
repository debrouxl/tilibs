You need a Windows DDK or MinGW to build this driver. (With MinGW, you'll not get SEH support, so exceptions during memory mapping/unmapping will not be caught.) A prebuilt binary can be found in the build/msvc directory.

You need MinGW to build the executable.

Those files come from the MPlayer project (http://svn.mplayerhq.hu/mplayer/trunk/vidix/dhahelperwin/) and have been modified by the TiEmu team. (They should be compatible with the original.)
