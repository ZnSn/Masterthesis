/*
 * ncs_simulation.cpp
 *
 *  Created on: Apr 18, 2016
 *      Author: stephan
 */

#include "system.h"
#include "discrete_system.h"
#include "controller.h"
#include "ncs_simulation.h"

#define _USE_MATH_DEFINES
#include <cmath>

void Simulation::preset_pd() {
	// Presetting priority matrix pd
	this->Pd  << -275.879245988434	 << 11.492622567332	 << 5.7501162958188	 << 1.64553399416964	 << 275.87751006145	 << -11.4939636222383	 << -5.72118755921982	 << -1.64288071739782	 << arma::endr
 << 11.492622567332	 << -252.126851671982	 << 11.766740682645	 << 5.1785003961474	 << -11.4948581327148	 << 252.126556054637	 << -11.7299740762987	 << -5.17878514817875	 << arma::endr
 << 5.7501162958188	 << 11.766740682645	 << -695.810963131488	 << -200.759492016221	 << -5.75035225545049	 << -11.7670850331625	 << 695.81427993655	 << 200.760203620277	 << arma::endr
 << 1.64553399416964	 << 5.1785003961474	 << -200.759492016221	 << -57.9010890878864	 << -1.64554229348757	 << -5.17854864651926	 << 200.759445107862	 << 57.9010923166349	 << arma::endr
 << 275.87751006145	 << -11.4948581327148	 << -5.75035225545049	 << -1.64554229348757	 << -275.875771052019	 << 11.4962003606708	 << 5.72140953904865	 << 1.64288516972879	 << arma::endr
 << -11.4939636222383	 << 252.126556054637	 << -11.7670850331625	 << -5.17854864651926	 << 11.4962003606708	 << -252.126252391606	 << 11.7302988745135	 << 5.17880921334793	 << arma::endr
 << -5.72118755921982	 << -11.7299740762987	 << 695.81427993655	 << 200.759445107862	 << 5.72140953904865	 << 11.7302988745135	 << -695.81735018174	 << -200.760087909938	 << arma::endr
 << -1.64288071739782	 << -5.17878514817875	 << 200.760203620277	 << 57.9010923166349	 << 1.64288516972879	 << 5.17880921334793	 << -200.760087909938	 << -57.9010044714571	 << arma::endr;


 
}

Simulation::Simulation(int sim_id, double sample_time, double arrival_rate){

this->sys = new System();

this->ds = sys->discretize(sample_time);

this->cont = new Controller(ds);
this->cont->calculate(arrival_rate);

this->x_dim = ds->A.n_cols;
this->y_dim = sys->C.n_rows;
this->u_dim = ds->B.n_cols;

// Initialize internals
this->sim_id = sim_id;
this->x = arma::zeros(x_dim);
this->x_hat = arma::zeros(x_dim);
this->u = arma::zeros(u_dim);
this->P = sys->P0;
this->y = arma::zeros(y_dim);

this->J_abs = x.t() * ds->Q * x;

preset_pd();

#ifdef DEBUG_EXTENDED
std::cout << std::endl << " Starting Simulation with the following system matrices:" << std::endl
	<< "  A: " << std::endl << ds->A << std::endl
	<< "  B: " << std::endl << ds->B << std::endl
	<< "  Q: " << std::endl << ds->Q << std::endl
	<< "  H: " << std::endl << ds->H << std::endl
	<< "  W: " << std::endl << ds->W << std::endl
	<< "  R: " << std::endl << ds->R << std::endl
		<< "  Ts: " << std::endl << ds->Ts << std::endl
		<< "  P0: " << std::endl << sys->P0 << std::endl
			<< "  V: " << std::endl << sys->V << std::endl
			<< "  C: " << std::endl << sys->C << std::endl;
	#endif
}

void Simulation::preset_x(double* values, int num) {

	if (num > this->x.n_elem)
		return;

	for (int i = 0; i < num; i++) {
		this->x[i] = values[i];
	}
	
	std::cout << std::endl << "SIMULATION (" << this->sim_id << "): Changed x to:" << this->x << std::endl;

}

double* Simulation::calculate_v() {

	arma::mat x_x_hat = arma::join_vert(this->x, this->x_hat);
	
	arma::mat v = x_x_hat.t() * this->Pd * x_x_hat;
	
	return v.memptr();
}

double* Simulation::calculate_y() {

	arma::vec v = arma::chol(sys->V) * arma::randn(this->sys->V.n_cols);
	y = sys->C * this->x + v;

	#ifdef DEBUG_EXTENDED
	std::cout 	<< "SIMULATION (" << this->sim_id << "): Current x " << std::endl << this->x << std::endl
				<< "SIMULATION (" << this->sim_id << "): Current v (random)" << std::endl << v << std::endl;
	#endif

	#ifdef DEBUG
	std::cout << "SIMULATION (" << this->sim_id << "): Calculated y ( C * x * v )" << std::endl << y << std::endl;
	#endif

	return y.memptr();
}

int Simulation::calculate_u(int gamma, double* y) {

	#ifdef DEBUG_EXTENDED
	std::cout << "SIMULATION (" << this->sim_id << "): Calculation matrices for P:" << std::endl
				<< "  A:" << std::endl << ds->A << std::endl
				<< "  P:" << std::endl << P << std::endl
				<< "  W:" << std::endl << ds->W << std::endl << std::endl;
	#endif

	P = ds->A * P * ds->A.t() + ds->W;

	#ifdef DEBUG_EXTENDED
	std::cout << "SIMULATION (" << this->sim_id << "): Calculated P ( A * P * A' + W )" << std::endl << P << std::endl;
	#endif

	arma::vec y_arma = arma::vec(y, y_dim);

	arma::mat left = P * sys->C.t();

	arma::mat right = sys->C * P * sys->C.t() + sys->V;

	#ifdef DEBUG_EXTENDED
	std::cout << "SIMULATION (" << this->sim_id << "): Calculation matrices for K:" << std::endl
			<< "  P * C':" << std::endl <<  left << std::endl
			<< "  C * P * C' + V:" << std::endl <<  right << std::endl << std::endl;
	#endif

	//arma::mat K_t = arma::solve(right.t(), left.t());

	arma::mat K = left * arma::inv(right);

	#ifdef DEBUG_EXTENDED
	std::cout << "SIMULATION (" << this->sim_id << "): Calculated K ( (P * C') / (C * P * C' + V) )" << std::endl << K << std::endl;
	#endif

	#ifdef DEBUG_EXTENDED
	std::cout << "SIMULATION (" << this->sim_id << "): Calculation matrices for x_hat:" << std::endl
				<< "  x_hat:" << std::endl <<  x_hat << std::endl
				<< "  K:" << std::endl << K << std::endl
				<< "  Y:" << std::endl <<  y_arma << std::endl << std::endl;
	#endif

	//x_hat = x_hat + gamma * K_t.t() * (y_arma - sys->C * x_hat);
	x_hat = x_hat + gamma * K * (y_arma - sys->C * x_hat);

	#ifdef DEBUG_EXTENDED
	std::cout << "SIMULATION (" << this->sim_id << "): Calculated x_hat ( x_hat + gamma * K * (y - C * x_hat) )" << std::endl << x_hat << std::endl;
	#endif

	//P = P - gamma * K_t.t() * sys->C * P;
	P = P - gamma * K * sys->C * P;

	#ifdef DEBUG_EXTENDED
	std::cout << "SIMULATION (" << this->sim_id << "): Calculated P ( P - gamma * K * C * P )" << std::endl << P << std::endl;
	#endif

	if (arma::norm(P) > 1e6 ){
		std::cout << "SIMULATION (" << this->sim_id << "): Calculate u -> Norm P greater than 1e6! ERROR!!!" << std::endl;
		return 0;
	}

	x_hat = ds->A * x_hat + ds->B * u;

	#ifdef DEBUG_EXTENDED
	std::cout << "SIMULATION (" << this->sim_id << "): Calculated x_hat ( A * x_hat + B * u )" << std::endl << x_hat << std::endl;
	#endif

	u = -cont->G * x_hat;

	#ifdef DEBUG
	std::cout << "SIMULATION (" << this->sim_id << "): Calculated u ( G * x_hat )" << std::endl << u << std::endl;
	#endif

	return 1;
}

int Simulation::calculate_x(double** val_j) {

	arma::vec w = arma::chol(ds->W) * arma::randn(this->ds->W.n_cols); // PRECALC CHOL(W) !!!
	//x = ds->A * x + ds->B * u + w;

	#ifdef DEBUG_EXTENDED
	std:: cout << "SIMULATION (" << this->sim_id << "): Calculation for x:" << std::endl
			<< "  x: " << std::endl << x << std::endl
			<< "  u: " << std::endl << u << std::endl
			<< "  w (random): " << std::endl << w << std::endl << std::endl;
	#endif

	x = ds->A * x + ds->B * u + w;

	#ifdef DEBUG
	std::cout << "SIMULATION (" << this->sim_id << "): Calculated x ( A * x + B * u + w )" << std::endl << x << std::endl;
	#endif
	
	if (fabs(x[2]) > M_PI/2) {
		std::cout << "SIMULATION (" << this->sim_id << "): ERROR - Angle over 90 degree (" << x[2] << ")" << std::endl;
		return 0;
	}


	J_abs = x.t() * ds->Q * x + 2 * x.t() * ds->H * u + u.t() * ds->R * u;
	
	#ifdef DEBUG_EXTENDED
	std::cout << "SIMULATION (" << this->sim_id << "): Performance Parameter J_abs: " << J_abs << std::endl;
	#endif

	*val_j = J_abs.memptr();

	
	return 1;
}


//////////////////////////////
// INTERFACE ACCESS METHODS //
//////////////////////////////


EXPORT_C Simulation* simulation_new(int sim_id, double sample_time, double arrival_rate)
{
    return new Simulation(sim_id, sample_time, arrival_rate);
}

EXPORT_C void simulation_delete(Simulation* instance)
{
    delete instance;
}

EXPORT_C void preset_pendulum(Simulation* instance, double* values, int num) {
	
	instance->preset_x(values, num);
}

EXPORT_C void simulation_calculate_y(Simulation* instance, double** y) {

	double* tmp = instance->calculate_y();
	memcpy(*y, tmp, sizeof(double) * 3);
}

EXPORT_C int simulation_calculate_u(Simulation* instance, int gamma, double* y) {

	return instance->calculate_u(gamma, y);
}

EXPORT_C int simulation_calculate_x(Simulation* instance, double** val_j) {

	return instance->calculate_x(val_j);
}

EXPORT_C void simulation_calculate_v(Simulation* instance, double** v) {

	double* tmp = instance->calculate_v();
	memcpy(*v, tmp, sizeof(double));
}
