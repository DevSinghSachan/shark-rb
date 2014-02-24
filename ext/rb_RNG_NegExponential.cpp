#include "rb_RNG_NegExponential.h"
#include "extras/utils/rb_pointer_wrapping.extras"

rb_RNG_NegExponential::rb_RNG_NegExponential() : model(shark::Rng::globalRng) {}

shark::NegExponential< shark::Rng::rng_type > * rb_RNG_NegExponential::getModel() {
	return &model;
}

extern VALUE rb_optimizer_rng_negexponential_klass;

VALUE rb_RNG_NegExponential::rb_class() {
	return rb_optimizer_rng_negexponential_klass;
}

VALUE method_rb_RNG_NegExponential_set_mean (VALUE self, VALUE parameter_1) {
	rb_RNG_NegExponential *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	r->getModel()->mean(NUM2DBL(parameter_1));
	return self
}


VALUE method_rb_RNG_NegExponential_get_mean (VALUE self) {
	rb_RNG_NegExponential *r;
	
	return rb_float_new(	r->getModel()->mean())
}


VALUE method_rb_RNG_NegExponential_get_p (VALUE self, VALUE parameter_1) {
	rb_RNG_NegExponential *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	return rb_float_new(	r->getModel()->p(NUM2DBL(parameter_1)))
}


VALUE method_rb_RNG_NegExponential_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_NegExponential *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	return rb_float_new(	r->getModel()->p(NUM2DBL(parameter_1)))
}


VALUE method_rb_RNG_NegExponential_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_NegExponential>(
		rb_RNG_NegExponential::rb_class(),
		new rb_RNG_NegExponential()
		);
}

VALUE method_rb_RNG_NegExponential_initialize (VALUE self) {
	return self;
}

VALUE method_rb_RNG_NegExponential_sample (VALUE self) {
	rb_RNG_NegExponential *r;
	
	return rb_float_new(	(*r->getModel())())
}


void Init_rb_RNG_NegExponential () {
	rb_define_method(rb_RNG_NegExponential::rb_class(), "mean=", (rb_method) method_rb_RNG_NegExponential_set_mean, 1);
	rb_define_method(rb_RNG_NegExponential::rb_class(), "mean", (rb_method) method_rb_RNG_NegExponential_get_mean, 0);
	rb_define_method(rb_RNG_NegExponential::rb_class(), "p", (rb_method) method_rb_RNG_NegExponential_get_p, 1);
	rb_define_method(rb_RNG_NegExponential::rb_class(), "prob", (rb_method) method_rb_RNG_NegExponential_get_prob, 1);
	rb_define_alloc_func(rb_RNG_NegExponential::rb_class(), (rb_alloc_func_t) method_rb_RNG_NegExponential_allocate);
	rb_define_method(rb_RNG_NegExponential::rb_class(), "initialize", (rb_method) method_rb_RNG_NegExponential_initialize, 0);
	rb_define_method(rb_RNG_NegExponential::rb_class(), "sample", (rb_method) method_rb_RNG_NegExponential_sample, 0);
}