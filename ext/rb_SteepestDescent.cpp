#include "rb_SteepestDescent.h"


// VALUE rb_optimizer_klass                 = rb_define_class("Optimizer", rb_cObject);
// VALUE rb_algorithms_module               = rb_define_module_under(rb_optimizer_klass, "Algorithms");
// VALUE rb_optimizer_steepestdescent_klass = rb_define_class_under(rb_algorithms_module, "SteepestDescent", rb_cObject);

extern VALUE rb_optimizer_steepestdescent_klass;

rb_SteepestDescent::rb_SteepestDescent() {};

template<class Obtype> void delete_objects(Obtype *ptr){
	delete ptr;
}

template<class Obtype> VALUE wrap_pointer(VALUE klass, Obtype *ptr){
	return Data_Wrap_Struct(klass,0,delete_objects<Obtype>,ptr);
}

template<class Obtype> VALUE alloc_ob(VALUE self) {
	return wrap_pointer<Obtype>(self,new Obtype());
}

VALUE method_steepestdescent_allocate (VALUE klass) {
	return wrap_pointer<rb_SteepestDescent>(
		rb_optimizer_steepestdescent_klass,
		new rb_SteepestDescent()
	);
};

VALUE method_steepestdescent_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	return self;
};

VALUE method_steepestdescent_set_momentum (VALUE self, VALUE rb_momentum) {
	if (TYPE(rb_momentum) != T_FIXNUM && TYPE(rb_momentum) != T_FLOAT)
		rb_raise(rb_eArgError, "Momentum is set using a Float of Fixnum.");

	rb_SteepestDescent *s;
	Data_Get_Struct(self, rb_SteepestDescent, s);

	s->algorithm.setMomentum(NUM2DBL(rb_momentum));

	return self;
}

VALUE method_steepestdescent_get_momentum (VALUE self) {
	rb_SteepestDescent *s;
	Data_Get_Struct(self, rb_SteepestDescent, s);

	return rb_float_new(s->algorithm.momentum());
}

VALUE method_steepestdescent_step (VALUE self, VALUE rb_objective_func) {

	Check_Type(rb_objective_func, T_DATA);
	if (rb_obj_is_kind_of(rb_objective_func, rb_optimizer_objective_function_klass) == Qtrue) {
		rb_SteepestDescent *s;
		Data_Get_Struct(self, rb_SteepestDescent, s);

		rb_ObjectiveFunc *f;
		Data_Get_Struct(rb_objective_func, rb_ObjectiveFunc, f);

		s->step(f->objective);

	} else {
		rb_raise(rb_eArgError, "Can only step using an ObjectiveFunction object.");
	}

	return self;

}

typedef VALUE (*rb_method)(...);

//rb_obj_is_kind_of answers inheritance questions

void Init_Steepest_Descent () {

	rb_define_alloc_func(rb_optimizer_steepestdescent_klass,  (rb_alloc_func_t) method_steepestdescent_allocate);
	rb_define_method(rb_optimizer_steepestdescent_klass, "step", (rb_method) method_steepestdescent_step, 1);
	rb_define_method(rb_optimizer_steepestdescent_klass, "initialize", (rb_method) method_steepestdescent_initialize, -1);
	rb_define_method(rb_optimizer_steepestdescent_klass, "momentum=", (rb_method) method_steepestdescent_set_momentum, 1);
	rb_define_method(rb_optimizer_steepestdescent_klass, "momentum", (rb_method) method_steepestdescent_get_momentum, 0);

}