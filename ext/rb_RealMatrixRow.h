#ifndef RB_REALMATRIX_ROW_H
#define RB_REALMATRIX_ROW_H

#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include "rb_conversions.h"
#include "rb_RealVector.h"

using namespace std;
using namespace shark;
class rb_RealMatrixRow {
	public:
		rb_RealMatrixRow(RealMatrix *, int);
		shark::blas::matrix_row<RealMatrix> data;
		shark::blas::matrix_row<RealMatrix>* getData();

};

void Init_RealMatrixRow ();

#endif