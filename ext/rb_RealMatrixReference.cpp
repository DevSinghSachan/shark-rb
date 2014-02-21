#include "rb_RealMatrixReference.h"
using namespace shark;
using namespace std;

#include "extras/utils/rb_pointer_wrapping.extras"
#include "extras/matrix/rb_matrix_methods.extras"

VALUE rb_RealMatrixReference::rb_class () {
	return rb_optimizer_realmatrix_reference_klass;
}

rb_RealMatrixReference::rb_RealMatrixReference(RealMatrix * _data) {
	data = _data;
}

RealMatrix* rb_RealMatrixReference::getData() {
	return data;
}

void Init_RealMatrixReference () {
	Init_MatrixMethods<rb_RealMatrixReference>();
}