#include "rb_RealVectorReference.h"
using namespace shark;
using namespace std;

#include "rb_pointer_wrapping.extras"
#include "rb_vector_methods.extras"

rb_RealVectorReference::rb_RealVectorReference(RealMatrix * _data) {
	data = _data;
}

RealMatrix* rb_RealVectorReference::getData() {
	return data;
}

void Init_RealMatrixReference () {
	Init_VectorMethods<rb_RealVectorReference>(rb_optimizer_realvector_reference_klass);
}