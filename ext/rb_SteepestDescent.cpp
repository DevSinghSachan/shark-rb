#include "ruby.h"

VALUE method_steepest_descent_set_momentum (VALUE self, VALUE rb_momentum) {


	return self;
}

void Init_Steepest_Descent () {


	rb_define_method(rb_optimizer_steepestdescent_klass, "momentum=", (rb_method), method_steepest_descent_set_momentum, 1);

}