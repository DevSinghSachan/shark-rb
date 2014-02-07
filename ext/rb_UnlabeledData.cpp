#include "rb_UnlabeledData.h"

using namespace shark;
using namespace std;

extern VALUE rb_optimizer_unlabeleddata_klass;
extern VALUE rb_optimizer_realvector_klass;
extern VALUE rb_optimizer_realmatrix_klass;

#include "wrappers.extras"

rb_UnlabeledData::rb_UnlabeledData(UnlabeledData<RealVector> _data) {
	data = _data;
}
rb_UnlabeledData::rb_UnlabeledData() {
}
vector<RealVector> rb_UnlabeledData::input () {
	vector<RealVector> my_input(data.numberOfElements());
	for (size_t i=0; i<data.numberOfElements(); i++)
	{
		RealVector output(data.element(i));
		my_input[i] = output;
	}
	return my_input;
}

void rb_UnlabeledData::remove_NaN(double replacement = 0.0) {
	BOOST_FOREACH(UnlabeledData<RealVector>::element_reference vector, data.elements()) {
		for (size_t i = 0;i< vector.size();i++)
			if (vector(i) != vector(i))
				vector(i) = replacement;
	}
}

void rb_UnlabeledData::fill(double replacement) {
	BOOST_FOREACH(UnlabeledData<RealVector>::element_reference vector, data.elements()) {
		std::fill(vector.begin(), vector.end(), replacement);
	}
}

VALUE method_unlabeleddata_remove_NaN (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE rb_replacement;
	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"01",
		&rb_replacement);
	double replacement = 0.0;
	if (rb_replacement != Qnil) {
		if (TYPE(rb_replacement) != T_FLOAT && TYPE(rb_replacement) != T_FIXNUM)
			rb_raise(rb_eArgError, "Can only replace NaN by floats.");
		replacement = NUM2DBL(rb_replacement);
	}
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);
	s->remove_NaN(replacement);
	return self;
}

VALUE method_unlabeleddata_to_ary (VALUE self) {
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);
	return stdvector_realvector_to_rb_ary(s->input());
}

VALUE method_unlabeleddata_allocate (VALUE klass) {
	return wrap_pointer<rb_UnlabeledData>(
			rb_optimizer_unlabeleddata_klass,
			new rb_UnlabeledData()
		);
}

VALUE method_unlabeleddata_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE dataset;
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);

	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"01",
		&dataset);

	if (TYPE(dataset) == T_ARRAY)
		s->data = rb_ary_to_unlabeleddata(dataset);

	return self;
}


VALUE method_unlabeleddata_length (VALUE self) {
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);
	return INT2FIX((s->data).numberOfElements());
}
VALUE method_unlabeleddata_batchlength (VALUE self) {
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);
	return INT2FIX((s->data).numberOfBatches());
}
VALUE method_unlabeleddata_empty (VALUE self) {
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);
	if ((s->data).empty())
		return Qtrue;
	return Qfalse;
}
VALUE method_unlabeleddata_fill (VALUE self, VALUE filling) {
	if (TYPE(filling) != T_FLOAT && TYPE(filling) != T_FIXNUM)
		rb_raise(rb_eArgError, "Can only fill UnlabeledData with floats.");
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);
	s->fill(NUM2DBL(filling));
	return self;
}
VALUE method_unlabeleddata_query (VALUE self, VALUE position) {
	Check_Type(position, T_FIXNUM);
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);
	if (NUM2INT(position) < 0) {
		if (NUM2INT(position) < (int)-(s->data).numberOfElements())
			rb_raise(rb_eArgError, "Out of range.");
		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector((s->data).element((s->data).numberOfElements() + NUM2INT(position)))
		);
	} else {
		if (NUM2INT(position) >= (int)(s->data).numberOfElements())
			rb_raise(rb_eArgError, "Out of range.");
		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector((s->data).element(NUM2INT(position)))
		);
	}
}

VALUE method_unlabeleddata_insert (VALUE self, VALUE position, VALUE assignment) {
	Check_Type(assignment, T_DATA);
	if (CLASS_OF(assignment) != rb_optimizer_realvector_klass)
		rb_raise(rb_eArgError, "Can only insert a RealVector.");
	Check_Type(position, T_FIXNUM);
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);
	rb_RealVector *a;
	Data_Get_Struct(assignment, rb_RealVector, a);
	if (NUM2INT(position) < 0) {
		if (NUM2INT(position) < (int)-(s->data).numberOfElements())
			rb_raise(rb_eArgError, "Out of range.");
		(s->data).element((s->data).numberOfElements() + NUM2INT(position)) = a->data;
	} else {
		if (NUM2INT(position) >= (int)(s->data).numberOfElements())
			rb_raise(rb_eArgError, "Out of range.");
		(s->data).element(NUM2INT(position)) = a->data;
	}
	return self;
}


// We wrap the mean method of Shark with a realvector
VALUE method_unlabeleddata_mean (VALUE self) {
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);
	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(mean(s->data))
	);
}
// We wrap the variance method of Shark with a realvector
VALUE method_unlabeleddata_variance (VALUE self) {
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);
	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(variance(s->data))
	);
}

VALUE method_unlabeleddata_covariance (VALUE self) {
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);
	return wrap_pointer<rb_RealMatrix>(
		rb_optimizer_realmatrix_klass,
		new rb_RealMatrix(covariance(s->data))
	);
}

VALUE method_unlabeleddata_shift (VALUE self, VALUE shift_vector) {
	Check_Type(shift_vector, T_DATA);
	if (CLASS_OF(shift_vector) != rb_optimizer_realvector_klass)
		rb_raise(rb_eArgError, "Shift can only be performed by a RealVector.");

	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);

	rb_RealVector *v;
	Data_Get_Struct(shift_vector, rb_RealVector, v);
	s->data = transform(s->data, shark::Shift(-(v->data)));
	return self;
}



VALUE method_unlabeleddata_posshift (VALUE self, VALUE shift_vector) {
	Check_Type(shift_vector, T_DATA);
	if (CLASS_OF(shift_vector) != rb_optimizer_realvector_klass)
		rb_raise(rb_eArgError, "Shift can only be performed by a RealVector.");
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);

	rb_RealVector *v;
	Data_Get_Struct(shift_vector, rb_RealVector, v);
	s->data = transform(s->data, shark::Shift((v->data)));
	return self;
}

VALUE method_unlabeleddata_truncate (VALUE self, VALUE minX, VALUE minY) {
	Check_Type(minX, T_DATA);
	Check_Type(minY, T_DATA);
	if (CLASS_OF(minX) != rb_optimizer_realvector_klass || CLASS_OF(minX) != rb_optimizer_realvector_klass)
		rb_raise(rb_eArgError, "Can only truncate using a RealVector.");
	// could also check classes...

	rb_UnlabeledData *s;
	rb_RealVector *minVX, *minVY;

	Data_Get_Struct(self, rb_UnlabeledData, s);
	Data_Get_Struct(minX, rb_RealVector,    minVX);
	Data_Get_Struct(minY, rb_RealVector,    minVY);

	s->data = transform(s->data, shark::Truncate(minVX->data, minVY->data));
	return self;
}


VALUE method_unlabeleddata_truncate_and_rescale (VALUE self, VALUE minX, VALUE minY, VALUE newMin, VALUE newMax) {
	Check_Type(minX, T_DATA);
	Check_Type(minY, T_DATA);
	if (CLASS_OF(minX) != rb_optimizer_realvector_klass || CLASS_OF(minX) != rb_optimizer_realvector_klass)
		rb_raise(rb_eArgError, "Can only truncate using a RealVector.");
	if (
		(TYPE(newMin) == T_FLOAT || TYPE(newMin) == T_FIXNUM) && 
		(TYPE(newMax) == T_FLOAT || TYPE(newMax) == T_FIXNUM)
		) {

		rb_UnlabeledData *s;
		rb_RealVector *minVX, *minVY;

		Data_Get_Struct(self, rb_UnlabeledData, s);
		Data_Get_Struct(minX, rb_RealVector,    minVX);
		Data_Get_Struct(minY, rb_RealVector,    minVY);

		s->data = transform(s->data, shark::TruncateAndRescale(minVX->data, minVY->data, NUM2DBL(newMin), NUM2DBL(newMax)));

	} else {
		rb_raise(rb_eArgError, "New scale must be bounded by numbers");
	}
	return self;
}

typedef VALUE (*rb_method)(...);

void Init_UnlabeledData () {
	// Shark Unlabeled data sets:
	rb_define_method(rb_optimizer_unlabeleddata_klass, "length", (rb_method)method_unlabeleddata_length, 0);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "number_of_batches", (rb_method)method_unlabeleddata_batchlength, 0);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "empty?",     (rb_method)method_unlabeleddata_empty, 0);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "element",    (rb_method)method_unlabeleddata_query, 1);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "[]",         (rb_method)method_unlabeleddata_query, 1);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "[]=",        (rb_method)method_unlabeleddata_insert, 2);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "fill",        (rb_method)method_unlabeleddata_fill, 1);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "remove_NaN", (rb_method)method_unlabeleddata_remove_NaN, -1);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "elements",   (rb_method)method_unlabeleddata_to_ary,0);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "to_a",       (rb_method)method_unlabeleddata_to_ary,0);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "mean",       (rb_method)method_unlabeleddata_mean,0);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "variance",   (rb_method)method_unlabeleddata_variance,0);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "covariance", (rb_method)method_unlabeleddata_covariance,0);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "shift",      (rb_method)method_unlabeleddata_shift,1);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "-",          (rb_method)method_unlabeleddata_shift,1);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "-=",          (rb_method)method_unlabeleddata_shift,1);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "+=",          (rb_method)method_unlabeleddata_posshift,1);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "+",          (rb_method)method_unlabeleddata_posshift,1);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "truncate_and_rescale", (rb_method)method_unlabeleddata_truncate_and_rescale,4);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "truncate",   (rb_method)method_unlabeleddata_truncate,2);
	rb_define_alloc_func(rb_optimizer_unlabeleddata_klass,           (rb_alloc_func_t) method_unlabeleddata_allocate);
	rb_define_method(rb_optimizer_unlabeleddata_klass, "initialize", (rb_method)method_unlabeleddata_initialize, -1);

}