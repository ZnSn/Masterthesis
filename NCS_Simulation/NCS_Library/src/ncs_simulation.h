/*
 * ncs_simulation.h
 *
 *  Created on: Apr 18, 2016
 *      Author: stephan
 */

#ifndef SRC_NCS_SIMULATION_H_
#define SRC_NCS_SIMULATION_H_


#ifdef __cplusplus // only actually define the class if this is C++

#include <iostream>
#include <fstream>
#include <armadillo>
#include <string>


class Simulation
{
    public:
        Simulation(int sim_id, double sample_time, double arrival_rate);

        // Internal state parameters
        arma::vec x;
        arma::vec x_hat;
        arma::vec u;
        arma::mat P;
        arma::vec y;
		arma::mat J_abs;
		arma::mat Pd;

        int x_dim;
        int y_dim;
        int u_dim;
        int sim_id;
        
        std::ofstream file_stream;
		std::string filename; 

        // Interface functions
        double* calculate_y();
		double* calculate_v();
        int calculate_u(int gamma, double* y);
        int calculate_x(double** val_j);
        void preset_x(double*, int);

    private:
        System* sys;
	Discrete_System* ds;
	Controller* cont;

	void preset_pd();

};

#else

// C doesn't know about classes, just say it's a struct
typedef enum { false, true } bool;
typedef struct Simulation Simulation;

#endif

// access functions
#ifdef __cplusplus
    #define EXPORT_C extern "C"
#else
    #define EXPORT_C
#endif

// INTERFACE FUNCTIONS

EXPORT_C Simulation* simulation_new(int sim_id, double sample_time, double arrival_rate);
EXPORT_C void simulation_delete(Simulation*);

EXPORT_C void preset_pendulum(Simulation*, double* values, int num);

EXPORT_C void simulation_calculate_y(Simulation*, double**);
EXPORT_C int simulation_calculate_u(Simulation* instance, int gamma, double* y);
EXPORT_C int simulation_calculate_x(Simulation* instance, double**);
EXPORT_C void simulation_calculate_v(Simulation* instance, double**);

#endif /* SRC_NCS_SIMULATION_H_ */
