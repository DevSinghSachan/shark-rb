#include "Optimizer.h"
/*#ifndef SHARK_PGM
#define SHARK_PGM
#include <shark/Data/Pgm.h>
#endif*/

using namespace std;
using namespace shark;

Optimizer::Optimizer(
	int    _visibleSize,
	int    _hiddenSize,
	double _rho,
	double _beta,
	double _lambda):
		steps(0),
		lambda(_lambda),
		rho(_rho),
		beta(_beta),
		visibleSize(_visibleSize),
		hiddenSize(_hiddenSize),
		error(&model, &loss, rho, beta)
		{
	model.setStructure(visibleSize, hiddenSize, visibleSize, true, false, false, true);
	error = SparseFFNetError(&model, &loss, rho, beta);
	optimizer.lineSearch().lineSearchType() = shark::LineSearch::WolfeCubic;
}

std::vector<shark::RealMatrix> Optimizer::eval(RealMatrix& sample) {
	boost::shared_ptr<State> s = model.createState();
	std::vector<shark::RealMatrix> result(3);
	model.eval(sample,result[0], *s);
	model.neuronResponses(*s);
	result[1].resize(sample.size1(), hiddenSize);
	noalias(result[1]) = subrange(trans(model.neuronResponses(*s)),0,sample.size1(),visibleSize,visibleSize + hiddenSize);
	return result;
}

void Optimizer::setStartingPoint() {
	// Set the starting point for the optimizer. This is 0 for all bias
	// weights and in the interval [-r, r] for non-bias weights.
	double r = sqrt(6) / sqrt(hiddenSize + visibleSize + 1);
	vector<RealMatrix>& layers = model.layerMatrices();
	for (int k = 0; k < 2; ++k)
		for (size_t i = 0; i < layers[k].size1(); ++i)
			for (size_t j = 0; j < layers[k].size2(); ++j)
				layers[k](i,j) = ((double)rand()/(double)RAND_MAX) * 2 * r - r;
	RealVector& bias = model.bias();
	for (size_t i = 0; i < bias.size(); ++i)
		bias(i) = 0.0;
}

void Optimizer::updateFunction() {
	error = SparseFFNetError(&model, &loss, rho, beta);
	func = shark::CombinedObjectiveFunction<shark::VectorSpace<double>, double>();
	func.add(error);
	func.add(lambda, regularizer);
	optimizer = shark::LBFGS();
	optimizer.lineSearch().lineSearchType() = shark::LineSearch::WolfeCubic;
	optimizer.init(func, model.parameterVector());
}
/*
void Optimizer::exportFeatureImages(int index) {
	RealMatrix& W = model.layerMatrices()[index];
	// Export the visualized features.
	// Each row of W corresponds to a feature. Some normalization is done and
	// then it is transformed into a psize x psize image.
	boost::format filename("feature%d.pgm");

	// Create feature images
	for (size_t i = 0; i < W.size1(); ++i)
	{
		shark::RealVector img(W.size2());
		for (size_t j = 0; j < W.size2(); ++j)
			img(j) = W(i,j);
		//exportPGM((filename % i).str().c_str(), img, 8, 8, true);
	}
}*/

void Optimizer::setData(shark::RegressionDataset data) {
	error.setDataset(data);
	regularizer = TwoNormRegularizer(error.numberOfVariables());
	func.add(error);
	func.add(lambda, regularizer);
	this->setStartingPoint();
	optimizer.init(func, model.parameterVector());
	steps = 0;
}

void Optimizer::setParameterVector(RealVector const & parameters) {
	model.setParameterVector(parameters);
}

void Optimizer::step() {
	steps++;
	optimizer.step(func);
}

double Optimizer::errorValue() {
	return optimizer.solution().value;
}

std::vector<RealMatrix> Optimizer::layer_matrices() {
	if (steps < 1)
		rb_raise(rb_eRuntimeError, "Layer matrices are only available once at least 1 training step has occured.");
	return model.layerMatrices();
}

RealMatrix& Optimizer::layer_matrices_at_index(int index) {
	/*if (steps < 1)
		rb_raise(rb_eRuntimeError, "Layer matrices are only available once at least 1 training step has occured.");*/
	return model.layerMatrices()[index];
}

Optimizer::~Optimizer() {
}

void Optimizer::display_layer_matrices() {
	vector<RealMatrix>& layers = model.layerMatrices();
	for (int k = 0; k < 2; ++k)
		for (size_t i = 0; i < layers[k].size1(); ++i)
			for (size_t j = 0; j < layers[k].size2(); ++j) {
				cout << layers[k](i,j) << endl;
			}
}