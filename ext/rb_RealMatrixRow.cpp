#include "rb_RealMatrixRow.h"
using namespace shark;
using namespace std;

#include "extras/utils/rb_pointer_wrapping.extras"
#include "extras/vector/rb_vector_min_methods.extras"

VALUE rb_RealMatrixRow::rb_class () {
	return rb_optimizer_realmatrix_row_klass;
}

rb_RealMatrixRow::rb_RealMatrixRow(RealMatrix *matrix, int row) : data(*matrix, row) {}

shark::blas::matrix_row<RealMatrix>* rb_RealMatrixRow::getData() {
	return &data;
}

void Init_RealMatrixRow () {
	Init_VectorMinMethods<rb_RealMatrixRow>();
}