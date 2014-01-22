#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
class rb_RegressionDataset {
	public:
		int visibleSize;
		rb_RegressionDataset(int,int,int);
		rb_RegressionDataset(VALUE, VALUE);
		shark::RegressionDataset data;
		std::vector<shark::RealVector> input ();
}