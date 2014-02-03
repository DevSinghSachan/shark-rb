#ifndef RB_ABSTRACT_OBJECTIVE_FUNC_H
#define RB_ABSTRACT_OBJECTIVE_FUNC_H

#include "ruby.h"
#include <shark/Algorithms/AbstractSingleObjectiveOptimizer.h>

class rb_AbstractSinglerObjectiveFunc {

	public:
		AbstractSingleObjectiveOptimizer objective();
		void step();

};


#endif