/* 
 * Copyright (c) 2014 Fukuda Laboratory and Shigemi ISHIDA, Kyushu University
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

/**
 * @file
 *      TCP listening module.
 * @author
 *      Shigemi Ishida <ishida+devel@f.ait.kyushu-u.ac.jp>
 *
 * Wait for TCP connections.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <sys/select.h>

#include "trace.h"
#include "main.h"
#include "lisn.h"
#include "conn.h"

/*======================================================================
 * global variables
 *======================================================================*/

/*------------------------------
 * private
 *------------------------------*/
static int sock[LISN_MAX_SOCK]; /* listen sockets */

/*======================================================================
 * prototype declarations for private functions
 *======================================================================*/

/*======================================================================
 * functions
 *======================================================================*/
int lisn_init(opr_t *opr)
{
    /* initialize sockets with -1 */
    memset(sock, 0xFF, sizeof(sock));

    return(0);
}

/*----------------------------------------------------------------------*/
void lisn_deinit(opr_t *opr)
{
    int cnt;

    /* close all opening sockets */
    for (cnt=0; cnt < LISN_MAX_SOCK; cnt++)
    {
        if (sock[cnt] >= 0)
        {
            T_M(T_D1, 0x01020100, "closing socket: %d.\n", sock[cnt]);
            close(sock[cnt]);
            sock[cnt] = -1;
        }
    }

    return;
}

/*----------------------------------------------------------------------*/
int lisn_start_listen(opr_t *opr)
{
    int ret;
    int sock_cnt;
    /* address info structures */
    struct addrinfo  hints;     /* for hinting */
    struct addrinfo *res;       /* pointer to results */
    struct addrinfo *res_cnt;   /* pointer for results handling */

    /*------------------------------
     * retrieve address information
     *------------------------------*/
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = PF_UNSPEC;   /* can use IPv4 and IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* TCP */
    hints.ai_flags    = AI_PASSIVE;  /* passive mode */

    ret = getaddrinfo(NULL, opr->port, &hints, &res);
    if (ret != 0)
    {
        T_M(T_E, 0x81030200, "cannot get address information: %s\n.", gai_strerror(ret));
        return(0x81030200);
    }

    /*------------------------------
     * bind listen for all addresses
     *------------------------------*/
    sock_cnt = 0;
    for (res_cnt = res; res && sock_cnt < LISN_MAX_SOCK; res = res->ai_next)
    {
        /* create socket */
        ret = socket(res->ai_family, res_cnt->ai_socktype, res_cnt->ai_protocol);
        if (ret < 0)
        {
            /* ignore when error */
            T_M(T_D1, 0x01030300, "cannot create socket: %s.\n", strerror(errno));
            continue;
        }
        sock[sock_cnt] = ret;
        T_M(T_D2, 0x01030380, "created socket sock[%d]=%d.\n", sock_cnt, sock[sock_cnt]);

        /* bind */
        ret = bind(sock[sock_cnt], res_cnt->ai_addr, res_cnt->ai_addrlen);
        if (ret < 0)
        {
            /* ignore when error */
            T_M(T_D1, 0x81030400, "cannot bind for sock[%d]=%d: %s.\n",
                sock_cnt, sock[sock_cnt], strerror(errno));
            close(sock[sock_cnt]);
            sock[sock_cnt] = -1;
            continue;
        }
        T_M(T_D2, 0x01030480, "bind sock[%d]=%d.\n", sock_cnt, sock[sock_cnt]);

        /* listen */
        ret = listen(sock[sock_cnt], 8);
        if (ret < 0)
        {
            /* ignore when error */
            T_M(T_D1, 0x81030500, "cannot listen on sock[%d]=%d: %s.\n",
                sock_cnt, sock[sock_cnt], strerror(errno));
            close(sock[sock_cnt]);
            sock[sock_cnt] = -1;
            continue;
        }
        T_M(T_D2, 0x01030480, "listen sock[%d]=%d.\n", sock_cnt, sock[sock_cnt]);
        sock_cnt++;
    }

    if (sock_cnt <= 0)
    {
        /* no listen succeeded */
        T_M(T_E, 0x8103f000, "cannot listen on any interface.\n");
        freeaddrinfo(res);
        return(0x8103f000);
    }

    freeaddrinfo(res);
    return(0);
}

/*----------------------------------------------------------------------*/
int lisn_fd_set(fd_set *fds)
{
    int cnt;
    int max_fd = 0;

    for (cnt = 0; cnt < LISN_MAX_SOCK; cnt++)
    {
        if (sock[cnt] >= 0)
        {
            FD_SET(sock[cnt], fds);
            if (sock[cnt] > max_fd)
            {
                max_fd = sock[cnt];
            }
        }
    }

    return(max_fd);
}

/*----------------------------------------------------------------------*/
int lisn_fd_process(fd_set *fds)
{
    int cnt;
    int ret;

    for (cnt = 0; cnt < LISN_MAX_SOCK; cnt++)
    {
        /* skip closed sockets */
        if (sock[cnt] < 0)
        {
            continue;
        }

        /* check if there is a connection request */
        if (FD_ISSET(sock[cnt], fds))
        {
            /* accept connection */
            ret = conn_accept(sock[cnt]);
            if (ret < 0)
            {
                return(ret);
            }
        }
    }

    return(0);
}

/*======================================================================
 * private functions
 *======================================================================*/

/*----------------------------------------------------------------------*/

/* end of lisn.c */
