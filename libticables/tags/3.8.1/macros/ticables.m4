# Configure paths for TICABLES
# Owen Taylor     97-11-3

dnl AM_PATH_TICABLES([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for TICABLES, and define TICABLES_CFLAGS and TICABLES_LIBS, if "gmodule" or 
dnl gthread is specified in MODULES, pass to ticables-config
dnl
AC_DEFUN(AM_PATH_TICABLES,
[dnl 
dnl Get the cflags and libraries from the ticables-config script
dnl
AC_ARG_WITH(ticables-prefix,[  --with-ticables-prefix=PFX   Prefix where TICABLES is installed (optional)],
            ticables_config_prefix="$withval", ticables_config_prefix="")
AC_ARG_WITH(ticables-exec-prefix,[  --with-ticables-exec-prefix=PFX Exec prefix where TICABLES is installed (optional)],
            ticables_config_exec_prefix="$withval", ticables_config_exec_prefix="")
AC_ARG_ENABLE(ticablestest, [  --disable-ticablestest       Do not try to compile and run a test TICABLES program],
		    , enable_ticablestest=yes)

  if test x$ticables_config_exec_prefix != x ; then
     ticables_config_args="$ticables_config_args --exec-prefix=$ticables_config_exec_prefix"
     if test x${TICABLES_CONFIG+set} != xset ; then
        TICABLES_CONFIG=$ticables_config_exec_prefix/bin/ticables-config
     fi
  fi
  if test x$ticables_config_prefix != x ; then
     ticables_config_args="$ticables_config_args --prefix=$ticables_config_prefix"
     if test x${TICABLES_CONFIG+set} != xset ; then
        TICABLES_CONFIG=$ticables_config_prefix/bin/ticables-config
     fi
  fi

  for module in . $4
  do
      case "$module" in
         gmodule) 
             ticables_config_args="$ticables_config_args gmodule"
         ;;
         gthread) 
             ticables_config_args="$ticables_config_args gthread"
         ;;
      esac
  done

  AC_PATH_PROG(TICABLES_CONFIG, ticables-config, no)
  min_ticables_version=ifelse([$1], ,0.99.7,$1)
  AC_MSG_CHECKING(for TICABLES - version >= $min_ticables_version)
  no_ticables=""
  if test "$TICABLES_CONFIG" = "no" ; then
    no_ticables=yes
  else
    TICABLES_CFLAGS=`$TICABLES_CONFIG $ticables_config_args --cflags`
    TICABLES_LIBS=`$TICABLES_CONFIG $ticables_config_args --libs`
    ticables_config_major_version=`$TICABLES_CONFIG $ticables_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    ticables_config_minor_version=`$TICABLES_CONFIG $ticables_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    ticables_config_micro_version=`$TICABLES_CONFIG $ticables_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_ticablestest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $TICABLES_CFLAGS"
      LIBS="$TICABLES_LIBS $LIBS"
dnl
dnl Now check if the installed TICABLES is sufficiently new. (Also sanity
dnl checks the results of ticables-config to some extent
dnl
      rm -f conf.ticablestest
      AC_TRY_RUN([
#include <ticables.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.ticablestest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_ticables_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_ticables_version");
     exit(1);
   }

  if ((ticables_major_version != $ticables_config_major_version) ||
      (ticables_minor_version != $ticables_config_minor_version) ||
      (ticables_micro_version != $ticables_config_micro_version))
    {
      printf("\n*** 'ticables-config --version' returned %d.%d.%d, but TICABLES (%d.%d.%d)\n", 
             $ticables_config_major_version, $ticables_config_minor_version, $ticables_config_micro_version,
             ticables_major_version, ticables_minor_version, ticables_micro_version);
      printf ("*** was found! If ticables-config was correct, then it is best\n");
      printf ("*** to remove the old version of TICABLES. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If ticables-config was wrong, set the environment variable TICABLES_CONFIG\n");
      printf("*** to point to the correct copy of ticables-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    } 
  else if ((ticables_major_version != TICABLES_MAJOR_VERSION) ||
	   (ticables_minor_version != TICABLES_MINOR_VERSION) ||
           (ticables_micro_version != TICABLES_MICRO_VERSION))
    {
      printf("*** TICABLES header files (version %d.%d.%d) do not match\n",
	     TICABLES_MAJOR_VERSION, TICABLES_MINOR_VERSION, TICABLES_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     ticables_major_version, ticables_minor_version, ticables_micro_version);
    }
  else
    {
      if ((ticables_major_version > major) ||
        ((ticables_major_version == major) && (ticables_minor_version > minor)) ||
        ((ticables_major_version == major) && (ticables_minor_version == minor) && (ticables_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of TICABLES (%d.%d.%d) was found.\n",
               ticables_major_version, ticables_minor_version, ticables_micro_version);
        printf("*** You need a version of TICABLES newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** TICABLES is always available from ftp://ftp.gtk.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the ticables-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of TICABLES, but you can also set the TICABLES_CONFIG environment to point to the\n");
        printf("*** correct copy of ticables-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_ticables=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_ticables" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$TICABLES_CONFIG" = "no" ; then
       echo "*** The ticables-config script installed by TICABLES could not be found"
       echo "*** If TICABLES was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the TICABLES_CONFIG environment variable to the"
       echo "*** full path to ticables-config."
     else
       if test -f conf.ticablestest ; then
        :
       else
          echo "*** Could not run TICABLES test program, checking why..."
          CFLAGS="$CFLAGS $TICABLES_CFLAGS"
          LIBS="$LIBS $TICABLES_LIBS"
          AC_TRY_LINK([
#include <ticables.h>
#include <stdio.h>
],      [ return ((ticables_major_version) || (ticables_minor_version) || (ticables_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding TICABLES or finding the wrong"
          echo "*** version of TICABLES. If it is not finding TICABLES, you'll need to set your"
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
          echo "*** exact error that occured. This usually means TICABLES was incorrectly installed"
          echo "*** or that you have moved TICABLES since it was installed. In the latter case, you"
          echo "*** may want to edit the ticables-config script: $TICABLES_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     TICABLES_CFLAGS=""
     TICABLES_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(TICABLES_CFLAGS)
  AC_SUBST(TICABLES_LIBS)
  rm -f conf.ticablestest
])
