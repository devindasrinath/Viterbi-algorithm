/**
 * @file viterbi.c
 * @brief Neccessary functions for implement and use viterbi algortihm is included.
 *
 * The following example show how to use Viterbi algorithm for decode convoluted data.
 * Let ,
 *		+ p_msg : message to decode (uint 32 bit format)
 *		+ p_num_data_bits : pointer to number of bits in message before decode
 *		+ p_puncture_mat : pointer to puncture matrix of the given functional channel
 *@code
 *		viterbi_decode(p_msg, p_num_data_bits, p_puncture_mat)
 *@endcode
 *
 * @author Devinda srinath
 * @bug No known bugs.
 * @attension Do not use other functions in external files.
 * @warning Do not change any content of this file.
 *
 */


#include "viterbi.h"

uint8_t current_states[NUM_STATES] 													  = { };  /**<current state of the states*/
uint8_t temp_states[NUM_STATES]   													  = { };  /**<temperory store for comparing*/
uint8_t pre_states[NUM_STATES] 														  = { };  /**<last previous states*/
uint8_t pre_state_matrix[NUM_STATES][NUM_CONVERSTIONS]								  = { };  /**<all prvious states */

uint8_t look_up_error[NUM_STATES][NUM_OUTPUTS][NUM_OUTPUT_PATTERNS][NUM_CONVERSTIONS] = { };  /**<error lookup table*/
uint8_t error[NUM_STATES] 															  = { };  /**<current error matrix*/
uint8_t temp_err[NUM_STATES] 														  = { };  /**<temperory error matrix*/

uint8_t states[MAX_NUM_BITS][NUM_STATES]											  = { };  /**<All states from the beggining*/
uint8_t inputs[MAX_NUM_BITS][NUM_STATES]											  = { };  /**<inputs of the berginning */
uint8_t trace_back[MAX_NUM_BITS]  											          = { };  /**<traceback(shortest path stored here) */



/*!
 * @brief decode the data where convolutionally encoded.
 * @param p_data : Pointer to data array.
 * @param p_num_data_bits : Pointer to how many bits related to data in the array.
 * @param p_mat :pointer to puncture matrix
 * @return None
 */
void viterbi_decode (uint32_t p_data[], uint16_t *p_num_data_bits,
	puncture_matrix *p_mat)
{

	pre_state_mat_gen(); //lookup-table generate for given k

	err_mat_gen();

	current_states[0] = temp_states[0] = pre_states[0] = 1;

	/*calculate forward paths such that occuring least error.*/
	for (uint8_t bit_index = 0; bit_index < *p_num_data_bits / 2; bit_index++)
	{

		uint8_t bit_couple = (uint8_t) ((p_data[(bit_index * 2) / WORD_SIZE]
				>> (30 - (2 * bit_index) % WORD_SIZE)) & 0x00000003);

		update_state();

		for (uint8_t i = 0; i < NUM_STATES; i++)
		{
			if (current_states[i] > 0)
			{
				uint8_t min_err = 255; //any random suggestion

				for (uint8_t j = 0; j < NUM_CONVERSTIONS; j++)
				{
					uint8_t bit_1 = look_up_error[i][j][bit_couple][0];
					uint8_t bit_2 = look_up_error[i][j][bit_couple][1];

					if (pre_states[pre_state_matrix[i][j]] == 0)
					{
						continue;
					}

					for (uint8_t erase_index = 0;
							erase_index < p_mat->num_erased; erase_index++)
					{

						if (p_mat->erased_indexes[erase_index] == bit_index * 2)
						{
							bit_1 = 0;
						}
						if (p_mat->erased_indexes[erase_index]
								== (bit_index * 2) + 1)
						{
							bit_2 = 0;
						}
						if (p_mat->erased_indexes[erase_index]
								> (bit_index * 2) + 1)
						{
							break;
						}
					}

					uint8_t curr_err = bit_1 + bit_2
							+ error[pre_state_matrix[i][j]];


					if (curr_err <= min_err)
					{
						min_err = curr_err;
						states[bit_index + 1][i] = pre_state_matrix[i][j];
						inputs[bit_index + 1][i] = i >> (CONSTRAIN_LENGTH - 2);
						temp_err[i] = curr_err;

					}
				}
			}
			else
			{
				states[bit_index + 1][i] = 0;
			}
		}
		for (uint8_t i = 0; i < NUM_STATES; i++)
		{
			error[i] = temp_err[i];
		}
	}

	/************backtrace***********/

//get minimum error state of last state states
	uint8_t min_err_state[2];
	min_array(error, NUM_STATES, min_err_state);

	uint8_t pre_state = states[*p_num_data_bits / 2][min_err_state[0]];
	trace_back[(*p_num_data_bits / 2) - 1] =
			inputs[*p_num_data_bits / 2][min_err_state[0]];
	for (uint8_t i = (*p_num_data_bits / 2) - 1; i > 0; i--)
	{
		trace_back[i - 1] = inputs[i][pre_state];
		pre_state = states[i][pre_state];
	}

	clr_array(p_data, 10);
	for (uint8_t i = 0; i < *p_num_data_bits / 2; i++)
	{

		p_data[i / WORD_SIZE] |= ((uint32_t) (trace_back[i] & 0X01))
				<< (WORD_SIZE - 1 - (i % 32));
	}

	(*p_num_data_bits) = (*p_num_data_bits) / 2;

}/*viterbi_decode()*/


/*!
 * @brief updata state of trellis diagrom.
 * @return None
 */
void update_state (void)
{

	for (uint8_t i = 0; i < NUM_STATES; i++)
	{
		temp_states[i] = current_states[pre_state_matrix[i][0]]
				| current_states[pre_state_matrix[i][1]];
	}
	for (uint8_t i = 0; i < NUM_STATES; i++)
	{
		pre_states[i] = current_states[i];
		current_states[i] = temp_states[i];
	}
}/*update_state()*/


/*!
 * @brief calculate minimum of the array.
 * @param arr : pointer to the array.
 * @param arr_len : length of the array.
 * @param results : pointer to store the results.
 * @return None
 */
void min_array (uint8_t arr[], uint8_t arr_len, uint8_t results[])
{
	results[0] = 0;
	results[1] = arr[0];
	for (uint8_t index = 1; index < arr_len; index++)
	{
		if (results[1] >= arr[index])
		{
			results[0] = index;
			results[1] = arr[index];
		}
	}
} /*min_array()*/

/*!
 * @brief generate pre state matrix for trellis diagrom.
 * @return None
 */
void pre_state_mat_gen ()
{
	for (uint8_t i = 0; i < NUM_STATES; i++)
	{
		pre_state_matrix[i][0] = (i & ~(0XFF << (CONSTRAIN_LENGTH - 2))) << 1;
		pre_state_matrix[i][1] = pre_state_matrix[i][0] + 1;
	}
}/*pre_state_mat_gen()*/


/*!
 * @brief generate error matrix matrix for trellis diagrom.
 * @return None
 */
void err_mat_gen (void)
{
	uint8_t g1 = 0b10011; // 1+ D3 + D4(MSB mapping to 1)
	uint8_t g2 = 0b11101; // 1+ D1+ D2 +D4
	for (uint16_t state = 0; state < NUM_STATES; state++)
	{
		uint16_t pre_val[2];
		pre_val[0] = state << 1;
		pre_val[1] = (state << 1) + 0X0001;

		for (uint8_t col = 0; col < NUM_CONVERSTIONS; col++)
		{
			uint16_t val_1 = pre_val[col] & g1;
			uint16_t val_2 = pre_val[col] & g2;
			uint8_t sum_1 = 0;
			uint8_t sum_2 = 0;
			for (uint8_t bit = 0; bit <= (CONSTRAIN_LENGTH - 1); bit++)
			{
				sum_1 ^= ((val_1 >> bit) & 0X0001);
				sum_2 ^= ((val_2 >> bit) & 0X0001);
			}

			for (uint8_t comparison = 0; comparison < pow(2, NUM_OUTPUTS);
					comparison++)
			{
				look_up_error[state][col][comparison][1] = sum_2
						^ (comparison & 0x01);
				look_up_error[state][col][comparison][0] = sum_1
						^ ((comparison >> 1) & 0x01);
			}
		}
	}
}/*err_mat_gen()*/

