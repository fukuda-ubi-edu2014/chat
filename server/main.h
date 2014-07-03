/* 
 * Copyright (c) 2014 Fukuda Laboratory and Shigemi ISHIDA, Kyushu University
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

/**
 * @file
 *      Header file for main module.
 * @author
 *      Shigemi Ishida <ishida+devel@f.ait.kyushu-u.ac.jp>
 */
#ifndef __MAIN_H_
#define __MAIN_H_

/*======================================================================
 * includes
 *======================================================================*/
#include <signal.h>

/*======================================================================
 * constants
 *======================================================================*/
/**
 * @enum
 *      status flags.
 */
enum status_flags
{
    STAT_INIT   = 0x00,         /**< initializing */
    STAT_WORK   = 0x01,         /**< working */
    STAT_ERR    = 0x40,         /**< error */
    STAT_FIN    = 0x80,         /**< closing */
};

/*======================================================================
 * typedefs, structures
 *======================================================================*/
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

/**
 * @struct
 *      operation parameters.
 */
typedef struct opr_strct {
    struct sigaction sa;        /**< @brief Signal handler */

    unsigned int trace_level;   /**< tracer output level */
    char port[128];             /**< listen port name */
} opr_t;

#endif  /* #ifndef __MAIN_H_ */
