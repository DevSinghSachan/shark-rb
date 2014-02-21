#include "rb_RBMGaussianLayer.h"

extern VALUE rb_optimizer_rbm_gaussianlayer_klass;

#include "extras/rbm/rb_neuronlayer_model.extras"

VALUE rb_RBMGaussianLayer::rb_class () {
	return rb_optimizer_rbm_gaussianlayer_klass;
}

shark::GaussianLayer* rb_RBMGaussianLayer::getData() {
	return model;
};
shark::GaussianLayer* rb_RBMGaussianLayer::getModel() {
	return model;
};

rb_RBMGaussianLayer::rb_RBMGaussianLayer(shark::GaussianLayer* _model) {
	model = _model;
}

void Init_RBMGaussianLayer() {
	Init_RBMNeuronLayer<rb_RBMGaussianLayer>();
};