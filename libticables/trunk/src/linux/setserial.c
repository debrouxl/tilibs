/* setserial.c - get/set Linux serial port info - rick sladkey */
/* modified to do work again and added setting fast serial speeds,
   Michael K. Johnson, johnsonm@stolaf.edu */
/*
 * Very heavily modified --- almost rewritten from scratch --- to have
 * a more flexible command structure.  Now able to set any of the
 * serial-specific options using the TIOCSSERIAL ioctl().
 * 			Theodore Ts'o, tytso@mit.edu, 1/1/93
 *
 * Last modified: [tytso:19940520.0036EDT]
 */

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_ASM_IOCTLS_H
#include <asm/ioctls.h>
#endif
#ifdef HAVE_LINUX_HAYESESP_H
#include <linux/hayesesp.h>
#endif
#include <linux/serial.h>

#include "version.h"

static char version_str[] = "setserial version " SETSERIAL_VERSION ", "
	SETSERIAL_DATE;

char *progname;

int	verbosity = 1;		/* 1 = normal, 0=boot-time, 2=everything */
				/* -1 == arguments to setserial */
int	verbose_flag = 0;	/* print results after setting a port */
int	quiet_flag = 0;
int	zero_flag = 0;

struct serial_type_struct {
	int id;
	char *name;
} serial_type_tbl[] = {
	PORT_UNKNOWN,	"unknown",
	PORT_8250,	"8250",
	PORT_16450,	"16450",
	PORT_16550,	"16550",
	PORT_16550A,	"16550A",
	PORT_CIRRUS,	"Cirrus",
	PORT_16650,	"16650",
	PORT_16650V2, 	"16650V2",
	PORT_16750,	"16750",
	PORT_16C950,	"16950/954",
	PORT_16C950,	"16950",
	PORT_16C950,	"16954",
	PORT_16654,	"16654",
	PORT_16850,	"16850",
	PORT_UNKNOWN,	"none",
	-1,		NULL
};

#define CMD_FLAG	1
#define CMD_PORT	2
#define CMD_IRQ		3
#define CMD_DIVISOR	4
#define CMD_TYPE	5
#define CMD_BASE	6
#define CMD_DELAY	7
#define CMD_WAIT	8
#define CMD_WAIT2	9
#define CMD_CONFIG	10
#define CMD_GETMULTI	11
#define CMD_SETMULTI	12
#define CMD_RX_TRIG     13
#define CMD_TX_TRIG     14
#define CMD_FLOW_OFF    15
#define CMD_FLOW_ON     16
#define CMD_RX_TMOUT    17
#define CMD_DMA_CHAN    18

#define FLAG_CAN_INVERT	0x0001
#define FLAG_NEED_ARG	0x0002

struct flag_type_table {
	int	cmd;
	char	*name;
	int	bits;
	int	mask;
	int	level;
	int	flags;
} flag_type_tbl[] = {
	CMD_FLAG,	"spd_normal",	0,		ASYNC_SPD_MASK,	2, 0,
	CMD_FLAG,	"spd_hi",	ASYNC_SPD_HI, 	ASYNC_SPD_MASK, 0, 0,
	CMD_FLAG,	"spd_vhi",	ASYNC_SPD_VHI,	ASYNC_SPD_MASK,	0, 0,
	CMD_FLAG,	"spd_shi",	ASYNC_SPD_SHI,	ASYNC_SPD_MASK,	0, 0,
	CMD_FLAG,	"spd_warp",	ASYNC_SPD_WARP,	ASYNC_SPD_MASK,	0, 0,
	CMD_FLAG,	"spd_cust",	ASYNC_SPD_CUST,	ASYNC_SPD_MASK,	0, 0,
	
	CMD_FLAG, 	"SAK", 		ASYNC_SAK, 	ASYNC_SAK, 	0, FLAG_CAN_INVERT,
	CMD_FLAG,	"Fourport",	ASYNC_FOURPORT, ASYNC_FOURPORT,	0, FLAG_CAN_INVERT,
	CMD_FLAG,	"hup_notify",	ASYNC_HUP_NOTIFY, ASYNC_HUP_NOTIFY, 0, FLAG_CAN_INVERT,
	CMD_FLAG,	"skip_test",	ASYNC_SKIP_TEST,ASYNC_SKIP_TEST,2, FLAG_CAN_INVERT,
	CMD_FLAG,	"auto_irq",	ASYNC_AUTO_IRQ,	ASYNC_AUTO_IRQ,	2, FLAG_CAN_INVERT,
	CMD_FLAG,	"split_termios", ASYNC_SPLIT_TERMIOS, ASYNC_SPLIT_TERMIOS, 2, FLAG_CAN_INVERT,
	CMD_FLAG,	"session_lockout", ASYNC_SESSION_LOCKOUT, ASYNC_SESSION_LOCKOUT, 2, FLAG_CAN_INVERT,
	CMD_FLAG,	"pgrp_lockout", ASYNC_PGRP_LOCKOUT, ASYNC_PGRP_LOCKOUT, 2, FLAG_CAN_INVERT,
	CMD_FLAG,	"callout_nohup", ASYNC_CALLOUT_NOHUP, ASYNC_CALLOUT_NOHUP, 2, FLAG_CAN_INVERT,
	CMD_FLAG,	"low_latency", ASYNC_LOW_LATENCY, ASYNC_LOW_LATENCY, 0, FLAG_CAN_INVERT,
	CMD_PORT,	"port",		0,		0,		0, FLAG_NEED_ARG,
	CMD_IRQ,	"irq",		0,		0,		0, FLAG_NEED_ARG,
	CMD_DIVISOR,	"divisor",	0,		0,		0, FLAG_NEED_ARG,
	CMD_TYPE,	"uart",		0,		0,		0, FLAG_NEED_ARG,
	CMD_BASE,	"base",		0,		0,		0, FLAG_NEED_ARG,
	CMD_BASE,	"baud_base",	0,		0,		0, FLAG_NEED_ARG,
	CMD_DELAY,	"close_delay",	0,		0,		0, FLAG_NEED_ARG,
	CMD_WAIT,	"closing_wait",	0,		0,		0, FLAG_NEED_ARG,
	CMD_CONFIG,	"autoconfig",	0,		0,		0, 0,
	CMD_CONFIG,	"autoconfigure",0,		0,		0, 0,
	CMD_GETMULTI,	"get_multiport",0,		0,		0, 0,
	CMD_SETMULTI,	"set_multiport",0,		0,		0, 0,
#ifdef TIOCGHAYESESP
	CMD_RX_TRIG,    "rx_trigger",   0,              0,              0, FLAG_NEED_ARG,
	CMD_TX_TRIG,    "tx_trigger",   0,              0,              0, FLAG_NEED_ARG,
	CMD_FLOW_OFF,   "flow_off",     0,              0,              0, FLAG_NEED_ARG,
	CMD_FLOW_ON,    "flow_on",      0,              0,              0, FLAG_NEED_ARG,
	CMD_RX_TMOUT,   "rx_timeout",   0,              0,              0, FLAG_NEED_ARG,
	CMD_DMA_CHAN,   "dma_channel",  0,              0,              0, FLAG_NEED_ARG,
#endif
	0,		0,		0,		0,		0, 0,
};
	
char *serial_type(int id)
{
	int i;

	for (i = 0; serial_type_tbl[i].id != -1; i++)
		if (id == serial_type_tbl[i].id)
			return serial_type_tbl[i].name;
	return "undefined";
}

int uart_type(char *name)
{
	int i;

	for (i = 0; serial_type_tbl[i].id != -1; i++)
		if (!strcasecmp(name, serial_type_tbl[i].name))
			return serial_type_tbl[i].id;
	return -1;
}


int atonum(char *s)
{
	int n;

	while (*s == ' ')
		s++;
	if (strncmp(s, "0x", 2) == 0 || strncmp(s, "0X", 2) == 0)
		sscanf(s + 2, "%x", &n);
	else if (s[0] == '0' && s[1])
		sscanf(s + 1, "%o", &n);
	else
		sscanf(s, "%d", &n);
	return n;
}

void print_flags(struct serial_struct *serinfo,
		 char *prefix, char *postfix)
{
	struct	flag_type_table	*p;
	int	flags;
	int	first = 1;

	flags = serinfo->flags;
	
	for (p = flag_type_tbl; p->name; p++) {
		if (p->cmd != CMD_FLAG)
			continue;
		if (verbosity == -1) {
			if ((flags & p->mask) == p->bits)
				printf(" %s", p->name);
			continue;
		}
		if (verbosity < p->level)
			continue;
		if ((flags & p->mask) == p->bits) {
			if (first) {
				printf("%s", prefix);
				first = 0;
			} else
				printf(" ");
			printf("%s", p->name);
		}
	}
	
	if (!first)
		printf("%s", postfix);
}

#ifdef TIOCSERGETMULTI
void print_multiport(char *device, int fd)
{
	struct serial_multiport_struct multi;

	if (ioctl(fd, TIOCSERGETMULTI, &multi) < 0)
		return;

	if (!multi.port1 && !multi.port2 &&
	    !multi.port3 && !multi.port4 && !multi.port_monitor)
		return;
	
	printf("%s", device);
	if (multi.port_monitor)
		printf(" port_monitor 0x%x", multi.port_monitor);
	if (multi.port1)
		printf(" port1 0x%x mask1 0x%x match1 0x%x", multi.port1,
		       multi.mask1, multi.match1);
	if (multi.port2)
		printf(" port2 0x%x mask2 0x%x match2 0x%x", multi.port2,
		       multi.mask2, multi.match2);
	if (multi.port3)
		printf(" port3 0x%x mask3 0x%x match3 0x%x", multi.port3,
		       multi.mask3, multi.match3);
	if (multi.port4)
		printf(" port4 0x%x mask4 0x%x match4 0x%x", multi.port4,
		       multi.mask4, multi.match4);
	printf("\n");
}


void multiport_usage()
{

	fprintf(stderr, "\nValid keywords after set_multiport are:\n");
	fprintf(stderr, "\tport_monitor, port[1-4], mask[1-4], "
		"match[1-4]\n\n");
	fprintf(stderr, "All arguments take an numeric argument following "
		"the keyword.\n");
	fprintf(stderr, "Use a leading '0x' for hex numbers.\n\n");
}

void get_multiport(char *device, int fd)
{
	struct serial_multiport_struct multi;

	if (ioctl(fd, TIOCSERGETMULTI, &multi) < 0) {
		perror("Cannot get multiport config");
		exit(1);
	}
	printf("Multiport config for irq %d:\n", multi.irq);
	printf("\tPort monitor = 0x%x\n", multi.port_monitor);
	printf("\tPort1 = 0x%x, mask=0x%x, match=0x%x\n", multi.port1,
	       multi.mask1, multi.match1);
	printf("\tPort2 = 0x%x, mask=0x%x, match=0x%x\n", multi.port2,
	       multi.mask2, multi.match2);
	printf("\tPort3 = 0x%x, mask=0x%x, match=0x%x\n", multi.port3,
	       multi.mask3, multi.match3);
	printf("\tPort4 = 0x%x, mask=0x%x, match=0x%x\n", multi.port4,
	       multi.mask4, multi.match4);
}

void set_multiport(char *device, int fd, char ***in_arg)
{
	char **arg = *in_arg;
	char *word, *argument;
	struct serial_multiport_struct multi;

	if (ioctl(fd, TIOCSERGETMULTI, &multi) < 0) {
		perror("Cannot get multiport config");
		exit(1);
	}
	if (*arg == 0) {
		multiport_usage();
		return;
	}
	while (*arg) {
		word = *arg++;
		if (*arg == 0) {
			multiport_usage();
			exit(1);
		}
		argument = *arg++;
		if (strcasecmp(word, "port_monitor") == 0) {
			multi.port_monitor = atonum(argument);
			continue;
		}
		if (strcasecmp(word, "port1") == 0) {
			multi.port1 = atonum(argument);
			continue;
		}
		if (strcasecmp(word, "mask1") == 0) {
			multi.mask1 = atonum(argument);
			continue;
		}
		if (strcasecmp(word, "match1") == 0) {
			multi.match1 = atonum(argument);
			continue;
		}
		if (strcasecmp(word, "port2") == 0) {
			multi.port2 = atonum(argument);
			continue;
		}
		if (strcasecmp(word, "mask2") == 0) {
			multi.mask2 = atonum(argument);
			continue;
		}
		if (strcasecmp(word, "match2") == 0) {
			multi.match2 = atonum(argument);
			continue;
		}
		if (strcasecmp(word, "port3") == 0) {
			multi.port3 = atonum(argument);
			continue;
		}
		if (strcasecmp(word, "mask3") == 0) {
			multi.mask3 = atonum(argument);
			continue;
		}
		if (strcasecmp(word, "match3") == 0) {
			multi.match3 = atonum(argument);
			continue;
		}
		if (strcasecmp(word, "port4") == 0) {
			multi.port4 = atonum(argument);
			continue;
		}
		if (strcasecmp(word, "mask4") == 0) {
			multi.mask4 = atonum(argument);
			continue;
		}
		if (strcasecmp(word, "match4") == 0) {
			multi.match4 = atonum(argument);
			continue;
		}
		fprintf(stderr, "Unknown keyword %s.\n", word);
		multiport_usage();
		exit(1);
	}
	if (ioctl(fd, TIOCSERSETMULTI, &multi) < 0) {
		perror("Cannot set multiport config");
		exit(1);
	}
	*in_arg = arg;
}
#else
void get_multiport(char *device, int fd)
{
	printf("Setserial was compiled under a kernel which did not\n");
	printf("support the special serial multiport configs.\n");
}
void set_multiport(char *device, int fd, char ***in_arg)
{
	printf("Setserial was compiled under a kernel which did not\n");
	printf("support the special serial multiport configs.\n");
}
#endif

#ifdef TIOCGHAYESESP
void print_hayesesp(int fd)
{
	struct hayes_esp_config esp;

	if (ioctl(fd, TIOCGHAYESESP, &esp) < 0)
		return;

	printf("\tHayes ESP enhanced mode configuration:\n");
	printf("\t\tRX trigger level: %d, TX trigger level: %d\n",
	       (int)esp.rx_trigger, (int)esp.tx_trigger);
	printf("\t\tFlow off level: %d, Flow on level: %d\n",
	       (int)esp.flow_off, (int)esp.flow_on);
	printf("\t\tRX timeout: %u, DMA channel: %d\n\n",
	       (unsigned int)esp.rx_timeout, (int)esp.dma_channel);
}

void set_hayesesp(int fd, int cmd, int arg)
{
	struct hayes_esp_config esp;

	if (ioctl(fd, TIOCGHAYESESP, &esp) < 0) {
		printf("\nError: rx_trigger, tx_trigger, flow_off, "
		       "flow_on, rx_timeout, and dma_channel\n"
		       "are only valid for Hayes ESP ports.\n\n");
		exit(1);
	}

	switch (cmd) {
	case CMD_RX_TRIG:
		esp.rx_trigger = arg;
		break;
	case CMD_TX_TRIG:
		esp.tx_trigger = arg;
		break;
	case CMD_FLOW_OFF:
		esp.flow_off = arg;
		break;
	case CMD_FLOW_ON:
		esp.flow_on = arg;
		break;
	case CMD_RX_TMOUT:
		esp.rx_timeout = arg;
		break;
	case CMD_DMA_CHAN:
		esp.dma_channel = arg;
		break;
	}

	if (ioctl(fd, TIOCSHAYESESP, &esp) < 0) {
		printf("Cannot set Hayes ESP info\n");
		exit(1);
	}
}
#endif

void get_serial(char *device)
{
	struct serial_struct serinfo;
	int	fd;
	char	buf1[40];

	if ((fd = open(device, O_RDWR|O_NONBLOCK)) < 0) {
		perror(device);
		return;
	}
	serinfo.reserved_char[0] = 0;
	if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0) {
		perror("Cannot get serial info");
		close(fd);
		return;
	}
	if (serinfo.irq == 9)
		serinfo.irq = 2;	/* People understand 2 better than 9 */
	if (verbosity==-1) {
		printf("%s uart %s port 0x%.4x irq %d baud_base %d", device,
		       serial_type(serinfo.type), serinfo.port,
		       serinfo.irq, serinfo.baud_base);
		print_flags(&serinfo, ", Flags: ", "");
		printf("\n");
	} else if (verbosity==2) {
		printf("%s, Line %d, UART: %s, Port: 0x%.4x, IRQ: %d\n",
		       device, serinfo.line, serial_type(serinfo.type),
		       serinfo.port, serinfo.irq);
		printf("\tBaud_base: %d, close_delay: %d, divisor: %d\n",
		       serinfo.baud_base, serinfo.close_delay,
		       serinfo.custom_divisor);
		if (serinfo.closing_wait == ASYNC_CLOSING_WAIT_INF)
			strcpy(buf1, "infinte");
		else if (serinfo.closing_wait == ASYNC_CLOSING_WAIT_NONE)
			strcpy(buf1, "none");
		else
			sprintf(buf1, "%d", serinfo.closing_wait);
		printf("\tclosing_wait: %s\n", buf1);
		print_flags(&serinfo, "\tFlags: ", "");
		printf("\n\n");

#ifdef TIOCGHAYESESP
		print_hayesesp(fd);
#endif
	} else if (verbosity==0) {
		if (serinfo.type) {
			printf("%s at 0x%.4x (irq = %d) is a %s",
			       device, serinfo.port, serinfo.irq,
			       serial_type(serinfo.type));
			print_flags(&serinfo, " (", ")");
			printf("\n");
		}
	} else {
		printf("%s, UART: %s, Port: 0x%.4x, IRQ: %d",
		       device, serial_type(serinfo.type),
		       serinfo.port, serinfo.irq);
		print_flags(&serinfo, ", Flags: ", "");
		printf("\n");
	}
	close(fd);
}

void set_serial(char *device, char ** arg)
{
	struct serial_struct old_serinfo, new_serinfo;
	struct	flag_type_table	*p;
	int	fd;
	int	do_invert = 0;
	char	*word;
	

	if ((fd = open(device, O_RDWR|O_NONBLOCK)) < 0) {
		if (verbosity==0 && errno==ENOENT)
			exit(201);
		perror(device);
		exit(201);
	}
	if (ioctl(fd, TIOCGSERIAL, &old_serinfo) < 0) {
		perror("Cannot get serial info");
		exit(1);
	}
	new_serinfo = old_serinfo;
	if (zero_flag)
		new_serinfo.flags = 0;
	while (*arg) {
		do_invert = 0;
		word = *arg++;
		if (*word == '^') {
			do_invert++;
			word++;
		}
		for (p = flag_type_tbl; p->name; p++) {
			if (!strcasecmp(p->name, word))
				break;
		}
		if (!p->name) {
			fprintf(stderr, "Invalid flag: %s\n", word);
			exit(1);
		}
		if (do_invert && !(p->flags & FLAG_CAN_INVERT)) {
			fprintf(stderr, "This flag can not be inverted: %s\n", word);
			exit(1);
		}
		if ((p->flags & FLAG_NEED_ARG) && !*arg) {
			fprintf(stderr, "Missing argument for %s\n", word);
			exit(1);
		}
		switch (p->cmd) {
		case CMD_FLAG:
			new_serinfo.flags &= ~p->mask;
			if (!do_invert)
				new_serinfo.flags |= p->bits;
			break;
		case CMD_PORT:
			new_serinfo.port = atonum(*arg++);
			break;
		case CMD_IRQ:
			new_serinfo.irq = atonum(*arg++);
			break;
		case CMD_DIVISOR:
			new_serinfo.custom_divisor = atonum(*arg++);
			break;
		case CMD_TYPE:
			new_serinfo.type = uart_type(*arg++);
			if (new_serinfo.type < 0) {
				fprintf(stderr, "Illegal UART type: %s", *--arg);
				exit(1);
			}
			break;
		case CMD_BASE:
			new_serinfo.baud_base = atonum(*arg++);
			break;
		case CMD_DELAY:
			new_serinfo.close_delay = atonum(*arg++);
			break;
		case CMD_WAIT:
			if (!strcasecmp(*arg, "infinite"))
				new_serinfo.closing_wait = ASYNC_CLOSING_WAIT_INF;
			else if (!strcasecmp(*arg, "none"))
				new_serinfo.closing_wait = ASYNC_CLOSING_WAIT_NONE;
			else
				new_serinfo.closing_wait = atonum(*arg);
			arg++;
			break;
		case CMD_WAIT2:
			if (!strcasecmp(*arg, "infinite"))
				new_serinfo.closing_wait2 = ASYNC_CLOSING_WAIT_INF;
			else if (!strcasecmp(*arg, "none"))
				new_serinfo.closing_wait2 = ASYNC_CLOSING_WAIT_NONE;
			else
				new_serinfo.closing_wait2 = atonum(*arg);
			arg++;
			break;
		case CMD_CONFIG:
			if (ioctl(fd, TIOCSSERIAL, &new_serinfo) < 0) {
				perror("Cannot set serial info");
				exit(1);
			}
			if (ioctl(fd, TIOCSERCONFIG) < 0) {
				perror("Cannot autoconfigure port");
				exit(1);
			}
			if (ioctl(fd, TIOCGSERIAL, &new_serinfo) < 0) {
				perror("Cannot get serial info");
				exit(1);
			}
			break;
		case CMD_GETMULTI:
			if (ioctl(fd, TIOCSSERIAL, &new_serinfo) < 0) {
				perror("Cannot set serial info");
				exit(1);
			}
			get_multiport(device, fd);
			break;
		case CMD_SETMULTI:
			if (ioctl(fd, TIOCSSERIAL, &new_serinfo) < 0) {
				perror("Cannot set serial info");
				exit(1);
			}
			set_multiport(device, fd, &arg);
			break;
#ifdef TIOCGHAYESESP
		case CMD_RX_TRIG:
		case CMD_TX_TRIG:
		case CMD_FLOW_OFF:
		case CMD_FLOW_ON:
		case CMD_RX_TMOUT:
		case CMD_DMA_CHAN:
			set_hayesesp(fd, p->cmd, atonum(*arg++));
			break;
#endif
		default:
			fprintf(stderr, "Internal error: unhandled cmd #%d\n", p->cmd);
			exit(1);
		}
	}
	if (ioctl(fd, TIOCSSERIAL, &new_serinfo) < 0) {
		perror("Cannot set serial info");
		exit(1);
	}
	close(fd);
	if (verbose_flag)
		get_serial(device);
}

void do_wild_intr(char *device)
{
	int	fd;
	int	i, mask;
	int	wild_mask = -1;
	
	if ((fd = open(device, O_RDWR|O_NONBLOCK)) < 0) {
		perror(device);
		exit(1);
	}
	if (ioctl(fd, TIOCSERSWILD, &wild_mask) < 0) {
		perror("Cannot scan for wild interrupts");
		exit(1);
	}
	if (ioctl(fd, TIOCSERGWILD, &wild_mask) < 0) {
		perror("Cannot get wild interrupt mask");
		exit(1);
	}
	close(fd);
	if (quiet_flag)
		return;
	if (wild_mask) {
		printf("Wild interrupts found: ");
		for (i=0, mask=1; mask <= wild_mask; i++, mask <<= 1)
			if (mask & wild_mask)
				printf(" %d", i);
		printf("\n");
	} else if (verbose_flag)
		printf("No wild interrupts found.\n");
	return;
}




void usage()
{
	fprintf(stderr, "%s\n\n", version_str);
	fprintf(stderr,
		"usage:\t %s serial-device -abqvVWz [cmd1 [arg]] ... \n", 
		progname);
	fprintf(stderr, "\t %s -g [-abGv] device1 ...\n\n", progname);
	fprintf(stderr, "Available commands: (* = Takes an argument)\n");
	fprintf(stderr, "\t\t(^ = can be preceded by a '^' to turn off the option)\n");
fprintf(stderr, "\t* port\t\tset the I/O port\n");
	fprintf(stderr, "\t* irq\t\tset the interrupt\n");	
	fprintf(stderr, "\t* uart\t\tset UART type (none, 8250, 16450, 16550, 16550A,\n");
	fprintf(stderr, "\t\t\t16650, 16650V2, 16750, 16850, 16950, 16954)\n");
	fprintf(stderr, "\t* baud_base\tset base baud rate (CLOCK_FREQ / 16)\n");
	fprintf(stderr, "\t* divisor\tset the custom divisor (see spd_custom)\n");
	fprintf(stderr, "\t* close_delay\tset the amount of time (in 1/100 of a\n");
	fprintf(stderr, "\t\t\t\tsecond) that DTR should be kept low\n");
	fprintf(stderr, "\t\t\t\twhile being closed\n");
	fprintf(stderr, "\t* closing_wait\tset the amount of time (in 1/100 of a\n");
	fprintf(stderr, "\t\t\t\tsecond) that the serial port should wait for\n");
	fprintf(stderr, "\t\t\t\tdata to be drained while being closed.\n");
	fprintf(stderr, "\t^ fourport\tconfigure the port as an AST Fourport\n");
	fprintf(stderr, "\t  autoconfig\tautomatically configure the serial port\n");
	fprintf(stderr, "\t^ auto_irq\ttry to determine irq during autoconfiguration\n");
	fprintf(stderr, "\t^ skip_test\tskip UART test during autoconfiguration\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\t^ sak\t\tset the break key as the Secure Attention Key\n");
	fprintf(stderr, "\t^ session_lockout Lock out callout port across different sessions\n");
	fprintf(stderr, "\t^ pgrp_lockout\tLock out callout port across different process groups\n");
	fprintf(stderr, "\t^ callout_nohup\tDon't hangup the tty when carrier detect drops\n");
	fprintf(stderr, "\t\t\t\t on the callout device\n");
	fprintf(stderr, "\t^ split_termios Use separate termios for callout and dailin lines\n");
	fprintf(stderr, "\t^ hup_notify\tNotify a process blocked on opening a dial in line\n");
	fprintf(stderr, "\t\t\t\twhen a process has finished using a callout\n");
	fprintf(stderr, "\t\t\t\tline by returning EAGAIN to the open.\n");
	fprintf(stderr, "\t^ low_latency\tMinimize receive latency at the cost of greater\n");
	fprintf(stderr, "\t\t\t\tCPU utilization.\n");
	fprintf(stderr, "\t  get_multiport\tDisplay the multiport configuration\n");
	fprintf(stderr, "\t  set_multiport\tSet the multiport configuration\n");
	fprintf(stderr, "\n");
#ifdef TIOCGHAYESESP
	fprintf(stderr, "\t* rx_trigger\tSet RX trigger level (ESP-only)\n");
	fprintf(stderr, "\t* tx_trigger\tSet TX trigger level (ESP-only)\n");
	fprintf(stderr, "\t* flow_off\tSet hardware flow off level (ESP-only)\n");
	fprintf(stderr, "\t* flow_on\tSet hardware flow on level (ESP-only)\n");
	fprintf(stderr, "\t* rx_timeout\tSet receive timeout (ESP-only)\n");
	fprintf(stderr, "\t* dma_channel\tSet DMA channel (ESP-only)\n");
#endif
	fprintf(stderr, "\n");
	fprintf(stderr, "\t  spd_hi\tuse 56kb instead of 38.4kb\n");
	fprintf(stderr, "\t  spd_vhi\tuse 115kb instead of 38.4kb\n");
	fprintf(stderr, "\t  spd_shi\tuse 230kb instead of 38.4kb\n");
	fprintf(stderr, "\t  spd_warp\tuse 460kb instead of 38.4kb\n");
	fprintf(stderr, "\t  spd_cust\tuse the custom divisor to set the speed at 38.4kb\n");
	fprintf(stderr, "\t\t\t\t(baud rate = baud_base / custom_divisor)\n");
	fprintf(stderr, "\t  spd_normal\tuse 38.4kb when a buad rate of 38.4kb is selected\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Use a leading '0x' for hex numbers.\n");
	fprintf(stderr, "CAUTION: Using an invalid port can lock up your machine!\n");
	exit(1);
}

main(int argc, char **argv)
{
	int	get_flag = 0, wild_intr_flag = 0;
	int	c;
	extern int optind;
	extern char *optarg;
	
	progname = argv[0];
	if (argc == 1)
		usage();
	while ((c = getopt(argc, argv, "abgGqvVWz")) != EOF) {
		switch (c) {
		case 'a':
			verbosity = 2;
			break;
		case 'b':
			verbosity = 0;
			break;
		case 'q':
			quiet_flag++;
			break;
		case 'v':
			verbose_flag++;
			break;
		case 'g':
			get_flag++;
			break;
		case 'G':
			verbosity = -1;
			break;
		case 'V':
			fprintf(stderr, "%s\n", version_str);
			exit(0);
		case 'W':
			wild_intr_flag++;
			break;
		case 'z':
			zero_flag++;
			break;
		default:
			usage();
		}
	}
	if (get_flag) {
		argv += optind;
		while (*argv)
			get_serial(*argv++);
		exit(0);
	}
	if (argc == optind)
		usage();
	if (wild_intr_flag) {
		do_wild_intr(argv[optind]);
		exit(0);
	}
	if (argc-optind == 1)
		get_serial(argv[optind]);
	else
		set_serial(argv[optind], argv+optind+1);
	exit(0);
}

