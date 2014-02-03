#include "rb_AbstractSingleObjectiveOptimizer.h"

void rb_SteepestDescent::step(const ObjectiveFunctionType & obj) {
	algorithm().step(obj);
}