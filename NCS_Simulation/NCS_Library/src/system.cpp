/*
 * System.cpp
 *
 *  Created on: Apr 18, 2016
 *      Author: stephan
 */

#include "system.h"
#include "common.h"
#include <cmath>

System::System() {

	Ac = { 	{0, 1, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 1},
			{0, 0, sys_K*sys_g, 0} };

	Bc = {0, 1, 0, sys_K};

	//arma::vec tmp = { 0.1, 0.1, 0.1, 0.1 };
	arma::vec tmp = { 1e-9, 1e-9, 1e-6, 1e-3 };
	Wc = arma::diagmat(tmp);

	P0 = arma::diagmat(tmp);

	arma::vec tmp2 = { 1e-9, 1e-9, 1e-3 };
	V = arma::diagmat(tmp2);

	C = {	{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0}};

	arma::vec tmp3 = {1, 0, 2, 0};
	Qc = arma::diagmat(tmp3);

	Rc << INPUT_COST << arma::endr;
}

System::~System() {
}

Discrete_System* System::discretize(double sampling_time) {

	#ifdef DEBUG
	std::cout << "SYSTEM: Discretizing System with sampling time of " << sampling_time << " seconds" << std::endl;
	#endif

	int n = Ac.n_rows;
	int m = Bc.n_cols;

	// Calculate Discrete Matrices A and B

	arma::mat M = arma::join_vert(arma::join_horiz(Ac, Bc), arma::zeros(m, n+m));
	arma::mat A_B_Phi = arma::expmat(M * sampling_time);
	arma::mat A = A_B_Phi(arma::span(0,n-1), arma::span(0,n-1));
	arma::mat B = A_B_Phi(arma::span(0,n-1), arma::span(n, n+m-1));

	// Calculate Discrete Matrix W

	M = arma::join_vert(arma::join_horiz(-Ac, Wc), arma::join_horiz(arma::zeros(n, n), Ac.t()));
	arma::mat W_Phi = arma::expmat(M * sampling_time);
	arma::mat W_Phi12 = W_Phi(arma::span(0,n-1), arma::span(n, 2*n-1));
	arma::mat W_Phi22 = W_Phi(arma::span(n,2*n-1), arma::span(n, 2*n-1));

	arma::mat W_2 = W_Phi22.t() * W_Phi12;
	arma::mat W = (W_2.t() + W_2)/2;

	// Calculate Discrete Matrices Q, H and R

	M = arma::join_vert(
						arma::join_vert(
							arma::join_horiz(
								arma::join_horiz(-Ac.t(),arma::zeros(n,m)),
								arma::join_horiz(Qc, arma::zeros(n,m))),
							arma::join_horiz(
								arma::join_horiz(-Bc.t(),arma::zeros(m,m)),
								arma::join_horiz(arma::zeros(n,m).t(), Rc))),
						arma::join_vert(
							arma::join_horiz(
								arma::join_horiz(arma::zeros(n,n), arma::zeros(n,m)),
								arma::join_horiz(Ac, Bc)),
							arma::join_horiz(
								arma::join_horiz(arma::zeros(m,n), arma::zeros(m,m)),
								arma::join_horiz(arma::zeros(m,n), arma::zeros(m,m))))
						);


	arma::mat Q_H_R_Phi = arma::expmat(M * sampling_time);
	int N = n+m;

	arma::mat Q_H_R_Phi12 = Q_H_R_Phi(arma::span(0,N-1), arma::span(N,2*N-1));
	arma::mat Q_H_R_Phi22 = Q_H_R_Phi(arma::span(N, 2*N-1), arma::span(N, 2*N-1));

	arma::mat QQ = Q_H_R_Phi22.t() * Q_H_R_Phi12;
	arma::mat QQ_2 = (QQ.t() + QQ)/2;

	arma::mat Q = QQ_2(arma::span(0, n-1), arma::span(0,n-1));
	arma::mat H = QQ_2(arma::span(0, n-1), arma::span(n,N-1));
	arma::mat R = QQ_2(arma::span(n,N-1), arma::span(n,N-1));

	return new Discrete_System(A, W, Q, B, H, R, sampling_time);
}

