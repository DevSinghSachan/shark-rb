#include "rb_SteepestDescent.h"

extern VALUE rb_optimizer_steepestdescent_klass;
// Datatypes:
extern VALUE rb_optimizer_realvector_klass;
extern VALUE rb_optimizer_solutionset_klass;

// Objective Functions:
extern VALUE rb_optimizer_objective_function_klass;
extern VALUE rb_optimizer_binarycd_klass;
extern VALUE rb_optimizer_exactgradient_klass;

rb_SteepestDescent::rb_SteepestDescent() {};

SteepestDescent& rb_SteepestDescent::algorithm() {
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

	s->algorithm().setMomentum(NUM2DBL(rb_momentum));

	return self;
}

VALUE method_steepestdescent_get_momentum (VALUE self) {
	rb_SteepestDescent *s;
	Data_Get_Struct(self, rb_SteepestDescent, s);

	return rb_float_new(s->algorithm().momentum());
}

VALUE method_steepestdescent_set_learning_rate (VALUE self, VALUE rb_learning_rate) {
	if (TYPE(rb_learning_rate) != T_FIXNUM && TYPE(rb_learning_rate) != T_FLOAT)
		rb_raise(rb_eArgError, "learning_rate is set using a Float of Fixnum.");

	rb_SteepestDescent *s;
	Data_Get_Struct(self, rb_SteepestDescent, s);

	s->algorithm().setLearningRate(NUM2DBL(rb_learning_rate));

	return self;
}

VALUE method_steepestdescent_init (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE rb_objective_func, rb_startpoint;

	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"11",
		&rb_objective_func,
		&rb_startpoint);

	rb_SteepestDescent *s;
	Data_Get_Struct(self, rb_SteepestDescent, s);

	Check_Type(rb_objective_func, T_DATA);
	if (rb_obj_is_kind_of(rb_objective_func, rb_optimizer_objective_function_klass) == Qtrue) {
		
		rb_SteepestDescent *s;
		Data_Get_Struct(self, rb_SteepestDescent, s);

		VALUE rb_objective_func_klass = CLASS_OF(rb_objective_func);

		if (rb_objective_func_klass == rb_optimizer_binarycd_klass) {
			rb_BinaryCD *obj;
			Data_Get_Struct(rb_objective_func, rb_BinaryCD, obj);
			if (rb_startpoint != Qnil) {

				Check_Type(rb_startpoint, T_DATA);
				if (CLASS_OF(rb_startpoint) != rb_optimizer_realvector_klass)
					rb_raise(rb_eArgError, "Steepest Descent is initialized using a RealVector.");
				rb_RealVector *v;
				Data_Get_Struct(rb_startpoint, rb_RealVector, v);
				
				s->algorithm().init(obj->objective(), v->data);
			} else {
				RealVector startingPoint;
				obj->objective().proposeStartingPoint(startingPoint);

				cout << "obj->objective().proposeStartingPoint(startingPoint) = " << startingPoint << endl;

				s->algorithm().init(obj->objective());
			}
		} else if (rb_objective_func_klass == rb_optimizer_exactgradient_klass) {
			rb_ExactGradient *obj;
			Data_Get_Struct(rb_objective_func, rb_ExactGradient, obj);
			if (rb_startpoint != Qnil) {

				Check_Type(rb_startpoint, T_DATA);
				if (CLASS_OF(rb_startpoint) != rb_optimizer_realvector_klass)
					rb_raise(rb_eArgError, "Steepest Descent is initialized using a RealVector.");
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

VALUE method_steepestdescent_get_learning_rate (VALUE self) {
	rb_SteepestDescent *s;
	Data_Get_Struct(self, rb_SteepestDescent, s);

	return rb_float_new(s->algorithm().learningRate());
}

VALUE method_steepestdescent_solution (VALUE self) {
	rb_SteepestDescent *s;
	Data_Get_Struct(self, rb_SteepestDescent, s);

	return wrap_pointer<rb_SolutionSet>(
		rb_optimizer_solutionset_klass,
		new rb_SolutionSet(s->algorithm().solution().point, s->algorithm().solution().value)
		);
}

VALUE method_steepestdescent_step (VALUE self, VALUE rb_objective_func) {

	Check_Type(rb_objective_func, T_DATA);
	if (rb_obj_is_kind_of(rb_objective_func, rb_optimizer_objective_function_klass) == Qtrue) {
		
		rb_SteepestDescent *s;
		Data_Get_Struct(self, rb_SteepestDescent, s);

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

void Init_Steepest_Descent () {

	rb_define_alloc_func(rb_optimizer_steepestdescent_klass,  (rb_alloc_func_t) method_steepestdescent_allocate);
	rb_define_method(rb_optimizer_steepestdescent_klass, "step", (rb_method) method_steepestdescent_step, 1);
	rb_define_method(rb_optimizer_steepestdescent_klass, "init", (rb_method) method_steepestdescent_init, -1);
	rb_define_method(rb_optimizer_steepestdescent_klass, "solution", (rb_method) method_steepestdescent_solution, 0);
	rb_define_method(rb_optimizer_steepestdescent_klass, "initialize", (rb_method) method_steepestdescent_initialize, -1);
	rb_define_method(rb_optimizer_steepestdescent_klass, "momentum=", (rb_method) method_steepestdescent_set_momentum, 1);
	rb_define_method(rb_optimizer_steepestdescent_klass, "momentum", (rb_method) method_steepestdescent_get_momentum, 0);
	rb_define_method(rb_optimizer_steepestdescent_klass, "learning_rate=", (rb_method) method_steepestdescent_set_learning_rate, 1);
	rb_define_method(rb_optimizer_steepestdescent_klass, "learning_rate", (rb_method) method_steepestdescent_get_learning_rate, 0);

}