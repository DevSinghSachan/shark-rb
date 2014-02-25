#include "rb_RNG_Poisson.h"
#include "extras/utils/rb_pointer_wrapping.extras"

rb_RNG_Poisson::rb_RNG_Poisson() : model(shark::Rng::globalRng) {}

shark::Poisson< shark::Rng::rng_type > * rb_RNG_Poisson::getModel() {
	return &model;
}

extern VALUE rb_optimizer_rng_poisson_klass;

VALUE rb_RNG_Poisson::rb_class() {
	return rb_optimizer_rng_poisson_klass;
}

VALUE method_rb_RNG_Poisson_set_mean (VALUE self, VALUE parameter_1) {
	rb_RNG_Poisson * r;
	Data_Get_Struct(self, rb_RNG_Poisson, r);
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	r->getModel()->mean(rb_float_new(parameter_1));
	return self
}

VALUE method_rb_RNG_Poisson_get_mean (VALUE self) {
	rb_RNG_Poisson * r;
	Data_Get_Struct(self, rb_RNG_Poisson, r);
	
	return rb_float_new(r->getModel()->mean());
}

VALUE method_rb_RNG_Poisson_get_p (VALUE self, VALUE parameter_1) {
	rb_RNG_Poisson * r;
	Data_Get_Struct(self, rb_RNG_Poisson, r);
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	return rb_float_new(r->getModel()->p(rb_float_new(parameter_1)));
}

VALUE method_rb_RNG_Poisson_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_Poisson * r;
	Data_Get_Struct(self, rb_RNG_Poisson, r);
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	return rb_float_new(r->getModel()->p(rb_float_new(parameter_1)));
}

VALUE method_rb_RNG_Poisson_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_Poisson>(rb_RNG_Poisson::rb_class(), new rb_RNG_Poisson());
}

VALUE method_rb_RNG_Poisson_initialize (VALUE self) {
	return self;
}

VALUE method_rb_RNG_Poisson_sample (VALUE self) {
	rb_RNG_Poisson * r;
	Data_Get_Struct(self, rb_RNG_Poisson, r);
	
	return rb_float_new((*(r->getModel()))());
}

void Init_rb_RNG_Poisson () {
	rb_define_method(rb_RNG_Poisson::rb_class(), "mean=", (rb_method) method_rb_RNG_Poisson_set_mean, 1);
	rb_define_method(rb_RNG_Poisson::rb_class(), "mean", (rb_method) method_rb_RNG_Poisson_get_mean, 0);
	rb_define_method(rb_RNG_Poisson::rb_class(), "p", (rb_method) method_rb_RNG_Poisson_get_p, 1);
	rb_define_method(rb_RNG_Poisson::rb_class(), "prob", (rb_method) method_rb_RNG_Poisson_get_prob, 1);
	rb_define_alloc_func(rb_RNG_Poisson::rb_class(), (rb_alloc_func_t) method_rb_RNG_Poisson_allocate);
	rb_define_method(rb_RNG_Poisson::rb_class(), "initialize", (rb_method) method_rb_RNG_Poisson_initialize, 0);
	rb_define_method(rb_RNG_Poisson::rb_class(), "sample", (rb_method) method_rb_RNG_Poisson_sample, 0);
}