///*
// * model.h
// *
// * @author Abdul Rahman Kreidieh
// * @version 1.0.0
// * @date February 24, 2019
// */
//
//#ifndef INCLUDE_VISION_MODEL_H_
//#define INCLUDE_VISION_MODEL_H_
//
//#include <vector>
//#include <tuple>
//#include <Eigen/Dense>
//#include <MiniDNN.h>
//
//using namespace MiniDNN;
//using namespace Eigen;
//using namespace std;
//
//
///* Constants associated different non-linearity */
//#define NoNonlinearity 0
//#define ReLUNonlinearity 1
//
//
//class VisionModel
//{
//private:
//	Network _net;
//	vector<Layer*> _layers;	/** list of neural network layers */
//	int _inputs;			/** number of inputs to the first layer */
//	int _outputs;			/** number of outputs from the last layer */
//	int _num_layers;		/** number of the layers in the network */
//
//public:
//	VisionModel(int inputs);
//	virtual ~VisionModel();
//
//	/** Add a fully connected layer.
//	 *
//	 * @param units number of nodes in the new layer
//	 * @param activation output non-linearity
//	 */
//	virtual void add_fc(int units, int activation=NoNonlinearity);
//
//	/** Add a batch normalization layer. */
//	virtual void add_batchnorm();
//
//	/** Add a convolutional layer.
//	 *
//	 * @param f filter size
//	 * @param s stride
//	 * @param p padding
//	 * @param nc number of channels
//	 * @param activation output non-linearity
//	 */
//	virtual void add_conv(int f, int s, int p, int nc, int activation=NoNonlinearity);
//
//	/** Add a pooling layer.
//	 *
//	 * @param f filter size
//	 * @param s stride
//	 */
//	virtual void add_pooling(int f, int s);
//
//	/** Import a list of weights into the layers of the network.
//	 *
//	 * @params weights
//	 * 		the weights for each layer of the neural network
//	 */
//	virtual void import(vector<MatrixXd> weights);
//
//	/** Perform a forward pass on the model.
//	 *
//	 * @param input
//	 * 		input to the neural network
//	 * @returns
//	 *		the output from the neural network
//	 */
//	virtual MatrixXd compute(MatrixXd input);
//};
//
//
//VisionModel::VisionModel(int inputs)  {
//	this->_inputs = inputs;
//	this->_num_layers = 0;
//	this->_outputs = 0;
//}
//
//
//VisionModel::~VisionModel() {
//	for (int i=0; i<this->_num_layers; i++)
//		delete this->_layers[i];
//}
//
//class MobileNet : public VisionModel
//{
//
//};
//
//
//class MobileNetV2 : public VisionModel
//{
//
//};
//
//
//#endif /* INCLUDE_VISION_MODEL_H_ */
