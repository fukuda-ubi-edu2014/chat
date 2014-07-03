/* 
 * Copyright (c) 2014 Fukuda Laboratory and Shigemi ISHIDA, Kyushu University
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

/**
 * @file
 *      Tools.
 * @author
 *      Shigemi Ishida <ishida+devel@f.ait.kyushu-u.ac.jp>
 *
 * Some common tools.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>

/*======================================================================
 * prototype declarations for private functions
 *======================================================================*/

/*======================================================================
 * constants and macros
 *======================================================================*/

/*======================================================================
 * global variables
 *======================================================================*/

/*------------------------------
 * private
 *------------------------------*/

/*======================================================================
 * functions
 *======================================================================*/
int is_number(char *num_str)
{
    for (; *num_str != '\0'; num_str++)
    {
        if (!isdigit((unsigned char)*num_str))
        {
            return(0);
        }
    }
    return(1);
}

/*----------------------------------------------------------------------*/
int timeval_sub(struct timeval *x, struct timeval *y)
{
    struct timeval out;

    if (!x || !y)
    {
        return(-1);
    }

    out.tv_sec  = x->tv_sec  - y->tv_sec;
    out.tv_usec = x->tv_usec - y->tv_usec;
    if (out.tv_usec < 0)
    {
        --(out.tv_usec);
        out.tv_usec += 1000000;
    }

    x->tv_sec  = out.tv_sec;
    x->tv_usec = out.tv_usec;

    return(0);
}

/*----------------------------------------------------------------------*/
void timeout_start(struct timeval *start)
{
    memset(start, 0, sizeof(*start));
    gettimeofday(start, NULL);

    return;
}

/*----------------------------------------------------------------------*/
int timeout_chk(struct timeval *start, unsigned int timeout)
{
    int ret;
    struct timeval now;
    struct timeval max_diff;

    /* convert timeout into struct timeval */
    max_diff.tv_sec  = timeout / 1000;
    max_diff.tv_usec = (timeout % 1000) * 1000;

    memset(&now, 0xff, sizeof(now));
    gettimeofday(&now, NULL);

    ret = timeval_sub(&now, start);
    if (ret < 0)
    {
        return(-1);
    }

    ret = (now.tv_sec < max_diff.tv_sec) ||
        ((now.tv_sec == max_diff.tv_sec) &&
         (now.tv_usec < max_diff.tv_usec));

    return(ret);
}

/* end of tool.c */
