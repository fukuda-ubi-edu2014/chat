/* 
 * Copyright (c) 2014 Fukuda Laboratory and Shigemi ISHIDA, Kyushu University
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

/**
 * @file
 *      Header file for connection management module.
 * @author
 *      Shigemi Ishida <ishida+devel@f.ait.kyushu-u.ac.jp>
 */
#ifndef __CONN_H_
#define __CONN_H_

/*======================================================================
 * includes
 *======================================================================*/
#include "main.h"

/*======================================================================
 * constants, macros
 *======================================================================*/
/**
 * @def CONN_MAX_SOCK
 * @brief Max number of connections.
 */
#define CONN_MAX_SOCK   8

/**
 * @def CONN_MAX_NAME
 * @brief Max length of client names.
 */
#define CONN_MAX_NAME   128

/**
 * @def CONN_MAX_MSG
 * @brief Max length of a message.
 */
#define CONN_MAX_MSG    128

/*======================================================================
 * typedefs, structures
 *======================================================================*/

/*======================================================================
 * prototype declarations
 *======================================================================*/

/**
 * @brief      Connection management module init.
 * @param[in,out] opr Operation parameters.
 * @return      Returns 0 on success.
 *              Returns minus value on any error.
 *
 * This function initializes connection management module.
 */
int conn_init(opr_t *opr);

/**
 * @brief       Connection management de-init.
 * @param[in,out] opr Pointer to the operation parameters.
 *
 * This function closes connection management module.
 */
void conn_deinit(opr_t *opr);

/**
 * @brief       Accept from new socket.
 * @param[in] new_sock Listening socket to be accepted.
 *
 * This function closes connection management module.
 */
int conn_accept(int new_sock);

/**
 * @brief       Set file descriptors to be observed.
 * @param[in,out] fds Pointer to file descriptor set for select.
 */
int conn_fd_set(fd_set *fds);

/**
 * @brief       Chcek file descriptors and process connections.
 * @param[in] fds Pointer to file descriptor set.
 */
int conn_fd_process(fd_set *fds);

#endif  /* #ifndef __CONN_H_ */
