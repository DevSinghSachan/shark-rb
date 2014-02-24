#include "rb_RNG_Weibull.h"
#include "extras/utils/rb_pointer_wrapping.extras"

rb_RNG_Weibull::rb_RNG_Weibull() : model(shark::Rng::globalRng) {}

shark::Weibull< shark::Rng::rng_type > * rb_RNG_Weibull::getModel() {
	return &model;
}

extern VALUE rb_optimizer_rng_weibull_klass;

VALUE rb_RNG_Weibull::rb_class() {
	return rb_optimizer_rng_weibull_klass;
}

VALUE method_rb_RNG_Weibull_set_alpha (VALUE self, VALUE parameter_1) {
	rb_RNG_Weibull *r;
	
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");

	r->getModel()->alpha(NUM2DBL(parameter_1));
	return self;


VALUE method_rb_RNG_Weibull_set_vbeta (VALUE self, VALUE parameter_1) {
	rb_RNG_Weibull *r;
	
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");

	r->getModel()->vbeta(NUM2DBL(parameter_1));
	return self;


VALUE method_rb_RNG_Weibull_set_beta (VALUE self, VALUE parameter_1) {
	rb_RNG_Weibull *r;
	
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");

	r->getModel()->vbeta(NUM2DBL(parameter_1));
	return self;


VALUE method_rb_RNG_Weibull_get_beta (VALUE self) {
	rb_RNG_Weibull *r;
	
	return rb_float_new(r->getModel()->beta());


VALUE method_rb_RNG_Weibull_get_alpha (VALUE self) {
	rb_RNG_Weibull *r;
	
	return rb_float_new(r->getModel()->alpha());


VALUE method_rb_RNG_Weibull_get_p (VALUE self, VALUE parameter_1) {
	rb_RNG_Weibull *r;
	
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");

	return rb_float_new(r->getModel()->p(NUM2DBL(parameter_1)));


VALUE method_rb_RNG_Weibull_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_Weibull *r;
	
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");

	return rb_float_new(r->getModel()->p(NUM2DBL(parameter_1)));


VALUE method_rb_RNG_Weibull_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_Weibull>(
		rb_RNG_Weibull::rb_class(),
		new rb_RNG_Weibull()
		);
}

VALUE method_rb_RNG_Weibull_initialize (VALUE self) {
	return self;
}

VALUE method_rb_RNG_Weibull_sample (VALUE self) {
	rb_RNG_Weibull *r;
	
	return rb_float_new((*r->getModel())());


void Init_rb_RNG_Weibull () {
	rb_define_method(rb_RNG_Weibull::rb_class(), "alpha=", (rb_method) method_rb_RNG_Weibull_set_alpha, 1);
	rb_define_method(rb_RNG_Weibull::rb_class(), "vbeta=", (rb_method) method_rb_RNG_Weibull_set_vbeta, 1);
	rb_define_method(rb_RNG_Weibull::rb_class(), "beta=", (rb_method) method_rb_RNG_Weibull_set_beta, 1);
	rb_define_method(rb_RNG_Weibull::rb_class(), "beta", (rb_method) method_rb_RNG_Weibull_get_beta, 0);
	rb_define_method(rb_RNG_Weibull::rb_class(), "alpha", (rb_method) method_rb_RNG_Weibull_get_alpha, 0);
	rb_define_method(rb_RNG_Weibull::rb_class(), "p", (rb_method) method_rb_RNG_Weibull_get_p, 1);
	rb_define_method(rb_RNG_Weibull::rb_class(), "prob", (rb_method) method_rb_RNG_Weibull_get_prob, 1);
	rb_define_alloc_func(rb_RNG_Weibull::rb_class(), (rb_alloc_func_t) method_rb_RNG_Weibull_allocate);
	rb_define_method(rb_RNG_Weibull::rb_class(), "initialize", (rb_method) method_rb_RNG_Weibull_initialize, 0);
	rb_define_method(rb_RNG_Weibull::rb_class(), "sample", (rb_method) method_rb_RNG_Weibull_sample, 0);
}