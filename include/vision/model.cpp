///**\file
// *
// * model.cpp
// *
// * Implements methods in model.h
// *
// * Each layer consists of two elements, a (integer) type value, and a
// * (MatrixXd) set of weights, and a (MatrixXd) set of biases.
// *
// * The type values are as follows:
// * - 1: fully-connected layer
// * - 2: pooling layer
// * - 3: convolutional layer
// * - 4: softmax layer
// *
// * @author Abdul Rahman Kreidieh
// * @version 1.0.0
// * @date February 26, 2019
// */
//
//#include <tuple>
//#include <vector>
//#include "model.h"
//#include <Eigen/Dense>
//#include <MiniDNN.h>
//#include <random>
//
//using namespace MiniDNN;
//using namespace Eigen;
//using namespace std;
//
//
//void VisionModel::add_fc(int units, int activation) {
//	Layer* layer;
//
//	if (activation == NoNonlinearity) {
//		layer = new FullyConnected<Identity>(this->_outputs, units);
//	} else if (activation == ReLUNonlinearity) {
//		layer = new FullyConnected<ReLU>(this->_outputs, units);
//	}
//
//    /* initial the weights and biases of the layer randomly */
//    vector<Scalar> params((this->_outputs+1)*units);
//	for (unsigned int i=0; i<params.size(); i++)
//		params[i] = rand() % 2 - 1;
//    layer->set_parameters(params);
//
//	/* add new element to the layers list */
//    this->_net.add_layer(layer);
//
//	/* update the number of layers and outputs. */
//	this->_num_layers += 1;
//	this->_outputs = units;
//}
//
//
////void VisionModel::add_batchnorm() {
////	LAYER ret;
////	get<0>(ret) = 3;  /* Value set to 1 for batch normalization. */
////
////	/* add new element to the layers list */
////	this->_layers.push_back(ret);
////	this->_num_layers += 1;
////}
//
//
//MatrixXd VisionModel::compute(MatrixXd input) {
//	return this->_net.predict(input);
//}
