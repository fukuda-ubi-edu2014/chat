/* 
 * Copyright (c) 2014 Fukuda Laboratory and Shigemi ISHIDA, Kyushu University
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

/**
 * @file
 *      Server program main module.
 * @author
 *      Shigemi Ishida <ishida+devel@f.ait.kyushu-u.ac.jp>
 *
 * Chat server program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>

#include "trace.h"
#include "tool.h"
#include "../com.h"
#include "main.h"
#include "lisn.h"
#include "conn.h"

/*======================================================================
 * global variables
 *======================================================================*/

/*------------------------------
 * private
 *------------------------------*/
static int status;             /* status flags */

/*======================================================================
 * prototype declarations for private functions
 *======================================================================*/
static int arg_handler(int argc, char *argv[], opr_t *opr);
static int global_init(opr_t *opr);
static void global_deinit(opr_t *opr);
static void usage(void);
static void ctrl_c_trap(int signo);

/*======================================================================
 * functions
 *======================================================================*/
int main(int argc, char *argv[])
{
    opr_t opr;                  /* operation parameters */
    int   ret;                  /* return value handler */
    int    fdnum;               /* number of changed file descriptors */
    fd_set readfds;             /* descriptor set for select */

    status = STAT_INIT;

    /* set a default trace level to ERROR */
    T_init(T_E);

    /* initialize operation parameters */
    memset(&opr, 0, sizeof(opr));

    /* handling options, arguments */
    ret = arg_handler(argc, argv, &opr);
    if (ret < 0)
    {
        return(0x80010010);
    }

    ret = global_init(&opr);
    if (ret < 0)
    {
        global_deinit(&opr);
        return(ret);
    }

    status |= STAT_WORK;
    T_M(T_D2, 0x00010100, "finished initialization.\n");

    /*----------------------------------------------------------------------*/

    /* start listening and wait for connections */
    ret = lisn_start_listen(&opr);
    if (ret < 0)
    {
        global_deinit(&opr);
        return(ret);
    }

    while (!(status & STAT_FIN) && !(status & STAT_ERR))
    {
        FD_ZERO(&readfds);      /* initialize fd set */

        /* set listening sockets */
        ret = lisn_fd_set(&readfds);
        /* set connection sockets */
        fdnum = conn_fd_set(&readfds);
        /* set max of file descriptors */
        ret = (fdnum > ret)? fdnum : ret;

        fdnum = select(ret+1, &readfds, NULL, NULL, NULL);
        if (fdnum < 0)
        {
            if (errno == EINTR)
            {
                /* ignore signal interrupts */
                T_M(T_D1, 0x800103f0, "select: signal interrupt.\n");
                continue;
            }

            T_M(T_E, 0x80010400, "select error: %s.\n", strerror(errno));
            status |= STAT_ERR;
            continue;
        }
        if (fdnum == 0)
        {
            /* there is no change */
            continue;
        }

        ret = lisn_fd_process(&readfds);
        if (ret < 0)
        {
            status |= STAT_ERR;
        }

        ret = conn_fd_process(&readfds);
        if (ret < 0)
        {
            status |= STAT_ERR;
        }
    }

    /*----------------------------------------------------------------------*/

    global_deinit(&opr);

    T_M(T_D2, 0x0001ffff, "program completed.\n");

    return(0);
}

/*======================================================================
 * private functions
 *======================================================================*/
static int arg_handler(int argc, char *argv[], opr_t *opr)
{
    int ret;

    /* set default parameters */
    snprintf(opr->port, sizeof(opr->port), "%d", COM_DEF_PORT);

    /*------------------------------
     * handling options
     *------------------------------*/
    for (;;)
    {
        ret = getopt(argc, argv, "hd:p:");

        if (ret < 0)
        {
            break;
        }
        switch (ret)
        {
        case 'h':
            usage();
            T_M(T_D2, 0x400100ff, "exit with showing help.\n");
            exit(0);
        case 'd':
            if (!is_number(optarg))
            {
                T_M(T_E, 0xc0010100, "invalid debug level parameter: %s.\n", optarg);
                return(0xc0010100);
            }
            opr->trace_level = (unsigned int)strtol(optarg, NULL, 10);
            ret = T_init(opr->trace_level);
            if (ret < 0)
            {
                T_init(T_E);
            }
            break;
        case 'p':               /* port name */
            strncpy(opr->port, optarg, sizeof(opr->port)-1);
            T_M(T_I, 0x40010290, "port changed to %s.\n", opr->port);
            break;
        case '?':               /* invalid option */
            T_M(T_E, 0xc00101ee, "invalid option.\n", optopt);
            usage();
            return(0xc00101ee);
            break;
        default:                /* no route to here */
            T_M(T_E, 0xc00101ff, "invalid option.\n", optopt);
            usage();
            return(0xc00101ff);
            break;
        }
    }

    /*------------------------------
     * handling arguments
     *------------------------------*/
    switch (argc - optind)
    {
    case 0:
        break;
    default:
        T_M(T_E, 0xc0010500, "invalid arguments.\n");
        usage();
        return(0xc0010700);
        break;
    }

    return(0);
}

/*----------------------------------------------------------------------*/
static int global_init(opr_t *opr)
{
    int ret;

    /* register Ctrl-C signal handler */
    memset(&opr->sa, 0, sizeof(struct sigaction));
    opr->sa.sa_handler = ctrl_c_trap;
    ret = sigaction(SIGINT, &opr->sa, NULL);
    if (ret < 0)
    {
        T_M(T_E, 0xc0020100, "cannot set signal handler.\n");
        return(0xc0020100);
    }

    /* TCP listening module */
    ret = lisn_init(opr);
    if (ret < 0)
    {
        return(ret);
    }

    /* connection management module */
    ret = conn_init(opr);
    if (ret < 0)
    {
        return(ret);
    }

    return(0);
}

/*----------------------------------------------------------------------*/
static void global_deinit(opr_t *opr)
{
    /* connection management module */
    conn_deinit(opr);

    /* TCP listening module */
    lisn_deinit(opr);

    return;
}

/*----------------------------------------------------------------------*/
static void usage(void)
{
    puts("Usage:");
    puts("\tchatserv [-h] [-d <debug_level>] [-p <port_name>]");
    puts("");
    puts("Options:");
    puts("\t-h show this help and exit");
    puts("\t-d specify debug message level");
    printf("\t\t%d\tERROR (default)\n", T_E);
    printf("\t\t%d\tWARNING\n", T_W);
    printf("\t\t%d\tINFO\n", T_I);
    printf("\t\t%d\tDEBUG1\n", T_D1);
    printf("\t\t%d\tDEBUG2\n", T_D2);
    printf("\t-p specify port name or port number (default: %d)\n", COM_DEF_PORT);

    return;
}

/*----------------------------------------------------------------------*/
static void ctrl_c_trap(int signo)
{
    /* stop operation */
    char *msg = "\nOperation is stopped by user operation.\n";
    write(STDOUT_FILENO, msg, strlen(msg));

    status |= STAT_FIN;

    return;
}

/* end of main.c */
