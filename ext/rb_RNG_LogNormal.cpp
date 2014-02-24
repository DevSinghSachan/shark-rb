#include "rb_RNG_LogNormal.h"
#include "extras/utils/rb_pointer_wrapping.extras"

rb_RNG_LogNormal::rb_RNG_LogNormal() : model(shark::Rng::globalRng) {}

shark::LogNormal< shark::Rng::rng_type > * rb_RNG_LogNormal::getModel() {
	return &model;
}

extern VALUE rb_optimizer_rng_lognormal_klass;

VALUE rb_RNG_LogNormal::rb_class() {
	return rb_optimizer_rng_lognormal_klass;
}

VALUE method_rb_RNG_LogNormal_set_scale (VALUE self, VALUE parameter_1) {
	rb_RNG_LogNormal *r;
	
		if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
			rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	
	r->getModel()->scale(NUM2DBL(parameter_1));
	return self;


VALUE method_rb_RNG_LogNormal_set_location (VALUE self, VALUE parameter_1) {
	rb_RNG_LogNormal *r;
	
		if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
			rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	
	r->getModel()->location(NUM2DBL(parameter_1));
	return self;


VALUE method_rb_RNG_LogNormal_get_scale (VALUE self) {
	rb_RNG_LogNormal *r;
	
	return rb_float_new(r->getModel()->scale());


VALUE method_rb_RNG_LogNormal_get_location (VALUE self) {
	rb_RNG_LogNormal *r;
	
	return rb_float_new(r->getModel()->location());


VALUE method_rb_RNG_LogNormal_get_p (VALUE self, VALUE parameter_1) {
	rb_RNG_LogNormal *r;
	
		if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
			rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	
	return rb_float_new(r->getModel()->p(NUM2DBL(parameter_1)));


VALUE method_rb_RNG_LogNormal_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_LogNormal *r;
	
		if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
			rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	
	return rb_float_new(r->getModel()->p(NUM2DBL(parameter_1)));


VALUE method_rb_RNG_LogNormal_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_LogNormal>(
		rb_RNG_LogNormal::rb_class(),
		new rb_RNG_LogNormal()
		);
}

VALUE method_rb_RNG_LogNormal_initialize (VALUE self) {
	return self;
}

VALUE method_rb_RNG_LogNormal_sample (VALUE self) {
	rb_RNG_LogNormal *r;
	
	return rb_float_new((*r->getModel())());


void Init_rb_RNG_LogNormal () {
	rb_define_method(rb_RNG_LogNormal::rb_class(), "scale=", (rb_method) method_rb_RNG_LogNormal_set_scale, 1);
	rb_define_method(rb_RNG_LogNormal::rb_class(), "location=", (rb_method) method_rb_RNG_LogNormal_set_location, 1);
	rb_define_method(rb_RNG_LogNormal::rb_class(), "scale", (rb_method) method_rb_RNG_LogNormal_get_scale, 0);
	rb_define_method(rb_RNG_LogNormal::rb_class(), "location", (rb_method) method_rb_RNG_LogNormal_get_location, 0);
	rb_define_method(rb_RNG_LogNormal::rb_class(), "p", (rb_method) method_rb_RNG_LogNormal_get_p, 1);
	rb_define_method(rb_RNG_LogNormal::rb_class(), "prob", (rb_method) method_rb_RNG_LogNormal_get_prob, 1);
	rb_define_alloc_func(rb_RNG_LogNormal::rb_class(), (rb_alloc_func_t) method_rb_RNG_LogNormal_allocate);
	rb_define_method(rb_RNG_LogNormal::rb_class(), "initialize", (rb_method) method_rb_RNG_LogNormal_initialize, 0);
	rb_define_method(rb_RNG_LogNormal::rb_class(), "sample", (rb_method) method_rb_RNG_LogNormal_sample, 0);
}