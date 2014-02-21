#ifndef RB_REGRESSIONDATASET_H
#define RB_REGRESSIONDATASET_H

#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include <shark/Data/Csv.h>//for reading in the images as csv
#include <shark/Data/Statistics.h> //for normalization
#include "rb_UnlabeledData.h"
#include "rb_conversions.h"


class rb_RegressionDataset {
	public:
		static VALUE rb_class();
		int visibleSize;
		rb_RegressionDataset(int,int,int);
		rb_RegressionDataset(VALUE);
		rb_RegressionDataset(VALUE, VALUE);
		shark::RegressionDataset data;
};

void Init_RegressionDataset();

#endif