#include "rb_LinearModel.h"
using namespace std;
using namespace shark;
RealVector& offset() {
	return (this->model).offset();
};
RealMatrix& matrix() {
	return (this->model).matrix();
};
void setStructure(RealMatrix const& matrix, RealVector const& offset = RealVector()) {
	(this->model).setStructure(matrix, offset);
};
void setStructure(unsigned int inputs, unsigned int outputs = 1, bool offset = false) {
	(this->model).setStructure(inputs, outputs, offset);
};
int numberOfParemeters() {
	return (int)(this->model).numberOfParemeters();
};
void setParameterVector(RealVector const& vec) {
	(this->model).setParameterVector(vec);
};
RealVector parameterVector() {
	return (this->model).parameterVector();
};
int outputSize() {
	return (int)(this->model).outputSize();
};
int inputSize() {
	return (int)(this->model).inputSize();
};
bool hasOffset() {
	return (this->model).hasOffset();
};
LinearModel model() {
	return this->model;
};
rb_LinearModel::rb_LinearModel() {
};
rb_LinearModel::rb_LinearModel(LinearModel _model) {
	this->model = _model;
};
rb_LinearModel::rb_LinearModel(RealMatrix const& matrix) {
	this->model = new LinearModel(matrix);
};
rb_LinearModel::rb_LinearModel(RealMatrix const& matrix, RealVector const& offset) {
	this->model = new LinearModel(matrix, offset);
};
// should add eval as well.