/*
  This is a test file for probe.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "str.h"

#define MAXCHARS 257

#define PAR_MAX 3
#define SER_MAX 4
struct port_info_
{
  char parallel_port_name[PAR_MAX][17];
  int parallel_port_addr[PAR_MAX];
  int parallel_port_mode[PAR_MAX];

  char serial_port_name[SER_MAX][17];
  int serial_port_addr[SER_MAX];
  int serial_port_mode[SER_MAX];
};
typedef struct port_info_ PortInfo;

int main(void)
{
  int fd;
  FILE *f;
  char buffer[MAXCHARS];
  char info[MAXCHARS];
  int i, j;
  DIR *dir;
  struct dirent *file;
  int res;
  char path[MAXCHARS] = "/proc/sys/dev/parport/";
  PortInfo *pi;

  /* Clear structure */
  pi = (PortInfo *)calloc(1, sizeof(PortInfo));
  if(pi == NULL)
    {
      fprintf(stderr, "Unable to allocate memory for PortInfo structure.\n");
      return -1;
    }

  /* Use /proc/sys/dev/parport/parportX/base-addr where X=0, 1, ... */  
  fprintf(stdout, "Probing parallel ports...\n");
  if( (dir=opendir("/proc/sys/dev/parport/")) == NULL) 
    {
      perror("Unable to do a dirlist in '/proc/sys/dev/parport/'.\n");
      return -1;
    }

  while( (file=readdir(dir)) != NULL) 
    {
      if(!strcmp(file->d_name, ".")) continue;
      if(!strcmp(file->d_name, "..")) continue;

      if(strstr(file->d_name, "parport"))
	{
	  res = sscanf(file->d_name, "parport%i", &i);
	  if(res == 1)
	    {
	      if(i >= PAR_MAX-1)
		break;
	      strcpy(path, "/proc/sys/dev/parport/");
	      strcat(path, file->d_name);
	      strcat(path, "/");
	      strcat(path, "base-addr");
	      sprintf(pi->parallel_port_name[i], "/dev/par%i", i);
	      f = fopen(path, "rt");
	      if(f == NULL)
		{
		  fprintf(stderr, "Unable to open this entry: <%s>\n",
			  path);
		}
	      else
		{
		  fscanf(f, "%i", &(pi->parallel_port_addr[i]));
		  fprintf(stdout, "-> %s at address 0x%03x\n",
			  pi->parallel_port_name[i],
			  pi->parallel_port_addr[i]);
		  fclose(f);
		}
	    }
	  else 
	    {
	      fprintf(stderr, "Invalid parport entry: <%s>.\n", file->d_name);
	    }
	}
    }
  
  if(closedir(dir)==-1)
    {
      perror("Closedir error\n");
    }

  /* Use /proc/tty/driver/serial */
  fprintf(stdout, "Probing serial ports...\n");
  fd=access("/proc/tty/driver/serial", F_OK);
  if(fd<0)
    {
      fprintf(stderr, "The file '/proc/tty/driver/serial' does not exist. Unable to probe serial port.\n");
      fprintf(stdout, "Done.\n");
      return -1;
    }

  f=fopen("/proc/tty/driver/serial", "rt");
  if(f == NULL)
    {
      fprintf(stderr, "Unable to open this entry: <%s>\n",
	      "/proc/tty/driver/serial");
      return -1;
    }

  fgets(buffer, 256, f);
  for(i=0; i<SER_MAX; i++)
    {
      fgets(buffer, 256, f);
      sscanf(buffer, "%i: uart:%s port:%03X ", &j, info, 
	     &((pi->serial_port_addr)[i]));
      fprintf(stdout, "-> /dev/ttyS%i: %8s adr:%03X\n", j, info, 
	      pi->serial_port_addr[i]);
    }
  fprintf(stdout, "Done.\n");

  return 0;
}



