#ifndef RB_UNLABELEDDATA_H
#define RB_UNLABELEDDATA_H

#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include <limits>
#include <shark/Data/Statistics.h> //for normalization
#include "rb_RealVector.h"
#include "rb_RealMatrix.h"
#include "rb_conversions.h"

using namespace std;
using namespace shark;
class rb_UnlabeledData {
	public:
		static VALUE rb_class();
		rb_UnlabeledData();
		rb_UnlabeledData(UnlabeledData<RealVector>);
		UnlabeledData<RealVector> data;
		vector<RealVector> input ();
		void fill(double);
		void remove_NaN(double);
};

void Init_UnlabeledData();

#endif