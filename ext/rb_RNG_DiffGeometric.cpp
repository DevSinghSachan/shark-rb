#include "rb_RNG_DiffGeometric.h"
#include "extras/utils/rb_pointer_wrapping.extras"

rb_RNG_DiffGeometric::rb_RNG_DiffGeometric() : model(shark::Rng::globalRng) {}

shark::DiffGeometric< shark::Rng::rng_type > * rb_RNG_DiffGeometric::getModel() {
	return &model;
}

extern VALUE rb_optimizer_rng_diffgeometric_klass;

VALUE rb_RNG_DiffGeometric::rb_class() {
	return rb_optimizer_rng_diffgeometric_klass;
}

VALUE method_rb_RNG_DiffGeometric_set_mean (VALUE self, VALUE parameter_1) {
	rb_RNG_DiffGeometric *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	r->getModel()->mean(NUM2DBL(parameter_1));
	return self
}

VALUE method_rb_RNG_DiffGeometric_get_mean (VALUE self) {
	rb_RNG_DiffGeometric *r;
	
	return rb_float_new(r->getModel()->mean());
}

VALUE method_rb_RNG_DiffGeometric_get_p (VALUE self, VALUE parameter_1) {
	rb_RNG_DiffGeometric *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	return rb_float_new(r->getModel()->p(NUM2DBL(parameter_1)));
}

VALUE method_rb_RNG_DiffGeometric_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_DiffGeometric *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	return rb_float_new(r->getModel()->p(NUM2DBL(parameter_1)));
}

VALUE method_rb_RNG_DiffGeometric_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_DiffGeometric>(rb_RNG_DiffGeometric::rb_class(), new rb_RNG_DiffGeometric());
}

VALUE method_rb_RNG_DiffGeometric_initialize (VALUE self) {
	return self;
}

VALUE method_rb_RNG_DiffGeometric_sample (VALUE self) {
	rb_RNG_DiffGeometric *r;
	
	return rb_float_new((*(r->getModel()))());
}

void Init_rb_RNG_DiffGeometric () {
	rb_define_method(rb_RNG_DiffGeometric::rb_class(), "mean=", (rb_method) method_rb_RNG_DiffGeometric_set_mean, 1);
	rb_define_method(rb_RNG_DiffGeometric::rb_class(), "mean", (rb_method) method_rb_RNG_DiffGeometric_get_mean, 0);
	rb_define_method(rb_RNG_DiffGeometric::rb_class(), "p", (rb_method) method_rb_RNG_DiffGeometric_get_p, 1);
	rb_define_method(rb_RNG_DiffGeometric::rb_class(), "prob", (rb_method) method_rb_RNG_DiffGeometric_get_prob, 1);
	rb_define_alloc_func(rb_RNG_DiffGeometric::rb_class(), (rb_alloc_func_t) method_rb_RNG_DiffGeometric_allocate);
	rb_define_method(rb_RNG_DiffGeometric::rb_class(), "initialize", (rb_method) method_rb_RNG_DiffGeometric_initialize, 0);
	rb_define_method(rb_RNG_DiffGeometric::rb_class(), "sample", (rb_method) method_rb_RNG_DiffGeometric_sample, 0);
}