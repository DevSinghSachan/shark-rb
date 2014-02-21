#include "rb_BinaryCD.h"
#include "extras/utils/rb_pointer_wrapping.extras"

extern VALUE rb_optimizer_binarycd_klass;

using namespace shark;
using namespace std;

VALUE rb_BinaryCD::rb_class() {
	return rb_optimizer_binarycd_klass;
}

rb_BinaryCD::rb_BinaryCD(BinaryRBM &rbm): _objective(&rbm) {};

BinaryCD& rb_BinaryCD::objective() {
	return _objective;
}

void static raise_objective_func_data_error () {
	rb_raise(rb_eArgError, "An objective function's data can only be set using UnlabeledData or Arrays.");
}

VALUE method_binarycd_allocate (VALUE klass) {
	rb_BinaryCD *ptr = (rb_BinaryCD*) malloc(sizeof(rb_BinaryCD));
	return Data_Wrap_Struct(klass, 0, delete_objects<rb_BinaryCD>, ptr);
}

VALUE method_binarycd_propose_starting_point (VALUE self) {
	rb_BinaryCD *b;
	Data_Get_Struct(self, rb_BinaryCD, b);
	RealVector vec;
	b->objective().proposeStartingPoint(vec);

	return wrap_pointer<rb_RealVector>(
		rb_RealVector::rb_class(),
		new rb_RealVector(vec)
		);
}

VALUE method_binarycd_initialize (VALUE self, VALUE rb_rbm) {
	rb_BinaryCD *b;
	Data_Get_Struct(self, rb_BinaryCD, b);

	Check_Type(rb_rbm, T_DATA);

	if (CLASS_OF(rb_rbm) != rb_BinaryRBM::rb_class()) 
		rb_raise(rb_eArgError, "BinaryCD (Contrastive Divergence) is initiliazed using a Binary RBM.");

	rb_BinaryRBM *r;
	Data_Get_Struct(rb_rbm, rb_BinaryRBM, r);
	// Placement new puts an object in a pre-allocated memory area.
	rb_BinaryCD *bplaced = new(b) rb_BinaryCD(r->model);

	return self;
}

VALUE method_binarycd_set_k (VALUE self, VALUE rb_k) {
	rb_BinaryCD *b;
	Data_Get_Struct(self, rb_BinaryCD, b);

	Check_Type(rb_k, T_FIXNUM);
	b->objective().setK(NUM2INT(rb_k));

	return self;
}

VALUE method_binarycd_get_number_of_variables (VALUE self) {
	rb_BinaryCD *b;
	Data_Get_Struct(self, rb_BinaryCD, b);
	return INT2FIX(b->objective().numberOfVariables());
}

VALUE method_binarycd_set_data (VALUE self, VALUE rb_data) {
	if (TYPE(rb_data) == T_DATA) {

		if (CLASS_OF(rb_data) != rb_UnlabeledData::rb_class())
			raise_objective_func_data_error();

		rb_BinaryCD *b;
		Data_Get_Struct(self, rb_BinaryCD, b);

		rb_UnlabeledData *d;
		Data_Get_Struct(rb_data, rb_UnlabeledData, d);

		b->objective().setData(d->data);
	} else if (TYPE(rb_data) == T_ARRAY) {

		rb_BinaryCD *b;
		Data_Get_Struct(self, rb_BinaryCD, b);

		b->objective().setData(rb_ary_to_unlabeleddata(rb_data));

	} else {
		raise_objective_func_data_error();
	}
	return self;
};

void Init_BinaryCD () {
	rb_define_alloc_func(rb_optimizer_binarycd_klass, (rb_alloc_func_t) method_binarycd_allocate);
	rb_define_method(rb_optimizer_binarycd_klass, "data=", (rb_method) method_binarycd_set_data, 1);
	rb_define_method(rb_optimizer_binarycd_klass, "propose_starting_point", (rb_method) method_binarycd_propose_starting_point, 0);
	rb_define_method(rb_optimizer_binarycd_klass, "initialize", (rb_method) method_binarycd_initialize, 1);
	rb_define_method(rb_optimizer_binarycd_klass, "data=", (rb_method) method_binarycd_set_data, 1);
	rb_define_method(rb_optimizer_binarycd_klass, "number_of_variables", (rb_method) method_binarycd_get_number_of_variables, 0);
	rb_define_method(rb_optimizer_binarycd_klass, "k=", (rb_method) method_binarycd_set_k, 1);
}