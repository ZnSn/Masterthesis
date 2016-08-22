/*
 * System.h
 *
 *  Created on: Apr 18, 2016
 *      Author: stephan
 */

#ifndef SRC_SYSTEM_H_
#define SRC_SYSTEM_H_

#include "common.h"
#include "discrete_system.h"
#include <armadillo>

class System {
public:

	arma::mat Ac; // state matrix 4x4
	arma::vec Bc; // Input matrix 4x1
	arma::mat Wc; // process noise covariance 4x4
	arma::mat P0; // initial state covariance 4x4
	arma::mat C; // measurement matrix (x1, x2, theta1) 3x4
	arma::mat V; // measurement noise 3x3

	// LQR Weights
	arma::mat Qc; // relative cost of state deviation 4x4
	arma::mat Rc; // relative cost of input 1x1

	System();
	virtual ~System();

	Discrete_System* discretize(double sampling_time);

};

#endif /* SRC_SYSTEM_H_ */
