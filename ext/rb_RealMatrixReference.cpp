#include "rb_RealMatrixReference.h"
using namespace shark;
using namespace std;

#include "rb_pointer_wrapping.extras"
#include "rb_matrix_methods.extras"

rb_RealMatrixReference::rb_RealMatrixReference(RealMatrix * _data) {
	data = _data;
}

RealMatrix* rb_RealMatrixReference::getData() {
	return data;
}

void Init_RealMatrixReference () {
	Init_MatrixMethods<rb_RealMatrixReference>();
}