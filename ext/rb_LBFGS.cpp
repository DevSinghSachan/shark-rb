#include "rb_LBFGS.h"
extern VALUE rb_optimizer_lbfgs_klass;
// datatypes:
extern VALUE rb_optimizer_solutionset_klass;
extern VALUE rb_optimizer_realvector_klass;
rb_LBFGS::rb_LBFGS() {};

typedef VALUE (*rb_method)(...);

LBFGS& rb_LBFGS::algorithm() {
	return _algorithm;
}

#include "rb_pointer_wrapping.extras"
#include "rb_objective_function.extras"

VALUE method_lbfgs_allocate (VALUE klass) {
	return wrap_pointer<rb_LBFGS>(
		rb_optimizer_lbfgs_klass,
		new rb_LBFGS()
	);
};

VALUE method_lbfgs_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	return self;
};

VALUE method_lbfgs_set_history_count (VALUE self, VALUE rb_history_count) {
	Check_Type(rb_history_count, T_FIXNUM);

	rb_LBFGS *s;
	Data_Get_Struct(self, rb_LBFGS, s);

	if (NUM2INT(rb_history_count) <= 0)
		rb_raise(rb_eArgError, "LBFGS cannot have an empty history.");
	s->algorithm().setHistCount(NUM2INT(rb_history_count));

	return self;
}
void Init_LBFGS () {
	InitObjectiveFunction<rb_LBFGS>(rb_optimizer_lbfgs_klass);
	rb_define_alloc_func(rb_optimizer_lbfgs_klass,  (rb_alloc_func_t) method_lbfgs_allocate);
	rb_define_method(rb_optimizer_lbfgs_klass, "initialize", (rb_method) method_lbfgs_initialize, -1);
	rb_define_method(rb_optimizer_lbfgs_klass, "history_count=", (rb_method) method_lbfgs_set_history_count, 1);
}