/*
 * discrete_system.cpp
 *
 *  Created on: Apr 18, 2016
 *      Author: stephan
 */

#include "discrete_system.h"

Discrete_System::Discrete_System(arma::mat A, arma::mat W, arma::mat Q, arma::vec B, arma::vec H, arma::mat R, double Ts) {

	this->A = A;
	this->W = W;
	this->Q = Q;
	this->B = B;
	this->H = H;
	this->R = R;
	this->Ts = Ts;
}

Discrete_System::~Discrete_System() {
}

