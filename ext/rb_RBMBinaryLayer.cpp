#include "rb_RBMBinaryLayer.h"
#include "rb_abstract_model.extras"
#include "rb_vector_resize.extras"
#include "rb_vector_length.extras"

void Init_RBMBinaryLayer() {
	InitAbstractModel<rb_RBMBinaryLayer>(rb_optimizer_rbm_binarylayer_klass);
	Init_VectorResize<rb_RBMBinaryLayer>(rb_optimizer_rbm_binarylayer_klass);
	Init_VectorLength<rb_RBMBinaryLayer>(rb_optimizer_rbm_binarylayer_klass);
}