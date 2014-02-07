#include "rb_SolutionSet.h"

extern VALUE rb_optimizer_solutionset_klass;
extern VALUE rb_optimizer_realvector_klass;

#include "rb_pointer_wrapping.extras"

rb_SolutionSet::rb_SolutionSet (RealVector const& _point, double _value) : point(_point), value(_value) {};
rb_SolutionSet::rb_SolutionSet () {};

VALUE method_solutionset_point (VALUE self) {
	rb_SolutionSet *s;
	Data_Get_Struct(self, rb_SolutionSet, s);

	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(s->point)
		);
}

VALUE method_solutionset_allocate (VALUE klass) {
	return wrap_pointer<rb_SolutionSet>(
		rb_optimizer_solutionset_klass,
		new rb_SolutionSet()
		);
}

void raise_solutionset_formaterror () {
	rb_raise(rb_eArgError, "A SolutionSet is initialized with a\n    1. RealVector, or Array, and a\n    2. Float or Fixnum.");
}

VALUE method_solutionset_initialize (VALUE self, VALUE rb_point, VALUE rb_value) {
	rb_SolutionSet *s;
	Data_Get_Struct(self, rb_SolutionSet, s);

	if (TYPE(rb_value) != T_FIXNUM && TYPE(rb_value) != T_FLOAT)
		raise_solutionset_formaterror();
	if (TYPE(rb_point) == T_DATA) {
		if (CLASS_OF(rb_point) != rb_optimizer_realvector_klass)
			raise_solutionset_formaterror();
		rb_RealVector *vec;
		Data_Get_Struct(rb_point, rb_RealVector, vec);

		s->value = NUM2DBL(rb_value);
		s->point = vec->data;

	} else if (TYPE(rb_point) == T_ARRAY) {

		s->value = NUM2DBL(rb_value);
		s->point = rb_ary_to_1d_realvector(rb_point);

	} else {
		raise_solutionset_formaterror();
	}
	return self;
}

VALUE method_solutionset_value (VALUE self) {
	rb_SolutionSet *s;
	Data_Get_Struct(self, rb_SolutionSet, s);
	return rb_float_new(s->value);
}

typedef VALUE (*rb_method)(...);

void Init_SolutionSet () {
	rb_define_alloc_func(rb_optimizer_solutionset_klass,  (rb_alloc_func_t) method_solutionset_allocate);
	rb_define_method(rb_optimizer_solutionset_klass, "initialize", (rb_method) method_solutionset_initialize, 2);
	rb_define_method(rb_optimizer_solutionset_klass, "point", (rb_method) method_solutionset_point, 0);
	rb_define_method(rb_optimizer_solutionset_klass, "value", (rb_method) method_solutionset_value, 0);
}