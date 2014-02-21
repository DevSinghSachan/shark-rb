#include "rb_RBMBinaryLayer.h"

extern VALUE rb_optimizer_realvector_klass;
extern VALUE rb_optimizer_realvector_reference_klass;
extern VALUE rb_optimizer_rbm_binarylayer_klass;
extern VALUE rb_optimizer_realmatrix_row_klass;
extern VALUE rb_optimizer_realmatrix_column_klass;

#include "extras/utils/rb_pointer_wrapping.extras"
#include "extras/models/rb_abstract_model.extras"
#include "extras/vector/rb_vector_resize.extras"
#include "extras/vector/rb_vector_length.extras"
#include "extras/rbm/rb_neuronlayer_bias.extras"

VALUE rb_RBMBinaryLayer::rb_class () {
	return rb_optimizer_rbm_binarylayer_klass;
}

shark::BinaryLayer* rb_RBMBinaryLayer::getData() {
	return &model;
};

void Init_RBMBinaryLayer() {
	InitAbstractModel<rb_RBMBinaryLayer>();
	Init_VectorResize<rb_RBMBinaryLayer>();
	Init_VectorLength<rb_RBMBinaryLayer>();
	Init_LayerBias<rb_RBMBinaryLayer>();
};