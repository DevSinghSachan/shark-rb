#include "rb_GaussianBinaryRBM.h"

extern VALUE rb_optimizer_gaussianbinaryrbm_klass;

#include "extras/rbm/rb_rbm_model.extras"

VALUE rb_GaussianBinaryRBM::rb_class () {
	return rb_optimizer_binaryrbm_klass;
}

BinaryRBM* rb_GaussianBinaryRBM::getModel() {
	return &model;
}

rb_GaussianBinaryRBM::rb_GaussianBinaryRBM (): model(Rng::globalRng) {};

void Init_GaussianBinaryRBM () {
	Init_RBM<rb_GaussianBinaryRBM, rb_RBMGaussianLayer, rb_RBMBinaryLayer>();
}