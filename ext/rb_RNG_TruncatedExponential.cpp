#include "rb_RNG_TruncatedExponential.h"
#include "extras/utils/rb_pointer_wrapping.extras"

rb_RNG_TruncatedExponential::rb_RNG_TruncatedExponential() : model(shark::Rng::globalRng) {}

shark::TruncatedExponential< shark::Rng::rng_type > * rb_RNG_TruncatedExponential::getDistribution() {
	return &model;
}

shark::TruncatedExponential< shark::Rng::rng_type > * rb_RNG_TruncatedExponential::getModel() {
	return &model;
}

extern VALUE rb_optimizer_rng_truncatedexponential_klass;

VALUE rb_RNG_TruncatedExponential::rb_class() {
	return rb_optimizer_rng_truncatedexponential_klass;
}

VALUE method_rb_RNG_TruncatedExponential_set_lambda (VALUE self, VALUE parameter_1) {
	rb_RNG_TruncatedExponential *r;
	
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");

	r->getDistribution()->setLambda(NUM2DBL(parameter_1));
	return self;


VALUE method_rb_RNG_TruncatedExponential_set_max (VALUE self, VALUE parameter_1) {
	rb_RNG_TruncatedExponential *r;
	
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");

	r->getDistribution()->setMax(NUM2DBL(parameter_1));
	return self;


VALUE method_rb_RNG_TruncatedExponential_get_lambda (VALUE self) {
	rb_RNG_TruncatedExponential *r;
	
	return rb_float_new(r->getDistribution()->lambda());


VALUE method_rb_RNG_TruncatedExponential_get_max (VALUE self) {
	rb_RNG_TruncatedExponential *r;
	
	return rb_float_new(r->getDistribution()->max());


VALUE method_rb_RNG_TruncatedExponential_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_TruncatedExponential *r;
	
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");

	return rb_float_new(r->getDistribution()->p(NUM2DBL(parameter_1)));


VALUE method_rb_RNG_TruncatedExponential_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_TruncatedExponential>(
		rb_RNG_TruncatedExponential::rb_class(),
		new rb_RNG_TruncatedExponential()
		);
}

VALUE method_rb_RNG_TruncatedExponential_initialize (VALUE self) {
	return self;
}

VALUE method_rb_RNG_TruncatedExponential_sample (VALUE self) {
	rb_RNG_TruncatedExponential *r;
	
	return rb_float_new((*r->getDistribution())());


void Init_rb_RNG_TruncatedExponential () {
	rb_define_method(rb_RNG_TruncatedExponential::rb_class(), "lambda=", (rb_method) method_rb_RNG_TruncatedExponential_set_lambda, 1);
	rb_define_method(rb_RNG_TruncatedExponential::rb_class(), "max=", (rb_method) method_rb_RNG_TruncatedExponential_set_max, 1);
	rb_define_method(rb_RNG_TruncatedExponential::rb_class(), "lambda", (rb_method) method_rb_RNG_TruncatedExponential_get_lambda, 0);
	rb_define_method(rb_RNG_TruncatedExponential::rb_class(), "max", (rb_method) method_rb_RNG_TruncatedExponential_get_max, 0);
	rb_define_method(rb_RNG_TruncatedExponential::rb_class(), "prob", (rb_method) method_rb_RNG_TruncatedExponential_get_prob, 1);
	rb_define_alloc_func(rb_RNG_TruncatedExponential::rb_class(), (rb_alloc_func_t) method_rb_RNG_TruncatedExponential_allocate);
	rb_define_method(rb_RNG_TruncatedExponential::rb_class(), "initialize", (rb_method) method_rb_RNG_TruncatedExponential_initialize, 0);
	rb_define_method(rb_RNG_TruncatedExponential::rb_class(), "sample", (rb_method) method_rb_RNG_TruncatedExponential_sample, 0);
}