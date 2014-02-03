#ifndef RB_CONVERSIONS_H
#define RB_CONVERSIONS_H
#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
using namespace std;
using namespace shark;

VALUE rb_sym_new(const char *);
VALUE method_is_a_directory(VALUE);

// STD and Shark helpers:
std::vector<shark::RealVector>          realvector_to_stdvector(RealVector);

// From Ruby
shark::RealVector                       rb_ary_to_1d_realvector(VALUE);
RealMatrix                              rb_ary_to_realmatrix(VALUE);
RealMatrix                              rb_1d_ary_to_realmatrix(VALUE);
std::vector<shark::RealVector>          rb_ary_to_realvector(VALUE);
std::vector<shark::RealMatrix>          rb_ary_to_realmatrices(VALUE);
shark::UnlabeledData<shark::RealVector> rb_ary_to_unlabeleddata(VALUE);

// To Ruby
VALUE realmatrix_to_rb_ary(const RealMatrix&);
VALUE realmatrix_to_rb_ary(const RealMatrix&, bool);
VALUE realvector_to_rb_ary(const RealVector&);
VALUE stdvector_realmatrix_to_rb_ary(const std::vector<RealMatrix>);
VALUE stdvector_realvector_to_rb_ary(const std::vector<RealVector>);

#endif