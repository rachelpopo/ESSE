/*
 *  Serial ESSE Execution
 *  Created by Rachel Popo on 11/25/18.
 *  Copyright © 2018. All rights reserved.
 *
 *  Compute ensemble perturbations in parallel. Cancel pending ensemble calculations if:
 *      1. convergence is achieved
 *      2. max execution time has been reached
 *      3. max ensemble size has been reached
 */


#include <stdio.h>
#include <iostream>
#include <chrono>
#include <time.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include <deque>

using namespace std;

const static int INITIAL_ENSEMBLE_SIZE = 100;                           // initial ensemble size
const static int MAX_ENSEMBLE_SIZE = 1000000;                           // maximum ensemble size
const static double MAX_EXECUTION_TIME = 10000000000000000;             // total time allowed for ensemble execution (seconds)
const static int DATA_DIMENSIONS = 4;                                   // 4D for ocean DA


const static string UCM_FILE1 = "ucm1";                     // file 1 for writing UCM to
const static string UCM_FILE2 = "ucm2";                     // file 2 for writing UCM to (alternating file) - PARALLEL
const static string SVD_FILE = "svd";                       // safe file for SVD calculations (read by SVD, written by alternating file 1/2) - PARALLEL


/*
 * Ocean model - ESSE ensemble member class
 */
class ocean_model{
public:
    double forecast[DATA_DIMENSIONS];                       // perturbed forecast
    double initial_conditions;
    ocean_model(){initial_conditions = 1;};
    
    /*
     *  Constructor for ocean_model - generates perturbation and forecast
     *  @param conditions (initial conditions)
     */
    ocean_model(double conditions){
        initial_conditions = conditions;
    };
    
    /*
     *  Perturbation of initial conditions, generating perturbed forecast
     */
    double perturb_forcast(){
        // perturb initial values
        // generate forecast
        return 0;
    }
    
};


/*
 * ESSE calculation methods
 */
class esse{
    
public:
    int n;                                                  // current ensemble size
    time_t start_time;                                      // start time
    time_t deadline_time;                                   // max time to completion
    double initial_conditions;                              // initial condition for dominant errors (assumed available)
    double central_forecast[DATA_DIMENSIONS];               // central forecast
    
    /*
     *  ESSE Constructor
     *  @param initial conditions
     */
    esse(){
        
        // set initial ensemble size
        n = INITIAL_ENSEMBLE_SIZE;
        
        // calculate unperturbed central forecast
        forecast();
        
        // record start time
        start_time = time(0);
        
        // calculate deadline time
        deadline_time = start_time + MAX_EXECUTION_TIME;
    }
    
    
    /*
     *  Calculate central (unperturbed) forecast
     */
    void forecast(){
        
        // TODO: calculate using initial conditions
        central_forecast[0] = 0;
        central_forecast[1] = 1;
        central_forecast[2] = 4;
        central_forecast[3] = 11;
        
    }
    
    
    /*
     *  Obtain matrix rank - singular values decomposition of matrix
     *
     *  @param ucm file (serial) OR safe file (parallel)
     *  @param rank (array) [0] = E   [1] = II
     */
    void svd_matrix(double rank[], string file){
        
        // TODO: Compute rank values
        rank[0] = 1;
        rank[1] = 1;
    }
    
    
    /*
     *  Test convergence between previous and new (decomposed) ranks (E, II)
     *
     *  @param prev_rank (array)    [0] = E   [1] = II
     *  @param new_rank (array)     [0] = E   [1] = II
     */
    bool converged(double prev_rank[], double new_rank[]){
        // TODO: Compute p from ranks and test convergences
        //double p = 0;
        
        // if new rank converges to 1, return true
        return false;
    }
    
    
    /*
     *  Generate UCM and write to file
     *  @param ensemble
     */
    void generate_ucm(ocean_model ensemble[], string filename){
        
        // covariance matrix (size = nxn)
        double covariance_matrix[sizeof(&ensemble)][sizeof(&ensemble)];
        
        // compute diff matrix
        for (int x = 0; x < sizeof(&ensemble); x++){
            
            double variance = 0;
            
            // calculate variance values (diagonal)
            for (int y = 0; y < DATA_DIMENSIONS; y++){
                variance += (central_forecast[y] - ensemble[x].forecast[y]) * (central_forecast[y] - ensemble[x].forecast[y]);
            }
            
            // calculate covariance values and reflect across diagonal
            for (int z = 0; z < x; z++){
                
            }
            
            covariance_matrix[x][x] = variance;
        }
        
        
        // generate uncertainty covariance matrix and write to file
        ofstream f;
        f.open (filename);
        for (int x = 0; x < sizeof(&ensemble); x++){
            for (int y = 0; y < DATA_DIMENSIONS; y++){
                f << central_forecast[y] - ensemble[x].forecast[y];
            }
            f << endl;
        }
        f.close();
    }
    
    
    /*
     * Add ensemble member data to UCM in file (csv)
     */
    void add_to_ucm(double member[], string filename){
        
        fstream f;
        f.open (filename);
        string line;
        
        // row to add to ucm after computation (will never be greater than max ensemble size)
        double new_ucm[MAX_ENSEMBLE_SIZE];
        
        // calculate variance for ucm
        double mem_covariance = 0;
        for (int i = 0; i < DATA_DIMENSIONS; i++){
            mem_covariance += (central_forecast[i] - member[i]) * (central_forecast[i] - member[i]);
        }
        
        // iterate each row of matrix to calculate each covariance --> cov(x)(y) = var(x) * var(y)
        int row = 0;
        while(getline(f, line)){
            
            istringstream s(line);
            string covariance;
            
            // iterate each column until covariance value reached (column index == row index)
            for (int i = 0; i < row; i++){
                getline(s, covariance, ',');
            }
            
            // calculate covariance and add to new ucm row
            new_ucm[row] = sqrt(stoi(covariance)) * sqrt(mem_covariance);
            
            // increment row
            row += 1;
            
        }
        
        if (row == 0){
            
            // if row == 0, this is the first element in the ucm --> simply write member covariance to file
            f << mem_covariance;
            
        }else{
            
            // append variance to new row
            new_ucm[row] = mem_covariance;
            
            // reflect matrix values over diaganol axis
            for (int i = 0; i < row; i++){
                new_ucm[row + 1 + i] = new_ucm[i];
            }
            
            // add new covariances to matrix
            f.close();
            f.open (filename);
            int i = 0;
            while(getline(f, line)){
                f << line << "," << new_ucm[i];
            }
            
        }
        
        f.close();
        
        // TODO: update SVD file
        
    }
};



/* Serial ESSE Execution */
int main() {
    
    esse se;
    time_t current_time = time(0);
    bool convergence = false;
    double prev_rank[2];
    
    
    // compute ESSE, increasing N until completion condition met
    while (convergence == false && current_time < se.deadline_time && se.n < MAX_ENSEMBLE_SIZE){
        
        ocean_model ensemble[se.n];                                                        // ensemble, size n
        double central_forecast[DATA_DIMENSIONS] = {0};                                 // central unperturbed forecast [TODO: generate]
        double v_matrix[DATA_DIMENSIONS][se.n];                                            // variance matrix (central forecast vs perturbations)
        double d_matrix[se.n][DATA_DIMENSIONS];                                            // difference matrix (central forecast vs perturbations)
        double ucm[se.n][se.n];                                                               // uncertainty covariance matrix
        double new_rank[2];                                                             // rank for new SVD         array[0] = E    array[1] = II
        
        // calculate each ensemble member
        for (int i = 0; i < se.n; i++){
            
            // generate model
            //ocean_model new_model = ocean_model(se.initial_conditions);
            ocean_model new_model = ocean_model();
            
            // perturb forecast
            new_model.perturb_forcast();
            
            // add to ensemble
            ensemble[i] = new_model;
        }
        
        // compute diff matrix
        for (int x = 0; x < se.n; x++){
            for (int y = 0; y < DATA_DIMENSIONS; y++){
                d_matrix[x][y] = central_forecast[y] - ensemble[x].forecast[y];
                v_matrix[y][x] = central_forecast[y] - ensemble[x].forecast[y];
            }
        }
        
        // compute uncertainty covariance matrix
        for (int row = 0; row < se.n; row++){
            for (int col = 0; col < se.n; col++){
                int product = 0;
                for (int y = 0; y < DATA_DIMENSIONS; y++){
                    product += d_matrix[col][y] * v_matrix[y][row];
                }
                ucm[col][row] = product;
            }
        }
        
        // write ucm to file
        se.generate_ucm(ensemble, UCM_FILE1);
        
        // calculate singular value decomposition - computing rank (E, II)
        //se.add_to_ucm((double *)ucm, SVD_FILE);
        
        // test convergence
        convergence = se.converged(prev_rank, new_rank);
        
        // if convergence not reached, increment ensemble size
        if (!convergence){
            se.n += 1;
            prev_rank[0] = new_rank[0];
            prev_rank[1] = new_rank[1];
        }
        
        // get current time
        current_time = time(0);
    }
    
    
    if (convergence){
        cout << "Error Subspace successfully calculated! " << endl;
        cout << "Total execution time: " << (current_time - se.start_time) << " seconds. " << endl;
    }else{
        
        if (se.n == MAX_ENSEMBLE_SIZE){
            cout << "Failed to calculated Error Subspace. Maximum ensemble size reached." << endl;
            cout << "Total execution time: " << (current_time - se.start_time) << " seconds. "<< endl;
        }
        
        if (current_time == se.deadline_time){
            cout << "Failed to calculated Error Subspace. Maximum execution time reached." << endl;
            cout << "Total execution time: " << (current_time - se.start_time) << " seconds. "<< endl;
        }
    }
    
    return 0;
}


