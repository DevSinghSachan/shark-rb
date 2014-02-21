#include "rb_SteepestDescent.h"
extern VALUE rb_optimizer_steepestdescent_klass;
// Datatypes:
VALUE rb_SteepestDescent::rb_class() {
	return rb_optimizer_steepestdescent_klass;
}

rb_SteepestDescent::rb_SteepestDescent() {};

SteepestDescent& rb_SteepestDescent::algorithm() {
	return _algorithm;
}

#include "extras/utils/rb_pointer_wrapping.extras"
#include "extras/models/rb_objective_function.extras"

VALUE method_steepestdescent_allocate (VALUE klass) {
	return wrap_pointer<rb_SteepestDescent>(
		rb_optimizer_steepestdescent_klass,
		new rb_SteepestDescent()
	);
};

VALUE method_steepestdescent_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	return self;
};

VALUE method_steepestdescent_set_momentum (VALUE self, VALUE rb_momentum) {
	if (TYPE(rb_momentum) != T_FIXNUM && TYPE(rb_momentum) != T_FLOAT)
		rb_raise(rb_eArgError, "Momentum is set using a Float of Fixnum.");

	rb_SteepestDescent *s;
	Data_Get_Struct(self, rb_SteepestDescent, s);

	s->algorithm().setMomentum(NUM2DBL(rb_momentum));

	return self;
}

VALUE method_steepestdescent_get_momentum (VALUE self) {
	rb_SteepestDescent *s;
	Data_Get_Struct(self, rb_SteepestDescent, s);

	return rb_float_new(s->algorithm().momentum());
}

VALUE method_steepestdescent_set_learning_rate (VALUE self, VALUE rb_learning_rate) {
	if (TYPE(rb_learning_rate) != T_FIXNUM && TYPE(rb_learning_rate) != T_FLOAT)
		rb_raise(rb_eArgError, "learning_rate is set using a Float of Fixnum.");

	rb_SteepestDescent *s;
	Data_Get_Struct(self, rb_SteepestDescent, s);

	s->algorithm().setLearningRate(NUM2DBL(rb_learning_rate));

	return self;
}

VALUE method_steepestdescent_get_learning_rate (VALUE self) {
	rb_SteepestDescent *s;
	Data_Get_Struct(self, rb_SteepestDescent, s);

	return rb_float_new(s->algorithm().learningRate());
}

void Init_Steepest_Descent () {
	InitObjectiveFunction<rb_SteepestDescent>();
	rb_define_alloc_func(rb_optimizer_steepestdescent_klass,  (rb_alloc_func_t) method_steepestdescent_allocate);
	rb_define_method(rb_optimizer_steepestdescent_klass, "initialize", (rb_method) method_steepestdescent_initialize, -1);
	rb_define_method(rb_optimizer_steepestdescent_klass, "momentum=", (rb_method) method_steepestdescent_set_momentum, 1);
	rb_define_method(rb_optimizer_steepestdescent_klass, "momentum", (rb_method) method_steepestdescent_get_momentum, 0);
	rb_define_method(rb_optimizer_steepestdescent_klass, "learning_rate=", (rb_method) method_steepestdescent_set_learning_rate, 1);
	rb_define_method(rb_optimizer_steepestdescent_klass, "learning_rate", (rb_method) method_steepestdescent_get_learning_rate, 0);

}