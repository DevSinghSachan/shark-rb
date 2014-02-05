#ifndef RB_REALMATRIX_H
#define RB_REALMATRIX_H

#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include "rb_conversions.h"
#include "rb_RealVector.h"

using namespace std;
using namespace shark;
class rb_RealMatrix {
	public:
		rb_RealMatrix(RealMatrix const &);
		rb_RealMatrix();
		void fill(double);
		RealMatrix data;
		RealVector get_row(int);
		RealVector get_column(int);

};

void Init_RealMatrix ();

#endif