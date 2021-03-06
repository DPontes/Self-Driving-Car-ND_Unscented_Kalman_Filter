# ** Project Unscented Kalman Filter**

## Overview

This project implements a **sensor fusion** algorithm to use a **standard (linear)** and an [**Unscented (non-linear)** Kalman Filter](https://en.wikipedia.org/wiki/Kalman_filter#Unscented_Kalman_filter) in C++ to estimate the state (position and velocity) of a moving object of interest with noisy RADAR and LIDAR measurements.

The motion model is a **CTRV (Constant Turn Rate and Velocity)** model that tracks state variables for position [px, py], velocity [v], yaw [sai], and yaw rate [sai_dot] to follow a turning object more accurately.

Since the motion equations are **non-linear**, the filter's **Predict** step uses **Unscented Kalman Filter** equations with the following steps:

1. Create augmented representative sigma points (Xsig_aug) using lambda factor and process noise (nu, Q)
2. Predict augmented sigma points (Xsig_pred) to current timestep using CTRV motion model
3. Calculate predicted mean state (x) and covariance (P) using predicted sigma points and lambda weights

For **LIDAR** measurement **Update** steps, the position [px, py] is measured directly. Therefore, **Linear Kalman Filter** equations can be used.

For **RADAR** measurement **Update** steps, the measurement [rho, phi, rho_dot] is in polar coordinates so the conversion to state variables is non-linear and uses **Unscented Kalman Filter** equations with the following steps:

1. Transform sigma points to measurements space (Zsig)
2. Calculate predicted measurement mean (z_pred) and covariance (S)
3. Calculate Kalman Gain (K) using state-measurement cross-variance (Tc)
4. Update state (x) and covariance (P) using Kalman Gain (K)

For each measurement received from the **Udacity Term 2 Simulator**, the program predicts the state for the current timestep, then updates the state with the RADAR or LIDAR measurement and calculated Kalman gain.

**Sensor measurement noise covariance (R)** values are provided by Udacity in this project. **Process Noise Covariance (Q, augmented into P)** values for **longitudinal acceleration** and **yaw rate acceleration** were chosen experimentally to reduce RMSE and make smooth tracking.

**Normalized Innovation Squared (NIS)** is also calculated for LIDAR (2 DoF) and RADAR (3 DoF) measurements to check the consistency of the model's uncertainty level based on 95% [chi-squared distribution](http://en.wikipedia.org/wiki/Chi-squared_distribution) thresholds.

## Reference Equations

The implementation uses the following matrices and equations:

* x = estimated state
* Xsig_aug = augmented representative sigma points
* Xsig_pred =augmented sigma points predicted to current timestep
* P = state covariance matrix
* Q = process noise covariance matrix
* H = measurement matrix (LIDAR only)
* R = measurement noise covariance matrix
* z = measurement vector
* Zsig = sigma points transformed to measurement space
* y = residual error (innovation) between measurement and estimate from state
* S = pre-fit covariance matrix
* Tc = state-measurement cross-variance
* K = Kalman Gain

**Unscented Kalman Filter Predict Step Equations**

* x_aug = [x, 0, 0];
* P_aug = [P, 0; 0, Q];
* lambda = 3- n_aug;
* Xsig_aug = [x_aug, x_aug + sqrt((lambda + n_aug) * P_aug), x_aug - sqrt((lambda + n_aug) * P_aug)]
* Xsig_pred = Xsig_aug -> CTRV motion equations f(x, nu) for [px, py, v, yaw, yaw_dot]
* weight = (lambda / (lambda + n_aug)) for mean, (0.5 * (lambda * n_aug)) for other sigma points
* x = sum(weight * Xsig_pred)
* P = sum(weight * (Xsig_pred - x) * (Xsig_pred - x)^T)

**Standard Linear Kalman Filter Update step equations**

* z_pred = H * x
* y = z - z_pred
* S = H * P * Ht + R
* K = P * Ht * Sinv
* x = x + (K * y)
* P = (I - K * H) * P

**Unscented Nonlinear Kalman Filter Update step equations**

* Zsig = Xsig_pred -> polar coordinate transformation h(px, py, vx, vy) for [rho, phi, rho_dot]
* z_pred = sum(weight * Zsig)
* S = sum(weight * (Zsig - z_pred) * (Zsig - z_pred)^T) + R
* Tc = sum(weight * (Xsig_pred - x) * (Zsig - z_pred)^T)
* K = Tc * Sinv
* y = z - z_pred
* x = x + (K * y)
* P = P - K * S * Kt

**Normalized Innovation Squared equation**

NIS = (z - z_pred)^T Sinv * (z - z_pred)

## Key files

| File                        | Description                                                                                                                     |
|:---------------------------:|:-------------------------------------------------------------------------------------------------------------------------------:|
| /src/main.cpp               | Source code for **main loop** that handles **uWebSockets communication to simulator**                                           |
| /src/ukf.cpp, .h            | Source code for **sensor fusion algorithm** that processes RADAR/LIDAR measurements using **Unscented Kalman Filter** equations with **CTRV motion model** |
| /src/tools.cpp, .h          | Source code for calculating **RMSE** values                                                                                     |
| /src/Eigen/                 | Eigen library for matrix calculations in C++                                                                                    |
| /build/UnscentedKF          | Output **executable program binary**                                                                                            |
| install-mac.sh              | Script for Mac to install uWebSocketIO required to interface with simulator                                                     |
| install-ubuntu.sh           | Script for Linux to install uWebSocketIO required to interface with simulator                                                   |

## How to build and Run Code

This project involves the Udacity Term 2 Simulator which can be downloaded [here](https://github.com/udacity/self-driving-car-sim/releases).

This repository includes two scripts (**install-mac.sh** and **install-ubuntu.sh**) that can be used to set up and isntall [uWebSocketIO](https://github.com/uWebSockets/uWebSockets) for either Linux or Mac operating systems.

Once the installation of uWebSockets is completed, the main program can be built and run doing the following from the project top directory:

1. mkdir build
2. cd build
3. cmake ..
4. make
5. ./UnscentedKF

## Other Important Dependencies

* cmake >= 3.5
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)

## Communication protocol between uWebSocketIO and Simulator

**INPUT to main.cpp**: values provided by the simulator to the C++ program

* ["sensor_measurement"] => the measurement that the simulator observed (either lidar or radar)

**OUTPUT from main.cpp**: values provided by the C++ program to the simulator

* ["estimate_x"] <= kalman filter estimated position x
* ["estimate_y"] <= kalman filter estimated position y
* ["rmse_x"]
* ["rmse_y"]
* ["rmse_vx"]
* ["rmse_vy"]
