/* 
 * Copyright (c) 2014 Fukuda Laboratory and Shigemi ISHIDA, Kyushu University
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

/**
 * @file
 *      Header file for TCP listening module.
 * @author
 *      Shigemi Ishida <ishida+devel@f.ait.kyushu-u.ac.jp>
 */
#ifndef __LISN_H_
#define __LISN_H_

/*======================================================================
 * includes
 *======================================================================*/
#include <sys/select.h>
#include "main.h"

/*======================================================================
 * constants, macros
 *======================================================================*/
/**
 * @def LISN_MAX_SOCK
 * @brief Max number of listening sockets.
 */
#define LISN_MAX_SOCK   4

/*======================================================================
 * typedefs, structures
 *======================================================================*/

/*======================================================================
 * prototype declarations
 *======================================================================*/

/**
 * @brief      Listening module init.
 * @param[in,out] opr Pointer to the operation parameters.
 * @return      Returns 0 on success.
 *              Returns minus value on any error.
 *
 * This function initializes listening module.
 */
int lisn_init(opr_t *opr);

/**
 * @brief       Listening de-init.
 * @param[in,out] opr Pointer to the operation parameters.
 *
 * This function closes listening module.
 */
void lisn_deinit(opr_t *opr);

/**
 * @brief       Start listening.
 * @param[in] opr Pointer to the operation parameters.
 */
int lisn_start_listen(opr_t *opr);

/**
 * @brief       Set file descriptors to be observed.
 * @param[in,out] fds Pointer to file descriptor set for select.
 */
int lisn_fd_set(fd_set *fds);

/**
 * @brief       Chcek file descriptors and process connections.
 * @param[in] fds Pointer to file descriptor set.
 */
int lisn_fd_process(fd_set *fds);

#endif  /* #ifndef __LISN_H_ */
