#include "rb_RealVector.h"

extern VALUE rb_optimizer_realmatrix_klass;
extern VALUE rb_optimizer_realvector_klass;
extern VALUE rb_array_klass;

//extern VALUE method_realvector_export_pgm; // this is a promise!

using namespace shark;
using namespace std;

extern void exportPGM( const char *, const RealVector &, int, int, bool);

#include "rb_pointer_wrapping.extras"

rb_RealVector::rb_RealVector(RealVector const& _data) {
	data = _data;
}
rb_RealVector::rb_RealVector() {};
void rb_RealVector::fill(double filling) {
	std::fill(data.begin(), data.end(), filling);
}
void rb_RealVector::remove_NaN(double replacement = 0.0) {
	for (size_t i = 0;i< data.size();i++)
		if (data(i) != data(i))
			data(i) = replacement;
}

VALUE method_realvector_export_pgm (VALUE self, VALUE data_hash) {
	Check_Type(data_hash, T_HASH);
	VALUE rb_path   = rb_hash_aref(data_hash, rb_sym_new("path"));
	VALUE rb_height = rb_hash_aref(data_hash, rb_sym_new("height"));
	VALUE rb_width  = rb_hash_aref(data_hash, rb_sym_new("width"));
	VALUE normalize = rb_hash_aref(data_hash, rb_sym_new("normalize"));

	rb_RealVector *v;
	Data_Get_Struct(self, rb_RealVector, v);

	Check_Type(rb_height, T_FIXNUM);
	Check_Type(rb_width, T_FIXNUM);
	Check_Type(rb_path, T_STRING);
	try {
		exportPGM(StringValueCStr(rb_path), v->data, NUM2INT(rb_width), NUM2INT(rb_height), normalize == Qtrue);
		return self;
	} catch (...) {
		rb_raise(rb_eRuntimeError, "Could not write PGM file.");
		return self;
	}
}

VALUE method_realvector_multiply (VALUE self, VALUE multiplier) {
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);

	if (TYPE(multiplier) == T_FLOAT || TYPE(multiplier) == T_FIXNUM) {
		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector(s->data * NUM2DBL(multiplier))
			);
	} else if (TYPE(multiplier) == T_DATA && CLASS_OF(multiplier) == rb_optimizer_realvector_klass) {
		rb_RealVector *o;
		Data_Get_Struct(multiplier, rb_RealVector, o);

		if (o->data.size() != s->data.size())
			rb_raise(rb_eArgError, "To multiply RealVectors together, use vectors of the same length.");

		return rb_float_new(inner_prod(s->data, o->data));

	} else if (TYPE(multiplier) == T_ARRAY) {

		if (RARRAY_LEN(multiplier) != (int)s->data.size())
			rb_raise(rb_eArgError, "To multiply RealVectors together, use vectors of the same length.");

		RealVector vec_multiplier = rb_ary_to_1d_realvector(multiplier);
		return rb_float_new(inner_prod(s->data, vec_multiplier));
	} else {
		rb_raise(rb_eArgError, "Can only multiply RealVector by a scalar, RealVector, or Array.");
	}
	return self;
}

VALUE method_realvector_multiply_equals (VALUE self, VALUE multiplier) {
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);
	if (TYPE(multiplier) == T_FLOAT || TYPE(multiplier) == T_FIXNUM) {
		s->data *= NUM2DBL(multiplier);
	} else if (TYPE(multiplier) == T_DATA && CLASS_OF(multiplier) == rb_optimizer_realvector_klass) {
		rb_RealVector *o;
		Data_Get_Struct(multiplier, rb_RealVector, o);

		if (o->data.size() != s->data.size())
			rb_raise(rb_eArgError, "To multiply RealVectors together, use vectors of the same length.");

		self = rb_float_new(inner_prod(s->data, o->data ));

	} else if (TYPE(multiplier) == T_ARRAY) {
		if (RARRAY_LEN(multiplier) != (int)s->data.size())
			rb_raise(rb_eArgError, "To multiply RealVectors together, use vectors of the same length.");

		RealVector vec_multiplier = rb_ary_to_1d_realvector(multiplier);
		self = rb_float_new(inner_prod(s->data, vec_multiplier ));
	} else {
		rb_raise(rb_eArgError, "Can only multiply RealVector by a scalar, RealVector, or Array.");
	}
	return self;
}

VALUE method_realvector_get_sqrt (VALUE self) {
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);
	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(sqrt(s->data))
		);
}

VALUE method_realvector_divide_equals (VALUE self, VALUE divider) {
	if (TYPE(divider) == T_FLOAT || TYPE(divider) == T_FIXNUM) {
		rb_RealVector *s;
		Data_Get_Struct(self, rb_RealVector, s);
		s->data /= NUM2DBL(divider);
	} else {
		rb_raise(rb_eArgError, "Can only divide RealVector by a number");
	}
	return self;
}

VALUE method_realvector_divide (VALUE self, VALUE divider) {
	if (TYPE(divider) == T_FLOAT || TYPE(divider) == T_FIXNUM) {
		rb_RealVector *s;
		Data_Get_Struct(self, rb_RealVector, s);
		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector(s->data / NUM2DBL(divider))
			);
	} else {
		rb_raise(rb_eArgError, "Can only divide RealVector by a number");
	}
	return self;
}

VALUE method_realvector_add (VALUE self, VALUE rb_vector) {
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);

	if (TYPE(rb_vector) == T_ARRAY) {
		if (RARRAY_LEN(rb_vector) != (int)s->data.size())
			rb_raise(rb_eArgError, "To sum RealVectors, use vectors of the same length.");

		RealVector summer;
		summer = rb_ary_to_1d_realvector(rb_vector);

		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector(s->data + summer)
			);
	} else if (TYPE(rb_vector) == T_DATA && CLASS_OF(rb_vector) == rb_optimizer_realvector_klass) {
		rb_RealVector *o;
		Data_Get_Struct(rb_vector, rb_RealVector, o);

		if (o->data.size() != s->data.size())
			rb_raise(rb_eArgError, "To sum RealVectors, use vectors of the same length.");

		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector(s->data + o->data)
			);
	} else {
		rb_raise(rb_eArgError, "RealVectors can be sum only with Arrays and other RealVectors");
	}
	return self;
}

VALUE method_realvector_add_equals (VALUE self, VALUE rb_vector) {
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);

	if (TYPE(rb_vector) == T_ARRAY) {
		if (RARRAY_LEN(rb_vector) != (int)s->data.size())
			rb_raise(rb_eArgError, "To sum RealVectors, use vectors of the same length.");

		RealVector summer;
		summer = rb_ary_to_1d_realvector(rb_vector);
		s->data += summer;
	} else if (TYPE(rb_vector) == T_DATA && CLASS_OF(rb_vector) == rb_optimizer_realvector_klass) {
		rb_RealVector *o;
		Data_Get_Struct(rb_vector, rb_RealVector, o);

		if (o->data.size() != s->data.size())
			rb_raise(rb_eArgError, "To sum RealVectors, use vectors of the same length.");

		s->data += o->data;
	} else {
		rb_raise(rb_eArgError, "RealVectors can be sum only with Arrays and other RealVectors");
	}
	return self;
}

VALUE method_realvector_remove (VALUE self, VALUE rb_vector) {
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);

	if (TYPE(rb_vector) == T_ARRAY) {
		if (RARRAY_LEN(rb_vector) != (int)s->data.size())
			rb_raise(rb_eArgError, "To substract RealVectors, use vectors of the same length.");

		RealVector summer;
		summer = rb_ary_to_1d_realvector(rb_vector);

		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector(s->data - summer)
			);
	} else if (TYPE(rb_vector) == T_DATA && CLASS_OF(rb_vector) == rb_optimizer_realvector_klass) {
		rb_RealVector *o;
		Data_Get_Struct(self, rb_RealVector, o);

		if (o->data.size() != s->data.size())
			rb_raise(rb_eArgError, "To substract RealVectors, use vectors of the same length.");

		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector(s->data - o->data)
			);
	} else {
		rb_raise(rb_eArgError, "RealVectors can be substract only with Arrays and other RealVectors");
	}
	return self;
}

VALUE method_realvector_remove_equals (VALUE self, VALUE rb_vector) {
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);

	if (TYPE(rb_vector) == T_ARRAY) {
		if (RARRAY_LEN(rb_vector) != (int)s->data.size())
			rb_raise(rb_eArgError, "To sum RealVectors, use vectors of the same length.");

		RealVector summer;
		summer = rb_ary_to_1d_realvector(rb_vector);
		s->data -= summer;
	} else if (TYPE(rb_vector) == T_DATA && CLASS_OF(rb_vector) == rb_optimizer_realvector_klass) {
		rb_RealVector *o;
		Data_Get_Struct(self, rb_RealVector, o);

		if (o->data.size() != s->data.size())
			rb_raise(rb_eArgError, "To sum RealVectors, use vectors of the same length.");

		s->data -= o->data;
	} else {
		rb_raise(rb_eArgError, "RealVectors can be sum only with Arrays and other RealVectors");
	}
	return self;
}

VALUE method_realvector_negate (VALUE self) {
	rb_RealVector *v;
	Data_Get_Struct(self, rb_RealVector, v);

	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(-(v->data))
	);
}

VALUE method_realvector_to_ary (VALUE self) {
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);
	return realvector_to_rb_ary(s->data);
}

VALUE method_realvector_to_matrix (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE height, width;
	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"02",
		&height,
		&width);
	rb_RealVector *v;
	int mat_height, mat_width;
	Data_Get_Struct(self, rb_RealVector, v);
	if (height == Qnil && width == Qnil) {
		mat_height = v->data.size();
		mat_width  = 1;
	} else if (TYPE(height) == T_FIXNUM && width == Qnil) {
		if (NUM2INT(height) <= 0)
			rb_raise(rb_eArgError, "RealMatrix cannot (or will not, or cannot stand) have(ing) a negative height.");
		if (v->data.size() % NUM2INT(height) != 0)
			rb_raise(rb_eArgError, "The specified dimension is incompatible with the vector's length (must be a divisor of the vector' length to be allowable).");
		mat_width = v->data.size()/NUM2INT(height);
		mat_height = NUM2INT(height);
	} else {
		Check_Type(height, T_FIXNUM);
		Check_Type(width, T_FIXNUM);
		if (NUM2INT(height) <= 0)
			rb_raise(rb_eArgError, "RealMatrix cannot (or will not, or cannot stand) have(ing) a negative height.");
		if (NUM2INT(width) <= 0)
			rb_raise(rb_eArgError, "RealMatrix cannot (or will not, or cannot stand) have(ing) a negative width.");
		if (v->data.size() - (NUM2INT(height) * NUM2INT(width)) != 0)
			rb_raise(rb_eArgError, "The product of the height and width is not equal to the length of the vector, therefore the resulting Matrix would be off to a bad start.");
		mat_width  = NUM2INT(width);
		mat_height = NUM2INT(height);
	}
	RealMatrix mat(mat_height, mat_width);

	int pos = 0;
	for (int i=0; i < mat_height; i++) {
		row(mat, i) = subrange(v->data, pos, pos + mat_width);
		pos += mat_width;
	}
	return wrap_pointer<rb_RealMatrix>(
			rb_optimizer_realmatrix_klass,
			new rb_RealMatrix(mat)
		);
}


VALUE method_realvector_length (VALUE self) {
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);
	return INT2FIX((s->data).size());
}

VALUE method_realvector_stride (VALUE self) {
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);
	return INT2FIX((s->data).stride());
}
VALUE method_realvector_resize (VALUE self, VALUE newlength) {
	Check_Type(newlength, T_FIXNUM);
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);
	if (NUM2INT(newlength) < 0)
		rb_raise(rb_eArgError, "Can only create positive length vectors due to lack of antimatter.");
	(s->data).resize(NUM2INT(newlength));
	return self;
}
VALUE method_realvector_empty (VALUE self) {
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);
	if ((s->data).empty())
		return Qtrue;
	return Qfalse;
}
VALUE method_realvector_clear (VALUE self) {
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);
	(s->data).clear();
	return self;
}
VALUE method_realvector_fill (VALUE self, VALUE filling) {
	if (TYPE(filling) != T_FLOAT && TYPE(filling) != T_FIXNUM)
		rb_raise(rb_eArgError, "Can only fill RealVector with floats.");
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);
	s->fill(NUM2DBL(filling));
	return self;
}
VALUE method_realvector_query (VALUE self, VALUE position) {
	Check_Type(position, T_FIXNUM);
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);
	if (NUM2INT(position) < 0) {
		if (NUM2INT(position) < (int)-(s->data).size())
			rb_raise(rb_eArgError, "Out of range.");
		// to get last element ask for -1 for instance:
		return rb_float_new((s->data)[(s->data).size()+NUM2INT(position)]);
	} else {
		if (NUM2INT(position) >= (int)(s->data).size())
			rb_raise(rb_eArgError, "Out of range.");
		// ordered normally
		return rb_float_new((s->data)[NUM2INT(position)]);
	}
}
VALUE method_realvector_insert (VALUE self, VALUE position, VALUE assignment) {
	Check_Type(position, T_FIXNUM);
	if (TYPE(assignment) != T_FIXNUM && TYPE(assignment) != T_FLOAT)
		rb_raise(rb_eArgError, "Can only insert floats in RealVector.");
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);
	if (NUM2INT(position) < 0) {
		if (NUM2INT(position) < (int)-(s->data).size())
			rb_raise(rb_eArgError, "Out of range.");
		// to get last element ask for -1 for instance:
		(s->data)[(s->data).size()+NUM2INT(position)] = NUM2DBL(assignment);
	} else {
		if (NUM2INT(position) >= (int)(s->data).size())
			rb_raise(rb_eArgError, "Out of range.");
		// ordered normally
		(s->data)[NUM2INT(position)] = NUM2DBL(assignment);
	}
	return self;
}

VALUE method_realvector_allocate (VALUE klass) {
	return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector()
			);
}

VALUE method_realvector_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE dataset;
	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"01",
		&dataset);
	rb_RealVector *v;
	Data_Get_Struct(self, rb_RealVector, v);

	if (TYPE(dataset) == T_ARRAY) {
		v->data = rb_ary_to_1d_realvector(dataset);
	} else if (TYPE(dataset) == T_FIXNUM) {
		method_realvector_resize(self, dataset);
	}
	return self;
}

VALUE method_rb_ary_to_realvector (VALUE self) {
	if (RARRAY_LEN(self) > 0) {
		if (TYPE(rb_ary_entry(self, 0)) != T_FIXNUM && TYPE(rb_ary_entry(self, 0)) != T_FLOAT)
			rb_raise(rb_eArgError, "Can only convert to RealVector 1-dimensional Float or Fixnum vectors (please be gentle).");
		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector(rb_ary_to_1d_realvector(self))
			);
	} else {
		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector()
			);
	}
	return self;
}



void Init_RealVector () {

	rb_define_method(rb_array_klass, "to_realvector", (rb_method)method_rb_ary_to_realvector, 0);
		// Shark Vectors:
	rb_define_method(rb_optimizer_realvector_klass, "to_pgm", (rb_method)method_realvector_export_pgm, 1);
	rb_define_method(rb_optimizer_realvector_klass, "sqrt", (rb_method)method_realvector_get_sqrt,0);
	rb_define_method(rb_optimizer_realvector_klass, "to_a", (rb_method)method_realvector_to_ary, 0);
	rb_define_method(rb_optimizer_realvector_klass, "to_matrix", (rb_method)method_realvector_to_matrix, -1);
	rb_define_method(rb_optimizer_realvector_klass, "*", (rb_method)method_realvector_multiply, 1);
	rb_define_method(rb_optimizer_realvector_klass, "*=", (rb_method)method_realvector_multiply_equals, 1);
	rb_define_method(rb_optimizer_realvector_klass, "/", (rb_method)method_realvector_divide, 1);
	rb_define_method(rb_optimizer_realvector_klass, "/=", (rb_method)method_realvector_divide_equals, 1);
	rb_define_method(rb_optimizer_realvector_klass, "+", (rb_method)method_realvector_add, 1);
	rb_define_method(rb_optimizer_realvector_klass, "+=", (rb_method)method_realvector_add_equals, 1);
	rb_define_method(rb_optimizer_realvector_klass, "-=", (rb_method)method_realvector_remove_equals, 1);
	rb_define_method(rb_optimizer_realvector_klass, "-", (rb_method)method_realvector_remove, 1);
	rb_define_method(rb_optimizer_realvector_klass, "length", (rb_method)method_realvector_length, 0);
	rb_define_method(rb_optimizer_realvector_klass, "size", (rb_method)method_realvector_length, 0);
	rb_define_method(rb_optimizer_realvector_klass, "-@", (rb_method)method_realvector_negate,0);
	rb_define_method(rb_optimizer_realvector_klass, "stride", (rb_method)method_realvector_stride,0);
	rb_define_method(rb_optimizer_realvector_klass, "resize", (rb_method)method_realvector_resize,1);
	rb_define_method(rb_optimizer_realvector_klass, "empty?", (rb_method)method_realvector_empty,0);
	rb_define_method(rb_optimizer_realvector_klass, "clear", (rb_method)method_realvector_clear,0);
	rb_define_method(rb_optimizer_realvector_klass, "fill", (rb_method)method_realvector_fill,1);
	rb_define_method(rb_optimizer_realvector_klass, "[]", (rb_method)method_realvector_query,1);
	rb_define_method(rb_optimizer_realvector_klass, "[]=", (rb_method)method_realvector_insert,2);
	rb_define_alloc_func(rb_optimizer_realvector_klass, (rb_alloc_func_t) method_realvector_allocate);
	rb_define_method(rb_optimizer_realvector_klass, "initialize", (rb_method)method_realvector_initialize,-1);

}