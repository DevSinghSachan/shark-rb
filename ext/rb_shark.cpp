#include "Optimizer.h"
#include "rb_RealVector.h"
#include "rb_UnlabeledData.h"
#include "rb_RegressionDataset.h"
#include "rb_RealMatrix.h"
#include "rb_LinearModel.h"
#include "rb_PCA.h"
#include "rb_BinaryRBM.h"
#include "rb_GaussianBinaryRBM.h"
#include "rb_SteepestDescent.h"

#include "rb_SteepestDescent.cpp"

#include "rb_LBFGS.h"
#include "rb_BFGS.h"
#include "rb_Rprop.h"
#include <shark/Data/Pgm.h>

using namespace std;
using namespace shark;

VALUE rb_sym_new(const char *s) {
	return ID2SYM(rb_intern(s));
}

static VALUE method_is_a_directory(VALUE path) {
	return rb_file_directory_p(Qnil, path);
}

VALUE rb_array_klass                   = rb_define_class("Array", rb_cObject);

// Big Wrapper for everything (not good):
VALUE rb_optimizer_klass               = rb_define_class("Optimizer", rb_cObject);

// Deprecated garbage:
VALUE rb_optimizer_samples_klass       = rb_define_class_under(rb_optimizer_klass, "Samples", rb_cObject);

// Datatypes:
VALUE rb_optimizer_realvector_klass    = rb_define_class_under(rb_optimizer_klass, "RealVector", rb_cObject);
VALUE rb_optimizer_unlabeleddata_klass = rb_define_class_under(rb_optimizer_klass, "UnlabeledData", rb_cObject);
VALUE rb_optimizer_regressionset_klass = rb_define_class_under(rb_optimizer_klass, "RegressionDataset", rb_cObject);
VALUE rb_optimizer_realmatrix_klass    = rb_define_class_under(rb_optimizer_klass, "RealMatrix", rb_cObject);

// LinearModel
VALUE rb_optimizer_linearmodel_klass   = rb_define_class_under(rb_optimizer_klass, "LinearModel", rb_cObject);

// PCA algorithm:
VALUE rb_optimizer_pca_klass           = rb_define_class_under(rb_optimizer_klass, "PCA", rb_cObject);

// RBM
VALUE rb_optimizer_binaryrbm_klass     = rb_define_class_under(rb_optimizer_klass, "BinaryRBM", rb_cObject);
VALUE rb_optimizer_gaussbinaryrbm_klass= rb_define_class_under(rb_optimizer_klass, "GaussianBinaryRBM", rb_cObject);

VALUE rb_algorithms_module               = rb_define_module_under(rb_optimizer_klass, "Algorithms");

// Algorithms:
VALUE rb_optimizer_steepestdescent_klass = rb_define_class_under(rb_algorithms_module, "SteepestDescent", rb_cObject);
VALUE rb_optimizer_rprop_klass           = rb_define_class_under(rb_algorithms_module, "RProp", rb_cObject);
VALUE rb_optimizer_bfgs_klass            = rb_define_class_under(rb_algorithms_module, "BFGS", rb_cObject);
VALUE rb_optimizer_lbfgs_klass           = rb_define_class_under(rb_algorithms_module, "LBFGS", rb_cObject);


#define StringValueCStr(v) rb_string_value_cstr(&(v))

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
		size_t length = tall ? W.size1() : W.size2();
		for (size_t i = 0; i < length; ++i)
			rb_ary_store(matrix, (int)i, rb_float_new(tall ? W(i,0) : W(0,i)));
		return matrix;
	} else {
		return realmatrix_to_rb_ary(W);
	}
	
}

VALUE stdvector_realmatrix_to_rb_ary(const std::vector<RealMatrix> W) {
	VALUE ary = rb_ary_new2((int)W.size());
	for (size_t i = 0; i < W.size(); ++i) {
		rb_ary_store(ary, (int)i,realmatrix_to_rb_ary(W[i]));
	}
	return ary;
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

VALUE realvector_to_rb_ary(const RealVector& W) {
	VALUE matrix = rb_ary_new2((int)W.size());
	for (size_t i = 0; i < W.size(); ++i)
	{
		rb_ary_store(matrix, (int)i, rb_float_new(W(i)));
	}
	return matrix;
}

VALUE stdvector_realvector_to_rb_ary(const std::vector<RealVector> W) {
	VALUE ary = rb_ary_new2((int)W.size());
	for (size_t i = 0; i < W.size(); ++i) {
		rb_ary_store(ary, (int)i,realvector_to_rb_ary(W[i]));
	}
	return ary;
}

VALUE stdvector_realvector_to_rb_ary_of_realvectors(const std::vector<RealVector> W) {
	VALUE ary = rb_ary_new2((int)W.size());
	for (size_t i = 0; i < W.size(); ++i) {
		rb_ary_store(ary, (int)i, wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector(W[i])
		));
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

VALUE method_pca_allocate (VALUE klass) {
return wrap_pointer<rb_PCA>(
			rb_optimizer_pca_klass,
			new rb_PCA()
		);
};

VALUE method_pca_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE rb_data,
			rb_whitening;

	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"02",
		&rb_data,
		&rb_whitening);

	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	if (TYPE(rb_data) == T_DATA) {

		if (CLASS_OF(rb_data) != rb_optimizer_unlabeleddata_klass)
			rb_raise(rb_eArgError, "PCA takes either:\n - UnlabeledData, and a boolean choice whether to do whitening, OR\n - a boolean whether to do whitening, OR\n - no parameters.");

		bool whitening = true;
		rb_UnlabeledData *d;
		Data_Get_Struct(rb_data, rb_UnlabeledData, d);

		if (rb_whitening != Qnil) {
			if (rb_whitening == Qtrue) {
				whitening = true;
			} else {
				whitening = false;
			}
		}
		p->m_whitening = whitening;
		p->trainer.setWhitening(whitening);
		p->trainer.setData(d->data);

	} else if (TYPE(rb_data) != Qnil) {
		if (rb_data == Qtrue) {
			p->setWhitening(true);
			p->m_whitening = true;
		} else {
			p->setWhitening(false);
			p->m_whitening = false;
		}
	} // else No settings made to structure.. doing that later I guess.

	return self;
};


VALUE method_pca_train (VALUE self, VALUE rb_model, VALUE rb_inputs) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);
	Check_Type(rb_model, T_DATA);
	Check_Type(rb_inputs, T_DATA);
	if (CLASS_OF(rb_model) != rb_optimizer_linearmodel_klass || CLASS_OF(rb_inputs) != rb_optimizer_unlabeleddata_klass)
		rb_raise(rb_eArgError, "PCA is trained using a LinearModel and UnlabeledData.");

	rb_LinearModel *m;
	rb_UnlabeledData *d;
	Data_Get_Struct(rb_model, rb_LinearModel, m);
	Data_Get_Struct(rb_inputs, rb_UnlabeledData, d);

	p->train(m->model, d->data);

	return self;
};

VALUE method_pca_setWhitening (VALUE self, VALUE whitening) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	if (whitening == Qtrue) {
		p->m_whitening = true;
		p->setWhitening(true);
	} else {
		p->m_whitening = false;
		p->setWhitening(false);
	}
	return self;
};

VALUE method_pca_get_whitening (VALUE self) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);
	return (p->whitening() ? Qtrue : Qfalse);
};

VALUE method_pca_setData (VALUE self, VALUE rb_data) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);
	Check_Type(rb_data, T_DATA);
	if (CLASS_OF(rb_data) != rb_optimizer_unlabeleddata_klass)
		rb_raise(rb_eArgError, "PCA's data is set using UnlabeledData.");
	rb_UnlabeledData *d;
	Data_Get_Struct(rb_data, rb_UnlabeledData, d);

	p->setData(d->data);
	return self;
};

VALUE method_pca_encoder (VALUE self, VALUE rb_model, VALUE numDimensions) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	Check_Type(numDimensions, T_FIXNUM);
	Check_Type(rb_model, T_DATA);
	if (CLASS_OF(rb_model) != rb_optimizer_linearmodel_klass)
		rb_raise(rb_eArgError, "PCA's encoder must be used with a LinearModel");

	rb_LinearModel *m;
	Data_Get_Struct(rb_model, rb_LinearModel, m);

	p->encoder(m->model, NUM2INT(numDimensions));

	return self;
};

VALUE method_import_pgm_dir (VALUE self, VALUE rb_directory) {
	Check_Type(rb_directory, T_STRING);
	VALUE is_directory = method_is_a_directory(rb_directory);

	const char *path = StringValueCStr(rb_directory);

	if (is_directory == Qtrue) {
		UnlabeledData<RealVector> images;
		Data<ImageInformation> imagesInfo;

		try {
			importPGMSet(path, images, imagesInfo);

			return wrap_pointer<rb_UnlabeledData>(
				rb_optimizer_unlabeleddata_klass,
				new rb_UnlabeledData(images)
				);

		} catch(...) {
			rb_raise(rb_eRuntimeError, "Could not find .pgm files in directory.");
			return self;
		}
	} else {
		try {
			RealVector vec;
			ImageInformation imgInfo;
			importPGM(path, vec, imgInfo.x, imgInfo.y);

			return wrap_pointer<rb_RealVector>(
				rb_optimizer_realvector_klass,
				new rb_RealVector(vec)
				);
		} catch(...) {
			rb_raise(rb_eRuntimeError, "Could not find .pgm file in this path.");

			return self;
		}
	}
}

VALUE method_export_pgm (VALUE self, VALUE data_hash) {
	Check_Type(data_hash, T_HASH);
	VALUE rb_data   = rb_hash_aref(data_hash, rb_sym_new("image"));
	VALUE rb_path   = rb_hash_aref(data_hash, rb_sym_new("path"));
	VALUE rb_height = rb_hash_aref(data_hash, rb_sym_new("height"));
	VALUE rb_width  = rb_hash_aref(data_hash, rb_sym_new("width"));
	VALUE normalize = rb_hash_aref(data_hash, rb_sym_new("normalize"));

	Check_Type(rb_data, T_DATA);
	if (CLASS_OF(rb_data) != rb_optimizer_realvector_klass)
		rb_raise(rb_eArgError, "PGM are exported from a RealVector");

	rb_RealVector *v;
	Data_Get_Struct(rb_data, rb_RealVector, v);

	Check_Type(rb_height, T_FIXNUM);
	Check_Type(rb_width, T_FIXNUM);
	Check_Type(rb_path, T_STRING);
	try {
		exportPGM(StringValueCStr(rb_path), v->data, NUM2INT(rb_width), NUM2INT(rb_height), (normalize == Qnil || normalize == Qtrue));
		return rb_path;
	} catch (...) {
		rb_raise(rb_eRuntimeError, "Could not write PGM file.");
		return rb_path;
	}
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

VALUE method_pca_test (VALUE self) {

	const char *facedirectory = "/Users/jonathanraiman/Desktop/orl_faces/"; //< set this to the directory containing the face database
	UnlabeledData<RealVector> images;
	Data<ImageInformation> imagesInfo;
	cout << "Read images ... " << flush;
	try {
		importPGMSet(facedirectory, images, imagesInfo);
	} catch(...) {
		cerr << "[PCATutorial] could not open face database directory\n\nThis file is part of the \"Principal Component Analysis\" tutorial.\nThe tutorial requires that you download the Cambridge Face Database\nfrom http://www.cl.cam.ac.uk/research/dtg/attarchive/facedatabase.html\nand adjust the facedirectory path in the source code to the directory\ncontaining the faces in PGM format." << endl;
		return 1;
	}
	cout << "done." << endl;
	
	unsigned l = images.numberOfElements();   // number of samples
	unsigned x = imagesInfo.element(0).x;     // width of images
	unsigned y = imagesInfo.element(0).y;     // height of images
	
	cout << "Eigenvalue decomposition ... " << flush;
	PCA pca(images);
	cout << "done." << endl;
	
	cout << "Writing mean face and eigenvalues... " << flush;
	ofstream ofs("facesEigenvalues.csv");
	for(unsigned i=0; i<l; i++) 
		ofs << pca.eigenvalue(i) << endl;
	exportPGM("facesMean.pgm", pca.mean(), x, y);
	cout << "done. " << endl;

	cout << "Encoding ... " << flush;
	unsigned m = 299;
	LinearModel<> enc;
	pca.encoder(enc, m);
	Data<RealVector> encodedImages = enc(images);
	cout << "done. " << endl;

	unsigned sampleImage = 0;
	cout << "Reconstructing face " << sampleImage << " ... " << flush;
	boost::format fmterTrue("face%d.pgm");
	exportPGM((fmterTrue % sampleImage).str().c_str(), images.element(sampleImage), x, y);
	LinearModel<> dec;
	pca.decoder(dec, m);
	boost::format fmterRec("facesReconstruction%d-%d.pgm");
	exportPGM((fmterRec % sampleImage % m).str().c_str(), dec(encodedImages.element(sampleImage)), x, y);
	cout << "done." << endl;

	return self;
}

VALUE method_pca_decoder (VALUE self, VALUE rb_model, VALUE numDimensions) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	Check_Type(numDimensions, T_FIXNUM);
	Check_Type(rb_model, T_DATA);
	if (CLASS_OF(rb_model) != rb_optimizer_linearmodel_klass)
		rb_raise(rb_eArgError, "PCA's decoder must be used with a LinearModel");

	rb_LinearModel *m;
	Data_Get_Struct(rb_model, rb_LinearModel, m);

	p->decoder(m->model, NUM2INT(numDimensions));

	return self;
};

VALUE method_pca_eigenvalues (VALUE self) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(p->eigenvalues())
	);
};

VALUE method_pca_eigenvalue (VALUE self, VALUE position) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	Check_Type(position, T_FIXNUM);

	return rb_float_new(p->eigenvalue(NUM2INT(position)));
};

VALUE method_pca_eigenvectors (VALUE self) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	return wrap_pointer<rb_RealMatrix>(
		rb_optimizer_realmatrix_klass,
		new rb_RealMatrix(p->eigenvectors())
	);
};

VALUE method_pca_mean (VALUE self) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(p->mean())
	);
};


VALUE method_linearmodel_allocate (VALUE klass) {
	return wrap_pointer<rb_LinearModel>(
			rb_optimizer_linearmodel_klass,
			new rb_LinearModel()
		);
}

VALUE method_linearmodel_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	
	VALUE rb_matrix,
			rb_offsets,
			rb_bias;

	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"03",
		&rb_matrix,
		&rb_offsets,
		&rb_bias);

	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);

	if (TYPE(rb_matrix) == T_DATA) {

		if (CLASS_OF(rb_matrix) != rb_optimizer_realmatrix_klass)
			rb_raise(rb_eArgError, "LinearModel takes either:\n - a number of inputs, a number of outputs, and a boolean choice whether to use offsets, OR\n - a RealMatrix and a RealVector, OR\n - no parameters.");

		rb_RealMatrix *mat;
		Data_Get_Struct(rb_matrix, rb_RealMatrix, mat);

		if (TYPE(rb_offsets) == T_DATA) {
			// With offsets vector.
			if (CLASS_OF(rb_offsets) != rb_optimizer_realvector_klass)
				rb_raise(rb_eArgError, "LinearModel takes either:\n - a number of inputs, a number of outputs, and a boolean choice whether to use offsets, OR\n - a RealMatrix and a RealVector, OR\n - no parameters.");
			rb_RealVector *vec;
			Data_Get_Struct(rb_offsets, rb_RealVector, vec);
			m->setStructure(mat->data, vec->data);

		} else {
			// Without offsets vector.
			m->setStructure(mat->data, *new RealVector());

		}

	} else if (TYPE(rb_matrix) == T_FIXNUM && TYPE(rb_offsets) == T_FIXNUM) {
		// Using numbers to choose structure.
		m->setStructure(NUM2INT(rb_matrix), NUM2INT(rb_offsets), rb_bias == Qtrue ? true : false);
	} else {
		// No settings for structure.
	}

	return self;
}

VALUE method_linearmodel_offset(VALUE self) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);
	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(m->offset())
	);
};

VALUE method_linearmodel_matrix(VALUE self) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);
	return wrap_pointer<rb_RealMatrix>(
		rb_optimizer_realmatrix_klass,
		new rb_RealMatrix(m->matrix())
	);
};


VALUE method_linearmodel_setParameterVector(VALUE self, VALUE paramVector) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);

	Check_Type(paramVector, T_DATA);
	if (CLASS_OF(paramVector) != rb_optimizer_realvector_klass)
		rb_raise(rb_eArgError, "LinearModel's parameter vector can only be set using a RealVector.");

	rb_RealVector *vec;
	Data_Get_Struct(paramVector, rb_RealVector, vec);

	m->setParameterVector(vec->data);
	return self;
};

VALUE method_linearmodel_parameterVector(VALUE self) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);
	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(m->parameterVector())
	);
};

VALUE method_linearmodel_numberOfParameters(VALUE self) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);
	return INT2FIX(m->numberOfParameters());
};

VALUE method_linearmodel_outputSize(VALUE self) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);
	return INT2FIX(m->outputSize());
};

VALUE method_linearmodel_inputSize(VALUE self) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);
	return INT2FIX(m->inputSize());
};

VALUE method_linearmodel_hasOffset(VALUE self) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);
	return (m->hasOffset() ? Qtrue : Qfalse);
};

VALUE method_linearmodel_eval(VALUE self, VALUE dataset) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);

	if (TYPE(dataset) == T_ARRAY) {

		if (RARRAY_LEN(dataset) > 0) {

			if (TYPE(rb_ary_entry(dataset, 0)) == T_ARRAY) {
				// 2d array case.

				return wrap_pointer<rb_UnlabeledData>(
					rb_optimizer_unlabeleddata_klass,
					new rb_UnlabeledData(rb_ary_to_unlabeleddata(dataset))
				);

			} else {
				return wrap_pointer<rb_RealVector>(
					rb_optimizer_realvector_klass,
					new rb_RealVector(m->eval(rb_ary_to_1d_realvector(dataset)))
				);
			}
		} else {
			rb_raise(rb_eArgError, "Can only evaluate non-empty data. Look at LinearModel#offset for the constant part of the transformation.");
		}
	} else {
		// add the ability to eval Arrays here!!
		Check_Type(dataset, T_DATA);

		if (CLASS_OF(dataset) == rb_optimizer_unlabeleddata_klass) {
			rb_UnlabeledData *d;
			Data_Get_Struct(dataset, rb_UnlabeledData, d);

			return wrap_pointer<rb_UnlabeledData>(
				rb_optimizer_unlabeleddata_klass,
				new rb_UnlabeledData(m->eval(d->data))
			);
		} else if (CLASS_OF(dataset) == rb_optimizer_realvector_klass) {
			rb_RealVector *d;
			Data_Get_Struct(dataset, rb_RealVector, d);

			return wrap_pointer<rb_RealVector>(
				rb_optimizer_realvector_klass,
				new rb_RealVector(m->eval(d->data))
			);
		} else {
			rb_raise(rb_eArgError, "LinearModel can evalute UnlabeledData, RealVectors, or Arrays.");
		}
		return self;
	}
};

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

VALUE method_realmatrix_add (VALUE self, VALUE rb_other) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	if (TYPE(rb_other) == T_DATA) {
		if (CLASS_OF(rb_other) == rb_optimizer_realmatrix_klass) {
			rb_RealMatrix *other;
			Data_Get_Struct(rb_other, rb_RealMatrix, other);
			if (other->data.size1() == m->data.size1() && other->data.size2() == m->data.size2()) {
				return wrap_pointer<rb_RealMatrix>(
					rb_optimizer_realmatrix_klass,
					new rb_RealMatrix(m->data + other->data)
					);
			} else if (other->data.size2() == m->data.size1() && other->data.size1() == m->data.size2()) {
				return wrap_pointer<rb_RealMatrix>(
					rb_optimizer_realmatrix_klass,
					new rb_RealMatrix(m->data + trans(other->data))
					);
			} else {
				rb_raise(rb_eArgError, "The Matrices you are summing don't have the same dimensions, so their sum is infeasible using our current (human) technology.");
			}
		} else if (CLASS_OF(rb_other) == rb_optimizer_realvector_klass) {
			rb_RealVector *other;
			Data_Get_Struct(rb_other, rb_RealVector, other);
			if (other->data.size() == m->data.size1()) {
				if (m->data.size2() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(column(m->data, 0) + other->data));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else if (other->data.size() == m->data.size2()) {
				if (m->data.size1() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(row(m->data, 0) + other->data));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else {
				rb_raise(rb_eArgError, "The RealVector you are adding doesn't have the same dimensions as either RealMatrix dimension, so their sum is infeasible using our current (human) technology.");
			}
		}
	} else if (TYPE(rb_other) == T_ARRAY) {
		if (RARRAY_LEN(rb_other) > 0 && TYPE(rb_ary_entry(rb_other, 0)) == T_ARRAY) {
			RealMatrix other = rb_ary_to_realmatrix(rb_other);
			if (other.size1() == m->data.size1() && other.size2() == m->data.size2()) {
				return wrap_pointer<rb_RealMatrix>(
					rb_optimizer_realmatrix_klass,
					new rb_RealMatrix(m->data + other)
					);
			} else if (other.size2() == m->data.size1() && other.size1() == m->data.size2()) {
				return wrap_pointer<rb_RealMatrix>(
					rb_optimizer_realmatrix_klass,
					new rb_RealMatrix(m->data + trans(other))
					);
			} else {
				rb_raise(rb_eArgError, "The Matrices you are summing don't have the same dimensions, so their sum is infeasible using our current (human) technology.");
			}
		} else {
			RealVector other = rb_ary_to_1d_realvector(rb_other);
			if (other.size() == m->data.size1()) {
				if (m->data.size2() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(row(m->data, 0) + other));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else if (other.size() == m->data.size2()) {
				if (m->data.size1() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(column(m->data, 0) + other));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else {
				rb_raise(rb_eArgError, "The RealVector you are adding doesn't have the same dimensions as either RealMatrix dimension, so their sum is infeasible using our current (human) technology.");
			}
		}
	} else {
		rb_raise(rb_eArgError, "You can only add Arrays, RealVectors, and RealMatrices to a RealMatrix.");
	}
	return self;
}

VALUE method_realmatrix_add_equals (VALUE self, VALUE rb_other) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	if (TYPE(rb_other) == T_DATA) {
		if (CLASS_OF(rb_other) == rb_optimizer_realmatrix_klass) {
			rb_RealMatrix *other;
			Data_Get_Struct(rb_other, rb_RealMatrix, other);
			if (other->data.size1() == m->data.size1() && other->data.size2() == m->data.size2()) {
				m->data += other->data;
			} else if (other->data.size2() == m->data.size1() && other->data.size1() == m->data.size2()) {
				m->data += trans(other->data);
			} else {
				rb_raise(rb_eArgError, "The Matrices you are summing don't have the same dimensions, so their sum is infeasible using our current (human) technology.");
			}
		} else if (CLASS_OF(rb_other) == rb_optimizer_realvector_klass) {
			rb_RealVector *other;
			Data_Get_Struct(rb_other, rb_RealVector, other);
			if (other->data.size() == m->data.size1()) {
				if (m->data.size2() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(column(m->data, 0) + other->data));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else if (other->data.size() == m->data.size2()) {
				if (m->data.size1() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(row(m->data, 0) + other->data));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else {
				rb_raise(rb_eArgError, "The RealVector you are adding doesn't have the same dimensions as either RealMatrix dimension, so their sum is infeasible using our current (human) technology.");
			}
		}
	} else if (TYPE(rb_other) == T_ARRAY) {
		if (RARRAY_LEN(rb_other) > 0 && TYPE(rb_ary_entry(rb_other, 0)) == T_ARRAY) {
			RealMatrix other = rb_ary_to_realmatrix(rb_other);
			if (other.size1() == m->data.size1() && other.size2() == m->data.size2()) {
				m->data += other;
			} else if (other.size2() == m->data.size1() && other.size1() == m->data.size2()) {
				m->data += trans(other);
			} else {
				rb_raise(rb_eArgError, "The Matrices you are summing don't have the same dimensions, so their sum is infeasible using our current (human) technology.");
			}
		} else {
			RealVector other = rb_ary_to_1d_realvector(rb_other);
			if (other.size() == m->data.size1()) {
				if (m->data.size2() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(row(m->data, 0) + other));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else if (other.size() == m->data.size2()) {
				if (m->data.size1() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(column(m->data, 0) + other));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else {
				rb_raise(rb_eArgError, "The RealVector you are adding doesn't have the same dimensions as either RealMatrix dimension, so their sum is infeasible using our current (human) technology.");
			}
		}
	} else {
		rb_raise(rb_eArgError, "You can only add Arrays, RealVectors, and RealMatrices to a RealMatrix.");
	}
	return self;
}

VALUE method_realmatrix_multiply (VALUE self, VALUE multiplier) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	if (TYPE(multiplier) == T_FLOAT || TYPE(multiplier) == T_FIXNUM) {
		return wrap_pointer<rb_RealMatrix>(
			rb_optimizer_realmatrix_klass,
			new rb_RealMatrix((m->data) * NUM2DBL(multiplier))
			);
	} else if (TYPE(multiplier) == T_DATA) {
		VALUE multiplier_klass = CLASS_OF(multiplier);
		if (multiplier_klass == rb_optimizer_realmatrix_klass) {
			rb_RealMatrix *multi_matrix;
			Data_Get_Struct(multiplier, rb_RealMatrix, multi_matrix);
			if ((m->data).size2() != (multi_matrix->data).size1()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % (multi_matrix->data).size1()).str().c_str());
			}/*
			RealMatrix temp(multi_matrix->data.size1(),m->data.size2());
			axpy_prod(m->data, multi_matrix->data, temp);*/
			return wrap_pointer<rb_RealMatrix>(
				rb_optimizer_realmatrix_klass,
				new rb_RealMatrix(prod(m->data, multi_matrix->data))
				);
		} else if (multiplier_klass == rb_optimizer_realvector_klass) {
			rb_RealVector *vec;
			Data_Get_Struct(multiplier, rb_RealVector, vec);
			if ((m->data).size2() != (vec->data).size()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % (vec->data).size()).str().c_str());
			}
			/*RealVector temp(m->data.size2());
			axpy_prod(m->data, vec->data, temp);*/
			// might be a vector?
			return wrap_pointer<rb_RealVector>(
				rb_optimizer_realvector_klass,
				new rb_RealVector(prod(m->data, vec->data))
				);
		} else {
			rb_raise(rb_eArgError, "Can only multiply RealMatrix by a scalar, RealMatrix, Array, or RealVector.");
		}
	} else if (TYPE(multiplier) == T_ARRAY) {
		if (RARRAY_LEN(multiplier) > 0 && TYPE(rb_ary_entry(multiplier, 0)) == T_ARRAY) {
			RealMatrix multi_matrix = rb_ary_to_realmatrix(multiplier);
			if ((m->data).size2() != multi_matrix.size1()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % multi_matrix.size1()).str().c_str());
			}/*
			RealMatrix temp(multi_matrix.size1(),m->data.size2());
			axpy_prod(m->data, multi_matrix, temp);*/
			return wrap_pointer<rb_RealMatrix>(
				rb_optimizer_realmatrix_klass,
				new rb_RealMatrix(prod(m->data, multi_matrix))
			);
		} else if (RARRAY_LEN(multiplier) > 0) {
			RealVector vec = rb_ary_to_1d_realvector(multiplier);
			if ((m->data).size2() != vec.size()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % vec.size()).str().c_str());
			}/*
			RealVector temp(m->data.size2());
			axpy_prod(m->data, vec, temp);*/
			return wrap_pointer<rb_RealVector>(
				rb_optimizer_realvector_klass,
				new rb_RealVector(prod(m->data, vec))
			);
		} else {
			rb_raise(rb_eArgError, "It is tough to multiply by an empty matrix since the computer's imagination is limited by the Kernel.");
		}
	} else {
		rb_raise(rb_eArgError, "Can only multiply RealMatrix by a scalar, RealMatrix, Array, or RealVector.");
	}
	return self;
}
VALUE method_realmatrix_multiply_equals (VALUE self, VALUE multiplier) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	if (TYPE(multiplier) == T_FLOAT || TYPE(multiplier) == T_FIXNUM) {
		(m->data) *= NUM2DBL(multiplier);
	} else if (TYPE(multiplier) == T_DATA) {
		VALUE multiplier_klass = CLASS_OF(multiplier);
		if (multiplier_klass == rb_optimizer_realmatrix_klass) {
			rb_RealMatrix *multi_matrix;
			Data_Get_Struct(multiplier, rb_RealMatrix, multi_matrix);
			if ((m->data).size2() != (multi_matrix->data).size1()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % (multi_matrix->data).size1()).str().c_str());
			}
			// RealMatrix temp(multi_matrix->data.size1(),m->data.size2());
			// axpy_prod(m->data, multi_matrix->data, temp);
			m->data = prod(m->data, multi_matrix->data);
		} else if (multiplier_klass == rb_optimizer_realvector_klass) {
			rb_RealVector *vec;
			Data_Get_Struct(multiplier, rb_RealVector, vec);
			if ((m->data).size2() != (vec->data).size()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % (vec->data).size()).str().c_str());
			}
			/*RealVector temp(m->data.size2());
			axpy_prod(m->data, vec->data, temp);*/
			RealMatrix temp_matrix(m->data.size1(), 1);
			column(temp_matrix, 0) = prod(m->data, vec->data);
			// might be a vector?
			m->data = temp_matrix;
		} else {
			rb_raise(rb_eArgError, "Can only multiply RealMatrix by a scalar, RealMatrix, Array, or RealVector.");
		}
	} else if (TYPE(multiplier) == T_ARRAY) {
		if (RARRAY_LEN(multiplier) > 0 && TYPE(rb_ary_entry(multiplier, 0)) == T_ARRAY) {
			RealMatrix multi_matrix = rb_ary_to_realmatrix(multiplier);
			if ((m->data).size2() != multi_matrix.size1()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % multi_matrix.size1()).str().c_str());
			}
			/*RealMatrix temp(multi_matrix.size1(),m->data.size2());
			axpy_prod(m->data, multi_matrix, temp);*/
			m->data = prod(m->data, multi_matrix);
		} else if (RARRAY_LEN(multiplier) > 0) {
			RealVector vec = rb_ary_to_1d_realvector(multiplier);
			if ((m->data).size2() != vec.size()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % vec.size()).str().c_str());
			}/*
			RealVector temp(m->data.size2());
			axpy_prod(m->data, vec, temp);*/
			RealMatrix temp_matrix(m->data.size1(), 1);
			column(temp_matrix, 0) = prod(m->data, vec);
			m->data = temp_matrix;
		} else {
			rb_raise(rb_eArgError, "It is tough to multiply by an empty matrix since the computer's imagination is limited by the Kernel.");
		}
	} else {
		rb_raise(rb_eArgError, "Can only multiply RealMatrix by a scalar, RealMatrix, Array, or RealVector.");
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

VALUE method_realmatrix_transpose (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);

	return wrap_pointer<rb_RealMatrix>(
		rb_optimizer_realmatrix_klass,
		new rb_RealMatrix(trans(m->data))
		);
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
	Check_Type(index, T_FIXNUM);
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	return INT2FIX((m->data).stride2());
}

VALUE method_realmatrix_get_row (VALUE self, VALUE rb_index) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);

	int index = NUM2INT(rb_index);

	if (index < 0 && (int)(index + m->data.size2()) < 0)
		rb_raise(rb_eArgError, "Out of range of RealMatrix");
	if (index >= (int)(m->data).size2())
		rb_raise(rb_eArgError, "Out of range of RealMatrix");

	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(m->get_row(
			index < 0 ?
				index + m->data.size2() :
				index
			))
		);
}
VALUE method_realmatrix_get_column (VALUE self, VALUE rb_index) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);

	int index = NUM2INT(rb_index),
		neg_index = 0;

	if (index < neg_index && (index + (int)m->data.size1() < neg_index))
		rb_raise(rb_eArgError, "Out of range of RealMatrix");
	if (index >= (int)(m->data).size1())
		rb_raise(rb_eArgError, "Out of range of RealMatrix");

	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(m->get_column(
			index < 0 ?
				index + m->data.size1() :
				index
			))
		);
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
	int neg_index = 0;
	if (NUM2INT(row) < neg_index || NUM2INT(column) < neg_index)
		rb_raise(rb_eArgError, "Can only access positive positions in RealMatrix");
	if (NUM2INT(row) >= (int)(m->data).size1() || NUM2INT(column) >= (int)(m->data).size2())
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
	int neg_index = 0;
	if (NUM2INT(row) < neg_index || NUM2INT(column) < neg_index)
		rb_raise(rb_eArgError, "Can only access positive positions in RealMatrix");
	if (NUM2INT(row) >= (int)(m->data).size1() || NUM2INT(column) >= (int)(m->data).size2())
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

std::vector<RealVector> realmatrix_to_stdvector_realvector (RealMatrix const& W) {
	std::vector<RealVector> vectors;
	for (size_t i=0; i< W.size1(); i++) {
		vectors.push_back(row(W, i));
	}
	return vectors;
}

VALUE method_realmatrix_to_realvectors (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);

	std::vector<RealVector> vectors = realmatrix_to_stdvector_realvector(m->data);

	return stdvector_realvector_to_rb_ary_of_realvectors(vectors);
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
			for (int row = 0; row < height; ++row)
				for (int column = 0; column < width; ++column)
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
	double rho             = 0.01; // Sparsity parameter
	double beta            = 3.0; // Regularization parameter
	double lambda          = 0.0002; // Weight decay paramater
	int visibleSize        = 0;
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

	VALUE output_hash, rb_evaluation, rb_hidden_evaluation;

	rb_evaluation        = realmatrix_to_rb_ary(evaluation[0], 1);
	rb_hidden_evaluation = realmatrix_to_rb_ary(evaluation[1], 1);

	output_hash = rb_hash_new();
	rb_hash_aset(output_hash, rb_sym_new("output_layer"), rb_evaluation);
	rb_hash_aset(output_hash, rb_sym_new("hidden_layer"), rb_hidden_evaluation);

	return output_hash;
}

/*static VALUE method_export_feature_images(int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
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
}*/

extern "C"  {

	void Init_rb_shark() {

		// TODO:
		//   - define .each method,
		//   - define a slice of a matrix as a RealVector.

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
			// rb_define_method(rb_optimizer_klass, "export", (rb_method)method_export_feature_images, -1);

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

			// Import PGM
			rb_define_singleton_method(rb_optimizer_klass, "export_pgm", (rb_method) method_export_pgm, 1);
			rb_define_singleton_method(rb_optimizer_klass, "import_pgm", (rb_method) method_import_pgm_dir, 1);

			// <deprecated>
			// rb_define_singleton_method(rb_optimizer_klass, "create_unlabeled_data", (rb_method)method_create_unlabeleddata, 1);
			// </deprecated>

		// Datatypes

			rb_define_method(rb_array_klass, "to_realvector", (rb_method)method_rb_ary_to_realvector, 0);

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
			rb_define_method(rb_optimizer_realmatrix_klass, "to_realvectors", (rb_method)method_realmatrix_to_realvectors, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "*", (rb_method)method_realmatrix_multiply, 1);
			rb_define_method(rb_optimizer_realmatrix_klass, "*=", (rb_method)method_realmatrix_multiply_equals, 1);
			rb_define_method(rb_optimizer_realmatrix_klass, "/", (rb_method)method_realmatrix_divide, 1);
			rb_define_method(rb_optimizer_realmatrix_klass, "+", (rb_method)method_realmatrix_add, 1);
			rb_define_method(rb_optimizer_realmatrix_klass, "+=", (rb_method)method_realmatrix_add_equals, 1);
			rb_define_method(rb_optimizer_realmatrix_klass, "length", (rb_method)method_realmatrix_length, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "size", (rb_method)method_realmatrix_length, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "transpose", (rb_method)method_realmatrix_transpose, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "~", (rb_method)method_realmatrix_transpose, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "size1", (rb_method)method_realmatrix_size1, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "size2", (rb_method)method_realmatrix_size2, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "stride1", (rb_method)method_realmatrix_stride1, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "stride2", (rb_method)method_realmatrix_stride2, 0);
			rb_define_method(rb_optimizer_realmatrix_klass, "row", (rb_method)method_realmatrix_get_row, 1);
			rb_define_method(rb_optimizer_realmatrix_klass, "column", (rb_method)method_realmatrix_get_column, 1);
			rb_define_method(rb_optimizer_realmatrix_klass, "col", (rb_method)method_realmatrix_get_column, 1);
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

		// Shark LinearModel class:
			rb_define_alloc_func(rb_optimizer_linearmodel_klass,  (rb_alloc_func_t) method_linearmodel_allocate);
			rb_define_method(rb_optimizer_linearmodel_klass, "initialize", (rb_method)method_linearmodel_initialize, -1);
			rb_define_method(rb_optimizer_linearmodel_klass, "set_structure", (rb_method)method_linearmodel_initialize, -1);
			rb_define_method(rb_optimizer_linearmodel_klass, "offset" ,(rb_method) method_linearmodel_offset, 0);
			rb_define_method(rb_optimizer_linearmodel_klass, "matrix",(rb_method) method_linearmodel_matrix, 0);
			rb_define_method(rb_optimizer_linearmodel_klass, "parameter_vector=",(rb_method) method_linearmodel_setParameterVector, 1);
			rb_define_method(rb_optimizer_linearmodel_klass, "parameter_vector",(rb_method) method_linearmodel_parameterVector, 0);
			rb_define_method(rb_optimizer_linearmodel_klass, "number_of_parameters",(rb_method) method_linearmodel_numberOfParameters, 0);
			rb_define_method(rb_optimizer_linearmodel_klass, "output_size",(rb_method) method_linearmodel_outputSize, 0);
			rb_define_method(rb_optimizer_linearmodel_klass, "input_size",(rb_method) method_linearmodel_inputSize, 0);
			rb_define_method(rb_optimizer_linearmodel_klass, "has_offset?",(rb_method) method_linearmodel_hasOffset, 0);
			rb_define_method(rb_optimizer_linearmodel_klass, "eval",(rb_method) method_linearmodel_eval, 1);

		// Shark PCA class:
			rb_define_alloc_func(rb_optimizer_pca_klass,  (rb_alloc_func_t) method_pca_allocate);
			rb_define_method(rb_optimizer_pca_klass, "initialize", (rb_method)method_pca_initialize, -1);
			rb_define_method(rb_optimizer_pca_klass, "train",(rb_method) method_pca_train, 2);
			rb_define_method(rb_optimizer_pca_klass, "whitening=",(rb_method) method_pca_setWhitening, 1);
			rb_define_method(rb_optimizer_pca_klass, "whitening",(rb_method) method_pca_get_whitening, 0);
			rb_define_method(rb_optimizer_pca_klass, "set_data",(rb_method) method_pca_setData, 1);
			rb_define_method(rb_optimizer_pca_klass, "encoder",(rb_method) method_pca_encoder, 2);
			rb_define_method(rb_optimizer_pca_klass, "decoder",(rb_method) method_pca_decoder, 2);
			rb_define_method(rb_optimizer_pca_klass, "eigenvalues",(rb_method) method_pca_eigenvalues, 0);
			rb_define_method(rb_optimizer_pca_klass, "eigenvalue",(rb_method) method_pca_eigenvalue, 1);
			rb_define_method(rb_optimizer_pca_klass, "eigenvectors",(rb_method) method_pca_eigenvectors, 0);
			rb_define_method(rb_optimizer_pca_klass, "mean",(rb_method) method_pca_mean, 0);
			rb_define_method(rb_optimizer_pca_klass, "test", (rb_method) method_pca_test, 0);


	}
}