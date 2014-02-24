#include "rb_RNG_Erlang.h"
#include "extras/utils/rb_pointer_wrapping.extras"

rb_RNG_Erlang::rb_RNG_Erlang() : model(shark::Rng::globalRng) {}

shark::Erlang< shark::Rng::rng_type > * rb_RNG_Erlang::getModel() {
	return &model;
}

extern VALUE rb_optimizer_rng_erlang_klass;

VALUE rb_RNG_Erlang::rb_class() {
	return rb_optimizer_rng_erlang_klass;
}

VALUE method_rb_RNG_Erlang_set_mean (VALUE self, VALUE parameter_1) {
	rb_RNG_Erlang *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	r->getModel()->mean(NUM2DBL(parameter_1));
	return self
}


VALUE method_rb_RNG_Erlang_set_variance (VALUE self, VALUE parameter_1) {
	rb_RNG_Erlang *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	r->getModel()->variance(NUM2DBL(parameter_1));
	return self
}


VALUE method_rb_RNG_Erlang_get_mean (VALUE self) {
	rb_RNG_Erlang *r;
	
	return rb_float_new(	r->getModel()->mean())
}


VALUE method_rb_RNG_Erlang_get_variance (VALUE self) {
	rb_RNG_Erlang *r;
	
	return rb_float_new(	r->getModel()->variance())
}


VALUE method_rb_RNG_Erlang_get_p (VALUE self, VALUE parameter_1) {
	rb_RNG_Erlang *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	return rb_float_new(	r->getModel()->p(NUM2DBL(parameter_1)))
}


VALUE method_rb_RNG_Erlang_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_Erlang *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	return rb_float_new(	r->getModel()->p(NUM2DBL(parameter_1)))
}


VALUE method_rb_RNG_Erlang_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_Erlang>(
		rb_RNG_Erlang::rb_class(),
		new rb_RNG_Erlang()
		);
}

VALUE method_rb_RNG_Erlang_initialize (VALUE self) {
	return self;
}

VALUE method_rb_RNG_Erlang_sample (VALUE self) {
	rb_RNG_Erlang *r;
	
	return rb_float_new(	(*r->getModel())())
}


void Init_rb_RNG_Erlang () {
	rb_define_method(rb_RNG_Erlang::rb_class(), "mean=", (rb_method) method_rb_RNG_Erlang_set_mean, 1);
	rb_define_method(rb_RNG_Erlang::rb_class(), "variance=", (rb_method) method_rb_RNG_Erlang_set_variance, 1);
	rb_define_method(rb_RNG_Erlang::rb_class(), "mean", (rb_method) method_rb_RNG_Erlang_get_mean, 0);
	rb_define_method(rb_RNG_Erlang::rb_class(), "variance", (rb_method) method_rb_RNG_Erlang_get_variance, 0);
	rb_define_method(rb_RNG_Erlang::rb_class(), "p", (rb_method) method_rb_RNG_Erlang_get_p, 1);
	rb_define_method(rb_RNG_Erlang::rb_class(), "prob", (rb_method) method_rb_RNG_Erlang_get_prob, 1);
	rb_define_alloc_func(rb_RNG_Erlang::rb_class(), (rb_alloc_func_t) method_rb_RNG_Erlang_allocate);
	rb_define_method(rb_RNG_Erlang::rb_class(), "initialize", (rb_method) method_rb_RNG_Erlang_initialize, 0);
	rb_define_method(rb_RNG_Erlang::rb_class(), "sample", (rb_method) method_rb_RNG_Erlang_sample, 0);
}