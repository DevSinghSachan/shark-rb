#include "rb_LinearModel.h"
using namespace std;
using namespace shark;
RealVector& rb_LinearModel::offset() {
	return model.offset();
};
RealMatrix& rb_LinearModel::matrix() {
	return model.matrix();
};
void rb_LinearModel::setStructure(RealMatrix const& matrix, RealVector const& offset = RealVector()) {
	model.setStructure(matrix, offset);
};
void rb_LinearModel::setStructure(int inputs, int outputs = 1, bool offset = false) {
	model.setStructure(inputs, outputs, offset);
};
int rb_LinearModel::numberOfParameters() {
	return (int)model.numberOfParameters();
};
void rb_LinearModel::setParameterVector(RealVector const& vec) {
	model.setParameterVector(vec);
};
RealVector rb_LinearModel::parameterVector() {
	return model.parameterVector();
};
int rb_LinearModel::outputSize() {
	return (int)model.outputSize();
};
int rb_LinearModel::inputSize() {
	return (int)model.inputSize();
};
bool rb_LinearModel::hasOffset() {
	return model.hasOffset();
};
rb_LinearModel::rb_LinearModel() {
};
rb_LinearModel::rb_LinearModel(LinearModel<RealVector> _model) {
	model = _model;
};
rb_LinearModel::rb_LinearModel(RealMatrix const& matrix) {
	LinearModel<RealVector> *new_model = new LinearModel<RealVector>(matrix, *new RealVector());
	model = *new_model;
};
rb_LinearModel::rb_LinearModel(RealMatrix const& matrix, RealVector const& offset) {
	LinearModel<RealVector> *new_model = new LinearModel<RealVector>(matrix, offset);
	model = *new_model;
};
// should add eval as well.