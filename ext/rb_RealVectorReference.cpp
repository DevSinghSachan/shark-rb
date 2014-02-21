#include "rb_RealVectorReference.h"
using namespace shark;
using namespace std;

extern VALUE rb_optimizer_realvector_reference_klass;

#include "extras/utils/rb_pointer_wrapping.extras"
#include "extras/vector/rb_vector_methods.extras"

VALUE rb_RealVectorReference::rb_class () {
	return rb_optimizer_realvector_reference_klass;
}

rb_RealVectorReference::rb_RealVectorReference(RealVector * _data) {
	data = _data;
}

RealVector* rb_RealVectorReference::getData() {
	return data;
}

void Init_RealVectorReference () {
	Init_VectorMethods<rb_RealVectorReference>();
}