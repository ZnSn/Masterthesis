/*
 * controller.h
 *
 *  Created on: Apr 21, 2016
 *      Author: stephan
 */

#ifndef SIMULATION_SRC_CONTROLLER_H_
#define SIMULATION_SRC_CONTROLLER_H_

#include <armadillo>
#include "discrete_system.h"

class Controller {
public:

	arma::mat G;

	double Je;
	double myeps;
	double maxnorm;

	Discrete_System* d_sys;

	Controller(Discrete_System* d_sys);
	virtual ~Controller();

	bool calculate(double p);
};

#endif /* SIMULATION_SRC_CONTROLLER_H_ */
