#include "rb_RNG_Bernoulli.h"
#include "extras/utils/rb_pointer_wrapping.extras"

rb_RNG_Bernoulli::rb_RNG_Bernoulli() : model(shark::Rng::globalRng) {}

shark::Bernoulli< shark::Rng::rng_type > * rb_RNG_Bernoulli::getDistribution() {
	return &model;
}

shark::Bernoulli< shark::Rng::rng_type > * rb_RNG_Bernoulli::getModel() {
	return &model;
}

extern VALUE rb_optimizer_rng_bernoulli_klass;

VALUE rb_RNG_Bernoulli::rb_class() {
	return rb_optimizer_rng_bernoulli_klass;
}

VALUE method_rb_RNG_Bernoulli_set_p (VALUE self, VALUE parameter_1) {
	rb_RNG_Bernoulli *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	r->getDistribution()->prob(NUM2DBL(parameter_1));
	return self};


VALUE method_rb_RNG_Bernoulli_get_p (VALUE self) {
	rb_RNG_Bernoulli *r;
	
	return rb_float_new(r->getDistribution()->prob())};


VALUE method_rb_RNG_Bernoulli_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_Bernoulli *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	return rb_float_new(r->getDistribution()->p(NUM2DBL(parameter_1)))};


VALUE method_rb_RNG_Bernoulli_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_Bernoulli>(
		rb_RNG_Bernoulli::rb_class(),
		new rb_RNG_Bernoulli()
		);
}

VALUE method_rb_RNG_Bernoulli_initialize (VALUE self) {
	return self;
}

VALUE method_rb_RNG_Bernoulli_sample (VALUE self) {
	rb_RNG_Bernoulli *r;
	
	return rb_float_new((*r->getDistribution())())};


void Init_rb_RNG_Bernoulli () {
	rb_define_method(rb_RNG_Bernoulli::rb_class(), "p=", (rb_method) method_rb_RNG_Bernoulli_set_p, 1);
	rb_define_method(rb_RNG_Bernoulli::rb_class(), "p", (rb_method) method_rb_RNG_Bernoulli_get_p, 0);
	rb_define_method(rb_RNG_Bernoulli::rb_class(), "prob", (rb_method) method_rb_RNG_Bernoulli_get_prob, 1);
	rb_define_alloc_func(rb_RNG_Bernoulli::rb_class(), (rb_alloc_func_t) method_rb_RNG_Bernoulli_allocate);
	rb_define_method(rb_RNG_Bernoulli::rb_class(), "initialize", (rb_method) method_rb_RNG_Bernoulli_initialize, 0);
	rb_define_method(rb_RNG_Bernoulli::rb_class(), "sample", (rb_method) method_rb_RNG_Bernoulli_sample, 0);
}