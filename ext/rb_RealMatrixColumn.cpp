#include "rb_RealMatrixColumn.h"
using namespace shark;
using namespace std;

#include "extras/utils/rb_pointer_wrapping.extras"
#include "extras/vector/rb_vector_min_methods.extras"

VALUE rb_RealMatrixColumn::rb_class () {
	return rb_optimizer_realmatrix_column_klass;
}

rb_RealMatrixColumn::rb_RealMatrixColumn(RealMatrix *matrix, int col) : data(*matrix, col) {}

shark::blas::matrix_column<RealMatrix>* rb_RealMatrixColumn::getData() {
	return &data;
}

void Init_RealMatrixColumn () {
	Init_VectorMinMethods<rb_RealMatrixColumn>();
}