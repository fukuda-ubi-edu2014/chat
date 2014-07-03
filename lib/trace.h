/* 
 * Copyright (c) 2014 Fukuda Laboratory and Shigemi ISHIDA, Kyushu University
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

/**
 * @file
 *      Header file for the tracer.
 * @author
 *      Shigemi Ishida <ishida+devel@f.ait.kyushu-u.ac.jp>
 *
 * This file provides tracer functions with distinguishable sequence number.
 * See inside the file to get examples of the sequence number usage.
 */
#ifndef __TRACE_H
#define __TRACE_H

/*======================================================================
 * Trace seq usage:
 *
 *   Each x exhibits the bit.
 *     xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx
 *     |||     | |       | |                 |
 *     ||+-----+ +-------+ +-----------------+
 *     || file     func           seq
 *     |+-private function flag
 *     +--error flag
 *
 *  (ex)
 *    global function:          0x01000100
 *    global funnction error:   0x81000200
 *    private function:         0x41000300
 *    private function  error:  0xc1000400
 *======================================================================*/

/*======================================================================
 * constants, macros
 *======================================================================*/
/**
 * @def T_MSG_MAX
 * @brief Max length of trace message.
 */
#define T_MSG_MAX     2048    /* max length of trace message */

/**
 * @enum T_LVL
 * @brief Trace levels.  The upper is the higher.
 */
enum T_LVL                    /* trace message levels */
{
    T_E         = 0,
    T_W         = 1,
    T_I         = 2,
    T_D1        = 3,
    T_D2        = 4,
};

/*======================================================================
 * prototype declarations
 *======================================================================*/
/**
 * @brief       Trace message.
 * @param[in] trace_lvl Output trace level.
 * @param[in] seq Trace sequence number.
 * @param[in] format Message print format like printf.
 *
 * This function prints trace message to stderr.
 * If the tracer is initialized to one trace level, the tracer will not
 * show messages with trace level less than the initialized level.
 */
void T_M(const int trace_lvl, const unsigned int seq, const char *format, ...);

/**
 * @brief       Trace variable dump.
 * @param[in] trace_lvl Output trace level.
 * @param[in] seq Trace sequence number.
 * @param[in] target Pointer to the target variable to dump.
 * @param[in] length Length to dump (in octet).
 *
 * This function dump variable in hexadecimal format.
 * If the tracer is initialized to one trace level, the tracer will not
 * show dumps with trace level less than the initialized level.
 */
void T_D(const int trace_lvl, const unsigned int seq, const void *target, const int length);

/**
 * @brief       Tracer initialization.
 * @param[in] trace_lvl Output trace level.
 *
 * This function initializes tracer to the specified level.
 */
int T_init(int trace_lvl);

#endif  /* #ifndef __TRACE_H */
