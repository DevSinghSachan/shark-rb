#include "rb_conversions.h"
#define StringValueCStr(v) rb_string_value_cstr(&(v))

typedef shark::blas::vector<double> shark::RealVector;

VALUE rb_sym_new(const char *s) {
	return ID2SYM(rb_intern(s));
}

VALUE method_is_a_directory(VALUE path) {
	return rb_file_directory_p(Qnil, path);
}

shark::RealVector rb_ary_to_1d_realvector(VALUE ary) {
	int length = RARRAY_LEN(ary);
	shark::RealVector vector(length);
	for (int i=0; i < length;i++) {
		vector(i) = NUM2DBL(rb_ary_entry(ary, i));
	}
	return vector;
}

shark::RealMatrix rb_ary_to_realmatrix(VALUE ary) {

	int rows = RARRAY_LEN(ary);
	int cols = rows > 0 ? RARRAY_LEN(rb_ary_entry(ary, 0)) : 0;
	shark::RealMatrix matrix(rows, cols);
	
	// accessing ary[i][j] and placing it in matrix(i, j):
	for (int i=0;i<rows;i++)
		for (int j=0;j<cols;j++)
			matrix(i,j) = NUM2DBL(rb_ary_entry(rb_ary_entry(ary, i), j));

	return matrix;
}

shark::RealMatrix rb_1d_ary_to_realmatrix(VALUE ary) {
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


VALUE realmatrix_to_rb_ary(const shark::RealMatrix& W) {
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

VALUE realmatrix_to_rb_ary(const shark::RealMatrix& W, bool two_d_array) {
	if (two_d_array == 1 && (W.size1() == 1 || W.size2() == 1)) {
		VALUE matrix = rb_ary_new2((int)W.size1()*W.size2());
		bool tall = W.size1() > W.size2() ? 1 : 0;
		size_t length = tall ? W.size1() : W.size2();
		for (size_t i = 0; i < length; ++i)
			rb_ary_store(matrix, (int)i, rb_float_new(tall ? W(i,0) : W(0,i)));
		return matrix;
	} else {
		return realmatrix_to_rb_ary(W);
	}
	
}

VALUE stdvector_realmatrix_to_rb_ary(const std::vector<shark::RealMatrix> W) {
	VALUE ary = rb_ary_new2((int)W.size());
	for (size_t i = 0; i < W.size(); ++i) {
		rb_ary_store(ary, (int)i,realmatrix_to_rb_ary(W[i]));
	}
	return ary;
}

std::vector<shark::RealVector> realvector_to_stdvector(shark::RealVector vector) {
	std::vector<shark::RealVector> vectors;
	vectors.push_back(vector);
	return vectors;
}
std::vector<double> realvector_to_stdvectordouble(shark::RealVector vector) {
	std::vector<double> vectors(vector.size());
	for (size_t i=0;i<vector.size();i++) {
		vectors[i] = vector[i];
	}
	return vectors;
}

std::vector<double> rb_ary_to_stdvector(VALUE ary) {
	int length = RARRAY_LEN(ary);
	std::vector<double> vector(length);
	for (int i=0; i < length;i++) {
		vector[i] = NUM2DBL(rb_ary_entry(ary, i));
	}
	return vector;
}

std::vector<shark::RealVector> rb_ary_to_realvector(VALUE ary) {
	int width = RARRAY_LEN(ary);
	std::vector<shark::RealVector> vectors;
	if (width > 0) {
		if (TYPE(rb_ary_entry(ary, 0)) == T_ARRAY) {
			int height = width > 0 ? RARRAY_LEN(rb_ary_entry(ary, 0)) : 0;
			for (int i=0;i<width;i++) {
				shark::RealVector vector(height);
				for (int j=0;j<height;j++) {
					vector(j) = NUM2DBL(rb_ary_entry(rb_ary_entry(ary, i), j));
				}
				vectors.push_back(vector);
			}
			return vectors;
		} else if (TYPE(rb_ary_entry(ary, 0)) == T_FIXNUM || TYPE(rb_ary_entry(ary, 0)) == T_FLOAT) {
			shark::RealVector vector(width);
			for (int j=0;j<width;j++) {
				vector(j) = NUM2DBL(rb_ary_entry(ary, j));
			}
			vectors.push_back(vector);
		}
	}
	return vectors;
}

shark::UnlabeledData<shark::RealVector> rb_ary_to_unlabeleddata(VALUE ary) {
	std::vector<shark::RealVector> created_data = rb_ary_to_realvector(ary);
	shark::UnlabeledData<shark::RealVector> samples = shark::createDataFromRange(created_data);
	return samples;
}

VALUE realvector_to_rb_ary(const shark::RealVector& W) {
	VALUE matrix = rb_ary_new2((int)W.size());
	for (size_t i = 0; i < W.size(); ++i)
	{
		rb_ary_store(matrix, (int)i, rb_float_new(W(i)));
	}
	return matrix;
}

VALUE stdvector_realvector_to_rb_ary(const std::vector<shark::RealVector> W) {
	VALUE ary = rb_ary_new2((int)W.size());
	for (size_t i = 0; i < W.size(); ++i) {
		rb_ary_store(ary, (int)i,realvector_to_rb_ary(W[i]));
	}
	return ary;
}