/*
 * discrete_system.h
 *
 *  Created on: Apr 18, 2016
 *      Author: stephan
 */

#ifndef SIMULATION_SRC_DISCRETE_SYSTEM_H_
#define SIMULATION_SRC_DISCRETE_SYSTEM_H_

#include <armadillo>

class Discrete_System {
public:

	arma::mat A; 		// 4x4
	arma::mat W;		// 4x4
	arma::mat Q;		// 4x4

	arma::vec B;		// 4x1
	arma::vec H;		// 4x1

	arma::mat R;		// 1x1
	double Ts;

	Discrete_System(arma::mat A, arma::mat W, arma::mat Q, arma::vec B, arma::vec H, arma::mat R, double Ts);
	virtual ~Discrete_System();
};

#endif /* SIMULATION_SRC_DISCRETE_SYSTEM_H_ */
