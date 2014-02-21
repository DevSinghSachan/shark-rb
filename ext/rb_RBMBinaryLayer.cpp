#include "rb_RBMBinaryLayer.h"

extern VALUE rb_optimizer_rbm_binarylayer_klass;

#include "extras/rbm/rb_neuronlayer_model.extras"

VALUE rb_RBMBinaryLayer::rb_class () {
	return rb_optimizer_rbm_binarylayer_klass;
}

shark::BinaryLayer* rb_RBMBinaryLayer::getData() {
	return model;
};
shark::BinaryLayer* rb_RBMBinaryLayer::getModel() {
	return model;
};

rb_RBMBinaryLayer::rb_RBMBinaryLayer(shark::BinaryLayer* _model) {
	model = _model;
}

void Init_RBMBinaryLayer() {
	Init_RBMNeuronLayer<rb_RBMBinaryLayer>();
};