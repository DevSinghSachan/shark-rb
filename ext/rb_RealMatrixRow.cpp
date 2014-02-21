#include "rb_RealMatrixRow.h"
using namespace shark;
using namespace std;

#include "rb_pointer_wrapping.extras"
#include "rb_vector_min_methods.extras"

rb_RealMatrixRow::rb_RealMatrixRow(RealMatrix *matrix, int row) : data(*matrix, row) {}

shark::blas::matrix_row<RealMatrix>* rb_RealMatrixRow::getData() {
	return &data;
}

void Init_RealMatrixRow () {
	Init_VectorMinMethods<rb_RealMatrixRow>();
}