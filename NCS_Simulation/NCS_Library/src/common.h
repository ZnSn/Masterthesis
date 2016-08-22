/*
 * common.h
 *
 *  Created on: Apr 18, 2016
 *      Author: stephan
 */

#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

#include <cmath>

#define ARMA_USE_CXX11

//#define DEBUG
//#define DEBUG_EXTENDED

// Make sure Debug is defined if debug extended is defined
#ifdef DEBUG_EXTENDED
#define DEBUG
#endif

// SYSTEM CONSTANTS
#define sys_l 	0.3
#define sys_d 	sys_l/2
#define sys_K 	sys_d/(pow(sys_d, 2)+sys_l/3)
#define sys_g 	9.8082

#define ZERO 0
#define MATRIX_SIZE 4
#define MEASSUREMENT_MATRIX_SIZE 3
#define INPUT_COST 1
#define SINGLE 1

#endif /* SRC_COMMON_H_ */
