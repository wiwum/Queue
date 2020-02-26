/* libc/sys/linux/include/mqueue.h - message queue functions */

/* Copyright 2002, Red Hat Inc. - all rights reserved */

// Modified by renameing 'mq_' to 'lq_'

#ifndef __LQUEUE_H
#define __LQUEUE_H

#include <sys/types.h>
#define __need_sigevent_t 1
#include "siginfo.h"

#include <sys/fcntl.h>

/* message queue types */
typedef int lqd_t;

struct lq_attr {
  long lq_flags;    /* message queue flags */
  long lq_maxmsg;   /* maximum number of messages */
  long lq_msgsize;  /* maximum message size */
  long lq_curmsgs;  /* number of messages currently queued */
};

#define lq_PRIO_MAX 16

/* prototypes */
//lqd_t lq_open (const char *__name, int __oflag, ...);
lqd_t lq_open (const char *__name, int __oflag);
lqd_t lq_open (const char *__name, int __oflag, int mode, lq_attr *attr);

int lq_close (lqd_t __msgid);
int lq_send (lqd_t __msgid, const char *__msg, size_t __msg_len, unsigned int __msg_prio);
//ssize_t lq_receive (lqd_t __msgid, char *__msg, size_t __msg_len, unsigned int *__msg_prio);
ssize_t lq_receive (lqd_t __msgid, char *__msg, size_t __msg_len, unsigned int *__msg_prio);
int lq_notify (lqd_t __msgid, const struct sigevent *__notification);
int lq_unlink (const char *__name);
int lq_getattr (lqd_t __msgid, struct lq_attr *__mqstat);
int lq_setattr (lqd_t __msgid, const struct lq_attr *__mqstat, struct lq_attr *__omqattr);

#endif /* __LQUEUE_H */
