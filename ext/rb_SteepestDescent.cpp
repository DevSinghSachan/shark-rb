#include "ruby.h"
#include "rb_SteepestDescent.h"

VALUE method_steepest_descent_set_momentum (VALUE self, VALUE rb_momentum) {

	return self;
}

typedef VALUE (*rb_method)(...);

void Init_Steepest_Descent () {

	VALUE rb_optimizer_klass                 = rb_define_class("Optimizer", rb_cObject);
	VALUE rb_algorithms_module               = rb_define_module_under(rb_optimizer_klass, "Algorithms");
	VALUE rb_optimizer_steepestdescent_klass = rb_define_class_under(rb_algorithms_module, "SteepestDescent", rb_cObject);

	rb_define_method(rb_optimizer_steepestdescent_klass, "momentum=", (rb_method) method_steepest_descent_set_momentum, 1);

}