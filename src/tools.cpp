#include <iostream>
#include "tools.h"

using Eigen::VectorXd;
using Eigen::MatrixXd;
using std::vector;

Tools::Tools() {}

Tools::~Tools() {}

VectorXd Tools::CalculateRMSE(const vector<VectorXd> &estimations,
                              const vector<VectorXd> &ground_truth) {

  VectorXd rmse(4);
  rmse << 0,0,0,0;

  // Check the validity of the following inputs:
  // The estimation vector size should mot be zero
  // The estimation vector size should equal ground truth vector size
  if((estimations.size() != ground_truth.size()) || (estimations.size() == 0)){
    cout << "Invalid estimation or ground_truth data size" << endl;
    return rmse;
  }

  // Accumulate squared error (residual)
  for(unsigned int i = 0; i < estimations.size(); ++i){
    VectorXd residual = estimations[i] - ground_truth[i];

    // Coefficient-wise multiplication to square the error
    residual = residual.array() * residual.array();
    rmse+=residual;
  }

  // Calculate the mean of squared error
  rmse = rmse / estimations.size();

  // Calculate the root of Mean Squared error
  rmse = rmse.array().sqrt();

  return rmse;
}
