#include "rb_RBMBinaryLayer.h"

extern VALUE rb_optimizer_realvector_klass;
extern VALUE rb_optimizer_realvector_reference_klass;
extern VALUE rb_optimizer_rbm_binarylayer_klass;
extern VALUE rb_optimizer_realmatrix_row_klass;
extern VALUE rb_optimizer_realmatrix_column_klass;

#include "rb_pointer_wrapping.extras"
#include "rb_abstract_model.extras"
#include "rb_vector_resize.extras"
#include "rb_vector_length.extras"
#include "rb_neuronlayer_bias.extras"

shark::BinaryLayer* rb_RBMBinaryLayer::getData() {
	return &model;
};

void Init_RBMBinaryLayer() {
	InitAbstractModel<rb_RBMBinaryLayer>(rb_optimizer_rbm_binarylayer_klass);
	Init_VectorResize<rb_RBMBinaryLayer>(rb_optimizer_rbm_binarylayer_klass);
	Init_VectorLength<rb_RBMBinaryLayer>(rb_optimizer_rbm_binarylayer_klass);
	Init_LayerBias<rb_RBMBinaryLayer>(rb_optimizer_rbm_binarylayer_klass);
};