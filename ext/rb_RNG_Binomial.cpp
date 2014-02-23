#include "rb_RNG_Binomial.h"
#include "extras/utils/rb_pointer_wrapping.extras"

extern VALUE rb_optimizer_rng_binomial;

VALUE rb_RNG_Binomial::rb_class() {
	return rb_optimizer_rng_binomial;
}

VALUE method_rb_RNG_Binomial_set_n (VALUE self, VALUE parameter_1) {
	rb_RNG_Binomial *r;
	Data_Get_Struct(self, rb_RNG_Binomial, r);
	r->getModel()->n(parameter_1);
	return self;
}

VALUE method_rb_RNG_Binomial_set_p (VALUE self, VALUE parameter_1) {
	rb_RNG_Binomial *r;
	Data_Get_Struct(self, rb_RNG_Binomial, r);
	r->getModel()->p(parameter_1);
	return self;
}

VALUE method_rb_RNG_Binomial_set_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_Binomial *r;
	Data_Get_Struct(self, rb_RNG_Binomial, r);
	r->getModel()->p(parameter_1);
	return self;
}

VALUE method_rb_RNG_Binomial_get_n (VALUE self) {
	rb_RNG_Binomial *r;
	Data_Get_Struct(self, rb_RNG_Binomial, r);
	return NUM2DBL(r->getModel()->n());
}

VALUE method_rb_RNG_Binomial_get_prob (VALUE self) {
	rb_RNG_Binomial *r;
	Data_Get_Struct(self, rb_RNG_Binomial, r);
	return NUM2DBL(r->getModel()->prob());
}

VALUE method_rb_RNG_Binomial_get_p (VALUE self) {
	rb_RNG_Binomial *r;
	Data_Get_Struct(self, rb_RNG_Binomial, r);
	return NUM2DBL(r->getModel()->prob());
}

VALUE method_rb_RNG_Binomial_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_Binomial>(
		rb_RNG_Binomial::rb_class(),
		new rb_RNG_Binomial()
		);
}

VALUE method_rb_RNG_Binomial_sample (VALUE self) {
	rb_RNG_Binomial *r;
	Data_Get_Struct(self, rb_RNG_Binomial, r);
	return NUM2DBL((*r->getModel())());
}

void Init_rb_RNG_Binomial () {
	rb_define_method(rb_RNG_Binomial::rb_class(), "n", (rb_method) method_rb_RNG_Binomial_set_n, 1);
	rb_define_method(rb_RNG_Binomial::rb_class(), "p", (rb_method) method_rb_RNG_Binomial_set_p, 1);
	rb_define_method(rb_RNG_Binomial::rb_class(), "prob", (rb_method) method_rb_RNG_Binomial_set_prob, 1);
	rb_define_method(rb_RNG_Binomial::rb_class(), "n", (rb_method) method_rb_RNG_Binomial_get_n, 0);
	rb_define_method(rb_RNG_Binomial::rb_class(), "prob", (rb_method) method_rb_RNG_Binomial_get_prob, 0);
	rb_define_method(rb_RNG_Binomial::rb_class(), "p", (rb_method) method_rb_RNG_Binomial_get_p, 0);
	rb_define_alloc_func(rb_RNG_Binomial::rb_class(), (rb_alloc_func_t) method_rb_RNG_Binomial_allocate);
	rb_define_method(rb_RNG_Binomial::rb_class(), "sample", (rb_method) method_rb_RNG_Binomial_sample, 0);
}