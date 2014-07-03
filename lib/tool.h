/* 
 * Copyright (c) 2014 Fukuda Laboratory and Shigemi ISHIDA, Kyushu University
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

/**
 * @file
 *      Header file for tools.
 * @author
 *      Shigemi Ishida <ishida+devel@f.ait.kyushu-u.ac.jp>
 */
#ifndef __TOOL_H
#define __TOOL_H

/*======================================================================
 * includes
 *======================================================================*/
#include <sys/time.h>

/*======================================================================
 * constants, macros
 *======================================================================*/

/*======================================================================
 * typedefs, structures
 *======================================================================*/

/*======================================================================
 * prototype declarations
 *======================================================================*/

/**
 * @brief       Validate decimal number string.
 * @param[in] num_str String to validate.
 * @return      Returns 1 when num_str is decimal number string.
 *              Returns 0 when num_str includes non-decimal character.
 */
int is_number(char *num_str);

/**
 * @brief       Time subtraction for struct timeval.
 * @param[in,out] x Destination operand.
 * @param[in] y Source operand.
 * @return      Returns 0 on success, minus value on any error.
 *
 * This function subtracts y from x, and store result to x.
 */
int timeval_sub(struct timeval *x, struct timeval *y);

/**
 * @brief       Start timeout timer.
 * @param[out] start Start time.
 *
 * This function retrieves current time and store to start.
 */
void timeout_start(struct timeval *start);

/**
 * @brief       Timeout check.
 * @param[in] start Start time.
 * @param[in] timeout Timeout in ms.
 * @return      Returns 1 when not timed out, 0 when timed out.
 *              Returns minus value on any error.
 *
 * This function compares time passed and timeout, and check
 * if time out occurs.
 */
int timeout_chk(struct timeval *start, unsigned int timeout);

#endif  /* #ifndef __TOOL_H */
