# Configure paths for TICALCS
# Owen Taylor     97-11-3

dnl AM_PATH_TICALCS([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for TICALCS, and define TICALCS_CFLAGS and TICALCS_LIBS, if "gmodule" or 
dnl gthread is specified in MODULES, pass to ticalcs-config
dnl
AC_DEFUN(AM_PATH_TICALCS,
[dnl 
dnl Get the cflags and libraries from the ticalcs-config script
dnl
AC_ARG_WITH(ticalcs-prefix,[  --with-ticalcs-prefix=PFX   Prefix where TICALCS is installed (optional)],
            ticalcs_config_prefix="$withval", ticalcs_config_prefix="")
AC_ARG_WITH(ticalcs-exec-prefix,[  --with-ticalcs-exec-prefix=PFX Exec prefix where TICALCS is installed (optional)],
            ticalcs_config_exec_prefix="$withval", ticalcs_config_exec_prefix="")
AC_ARG_ENABLE(ticalcstest, [  --disable-ticalcstest       Do not try to compile and run a test TICALCS program],
		    , enable_ticalcstest=yes)

  if test x$ticalcs_config_exec_prefix != x ; then
     ticalcs_config_args="$ticalcs_config_args --exec-prefix=$ticalcs_config_exec_prefix"
     if test x${TICALCS_CONFIG+set} != xset ; then
        TICALCS_CONFIG=$ticalcs_config_exec_prefix/bin/ticalcs-config
     fi
  fi
  if test x$ticalcs_config_prefix != x ; then
     ticalcs_config_args="$ticalcs_config_args --prefix=$ticalcs_config_prefix"
     if test x${TICALCS_CONFIG+set} != xset ; then
        TICALCS_CONFIG=$ticalcs_config_prefix/bin/ticalcs-config
     fi
  fi

  for module in . $4
  do
      case "$module" in
         gmodule) 
             ticalcs_config_args="$ticalcs_config_args gmodule"
         ;;
         gthread) 
             ticalcs_config_args="$ticalcs_config_args gthread"
         ;;
      esac
  done

  AC_PATH_PROG(TICALCS_CONFIG, ticalcs-config, no)
  min_ticalcs_version=ifelse([$1], ,0.99.7,$1)
  AC_MSG_CHECKING(for TICALCS - version >= $min_ticalcs_version)
  no_ticalcs=""
  if test "$TICALCS_CONFIG" = "no" ; then
    no_ticalcs=yes
  else
    TICALCS_CFLAGS=`$TICALCS_CONFIG $ticalcs_config_args --cflags`
    TICALCS_LIBS=`$TICALCS_CONFIG $ticalcs_config_args --libs`
    ticalcs_config_major_version=`$TICALCS_CONFIG $ticalcs_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    ticalcs_config_minor_version=`$TICALCS_CONFIG $ticalcs_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    ticalcs_config_micro_version=`$TICALCS_CONFIG $ticalcs_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_ticalcstest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $TICALCS_CFLAGS"
      LIBS="$TICALCS_LIBS $LIBS"
dnl
dnl Now check if the installed TICALCS is sufficiently new. (Also sanity
dnl checks the results of ticalcs-config to some extent
dnl
      rm -f conf.ticalcstest
      AC_TRY_RUN([
#include <ticalcs.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.ticalcstest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_ticalcs_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_ticalcs_version");
     exit(1);
   }

  if ((ticalcs_major_version != $ticalcs_config_major_version) ||
      (ticalcs_minor_version != $ticalcs_config_minor_version) ||
      (ticalcs_micro_version != $ticalcs_config_micro_version))
    {
      printf("\n*** 'ticalcs-config --version' returned %d.%d.%d, but TICALCS (%d.%d.%d)\n", 
             $ticalcs_config_major_version, $ticalcs_config_minor_version, $ticalcs_config_micro_version,
             ticalcs_major_version, ticalcs_minor_version, ticalcs_micro_version);
      printf ("*** was found! If ticalcs-config was correct, then it is best\n");
      printf ("*** to remove the old version of TICALCS. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If ticalcs-config was wrong, set the environment variable TICALCS_CONFIG\n");
      printf("*** to point to the correct copy of ticalcs-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    } 
  else if ((ticalcs_major_version != TICALCS_MAJOR_VERSION) ||
	   (ticalcs_minor_version != TICALCS_MINOR_VERSION) ||
           (ticalcs_micro_version != TICALCS_MICRO_VERSION))
    {
      printf("*** TICALCS header files (version %d.%d.%d) do not match\n",
	     TICALCS_MAJOR_VERSION, TICALCS_MINOR_VERSION, TICALCS_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     ticalcs_major_version, ticalcs_minor_version, ticalcs_micro_version);
    }
  else
    {
      if ((ticalcs_major_version > major) ||
        ((ticalcs_major_version == major) && (ticalcs_minor_version > minor)) ||
        ((ticalcs_major_version == major) && (ticalcs_minor_version == minor) && (ticalcs_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of TICALCS (%d.%d.%d) was found.\n",
               ticalcs_major_version, ticalcs_minor_version, ticalcs_micro_version);
        printf("*** You need a version of TICALCS newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** TICALCS is always available from ftp://ftp.gtk.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the ticalcs-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of TICALCS, but you can also set the TICALCS_CONFIG environment to point to the\n");
        printf("*** correct copy of ticalcs-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_ticalcs=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_ticalcs" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$TICALCS_CONFIG" = "no" ; then
       echo "*** The ticalcs-config script installed by TICALCS could not be found"
       echo "*** If TICALCS was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the TICALCS_CONFIG environment variable to the"
       echo "*** full path to ticalcs-config."
     else
       if test -f conf.ticalcstest ; then
        :
       else
          echo "*** Could not run TICALCS test program, checking why..."
          CFLAGS="$CFLAGS $TICALCS_CFLAGS"
          LIBS="$LIBS $TICALCS_LIBS"
          AC_TRY_LINK([
#include <ticalcs.h>
#include <stdio.h>
],      [ return ((ticalcs_major_version) || (ticalcs_minor_version) || (ticalcs_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding TICALCS or finding the wrong"
          echo "*** version of TICALCS. If it is not finding TICALCS, you'll need to set your"
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
          echo "*** exact error that occured. This usually means TICALCS was incorrectly installed"
          echo "*** or that you have moved TICALCS since it was installed. In the latter case, you"
          echo "*** may want to edit the ticalcs-config script: $TICALCS_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     TICALCS_CFLAGS=""
     TICALCS_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(TICALCS_CFLAGS)
  AC_SUBST(TICALCS_LIBS)
  rm -f conf.ticalcstest
])
