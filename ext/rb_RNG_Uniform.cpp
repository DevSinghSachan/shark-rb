#include "rb_RNG_Uniform.h"
#include "extras/utils/rb_pointer_wrapping.extras"

extern VALUE rb_optimizer_rng_uniform;

VALUE rb_RNG_Uniform::rb_class() {
	return rb_optimizer_rng_uniform;
}

VALUE method_rb_RNG_Uniform_set_n (VALUE self, VALUE parameter_1) {
	rb_RNG_Uniform *r;
	Data_Get_Struct(self, rb_RNG_Uniform, r);
	r->getModel()->n(parameter_1);
	return self;
}

VALUE method_rb_RNG_Uniform_set_set_range (VALUE self, VALUE parameter_1, VALUE parameter_2) {
	rb_RNG_Uniform *r;
	Data_Get_Struct(self, rb_RNG_Uniform, r);
	r->getModel()->setRange(parameter_1, parameter_2);
	return self;
}

VALUE method_rb_RNG_Uniform_get_low (VALUE self) {
	rb_RNG_Uniform *r;
	Data_Get_Struct(self, rb_RNG_Uniform, r);
	return NUM2DBL(r->getModel()->low());
}

VALUE method_rb_RNG_Uniform_get_high (VALUE self) {
	rb_RNG_Uniform *r;
	Data_Get_Struct(self, rb_RNG_Uniform, r);
	return NUM2DBL(r->getModel()->high());
}

VALUE method_rb_RNG_Uniform_get_p (VALUE self, VALUE parameter_1) {
	rb_RNG_Uniform *r;
	Data_Get_Struct(self, rb_RNG_Uniform, r);
	return NUM2DBL(r->getModel()->prob(parameter_1));
}

VALUE method_rb_RNG_Uniform_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_Uniform *r;
	Data_Get_Struct(self, rb_RNG_Uniform, r);
	return NUM2DBL(r->getModel()->prob(parameter_1));
}

VALUE method_rb_RNG_Uniform_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_Uniform>(
		rb_RNG_Uniform::rb_class(),
		new rb_RNG_Uniform()
		);
}

VALUE method_rb_RNG_Uniform_sample (VALUE self) {
	rb_RNG_Uniform *r;
	Data_Get_Struct(self, rb_RNG_Uniform, r);
	return NUM2DBL((*r->getModel())());
}

void Init_rb_RNG_Uniform () {
	rb_define_method(rb_RNG_Uniform::rb_class(), "n", (rb_method) method_rb_RNG_Uniform_set_n, 1);
	rb_define_method(rb_RNG_Uniform::rb_class(), "set_range", (rb_method) method_rb_RNG_Uniform_set_set_range, 2);
	rb_define_method(rb_RNG_Uniform::rb_class(), "low", (rb_method) method_rb_RNG_Uniform_get_low, 0);
	rb_define_method(rb_RNG_Uniform::rb_class(), "high", (rb_method) method_rb_RNG_Uniform_get_high, 0);
	rb_define_method(rb_RNG_Uniform::rb_class(), "p", (rb_method) method_rb_RNG_Uniform_get_p, 1);
	rb_define_method(rb_RNG_Uniform::rb_class(), "prob", (rb_method) method_rb_RNG_Uniform_get_prob, 1);
	rb_define_alloc_func(rb_RNG_Uniform::rb_class(), (rb_alloc_func_t) method_rb_RNG_Uniform_allocate);
	rb_define_method(rb_RNG_Uniform::rb_class(), "sample", (rb_method) method_rb_RNG_Uniform_sample, 0);
}