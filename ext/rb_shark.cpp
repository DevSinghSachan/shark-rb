#include "Optimizer.h"
#include "rb_RealVector.h"
#include "rb_UnlabeledData.h"
#include "rb_RegressionDataset.h"
#include "rb_RealMatrix.h"
#include "rb_LinearModel.h"

using namespace std;
using namespace shark;

VALUE rb_sym_new(const char *s) {
	return ID2SYM(rb_intern(s));
}

VALUE rb_optimizer_klass               = rb_define_class("Optimizer", rb_cObject);
VALUE rb_optimizer_samples_klass       = rb_define_class_under(rb_optimizer_klass, "Samples", rb_cObject);
VALUE rb_optimizer_realvector_klass    = rb_define_class_under(rb_optimizer_klass, "RealVector", rb_cObject);
VALUE rb_optimizer_unlabeleddata_klass = rb_define_class_under(rb_optimizer_klass, "UnlabeledData", rb_cObject);
VALUE rb_optimizer_regressionset_klass = rb_define_class_under(rb_optimizer_klass, "RegressionDataset", rb_cObject);
VALUE rb_optimizer_realmatrix_klass    = rb_define_class_under(rb_optimizer_klass, "RealMatrix", rb_cObject);

template<class Obtype> void delete_objects(Obtype *ptr){
	delete ptr;
}

template<class Obtype> VALUE wrap_pointer(VALUE klass, Obtype *ptr){
	return Data_Wrap_Struct(klass,0,delete_objects<Obtype>,ptr);
}

template<class Obtype> VALUE alloc_ob(VALUE self) {
	return wrap_pointer<Obtype>(self,new Obtype());
}


shark::RealVector rb_ary_to_1d_realvector(VALUE ary) {
	int length = RARRAY_LEN(ary);
	shark::RealVector vector(length);
	for (int i=0; i < length;i++) {
		vector(i) = NUM2DBL(rb_ary_entry(ary, i));
	}
	return vector;
}
RealMatrix rb_ary_to_realmatrix(VALUE ary) {

	int rows = RARRAY_LEN(ary);
	int cols = rows > 0 ? RARRAY_LEN(rb_ary_entry(ary, 0)) : 0;
	shark::RealMatrix matrix(rows, cols);
	
	// accessing ary[i][j] and placing it in matrix(i, j):
	for (int i=0;i<rows;i++)
		for (int j=0;j<cols;j++)
			matrix(i,j) = NUM2DBL(rb_ary_entry(rb_ary_entry(ary, i), j));

	return matrix;
}

RealMatrix rb_1d_ary_to_realmatrix(VALUE ary) {
	int height = RARRAY_LEN(ary);
	shark::RealMatrix matrix(1, height);
	for (int i=0;i<height;i++)
		matrix(0,i) = NUM2DBL(rb_ary_entry(ary, i));
	return matrix;
}

std::vector<shark::RealMatrix> rb_ary_to_realmatrices(VALUE ary) {

	std::vector<shark::RealMatrix> matrices;

	if (TYPE(rb_ary_entry(ary, 0)) == T_ARRAY) {
		// 2D array
		matrices.push_back(rb_ary_to_realmatrix(ary));
	} else {
		// 1D array
		matrices.push_back(rb_1d_ary_to_realmatrix(ary));
	}
	
	return matrices;
}


VALUE realmatrix_to_rb_ary(const RealMatrix& W) {
	VALUE matrix = rb_ary_new2((int)W.size1());
	for (size_t i = 0; i < W.size1(); ++i)
	{
		rb_ary_store(matrix, (int)i, rb_ary_new2((int) W.size2()));
		for (size_t j = 0; j < W.size2(); ++j) {
			// printf("W(%d,%d) = %f\n", (int)i, (int)j, W(i,j));
			rb_ary_store(rb_ary_entry(matrix, (int)i), (int)j, rb_float_new(W(i,j)));
		}
	}
	return matrix;
}

VALUE realmatrix_to_rb_ary(const RealMatrix& W, bool two_d_array) {
	if (two_d_array == 1 && (W.size1() == 1 || W.size2() == 1)) {
		VALUE matrix = rb_ary_new2((int)W.size1()*W.size2());
		bool tall = W.size1() > W.size2() ? 1 : 0;
		int length = tall ? W.size1() : W.size2();
		for (size_t i = 0; i < length; ++i)
			rb_ary_store(matrix, i, rb_float_new(tall ? W(i,0) : W(0,i)));
		return matrix;
	} else {
		return realmatrix_to_rb_ary(W);
	}
	
}

VALUE stdvector_realmatrix_to_rb_ary(const std::vector<RealMatrix> W) {
	VALUE ary = rb_ary_new2((int)W.size());
	for (int i = 0; i < W.size(); ++i) {
		rb_ary_store(ary, i,realmatrix_to_rb_ary(W[i]));
	}
	return ary;
}

std::vector<shark::RealVector> rb_ary_to_realvector(VALUE ary) {
	int width = RARRAY_LEN(ary);
	int height = width > 0 ? RARRAY_LEN(rb_ary_entry(ary, 0)) : 0;
	std::vector<shark::RealVector> matrix;
	for (int i=0;i<width;i++) {
		shark::RealVector vector(height);
		for (int j=0;j<height;j++) {
			vector(j) = NUM2DBL(rb_ary_entry(rb_ary_entry(ary, i), j));
		}
		matrix.push_back(vector);
	}
	return matrix;
}

shark::UnlabeledData<shark::RealVector> rb_ary_to_unlabeleddata(VALUE ary) {
	std::vector<shark::RealVector> created_data = rb_ary_to_realvector(ary);
	shark::UnlabeledData<shark::RealVector> samples = shark::createDataFromRange(created_data);
	return samples;
}

VALUE realvector_to_rb_ary(const RealVector& W) {
	VALUE matrix = rb_ary_new2((int)W.size());
	for (int i = 0; i < W.size(); ++i)
	{
		rb_ary_store(matrix, i, rb_float_new(W(i)));
	}
	return matrix;
}

VALUE stdvector_realvector_to_rb_ary(const std::vector<RealVector> W) {
	VALUE ary = rb_ary_new2((int)W.size());
	for (int i = 0; i < W.size(); ++i) {
		rb_ary_store(ary, i,realvector_to_rb_ary(W[i]));
	}
	return ary;
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

VALUE method_regressionset_create (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE samples,
		  labels,
		  width;
	// these variables are also:
	//    numSamples, height, width;

	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"12",
		&samples,
		&labels,
		&width);

	if (TYPE(samples) == T_DATA && CLASS_OF(labels) == rb_optimizer_unlabeleddata_klass) {
		// input is either labels and data or just data:
		if (TYPE(labels) == T_DATA && CLASS_OF(labels) == rb_optimizer_unlabeleddata_klass) {
			return wrap_pointer<rb_RegressionDataset>(
				rb_optimizer_regressionset_klass,
				new rb_RegressionDataset(samples, labels)
			);
		} else {
			return wrap_pointer<rb_RegressionDataset>(
				rb_optimizer_regressionset_klass,
				new rb_RegressionDataset(samples)
			);
		}
	} else {
		// create a random set from csv file.
		Check_Type(samples, T_FIXNUM);
		Check_Type(labels,  T_FIXNUM);
		Check_Type(width,   T_FIXNUM);
		return wrap_pointer<rb_RegressionDataset>(
			rb_optimizer_regressionset_klass,
			new rb_RegressionDataset(NUM2INT(samples), NUM2INT(labels), NUM2INT(width))
		);
	}
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

VALUE method_realmatrix_get_sqrt (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	m->data = sqrt(m->data);
	return self;
}
VALUE method_realmatrix_to_ary (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	return realmatrix_to_rb_ary(m->data);
}
VALUE method_realmatrix_multiply (VALUE self, VALUE multiplier) {
	if (TYPE(multiplier) == T_FLOAT || TYPE(multiplier) == T_FIXNUM) {
		rb_RealMatrix *m;
		Data_Get_Struct(self, rb_RealMatrix, m);
		m->data *= NUM2DBL(multiplier);
	} else {
		rb_raise(rb_eArgError, "Can only multiply RealMatrix by a number");
	}
	return self;
}
VALUE method_realmatrix_divide (VALUE self, VALUE divider) {
	if (TYPE(divider) == T_FLOAT || TYPE(divider) == T_FIXNUM) {
		rb_RealMatrix *m;
		Data_Get_Struct(self, rb_RealMatrix, m);
		m->data /= NUM2DBL(divider);
	} else {
		rb_raise(rb_eArgError, "Can only divide RealMatrix by a number");
	}
	return self;
}
VALUE method_realmatrix_length (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	VALUE size = rb_ary_new2(2);
	// number of rows
	rb_ary_store(size, 0, INT2FIX((m->data).size1()));
	// number of columns
	rb_ary_store(size, 1, INT2FIX((m->data).size2()));
	return size;
}

VALUE method_realmatrix_size1 (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	return INT2FIX((m->data).size1());
}
VALUE method_realmatrix_size2 (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	return INT2FIX((m->data).size2());
}
VALUE method_realmatrix_stride1 (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	return INT2FIX((m->data).stride1());
}
VALUE method_realmatrix_stride2 (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	return INT2FIX((m->data).stride2());
}
VALUE method_realmatrix_clear (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	(m->data).clear();
	return self;
}

VALUE method_realmatrix_fill (VALUE self, VALUE filling) {
	if (TYPE(filling) != T_FLOAT && TYPE(filling) != T_FIXNUM)
		rb_raise(rb_eArgError, "Can only fill RealMatrix with floats.");
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	m->fill(NUM2DBL(filling));
	return self;
}

VALUE method_realmatrix_query (VALUE self, VALUE row, VALUE column) {
	Check_Type(row, T_FIXNUM);
	Check_Type(column, T_FIXNUM);
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	if (NUM2INT(row) < 0 || NUM2INT(column) < 0)
		rb_raise(rb_eArgError, "Can only access positive positions in RealMatrix");
	if (NUM2INT(row) >= (m->data).size1() || NUM2INT(column) >= (m->data).size2())
		rb_raise(rb_eArgError, "Out of range of RealMatrix");
	return rb_float_new((m->data)(NUM2INT(row), NUM2INT(column)));
}

VALUE method_realmatrix_insert (VALUE self, VALUE row, VALUE column, VALUE assignment) {
	Check_Type(row, T_FIXNUM);
	Check_Type(column, T_FIXNUM);
	if (TYPE(assignment) != T_FIXNUM && TYPE(assignment) != T_FLOAT)
		rb_raise(rb_eArgError, "Can only insert floats into RealMatrix.");

	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	if (NUM2INT(row) < 0 || NUM2INT(column) < 0)
		rb_raise(rb_eArgError, "Can only access positive positions in RealMatrix");
	if (NUM2INT(row) >= (m->data).size1() || NUM2INT(column) >= (m->data).size2())
		rb_raise(rb_eArgError, "Out of range of RealMatrix");
	(m->data)(NUM2INT(row), NUM2INT(column)) = NUM2DBL(assignment);
	return self;
}

VALUE method_realmatrix_resize (VALUE self, VALUE newRows, VALUE newColumns) {
	Check_Type(newRows, T_FIXNUM);
	Check_Type(newColumns, T_FIXNUM);
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	(m->data).resize(NUM2INT(newRows), NUM2INT(newColumns));
	return self;
}

VALUE method_realmatrix_negate (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	return wrap_pointer<rb_RealMatrix>(
		rb_optimizer_realmatrix_klass,
		new rb_RealMatrix(-(m->data))
	);
}
VALUE method_realmatrix_allocate (VALUE klass) {
	return wrap_pointer<rb_RealMatrix>(
			rb_optimizer_realmatrix_klass,
			new rb_RealMatrix()
			);
}
VALUE method_realmatrix_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE dataset;
	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"01",
		&dataset);
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);

	if (TYPE(dataset) == T_ARRAY)
		m->data = (RARRAY_LEN(dataset) > 0 && TYPE(rb_ary_entry(dataset, 0)) == T_ARRAY) ?
						rb_ary_to_realmatrix(dataset) : 
						rb_1d_ary_to_realmatrix(dataset);

	return self;
}

VALUE method_realvector_multiply (VALUE self, VALUE multiplier) {
	if (TYPE(multiplier) == T_FLOAT || TYPE(multiplier) == T_FIXNUM) {
		rb_RealVector *s;
		Data_Get_Struct(self, rb_RealVector, s);
		s->data *= NUM2DBL(multiplier);
	} else {
		rb_raise(rb_eArgError, "Can only multiply RealVector by a number");
	}
	return self;
}

VALUE method_realvector_get_sqrt (VALUE self) {
	rb_RealVector *s;
	Data_Get_Struct(self, rb_RealVector, s);
	s->data = sqrt(s->data);
	return self;
}

VALUE method_realvector_divide (VALUE self, VALUE divider) {
	if (TYPE(divider) == T_FLOAT || TYPE(divider) == T_FIXNUM) {
		rb_RealVector *s;
		Data_Get_Struct(self, rb_RealVector, s);
		s->data /= NUM2DBL(divider);
	} else {
		rb_raise(rb_eArgError, "Can only divide RealVector by a number");
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
		if (NUM2INT(position) < -(s->data).size())
			rb_raise(rb_eArgError, "Out of range.");
		// to get last element ask for -1 for instance:
		return rb_float_new((s->data)[(s->data).size()+NUM2INT(position)]);
	} else {
		if (NUM2INT(position) >= (s->data).size())
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
		if (NUM2INT(position) < -(s->data).size())
			rb_raise(rb_eArgError, "Out of range.");
		// to get last element ask for -1 for instance:
		(s->data)[(s->data).size()+NUM2INT(position)] = NUM2DBL(assignment);
	} else {
		if (NUM2INT(position) >= (s->data).size())
			rb_raise(rb_eArgError, "Out of range.");
		// ordered normally
		(s->data)[NUM2INT(position)] = NUM2DBL(assignment);
	}
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
		if (NUM2INT(position) < -(s->data).numberOfElements())
			rb_raise(rb_eArgError, "Out of range.");
		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector((s->data).element((s->data).numberOfElements() + NUM2INT(position)))
		);
	} else {
		if (NUM2INT(position) >= (s->data).numberOfElements())
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
		if (NUM2INT(position) < -(s->data).numberOfElements())
			rb_raise(rb_eArgError, "Out of range.");
		(s->data).element((s->data).numberOfElements() + NUM2INT(position)) = a->data;
	} else {
		if (NUM2INT(position) >= (s->data).numberOfElements())
			rb_raise(rb_eArgError, "Out of range.");
		(s->data).element(NUM2INT(position)) = a->data;
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

	if (TYPE(dataset) == T_ARRAY)
		v->data = rb_ary_to_1d_realvector(dataset);

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

/* end Ruby Stuff */

shark::UnlabeledData<shark::RealVector> getSamples(int numSamples, int height, int width) {
	// Read images
	shark::UnlabeledData<shark::RealVector> images;
	import_csv(images, "data/images.csv");

	const unsigned int w = 512;//width of loaded image
	const unsigned int h = 512;//height of loaded image

	unsigned int n = images.numberOfElements(); // number of images
	std::cout << "Found " << n << " images of size " << w << "x" << h << std::endl;

	// Create the samples at random
	// Important notes: Since the images are in csv format, the width and
	// height is hardcoded. Because width = height we only have one integer
	// distribution below.
	
	// Sample equal amount of patches per image
	size_t patchesPerImg = numSamples / n;
	typedef shark::UnlabeledData<shark::RealVector>::element_range::iterator ElRef;
	
	// Create patches
	std::vector<shark::RealVector> patches;
	for (ElRef it = images.elements().begin(); it != images.elements().end(); ++it) {
		for (size_t i = 0; i < patchesPerImg; ++i) {
			// Upper left corner of image
			unsigned int ulx = rand() % (w - width);
			unsigned int uly = rand() % (h - height);
			// Transform 2d coordinate into 1d coordinate and get the sample
			unsigned int ul = ulx * h + uly;
			shark::RealVector sample(width * height);
			const shark::RealVector& img = *it;
			for (size_t row = 0; row < height; ++row)
				for (size_t column = 0; column < width; ++column)
					sample(row * width + column) = img(ul + column + row * h);
			patches.push_back(sample);
		}
	}
	
	shark::UnlabeledData<shark::RealVector> samples = shark::createDataFromRange(patches);
	
	// zero mean
	shark::RealVector meanvec = mean(samples);
	samples = transform(samples, shark::Shift(-meanvec));

	// Remove outliers outside of +/- 3 standard deviations
	// and normalize to [0.1, 0.9]
	shark::RealVector pstd = 3 * sqrt(variance(samples));
	samples = transform(samples, shark::TruncateAndRescale(-pstd, pstd, 0.1, 0.9));
	
	return samples;
}


Samples::Samples(VALUE self, VALUE rb_data) {
	Check_Type(rb_data, T_ARRAY);

	std::vector<shark::RealVector> created_data = rb_ary_to_realvector(rb_data);

	shark::UnlabeledData<shark::RealVector> samples = shark::createDataFromRange(created_data);

	data = RegressionDataset(samples, samples);
	visibleSize = RARRAY_LEN(rb_ary_entry(rb_data, 0));
}

Samples::Samples(int numSamples, int height, int width) {
	// Read the data
	shark::UnlabeledData<shark::RealVector> samples = getSamples(numSamples, height, width);
	cout << "Getting regression dataset," << endl;
	cout << "Generated : " << samples.numberOfElements() << " patches." << endl;
	data = RegressionDataset(samples, samples);
	visibleSize = height * width;
}

std::vector<shark::RealVector> Samples::input () {
	std::vector<shark::RealVector> my_input(data.numberOfElements());
	for (size_t i=0; i<data.numberOfElements(); i++)
	{
		RealVector output = data.element(i).input;
		my_input[i] = output;
	}
	return my_input;
}


static VALUE method_get_samples(int number_of_arguments, VALUE* ruby_arguments, VALUE self)
{
	VALUE rb_number_of_samples, r_height, r_width;
	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"03",
		&rb_number_of_samples,
		&r_height,
		&r_width);

	return wrap_pointer<Samples>(
		rb_optimizer_samples_klass,
		new Samples(
			rb_number_of_samples != Qnil ? NUM2INT(rb_number_of_samples) : 1000,
			r_height != Qnil ? NUM2INT(r_height) : 8,
			r_width != Qnil ? NUM2INT(r_width) : 8)
		);
}

static VALUE method_create_samples(VALUE self, VALUE data) {
	return wrap_pointer<Samples>(
		rb_optimizer_samples_klass,
		new Samples(self, data));
}

static VALUE method_get_visible_size(VALUE self) {
	Samples *s;
	Data_Get_Struct(self, Samples, s);
	return INT2FIX(s->visibleSize);
}

static VALUE method_samples_get_elements(VALUE self) {
	Samples *s;
	Data_Get_Struct(self, Samples, s);
	std::vector<shark::RealVector> inputData = s->input();
	return stdvector_realvector_to_rb_ary(inputData);
}

static VALUE method_samples_get_size(VALUE self) {
	Samples *s;
	Data_Get_Struct(self, Samples, s);
	return INT2FIX((s->data).numberOfElements());
}

// Regression Data set wrapper methods:

static VALUE method_regressionset_get_visible_size(VALUE self) {
	rb_RegressionDataset *s;
	Data_Get_Struct(self, rb_RegressionDataset, s);
	return INT2FIX(s->visibleSize);
}

static VALUE method_regressionset_get_labels(VALUE self) {
	rb_RegressionDataset *s;
	Data_Get_Struct(self, rb_RegressionDataset, s);
	return wrap_pointer<rb_UnlabeledData>(
		rb_optimizer_unlabeleddata_klass,
		new rb_UnlabeledData((s->data).labels())
		);
}
static VALUE method_regressionset_get_inputs(VALUE self) {
	rb_RegressionDataset *s;
	Data_Get_Struct(self, rb_RegressionDataset, s);
	return wrap_pointer<rb_UnlabeledData>(
		rb_optimizer_unlabeleddata_klass,
		new rb_UnlabeledData((s->data).inputs())
		);
}

static VALUE method_regressionset_get_size(VALUE self) {
	rb_RegressionDataset *s;
	Data_Get_Struct(self, rb_RegressionDataset, s);
	return INT2FIX((s->data).numberOfElements());
}

static VALUE method_autoencode(int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	unsigned int numhidden = 25;
	double rho = 0.01; // Sparsity parameter
	double beta = 3.0; // Regularization parameter
	double lambda = 0.0002; // Weight decay paramater
	int visibleSize = 0;
	shark::RegressionDataset data;

	if (number_of_arguments == 1 && TYPE(ruby_arguments[0]) == T_DATA ) {
		if (CLASS_OF(ruby_arguments[0]) != rb_optimizer_regressionset_klass)
			rb_raise(rb_eArgError, "Samples must be of class RegressionDataset.");

		rb_RegressionDataset *s;
		Data_Get_Struct(ruby_arguments[0], rb_RegressionDataset, s);
		data = s->data;
		visibleSize = s-> visibleSize;
	} else if (number_of_arguments == 1 && TYPE(ruby_arguments[0]) == T_HASH) {
		VALUE rb_beta, rb_rho, rb_lambda, rb_numHidden, rb_data;

		rb_beta      = rb_hash_aref(ruby_arguments[0], rb_sym_new("beta"));
		rb_rho       = rb_hash_aref(ruby_arguments[0], rb_sym_new("rho"));
		rb_lambda    = rb_hash_aref(ruby_arguments[0], rb_sym_new("lambda"));
		rb_data      = rb_hash_aref(ruby_arguments[0], rb_sym_new("data"));
		rb_numHidden = rb_hash_aref(ruby_arguments[0], rb_sym_new("hidden_neurons"));

		if (rb_beta != Qnil) {
			Check_Type(rb_beta, T_FLOAT);
			beta = NUM2DBL(rb_beta);
		}
		if (rb_rho != Qnil) {
			Check_Type(rb_rho, T_FLOAT);
			rho = NUM2DBL(rb_rho);
		}
		if (rb_lambda != Qnil) {
			Check_Type(rb_lambda, T_FLOAT);
			lambda = NUM2DBL(rb_lambda);
		}
		if (rb_numHidden != Qnil) {
			Check_Type(rb_numHidden, T_FIXNUM);
			numhidden = NUM2INT(rb_numHidden);
		}
		if (rb_data != Qnil) {
			Check_Type(rb_data, T_DATA);
			if (CLASS_OF(rb_data) != rb_optimizer_regressionset_klass)
				rb_raise(rb_eArgError, "Samples can only be a RegressionDataset.");
			rb_RegressionDataset *s;
			Data_Get_Struct(rb_data, rb_RegressionDataset, s);
			data = s->data;
			visibleSize = s-> visibleSize;
		}
	} else if (number_of_arguments > 1) {
		rb_raise(rb_eArgError, "Autoencoder takes 1 argument (Optimizer::RegressionDataset), or a Hash of parameters (e.g\n  {\n    :data => Optimizer::RegressionDataset,\n    :beta => 3.0,\n    :rho => 0.01,\n    :lambda => 0.0002,\n    :hidden_neurons => 25\n  }).");
	}
	
	Optimizer *my_optimizer = new Optimizer(visibleSize, numhidden, rho, beta, lambda);
	my_optimizer->setData(data);

	return wrap_pointer<Optimizer>(self, my_optimizer);
}


static VALUE method_autoencode_set_parameter_vector(VALUE self, VALUE rb_parameter_vector) {

	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);

	rb_RealVector *v;
	Data_Get_Struct(rb_parameter_vector, rb_RealVector, v);

	o->setParameterVector(v->data);

	return self;
}

static VALUE method_autoencode_layer_matrices(int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE rb_layer_index;
	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"01",
		&rb_layer_index);

	if (TYPE(rb_layer_index) == T_FIXNUM) {
		Optimizer *o;
		Data_Get_Struct(self, Optimizer, o);
		int index = rb_layer_index != Qnil ? NUM2INT(rb_layer_index) : 0;

		if (index > 1 || index < 0)
			rb_raise(rb_eArgError, "Layer index must be between 0 and 1. There are only 2 layers in this Neural-Net.");
		RealMatrix& matrix = o->layer_matrices_at_index(index);

		return wrap_pointer<rb_RealMatrix>(
			rb_optimizer_realmatrix_klass,
			new rb_RealMatrix(matrix)
		);
	} else if (TYPE(rb_layer_index) == T_DATA && CLASS_OF(rb_layer_index) == rb_optimizer_realvector_klass) {
		return method_autoencode_set_parameter_vector(self, rb_layer_index);
	} else {
		rb_raise(rb_eArgError, "Can only query layer parameters by specifying a Fixnum index (e.g. 0), or by providing a RealVector to reassign the parameters.");
		return self;
	}
}

static VALUE method_autoencode_layer_matrices_to_a(int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE rb_layer_index;
	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"01",
		&rb_layer_index);

	Check_Type(rb_layer_index, T_FIXNUM);

	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	int index = rb_layer_index != Qnil ? NUM2INT(rb_layer_index) : 0;

	if (index > 1 || index < 0)
		rb_raise(rb_eArgError, "Layer index must be between 0 and 1. There are only 2 layers in this Neural-Net.");
	RealMatrix& matrix = o->layer_matrices_at_index(index);

	return realmatrix_to_rb_ary(matrix);
}

static VALUE method_autoencode_set_starting_point(VALUE self) {

	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);

	o->setStartingPoint();

	return self;
}

typedef VALUE (*rb_method)(...);


static VALUE method_autoencode_step(VALUE self) {
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	o->step();
	return Qnil;
}

static VALUE method_autoencode_error(VALUE self) {
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	return rb_float_new(o->errorValue());
}

static VALUE method_autoencode_numSteps(VALUE self) {
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	return INT2FIX(o->steps);
}

static VALUE method_autoencode_visibleSize(VALUE self) {
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	return INT2FIX(o->visibleSize);
}

static VALUE method_autoencode_hiddenSize(VALUE self) {
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	return INT2FIX(o->hiddenSize);
}

static VALUE method_autoencode_beta(VALUE self) {
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	return rb_float_new(o->beta);
}

static VALUE method_autoencode_lambda(VALUE self) {
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	return rb_float_new(o->lambda);
}

static VALUE method_autoencode_rho(VALUE self) {
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	return rb_float_new(o->rho);
}

static VALUE method_autoencode_number_of_parameters(VALUE self) {
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	return INT2FIX((o->model).numberOfParameters());
}

static VALUE method_autoencode_number_of_neurons(VALUE self) {
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	return INT2FIX((o->model).numberOfNeurons());
}

static VALUE method_autoencode_input_size(VALUE self) {
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	return INT2FIX((o->model).inputSize());
}

static VALUE method_autoencode_output_size(VALUE self) {
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	return INT2FIX((o->model).outputSize());
}

static VALUE method_autoencode_eval(VALUE self, VALUE sample) {
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);

	// Type and length checking:
	Check_Type(sample, T_ARRAY);
	if (RARRAY_LEN(sample) < 1)
		rb_raise(rb_eArgError, "There must be at least one sample to evaluate.");

	std::vector<shark::RealMatrix> samples = rb_ary_to_realmatrices(sample);

	// convert sample to Realmatrix.
	std::vector<shark::RealMatrix> evaluation = o->eval(samples[0]);

	VALUE output_hash, rb_evaluation, rb_hidden_evaluation, rb_other_evaluation, rb_all_evaluation;

	rb_evaluation        = realmatrix_to_rb_ary(evaluation[0], 1);
	rb_hidden_evaluation = realmatrix_to_rb_ary(evaluation[1], 1);

	output_hash = rb_hash_new();
	rb_hash_aset(output_hash, rb_sym_new("output_layer"), rb_evaluation);
	rb_hash_aset(output_hash, rb_sym_new("hidden_layer"), rb_hidden_evaluation);

	return output_hash;
}

static VALUE method_export_feature_images(int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE rb_layer_index;
	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"01",
		&rb_layer_index);
	Optimizer *o;
	Data_Get_Struct(self, Optimizer, o);
	int index = rb_layer_index != Qnil ? NUM2INT(rb_layer_index) : 0;
	if (index > 1 || index < 0)
		rb_raise(rb_eArgError, "Layer index must be between 0 and 1. There are only 2 layers in this Neural-Net.");
	o->exportFeatureImages(index);
	return self;
}

extern "C"  {

	void Init_rb_shark() {

		// for better naming conventions:
		rb_define_global_const("Shark", rb_optimizer_klass);

		// Autoencoder
			// Ruby methods for AutoEncoder
			rb_define_singleton_method(rb_optimizer_klass, "autoencoder", (rb_method)method_autoencode, -1);
			
			// advance learning by one iteration:
			rb_define_method(rb_optimizer_klass, "step", (rb_method)method_autoencode_step,0);
			rb_define_method(rb_optimizer_klass, "train", (rb_method)method_autoencode_step,0);
			
			// current training error:
			rb_define_method(rb_optimizer_klass, "error", (rb_method)method_autoencode_error,0);
			rb_define_method(rb_optimizer_klass, "solution", (rb_method)method_autoencode_error,0);
			
			// The current parameter assignments on the neural net
			rb_define_method(rb_optimizer_klass, "parameters", (rb_method)method_autoencode_layer_matrices, -1);
			rb_define_method(rb_optimizer_klass, "parameters_to_a", (rb_method)method_autoencode_layer_matrices_to_a, -1);
			rb_define_method(rb_optimizer_klass, "set_starting_point", (rb_method)method_autoencode_set_starting_point, 0);
			rb_define_method(rb_optimizer_klass, "layers", (rb_method)method_autoencode_layer_matrices, -1);
			rb_define_method(rb_optimizer_klass, "export", (rb_method)method_export_feature_images, -1);
			
			// number of iterations
			rb_define_method(rb_optimizer_klass, "steps", (rb_method)method_autoencode_numSteps,0);
			
			// Neural-Net shape:
			rb_define_method(rb_optimizer_klass, "hidden_size", (rb_method)method_autoencode_hiddenSize,0);
			rb_define_method(rb_optimizer_klass, "visible_size", (rb_method)method_autoencode_visibleSize,0);

			// Autoencoder parameters:
			rb_define_method(rb_optimizer_klass, "beta", (rb_method)method_autoencode_beta,0);
			rb_define_method(rb_optimizer_klass, "lambda", (rb_method)method_autoencode_lambda,0);
			rb_define_method(rb_optimizer_klass, "rho", (rb_method)method_autoencode_rho,0);

			// Neural-Net model:
			rb_define_method(rb_optimizer_klass, "number_of_neurons", (rb_method)method_autoencode_number_of_neurons,0);
			rb_define_method(rb_optimizer_klass, "input_size", (rb_method)method_autoencode_input_size,0);
			rb_define_method(rb_optimizer_klass, "output_size", (rb_method)method_autoencode_output_size,0);
			rb_define_method(rb_optimizer_klass, "number_of_parameters", (rb_method)method_autoencode_number_of_parameters,0);

			// Neural-Net evaluation:
			rb_define_method(rb_optimizer_klass, "eval", (rb_method)method_autoencode_eval, 1);

		// Datatypes
		// Ruby methods for samples
			rb_define_singleton_method(rb_optimizer_klass, "getSamples", (rb_method)method_get_samples,-1);
			rb_define_singleton_method(rb_optimizer_klass, "samples", (rb_method)method_create_samples,1);
			rb_define_singleton_method(rb_optimizer_samples_klass, "getSamples", (rb_method)method_get_samples,-1);
			rb_define_singleton_method(rb_optimizer_samples_klass, "samples", (rb_method)method_create_samples,1);

			rb_define_method(rb_optimizer_samples_klass, "visible_size", (rb_method)method_get_visible_size, 0);
			rb_define_method(rb_optimizer_samples_klass, "length", (rb_method)method_samples_get_size,0);
			rb_define_method(rb_optimizer_samples_klass, "size", (rb_method)method_samples_get_size,0);
			rb_define_method(rb_optimizer_samples_klass, "elements", (rb_method)method_samples_get_elements,0);
			rb_define_method(rb_optimizer_samples_klass, "to_a", (rb_method)method_samples_get_elements,0);


		// Shark Vectors:
			rb_define_method(rb_optimizer_realvector_klass, "sqrt", (rb_method)method_realvector_get_sqrt,0);
			rb_define_method(rb_optimizer_realvector_klass, "to_a", (rb_method)method_realvector_to_ary, 0);
			rb_define_method(rb_optimizer_realvector_klass, "*", (rb_method)method_realvector_multiply, 1);
			rb_define_method(rb_optimizer_realvector_klass, "/", (rb_method)method_realvector_divide, 1);
			rb_define_method(rb_optimizer_realvector_klass, "length", (rb_method)method_realvector_length, 0);
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

		// Shark Matrices:
			rb_define_method(rb_optimizer_realmatrix_klass, "sqrt", (rb_method)method_realmatrix_get_sqrt,0);
			rb_define_method(rb_optimizer_realmatrix_klass, "to_a", (rb_method)method_realmatrix_to_ary, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "*", (rb_method)method_realmatrix_multiply, 1);
			rb_define_method(rb_optimizer_realmatrix_klass, "/", (rb_method)method_realmatrix_divide, 1);
			rb_define_method(rb_optimizer_realmatrix_klass, "length", (rb_method)method_realmatrix_length, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "size", (rb_method)method_realmatrix_length, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "size1", (rb_method)method_realmatrix_size1, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "size2", (rb_method)method_realmatrix_size2, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "stride1", (rb_method)method_realmatrix_stride1, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "stride2", (rb_method)method_realmatrix_stride2, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "fill", (rb_method)method_realmatrix_fill, 1);
			rb_define_method(rb_optimizer_realmatrix_klass, "clear", (rb_method)method_realmatrix_clear, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "-@", (rb_method)method_realmatrix_negate,0);
			rb_define_method(rb_optimizer_realmatrix_klass, "[]", (rb_method)method_realmatrix_query,2);
			rb_define_method(rb_optimizer_realmatrix_klass, "resize", (rb_method)method_realmatrix_resize, 2);
			rb_define_method(rb_optimizer_realmatrix_klass, "[]=", (rb_method)method_realmatrix_insert,3);
			rb_define_alloc_func(rb_optimizer_realmatrix_klass, (rb_alloc_func_t) method_realmatrix_allocate);
			rb_define_method(rb_optimizer_realmatrix_klass, "initialize", (rb_method)method_realmatrix_initialize,-1);

		// Shark Regression sets for supervisied / labeled learning:
			rb_define_method(rb_optimizer_regressionset_klass, "visible_size",   (rb_method)method_regressionset_get_visible_size, 0);
			rb_define_method(rb_optimizer_regressionset_klass, "labels",         (rb_method)method_regressionset_get_labels, 0);
			rb_define_method(rb_optimizer_regressionset_klass, "inputs",         (rb_method)method_regressionset_get_inputs, 0);
			rb_define_method(rb_optimizer_regressionset_klass, "length",         (rb_method)method_regressionset_get_size,0);
			rb_define_method(rb_optimizer_regressionset_klass, "size",           (rb_method)method_regressionset_get_size,0);
			rb_define_method(rb_optimizer_regressionset_klass, "elements",       (rb_method)method_regressionset_get_inputs,0);
			rb_define_method(rb_optimizer_regressionset_klass, "to_a",           (rb_method)method_regressionset_get_inputs,0);
			rb_define_singleton_method(rb_optimizer_klass, "regression_dataset", (rb_method)method_regressionset_create,-1);

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

		// <deprecated>
		// rb_define_singleton_method(rb_optimizer_klass, "create_unlabeled_data", (rb_method)method_create_unlabeleddata, 1);
		// </deprecated>

	}
}