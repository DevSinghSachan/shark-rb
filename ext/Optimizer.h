#include "ruby.h"
#include <shark/Models/FFNet.h> //the feed forward neural network
//#include <shark/Algorithms/GradientDescent/Rprop.h> //resilient propagation as optimizer
#include <shark/ObjectiveFunctions/Loss/NegativeClassificationLogLikelihood.h> // loss during training
#include <shark/ObjectiveFunctions/ErrorFunction.h> //error function to connect data model and loss
#include <shark/ObjectiveFunctions/Loss/ZeroOneLoss.h> //loss for classification
#include <fstream>
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Csv.h>//for reading in the images as csv
#include <shark/Data/Statistics.h> //for normalization
#include <shark/ObjectiveFunctions/SparseFFNetError.h>//the error function performing the regularisation of the hidden neurons
#include <shark/Algorithms/GradientDescent/LBFGS.h>// the L-BFGS optimization algorithm
#include <shark/ObjectiveFunctions/Loss/SquaredLoss.h> // squard loss usd for regression
#include <shark/ObjectiveFunctions/Regularizer.h> //L2 rgulariziation
#include <shark/ObjectiveFunctions/CombinedObjectiveFunction.h> //binds together the regularizer with the Error


using namespace std;
using namespace shark;
class Optimizer {
	public:
		shark::FFNet<shark::LogisticNeuron, shark::LogisticNeuron> model;
		shark::LBFGS optimizer;
		shark::SparseFFNetError error;
		shark::SquaredLoss<shark::RealVector> loss;
		shark::CombinedObjectiveFunction<shark::VectorSpace<double>, double> func;
		shark::TwoNormRegularizer regularizer;
		int visibleSize;
		double rho;
		double beta;
		int steps;
		double lambda;
		int hiddenSize;
		// const RealMatrix & neuronResponses();
		void setParameterVector(RealVector const &);
		std::vector<shark::RealMatrix> eval(RealMatrix&);
		void setData(shark::RegressionDataset);
		Optimizer(int, int, double, double, double);
		double errorValue();
		void step();
		~Optimizer();
		void display_layer_matrices();
		std::vector<shark::RealMatrix> layer_matrices();
		RealMatrix& layer_matrices_at_index(int);
		void updateFunction();
		void setStartingPoint();
};

/*class Samples {
	public:
		int visibleSize;
		Samples(int,int,int);
		Samples(VALUE, VALUE);
		shark::RegressionDataset data;
		std::vector<shark::RealVector> input ();
};*/