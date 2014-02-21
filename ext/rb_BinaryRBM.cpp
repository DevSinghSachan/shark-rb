#include "rb_BinaryRBM.h"

extern VALUE rb_optimizer_binaryrbm_klass;

#include "extras/rbm/rb_rbm_model.extras"

VALUE rb_BinaryRBM::rb_class () {
	return rb_optimizer_binaryrbm_klass;
}

BinaryRBM* rb_BinaryRBM::getModel() {
	return &model;
}

rb_BinaryRBM::rb_BinaryRBM (): model(Rng::globalRng) {};

void Init_BinaryRBM () {
	Init_RBM<rb_BinaryRBM, rb_RBMBinaryLayer, rb_RBMBinaryLayer>();
}