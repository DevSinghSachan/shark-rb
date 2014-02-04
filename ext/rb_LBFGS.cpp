#include "rb_LBFGS.h"

extern VALUE rb_optimizer_lbfgs_klass;

// datatypes:
extern VALUE rb_optimizer_solutionset_klass;
extern VALUE rb_optimizer_realvector_klass;

// Objective Functions:
extern VALUE rb_optimizer_objective_function_klass;
extern VALUE rb_optimizer_binarycd_klass;
extern VALUE rb_optimizer_exactgradient_klass;

rb_LBFGS::rb_LBFGS() {};

LBFGS rb_LBFGS::algorithm() {
	return _algorithm;
}

template<class Obtype> void delete_objects(Obtype *ptr){
	delete ptr;
}

template<class Obtype> VALUE wrap_pointer(VALUE klass, Obtype *ptr){
	return Data_Wrap_Struct(klass,0,delete_objects<Obtype>,ptr);
}

template<class Obtype> VALUE alloc_ob(VALUE self) {
	return wrap_pointer<Obtype>(self,new Obtype());
}


VALUE method_lbfgs_solution (VALUE self) {

	rb_LBFGS *s;
	Data_Get_Struct(self, rb_LBFGS, s);

	return wrap_pointer<rb_SolutionSet>(
		rb_optimizer_solutionset_klass,
		new rb_SolutionSet(s->algorithm().solution().point, s->algorithm().solution().value)
		);
}

VALUE method_lbfgs_allocate (VALUE klass) {
	return wrap_pointer<rb_LBFGS>(
		rb_optimizer_lbfgs_klass,
		new rb_LBFGS()
	);
};

VALUE method_lbfgs_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	return self;
};

VALUE method_lbfgs_set_history_count (VALUE self, VALUE rb_history_count) {
	Check_Type(rb_history_count, T_FIXNUM);

	rb_LBFGS *s;
	Data_Get_Struct(self, rb_LBFGS, s);

	if (NUM2INT(rb_history_count) <= 0)
		rb_raise(rb_eArgError, "LBFGS cannot have an empty history.");
	s->algorithm().setHistCount(NUM2INT(rb_history_count));

	return self;
}

VALUE method_lbfgs_init (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE rb_objective_func, rb_startpoint;

	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"11",
		&rb_objective_func,
		&rb_startpoint);

	rb_LBFGS *s;
	Data_Get_Struct(self, rb_LBFGS, s);

	Check_Type(rb_objective_func, T_DATA);
	if (rb_obj_is_kind_of(rb_objective_func, rb_optimizer_objective_function_klass) == Qtrue) {
		
		rb_LBFGS *s;
		Data_Get_Struct(self, rb_LBFGS, s);

		VALUE rb_objective_func_klass = CLASS_OF(rb_objective_func);

		if (rb_objective_func_klass == rb_optimizer_binarycd_klass) {
			rb_BinaryCD *obj;
			Data_Get_Struct(rb_objective_func, rb_BinaryCD, obj);
			if (rb_startpoint != Qnil) {

				Check_Type(rb_startpoint, T_DATA);
				if (CLASS_OF(rb_startpoint) != rb_optimizer_realvector_klass)
					rb_raise(rb_eArgError, "LBFGS is initialized using a RealVector.");
				rb_RealVector *v;
				Data_Get_Struct(rb_startpoint, rb_RealVector, v);
				
				s->algorithm().init(obj->objective(), v->data);
			} else {
				s->algorithm().init(obj->objective());
			}
		} else if (rb_objective_func_klass == rb_optimizer_exactgradient_klass) {
			rb_ExactGradient *obj;
			Data_Get_Struct(rb_objective_func, rb_ExactGradient, obj);
			if (rb_startpoint != Qnil) {

				Check_Type(rb_startpoint, T_DATA);
				if (CLASS_OF(rb_startpoint) != rb_optimizer_realvector_klass)
					rb_raise(rb_eArgError, "LBFGS is initialized using a RealVector.");
				rb_RealVector *v;
				Data_Get_Struct(rb_startpoint, rb_RealVector, v);
				
				s->algorithm().init(obj->objective(), v->data);
			} else {
				s->algorithm().init(obj->objective());
			}
		} else {
			rb_raise(rb_eArgError, "Unsupported ObjectiveFunction.");
		}
	} else {
		rb_raise(rb_eArgError, "Can only step using an ObjectiveFunction object.");
	}
	return self;
}
VALUE method_lbfgs_step (VALUE self, VALUE rb_objective_func) {

	Check_Type(rb_objective_func, T_DATA);
	if (rb_obj_is_kind_of(rb_objective_func, rb_optimizer_objective_function_klass) == Qtrue) {
		
		rb_LBFGS *s;
		Data_Get_Struct(self, rb_LBFGS, s);

		VALUE rb_objective_func_klass = CLASS_OF(rb_objective_func);
		if (rb_objective_func_klass == rb_optimizer_binarycd_klass) {
			rb_BinaryCD *obj;
			Data_Get_Struct(rb_objective_func, rb_BinaryCD, obj);
			s->algorithm().step(obj->objective());
		} else if (rb_objective_func_klass == rb_optimizer_exactgradient_klass) {
			rb_ExactGradient *obj;
			Data_Get_Struct(rb_objective_func, rb_ExactGradient, obj);
			s->algorithm().step(obj->objective());
		} else {
			rb_raise(rb_eArgError, "Unsupported ObjectiveFunction.");
		}
	} else {
		rb_raise(rb_eArgError, "Can only step using an ObjectiveFunction object.");
	}

	return self;
}

typedef VALUE (*rb_method)(...);

void Init_LBFGS () {

	rb_define_alloc_func(rb_optimizer_lbfgs_klass,  (rb_alloc_func_t) method_lbfgs_allocate);
	rb_define_method(rb_optimizer_lbfgs_klass, "solution", (rb_method) method_lbfgs_solution, 0);
	rb_define_method(rb_optimizer_lbfgs_klass, "step", (rb_method) method_lbfgs_step, 1);
	rb_define_method(rb_optimizer_lbfgs_klass, "init", (rb_method) method_lbfgs_init, -1);
	rb_define_method(rb_optimizer_lbfgs_klass, "initialize", (rb_method) method_lbfgs_initialize, -1);
	rb_define_method(rb_optimizer_lbfgs_klass, "history_count=", (rb_method) method_lbfgs_set_history_count, 1);

}