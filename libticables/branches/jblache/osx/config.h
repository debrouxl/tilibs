#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef __MACOSX__ /* maybe we should use __OSX__ here, as darwin also exists on i386 */

#define VERSION "2.0.6 OS X 0.0.1"
#define HAVE_SYS_PERM_H
#define HAVE_SYS_IPC_H  /* needed for virtual linking */
#define HAVE_SYS_SHM_H

#endif __MACOSX__

#endif /* !__CONFIG_H__ */