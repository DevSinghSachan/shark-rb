#include "rb_ExactGradient.h"

extern VALUE rb_optimizer_exactgradient_klass;
extern VALUE rb_optimizer_unlabeleddata_klass;
extern VALUE rb_optimizer_binaryrbm_klass;
extern VALUE rb_optimizer_solutionset_klass;

using namespace shark;
using namespace std;

template<class Obtype> void delete_objects(Obtype *ptr){
	delete ptr;
}

template<class Obtype> VALUE wrap_pointer(VALUE klass, Obtype *ptr){
	return Data_Wrap_Struct(klass,0,delete_objects<Obtype>,ptr);
}

template<class Obtype> VALUE alloc_ob(VALUE self) {
	return wrap_pointer<Obtype>(self,new Obtype());
}

rb_ExactGradient::rb_ExactGradient(BinaryRBM &rbm): _objective(&rbm) {};

ExactGradient<BinaryRBM>& rb_ExactGradient::objective() {
	return _objective;
}

void static raise_objective_func_data_error () {
	rb_raise(rb_eArgError, "An objective function's data can only be set using UnlabeledData or Arrays.");
}

VALUE method_exactgradient_allocate (VALUE klass) {
	rb_ExactGradient *ptr = (rb_ExactGradient*) malloc(sizeof(rb_ExactGradient));
	return Data_Wrap_Struct(klass, 0, delete_objects<rb_ExactGradient>, ptr);
}

VALUE method_exactgradient_initialize (VALUE self, VALUE rb_rbm) {
	rb_ExactGradient *b;
	Data_Get_Struct(self, rb_ExactGradient, b);

	Check_Type(rb_rbm, T_DATA);

	if (CLASS_OF(rb_rbm) != rb_optimizer_binaryrbm_klass) 
		rb_raise(rb_eArgError, "Exact Gradient is initiliazed using a Binary RBM.");

	rb_BinaryRBM *r;
	Data_Get_Struct(rb_rbm, rb_BinaryRBM, r);
	rb_ExactGradient *bupdated = new(b) rb_ExactGradient(r->rbm);

	return self;
}

VALUE method_exactgradient_get_number_of_variables (VALUE self) {
	rb_ExactGradient *b;
	Data_Get_Struct(self, rb_ExactGradient, b);
	return INT2FIX(b->objective().numberOfVariables());
}

VALUE method_exactgradient_set_data (VALUE self, VALUE rb_data) {
	if (TYPE(rb_data) == T_DATA) {

		if (CLASS_OF(rb_data) != rb_optimizer_unlabeleddata_klass)
			raise_objective_func_data_error();

		rb_ExactGradient *b;
		Data_Get_Struct(self, rb_ExactGradient, b);

		rb_UnlabeledData *d;
		Data_Get_Struct(rb_data, rb_UnlabeledData, d);

		b->objective().setData(d->data);
	} else if (TYPE(rb_data) == T_ARRAY) {

		rb_ExactGradient *b;
		Data_Get_Struct(self, rb_ExactGradient, b);

		b->objective().setData(rb_ary_to_unlabeleddata(rb_data));

	} else {
		raise_objective_func_data_error();
	}
	return self;
};

typedef VALUE (*rb_method)(...);

void Init_ExactGradient () {
	rb_define_alloc_func(rb_optimizer_exactgradient_klass, (rb_alloc_func_t) method_exactgradient_allocate);
	rb_define_method(rb_optimizer_exactgradient_klass, "data=", (rb_method) method_exactgradient_set_data, 1);
	rb_define_method(rb_optimizer_exactgradient_klass, "initialize", (rb_method) method_exactgradient_initialize, 1);
	rb_define_method(rb_optimizer_exactgradient_klass, "data=", (rb_method) method_exactgradient_set_data, 1);
	rb_define_method(rb_optimizer_exactgradient_klass, "number_of_variables", (rb_method) method_exactgradient_get_number_of_variables, 0);
}