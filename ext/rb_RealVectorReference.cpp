#include "rb_RealVectorReference.h"
using namespace shark;
using namespace std;

#include "rb_pointer_wrapping.extras"
#include "rb_vector_methods.extras"

rb_RealVectorReference::rb_RealVectorReference(RealVector * _data) {
	data = _data;
}

RealVector* rb_RealVectorReference::getData() {
	return data;
}

void Init_RealVectorReference () {
	Init_VectorMethods<rb_RealVectorReference>();
}