#include "rb_RNG_DiscreteUniform.h"
#include "extras/utils/rb_pointer_wrapping.extras"

rb_RNG_DiscreteUniform::rb_RNG_DiscreteUniform() : model(shark::Rng::globalRng) {}

shark::DiscreteUniform< shark::Rng::rng_type > * rb_RNG_DiscreteUniform::getModel() {
	return &model;
}

extern VALUE rb_optimizer_rng_discreteuniform_klass;

VALUE rb_RNG_DiscreteUniform::rb_class() {
	return rb_optimizer_rng_discreteuniform_klass;
}

VALUE method_rb_RNG_DiscreteUniform_get_low (VALUE self) {
	rb_RNG_DiscreteUniform *r;
	
	return rb_float_new(r->getModel()->low());
}


VALUE method_rb_RNG_DiscreteUniform_get_high (VALUE self) {
	rb_RNG_DiscreteUniform *r;
	
	return rb_float_new(r->getModel()->high());
}


VALUE method_rb_RNG_DiscreteUniform_get_p (VALUE self, VALUE parameter_1) {
	rb_RNG_DiscreteUniform *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	return rb_float_new(r->getModel()->p(NUM2DBL(parameter_1)));
}


VALUE method_rb_RNG_DiscreteUniform_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_DiscreteUniform *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	return rb_float_new(r->getModel()->p(NUM2DBL(parameter_1)));
}


VALUE method_rb_RNG_DiscreteUniform_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_DiscreteUniform>(
		rb_RNG_DiscreteUniform::rb_class(),
		new rb_RNG_DiscreteUniform()
		);
}

VALUE method_rb_RNG_DiscreteUniform_initialize (VALUE self) {
	return self;
}

VALUE method_rb_RNG_DiscreteUniform_set_range (VALUE self, VALUE parameter_1, VALUE parameter_2) {
	rb_RNG_DiscreteUniform *r;
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");

	// Checking whether parameter_2 is a "double"
	if (TYPE(parameter_2) != T_FIXNUM && TYPE(parameter_2) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 2 must be a Float.");
	r->getModel()->setRange(NUM2DBL(parameter_1), NUM2DBL(parameter_2));
	return self
}


VALUE method_rb_RNG_DiscreteUniform_sample (VALUE self) {
	rb_RNG_DiscreteUniform *r;
	
	return rb_float_new((*r->getModel())());
}


void Init_rb_RNG_DiscreteUniform () {
	rb_define_method(rb_RNG_DiscreteUniform::rb_class(), "low", (rb_method) method_rb_RNG_DiscreteUniform_get_low, 0);
	rb_define_method(rb_RNG_DiscreteUniform::rb_class(), "high", (rb_method) method_rb_RNG_DiscreteUniform_get_high, 0);
	rb_define_method(rb_RNG_DiscreteUniform::rb_class(), "p", (rb_method) method_rb_RNG_DiscreteUniform_get_p, 1);
	rb_define_method(rb_RNG_DiscreteUniform::rb_class(), "prob", (rb_method) method_rb_RNG_DiscreteUniform_get_prob, 1);
	rb_define_alloc_func(rb_RNG_DiscreteUniform::rb_class(), (rb_alloc_func_t) method_rb_RNG_DiscreteUniform_allocate);
	rb_define_method(rb_RNG_DiscreteUniform::rb_class(), "initialize", (rb_method) method_rb_RNG_DiscreteUniform_initialize, 0);
	rb_define_method(rb_RNG_DiscreteUniform::rb_class(), "set_range", (rb_method) method_rb_RNG_DiscreteUniform_set_range, 2);
	rb_define_method(rb_RNG_DiscreteUniform::rb_class(), "sample", (rb_method) method_rb_RNG_DiscreteUniform_sample, 0);
}