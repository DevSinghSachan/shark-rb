#include "rb_RealMatrixColumn.h"
using namespace shark;
using namespace std;

#include "rb_pointer_wrapping.extras"
#include "rb_vector_min_methods.extras"

rb_RealMatrixColumn::rb_RealMatrixColumn(RealMatrix *matrix, int col) : data(*matrix, col) {}

shark::blas::matrix_column<RealMatrix>* rb_RealMatrixColumn::getData() {
	return &data;
}

void Init_RealMatrixColumn () {
	Init_VectorMinMethods<rb_RealMatrixColumn>();
}