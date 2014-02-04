#ifndef RB_SOLUTIONSET_H
#define RB_SOLUTIONSET_H

#include "ruby.h"
#include <shark/Core/ResultSets.h>
#include "rb_RealVector.h"

using namespace shark;
using namespace std;

class rb_SolutionSet {

	public:
		ResultSet<RealVector, Double> result;
		rb_SolutionSet(const ResultSet&);
		rb_SolutionSet();

};

void Init_SolutionSet();

#endif