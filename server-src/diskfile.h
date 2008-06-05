/*
 * Amanda, The Advanced Maryland Automatic Network Disk Archiver
 * Copyright (c) 1991-1998 University of Maryland at College Park
 * All Rights Reserved.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of U.M. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  U.M. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * U.M. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL U.M.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: James da Silva, Systems Design and Analysis Group
 *			   Computer Science Department
 *			   University of Maryland at College Park
 */
/*
 * $Id: diskfile.h,v 1.38 2006/06/22 20:41:33 martinea Exp $
 *
 * interface for disklist file reading code
 */
#ifndef DISKFILE_H
#define DISKFILE_H

#include "amanda.h"
#include "conffile.h"
#include "amfeatures.h"

typedef struct netif_s {
    struct netif_s *next;
    interface_t *config;
    unsigned long curusage;
} netif_t;

typedef struct amhost_s {
    struct amhost_s *next;		/* next host */
    char *hostname;			/* name of host */
    struct disk_s *disks;		/* linked list of disk records */
    int inprogress;			/* # dumps in progress */
    int maxdumps;			/* maximum dumps in parallel */
    netif_t *netif;			/* network interface this host is on */
    time_t start_t;			/* start dump after this time */
    char *up;				/* generic user pointer */
    am_feature_t *features;		/* feature set */
    int	 pre_script;
    int  post_script;
} am_host_t;

typedef struct disk_s {
    int		line;			/* line number of last definition */
    struct disk_s *prev, *next;		/* doubly linked disk list */

    am_host_t	*host;			/* host list */
    struct disk_s *hostnext;

    char        *hostname;		/* hostname */
    char	*name;			/* label name for disk */
    char	*device;		/* device name for disk, eg "sd0g" */
    char	*dtype_name;		/* name of dump type   XXX shouldn't need this */
    char	*program;		/* dump program, eg DUMP, STAR, GNUTAR */
    char	*srvcompprog;		/* custom compression server filter */
    char	*clntcompprog;		/* custom compression client filter */
    char	*srv_encrypt;		/* custom encryption server filter */
    char	*clnt_encrypt;		/* custom encryption client filter */
    char	*amandad_path;		/* amandad path on the client */
    char	*client_username;	/* username to connect on the client */
    char	*ssh_keys;		/* ssh_key file to use */
    sl_t	*exclude_file;		/* file exclude spec */
    sl_t	*exclude_list;		/* exclude list */
    sl_t	*include_file;		/* file include spec */
    sl_t	*include_list;		/* include list */
    int		exclude_optional;	/* exclude list are optional */
    int		include_optional;	/* include list are optional */
    int		priority;		/* priority of disk */
    off_t	tape_splitsize;		/* size of dumpfile chunks on tape */
    char	*split_diskbuffer;	/* place where we can buffer PORT-WRITE dumps other than RAM */
    off_t	fallback_splitsize;	/* size for in-RAM PORT-WRITE buffers */
    int		dumpcycle;		/* days between fulls */
    long	frequency;		/* XXX - not used */
    char	*security_driver;	/* type of authentication (per disk) */
    int		maxdumps;		/* max number of parallel dumps (per system) */
    int		maxpromoteday;		/* maximum of promote day */
    int		bumppercent;
    off_t	bumpsize;
    int		bumpdays;
    double	bumpmult;
    time_t	starttime;		/* start this dump after this time */
    time_t	start_t;		/* start this dump after this time */
    int		strategy;		/* what dump strategy to use */
    int		ignore;			/* ignore */
    int		estimate;		/* what estimate strategy to use */
    int		compress;		/* type of compression to use */
    int		encrypt;		/* type of encryption to use */
    char	*srv_decrypt_opt;	/* server-side decryption option parameter to use */
    char	*clnt_decrypt_opt;	/* client-side decryption option parameter to use */
    double	comprate[2];		/* default compression rates */
    /* flag options */
    int		record;			/* record dump in /etc/dumpdates ? */
    int		skip_incr;		/* incs done externally ? */
    int		skip_full;		/* fulls done externally ? */
    int		to_holdingdisk;		/* use holding disk ? */
    int		kencrypt;
    int		index;			/* produce an index ? */
    int		spindle;		/* spindle # - for parallel dumps */
    int		inprogress;		/* being dumped now? */
    int		todo;
    application_t *application;
    pp_scriptlist_t pp_scriptlist;
    void	*up;			/* generic user pointer */
} disk_t;

typedef struct disklist_s {
    disk_t *head, *tail;
} disklist_t;

#define empty(dlist)	((dlist).head == NULL)

/* This function is integrated with the conffile.c error-handling; handle its return
 * value just as you would the return of config_init() */
cfgerr_level_t read_diskfile(const char *, disklist_t *);

am_host_t *lookup_host(const char *hostname);
disk_t *lookup_disk(const char *hostname, const char *diskname);

disk_t *add_disk(disklist_t *list, char *hostname, char *diskname);

void enqueue_disk(disklist_t *list, disk_t *disk);
void headqueue_disk(disklist_t *list, disk_t *disk);
void insert_disk(disklist_t *list, disk_t *disk, int (*f)(disk_t *a, disk_t *b));
int  find_disk(disklist_t *list, disk_t *disk);
void sort_disk(disklist_t *in, disklist_t *out, int (*f)(disk_t *a, disk_t *b));
disk_t *dequeue_disk(disklist_t *list);
void remove_disk(disklist_t *list, disk_t *disk);

void dump_queue(char *str, disklist_t q, int npr, FILE *f);

char *optionstr(disk_t *dp, am_feature_t *their_features, FILE *fdout);
char *xml_optionstr(disk_t *dp, am_feature_t *their_features, FILE *fdout);
char *xml_application(application_t *);
char *xml_scripts(pp_scriptlist_t pp_scriptlist);

char *match_disklist(disklist_t *origqp, int sargc, char **sargv);
void free_disklist(disklist_t *dl);

netif_t *disklist_netifs(void);

#endif /* ! DISKFILE_H */
