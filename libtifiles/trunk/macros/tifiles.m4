# Configure paths for TIFILES
# Owen Taylor     97-11-3

dnl AM_PATH_TIFILES([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for TIFILES, and define TIFILES_CFLAGS and TIFILES_LIBS, if "gmodule" or 
dnl gthread is specified in MODULES, pass to tifiles-config
dnl
AC_DEFUN(AM_PATH_TIFILES,
[dnl 
dnl Get the cflags and libraries from the tifiles-config script
dnl
AC_ARG_WITH(tifiles-prefix,[  --with-tifiles-prefix=PFX   Prefix where TIFILES is installed (optional)],
            tifiles_config_prefix="$withval", tifiles_config_prefix="")
AC_ARG_WITH(tifiles-exec-prefix,[  --with-tifiles-exec-prefix=PFX Exec prefix where TIFILES is installed (optional)],
            tifiles_config_exec_prefix="$withval", tifiles_config_exec_prefix="")
AC_ARG_ENABLE(tifilestest, [  --disable-tifilestest       Do not try to compile and run a test TIFILES program],
		    , enable_tifilestest=yes)

  if test x$tifiles_config_exec_prefix != x ; then
     tifiles_config_args="$tifiles_config_args --exec-prefix=$tifiles_config_exec_prefix"
     if test x${TIFILES_CONFIG+set} != xset ; then
        TIFILES_CONFIG=$tifiles_config_exec_prefix/bin/tifiles-config
     fi
  fi
  if test x$tifiles_config_prefix != x ; then
     tifiles_config_args="$tifiles_config_args --prefix=$tifiles_config_prefix"
     if test x${TIFILES_CONFIG+set} != xset ; then
        TIFILES_CONFIG=$tifiles_config_prefix/bin/tifiles-config
     fi
  fi

  for module in . $4
  do
      case "$module" in
         gmodule) 
             tifiles_config_args="$tifiles_config_args gmodule"
         ;;
         gthread) 
             tifiles_config_args="$tifiles_config_args gthread"
         ;;
      esac
  done

  AC_PATH_PROG(TIFILES_CONFIG, tifiles-config, no)
  min_tifiles_version=ifelse([$1], ,0.99.7,$1)
  AC_MSG_CHECKING(for TIFILES - version >= $min_tifiles_version)
  no_tifiles=""
  if test "$TIFILES_CONFIG" = "no" ; then
    no_tifiles=yes
  else
    TIFILES_CFLAGS=`$TIFILES_CONFIG $tifiles_config_args --cflags`
    TIFILES_LIBS=`$TIFILES_CONFIG $tifiles_config_args --libs`
    tifiles_config_major_version=`$TIFILES_CONFIG $tifiles_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    tifiles_config_minor_version=`$TIFILES_CONFIG $tifiles_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    tifiles_config_micro_version=`$TIFILES_CONFIG $tifiles_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_tifilestest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $TIFILES_CFLAGS"
      LIBS="$TIFILES_LIBS $LIBS"
dnl
dnl Now check if the installed TIFILES is sufficiently new. (Also sanity
dnl checks the results of tifiles-config to some extent
dnl
      rm -f conf.tifilestest
      AC_TRY_RUN([
#include <tifiles.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.tifilestest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_tifiles_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_tifiles_version");
     exit(1);
   }

  if ((tifiles_major_version != $tifiles_config_major_version) ||
      (tifiles_minor_version != $tifiles_config_minor_version) ||
      (tifiles_micro_version != $tifiles_config_micro_version))
    {
      printf("\n*** 'tifiles-config --version' returned %d.%d.%d, but TIFILES (%d.%d.%d)\n", 
             $tifiles_config_major_version, $tifiles_config_minor_version, $tifiles_config_micro_version,
             tifiles_major_version, tifiles_minor_version, tifiles_micro_version);
      printf ("*** was found! If tifiles-config was correct, then it is best\n");
      printf ("*** to remove the old version of TIFILES. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If tifiles-config was wrong, set the environment variable TIFILES_CONFIG\n");
      printf("*** to point to the correct copy of tifiles-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    } 
  else if ((tifiles_major_version != TIFILES_MAJOR_VERSION) ||
	   (tifiles_minor_version != TIFILES_MINOR_VERSION) ||
           (tifiles_micro_version != TIFILES_MICRO_VERSION))
    {
      printf("*** TIFILES header files (version %d.%d.%d) do not match\n",
	     TIFILES_MAJOR_VERSION, TIFILES_MINOR_VERSION, TIFILES_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     tifiles_major_version, tifiles_minor_version, tifiles_micro_version);
    }
  else
    {
      if ((tifiles_major_version > major) ||
        ((tifiles_major_version == major) && (tifiles_minor_version > minor)) ||
        ((tifiles_major_version == major) && (tifiles_minor_version == minor) && (tifiles_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of TIFILES (%d.%d.%d) was found.\n",
               tifiles_major_version, tifiles_minor_version, tifiles_micro_version);
        printf("*** You need a version of TIFILES newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** TIFILES is always available from ftp://ftp.gtk.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the tifiles-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of TIFILES, but you can also set the TIFILES_CONFIG environment to point to the\n");
        printf("*** correct copy of tifiles-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_tifiles=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_tifiles" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$TIFILES_CONFIG" = "no" ; then
       echo "*** The tifiles-config script installed by TIFILES could not be found"
       echo "*** If TIFILES was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the TIFILES_CONFIG environment variable to the"
       echo "*** full path to tifiles-config."
     else
       if test -f conf.tifilestest ; then
        :
       else
          echo "*** Could not run TIFILES test program, checking why..."
          CFLAGS="$CFLAGS $TIFILES_CFLAGS"
          LIBS="$LIBS $TIFILES_LIBS"
          AC_TRY_LINK([
#include <tifiles.h>
#include <stdio.h>
],      [ return ((tifiles_major_version) || (tifiles_minor_version) || (tifiles_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding TIFILES or finding the wrong"
          echo "*** version of TIFILES. If it is not finding TIFILES, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***"
          echo "*** If you have a RedHat 5.0 system, you should remove the GTK package that"
          echo "*** came with the system with the command"
          echo "***"
          echo "***    rpm --erase --nodeps gtk gtk-devel" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means TIFILES was incorrectly installed"
          echo "*** or that you have moved TIFILES since it was installed. In the latter case, you"
          echo "*** may want to edit the tifiles-config script: $TIFILES_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     TIFILES_CFLAGS=""
     TIFILES_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(TIFILES_CFLAGS)
  AC_SUBST(TIFILES_LIBS)
  rm -f conf.tifilestest
])
