#ifndef RB_SOLUTIONSET_H
#define RB_SOLUTIONSET_H

#include "ruby.h"
#include "rb_RealVector.h"
#include "rb_conversions.h"
#include "rb_classes.h"

using namespace shark;
using namespace std;

class rb_SolutionSet {

	public:
		static VALUE rb_class;
		RealVector point;
		double value;
		rb_SolutionSet(RealVector const&, double);
		rb_SolutionSet();

};

void Init_SolutionSet();

VALUE rb_SolutionSet::rb_class = rb_optimizer_solutionset_klass;

#endif