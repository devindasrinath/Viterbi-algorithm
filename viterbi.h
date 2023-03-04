/**
 * @file viterbi.h
 * @brief Function protytpes and macros which used in viterbi algorithm is included.
 * @author Devinda srinath
 * @bug No known bugs.
 *
 */
#ifndef VITERBI_H
#define VITERBI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include "constants.h"
#include "common_functions.h"
#include "functional_channels.h"


/** \addtogroup viterbi_constants
 * constraints for decode data
 *  @{
 */
#define CONSTRAIN_LENGTH 5
#define NUM_STATES 16         /**< 2^(k-1) , k = constraint length*/
#define MAX_NUM_BITS 203
#define NUM_OUTPUTS 2
#define NUM_OUTPUT_PATTERNS 4 /**< 2^NUM_OUTPUTS*/
#define NUM_INPUTS 1
#define NUM_CONVERSTIONS 2 /**<2^NUM_INPUTS*/
/** @} */

void viterbi_decode (uint32_t p_data[], uint16_t *p_num_data_bits, puncture_matrix *p_mat);//decode given data

void update_state (); //update state diagrom

void min_array (uint8_t arr[], uint8_t arr_len, uint8_t results[]); //caculate minimum of given array

void err_mat_gen (); //generate error matrix for state diagrom

void pre_state_mat_gen (); //generate matrix that include previous states

#endif // VITERBI_H
