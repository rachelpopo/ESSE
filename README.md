# ESSE
Error Subspace Statistical Estimation for Uncertainty



DESCRIPTION

Error Subspace Statistical Estimation (ESSE) is an approach used for calculating uncertainty when formulating forecasts from large data ensembles. The intention is to estimate and predict the largest uncertainties, then combine the models and data accordingly. 

This project will introduce two programs:
1.	ESSE Serial Implementation (C++)
2.	ESSE Parallel Implementation (C++/OpenMP)




ESSE ALGORITHM - OVERVIEW

The following is the general process for determining error subspace via ESSE:

1.	Initial conditions for data models are provided
2.	The following are defined:
a.	Initial ensemble size, N
b.	Maximum ensemble size, Nmax
c.	Maximum forecast time, Tmax
3.	ESSE calculation begins at an ensemble size, N
4.	A single, central, unperturbed forecast is generated from ensemble members
5.	For each ensemble member, a perturbed forecast is generated
6.	Each perturbed forecast is compared against the central forecast, and an uncertainty covariance matrix is generated
7.	A singular value decomposition is performed on the uncertainty covariance matrix. SVD results produce a “rank”.
8.	A convergence test is performed using the new ensemble rank, along with the previous ensemble rank. 
9.	If convergence has been reached, then the error subspace has been identified and ESSE is complete. 
10.	If convergence hasn't been reached, ESSE increases ensemble size N+1 and loop at step 4 until one of the following conditions occurs:
    a.	Convergence is reached
    b.	Nmax is reached
    c.	Tmax is reached




HOW TO RUN

Compile:  
-	g++ -o esse_serial esse_serial.cpp  
-	g++ -o esse_parallel esse_parallel.cpp

Execute:  
-	./esse_serial 
-	./esse_parallel
