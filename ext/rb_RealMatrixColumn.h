#ifndef RB_REALMATRIX_COLUMN_H
#define RB_REALMATRIX_COLUMN_H

#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include "rb_conversions.h"
#include "rb_RealVector.h"

using namespace std;
using namespace shark;
class rb_RealMatrixColumn {
	public:
		static VALUE rb_class;
		rb_RealMatrixColumn(RealMatrix *, int);
		shark::blas::matrix_column<RealMatrix> data;
		shark::blas::matrix_column<RealMatrix>* getData();

};

void Init_RealMatrixColumn ();

#endif