#include "extras/utils/rb_pointer_wrapping.extras"

VALUE method_rb_rng_seed (VALUE rb_seed) {
	Check_Type(rb_seed, T_FIXNUM);
	shark::Rng::seed(NUM2INT(rb_seed));
	return Qnil;
}

extern VALUE rb_optimizer_rng_klass;

void Init_RNG () {
	rb_define_singleton_method(rb_optimizer_rng_klass, "seed", (rb_method) method_rb_rng_seed, 1);
}