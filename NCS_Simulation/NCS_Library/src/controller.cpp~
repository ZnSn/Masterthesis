/*
 * controller.cpp
 *
 *  Created on: Apr 21, 2016
 *      Author: stephan
 */

#include "controller.h"
#include "common.h"
#include <armadillo>

#include <iostream>

Controller::Controller(Discrete_System* d_sys) {

	this->d_sys = d_sys;

	myeps = 1e-5;
	maxnorm = 1e10;

	Je = 0;
	G = arma::zeros<arma::vec>(4);
}

Controller::~Controller() {
}


bool Controller::calculate(double p){

	#ifdef DEBUG
	std::cout << "SYSTEM CONTROLLER: Calculating control parameters with arrival rate of " << p << std::endl;
	#endif

	arma::mat K(4, 4, arma::fill::zeros);
	arma::mat M(4, 4, arma::fill::zeros);

	arma::mat K_prev = K;
	arma::mat M_prev = M;

	bool continue_loop = true;
	while(continue_loop) {

		K_prev = K;
		M_prev = M;

		// G = (R + B' * K * B) \ (B' * K * A + H');
		// M = (1-p)*(A' * M * A) + (A' * K * B + H) * G;
		// K = A' * K * A + Q - (A' * K * B + H) * G;

		G = arma::solve(d_sys->B.t() * K * d_sys->B + d_sys->R,  d_sys->B.t() * K * d_sys->A + d_sys->H.t());

		M = (1-p)*(d_sys->A.t() * M * d_sys->A) + (d_sys->A.t() * K * d_sys->B + d_sys->H) * G;

		K = d_sys->A.t() * K * d_sys->A + d_sys->Q - (d_sys->A.t() * K * d_sys->B + d_sys->H) * G;

		if ((arma::norm(K_prev-K) < myeps) && (arma::norm(M_prev-M) < myeps)) {
			#ifdef DEBUG
			std::cout << "SYSTEM CONTROLLER: Final Matrix G: " << std::endl << G << std::endl;
			#endif
			Je = arma::trace((K+M)*d_sys->W)/d_sys->Ts;
			return true;
		}

		if ((arma::norm(K) >= maxnorm) || (arma::norm(M) >= maxnorm)) {
			std::cout << "WARNING (SYSTEM CONTROLLER): iteration diverges!! Returning..." << std::endl << std::endl;
			continue_loop = false;
		}
	}

	return false;
}
