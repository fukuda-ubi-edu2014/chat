/* 
 * Copyright (c) 2014 Fukuda Laboratory and Shigemi ISHIDA, Kyushu University
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

/**
 * @file
 *      Connection management module.
 * @author
 *      Shigemi Ishida <ishida+devel@f.ait.kyushu-u.ac.jp>
 *
 * Accept TCP connection and handle massages from/to each connection.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include "trace.h"
#include "main.h"
#include "conn.h"

/*======================================================================
 * global variables
 *======================================================================*/

/*------------------------------
 * private
 *------------------------------*/
static int  sock[CONN_MAX_SOCK]; /* accepted sockets */
static char names[CONN_MAX_SOCK][CONN_MAX_NAME]; /* host names of accepted sockets */
static char *quit_msg[] = {                      /* quit messages */
    "bye\r\n",
    "exit\r\n",
    "quit\r\n",
    NULL
};

/*======================================================================
 * prototype declarations for private functions
 *======================================================================*/
static int conn_find_vacant_sock(void);
static int conn_recv(int sock_cnt, char *buf);
static void conn_disconnect(int sock_cnt);
static int conn_broadcast(char *client_name, char *msg);
static int conn_recv_broadcast(int sock_cnt);

/*======================================================================
 * functions
 *======================================================================*/
int conn_init(opr_t *opr)
{
    /* initialize sockets with -1 */
    memset(sock, 0xFF, sizeof(sock));

    /* initialize name */
    memset(names, 0, sizeof(names));

    return(0);
}

/*----------------------------------------------------------------------*/
void conn_deinit(opr_t *opr)
{
    int cnt;

    /* close all opening sockets */
    for (cnt=0; cnt < CONN_MAX_SOCK; cnt++)
    {
        if (sock[cnt] >= 0)
        {
            T_M(T_D1, 0x02030100, "closing socket: %d.\n", sock[cnt]);
            close(sock[cnt]);
            sock[cnt] = -1;
        }
    }

    return;
}

/*----------------------------------------------------------------------*/
int conn_accept(int new_sock)
{
    int ret;
    int cnt;
    socklen_t caddrlen;         /* client address length */
    struct sockaddr_in caddr;   /* client address structure */

    ret = conn_find_vacant_sock();
    if (ret < 0)
    {
        return(ret);
    }
    cnt = ret;

    /* accept */
    sock[cnt] = accept(new_sock, (struct sockaddr*)&caddr, &caddrlen);
    if (sock[cnt] < 0)
    {
        T_M(T_E, 0x82030200, "cannot accept: %s.\n", strerror(errno));
        sock[cnt] = -1;
        return(0x82030200);
    }

    /* retrieve host name */
    ret = getnameinfo((struct sockaddr *)&caddr, caddrlen,
                      names[cnt], sizeof(names[cnt]),
                      NULL, 0, NI_NAMEREQD);
    if (strlen(names[cnt]) == 0)
    {
        /* use specific name when no name retrieved */
        snprintf(names[cnt], sizeof(names[cnt]), "noname");
    }
    T_M(T_D1, 0x02030400, "connection established with %s.\n", names[cnt]);

    return(0);
}

/*----------------------------------------------------------------------*/
int conn_fd_set(fd_set *fds)
{
    int cnt;
    int max_fd = 0;

    for (cnt = 0; cnt < CONN_MAX_SOCK; cnt++)
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
int conn_fd_process(fd_set *fds)
{
    int cnt;
    int ret;

    for (cnt = 0; cnt < CONN_MAX_SOCK; cnt++)
    {
        /* skip closed sockets */
        if (sock[cnt] < 0)
        {
            continue;
        }

        /* check if there is message */
        if (FD_ISSET(sock[cnt], fds))
        {
            T_M(T_D1, 0x020050400, "process a message from sock[%d]=%d.\n",
                cnt, sock[cnt]);
            /* receive a message and broadcast it */
            ret = conn_recv_broadcast(cnt);
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
static int conn_find_vacant_sock(void)
{
    int cnt;

    T_D(T_D2, 0x42010100, sock, sizeof(sock));
    for (cnt = 0; cnt < CONN_MAX_SOCK; cnt++)
    {
        if (sock[cnt] < 0)
        {
            T_M(T_D1, 0x42010200, "use connection sock[%d].\n");
            return(cnt);
        }
    }

    T_M(T_E, 0xc201e000, "no more space to save sockets.\n");
    return(0xc201e000);
}

/*----------------------------------------------------------------------*/
static int conn_recv(int sock_cnt, char *buf)
{
    int ret;

    ret = recv(sock[sock_cnt], buf, CONN_MAX_MSG-1, 0);
    if (ret < 0)
    {
        if (errno == EAGAIN)
        {
            /* timeout */
            T_M(T_D1, 0x42020080, "timed out.\n");
            return(0);
        }

        /* other error */
        T_M(T_E, 0xc2020100, "cannot recv from sock[%d]=%d: %s.\n",
            sock_cnt, sock[sock_cnt], strerror(errno));
        return(0xc2020100);
    }
    if (ret == 0)
    {
        T_M(T_W, 0xc2020200, "connection closed by remote host.\n");
        conn_disconnect(sock_cnt);
    }

    return(ret);
}

/*----------------------------------------------------------------------*/
static void conn_disconnect(int sock_cnt)
{
    close(sock[sock_cnt]);
    sock[sock_cnt] = -1;
    memset(names[sock_cnt], 0, sizeof(names[sock_cnt]));

    return;
}

/*----------------------------------------------------------------------*/
static int conn_broadcast(char *client_name, char *msg)
{
    int cnt;
    int ret;
    char buf[CONN_MAX_MSG+CONN_MAX_NAME+3];

    /* clear send buffer */
    memset(buf, 0, sizeof(buf));

    /* generate send message */
    snprintf(buf, sizeof(buf), "[%s] %s", client_name, msg);
    T_M(T_D1, 0x42040200, "send message: %s\n", buf);

    for (cnt=0; cnt < CONN_MAX_SOCK; cnt++)
    {
        /* skip closed sockets */
        if (sock[cnt] < 0)
        {
            continue;
        }

        ret = send(sock[cnt], buf, strlen(buf), 0);
        if (ret < 0)
        {
            T_M(T_W, 0xc2044100, "cannot send to sock[%d]=%d, %s.\n",
                cnt, sock[cnt], names[cnt]);
        }
    }

    return(0);
}

/*----------------------------------------------------------------------*/
static int conn_recv_broadcast(int sock_cnt)
{
    int ret;
    int cnt;
    char *buffer = NULL;

    /* receive message */
    ret = conn_recv(sock_cnt, buffer);
    if (ret <= 0)
    {
        return(ret);
    }

    /* check if quit */
    T_M(T_D2, 0x420501f0, "%p.\n", quit_msg[0]);
    T_M(T_D2, 0x420501f1, "%s.\n", quit_msg[0]);
    for (cnt = 0; quit_msg[cnt] != NULL; cnt++)
    {
        T_D(T_D2, 0x42050200, quit_msg[cnt], strlen(quit_msg[cnt])+1);
        T_D(T_D2, 0x42050210, buffer, CONN_MAX_MSG);
        ret = strcmp(quit_msg[cnt], buffer);
        if (ret == 0)
        {
            /* send bye bye  */
            sprintf(buffer, "Bye!\r\n");
            (void)send(sock[sock_cnt], buffer, strlen(buffer), 0);

            /* disconnect */
            conn_disconnect(sock_cnt);
            return(0);
        }
    }

    /* broadcast message */
    ret = conn_broadcast(names[sock_cnt], buffer);
    if (ret < 0)
    {
        return(ret);
    }

    return(0);
}

/* end of conn.c */
