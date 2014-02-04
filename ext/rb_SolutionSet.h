#ifndef RB_SOLUTIONSET_H
#define RB_SOLUTIONSET_H

#include "ruby.h"
#include "rb_RealVector.h"
#include "rb_conversions.h"

using namespace shark;
using namespace std;

class rb_SolutionSet {

	public:
		RealVector point;
		double value;
		rb_SolutionSet(RealVector, double);
		rb_SolutionSet();

};

void Init_SolutionSet();

#endif