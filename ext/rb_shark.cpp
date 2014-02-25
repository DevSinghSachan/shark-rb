// Ruby helpers and classes:
#include "rb_conversions.h"
#include "rb_classes.h"
// datatypes:
#include "rb_Datatypes.h"
// algorithms:
#include "rb_LinearModel.h"
#include "rb_PCA.h"

// RBM types:
#include "rb_BinaryRBM.h"
#include "rb_GaussianBinaryRBM.h"
#include "rb_RBMBinaryLayer.h"
#include "rb_RBMGaussianLayer.h"
// additional RBM goodies:
#include "rb_RBM_Analytics.h"

// Objective function optimizers:
#include "rb_SteepestDescent.h"
#include "rb_LBFGS.h"
// Objective functions should return their value / assignments.
// non-implemented:
// Quickprop
// Noisy R Prop
// LineSearch
// IRLS
// CG
// #include "rb_BFGS.h"
// #include "rb_Rprop.h"

// Objective functions:
#include "rb_BinaryCD.h"
#include "rb_ExactGradient.h"

// samples problems:
#include "rb_BarsAndStripes.h"

// miscellaneous:
#include <shark/Data/Pgm.h>
#include "Optimizer.h"

#include "extras/utils/rb_pointer_wrapping.extras"

#include "rb_RNG_Binomial.h"
#include "rb_RNG_Cauchy.h"
#include "rb_RNG_Uniform.h"
#include "rb_RNG_Weibull.h"

#include "extras/utils/rb_generated_headers.extras"

using namespace std;
using namespace shark;

// Big Wrapper for everything (not good);
extern VALUE rb_optimizer_klass;
// Datatypes:;

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
				rb_UnlabeledData::rb_class(),
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
				rb_RealVector::rb_class(),
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
	if (CLASS_OF(rb_data) != rb_RealVector::rb_class())
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

/* end Ruby Stuff */

static VALUE method_autoencode(int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	unsigned int numhidden = 25;
	double rho             = 0.01; // Sparsity parameter
	double beta            = 3.0; // Regularization parameter
	double lambda          = 0.0002; // Weight decay paramater
	int visibleSize        = 0;
	shark::RegressionDataset data;

	if (number_of_arguments == 1 && TYPE(ruby_arguments[0]) == T_DATA ) {
		if (CLASS_OF(ruby_arguments[0]) != rb_RegressionDataset::rb_class())
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
			if (CLASS_OF(rb_data) != rb_RegressionDataset::rb_class())
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
			rb_RealMatrix::rb_class(),
			new rb_RealMatrix(matrix)
		);
	} else if (TYPE(rb_layer_index) == T_DATA && CLASS_OF(rb_layer_index) == rb_RealVector::rb_class()) {
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
		//   - separate SparseFFNet from autoencoder
		//   - Add RealVector/Int labeleddata for classification
		//   - Add more cases
		//   - Add GaussianBinaryRBM
		//   - Add tempering RBM

		// to respect original naming conventions:
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

		// Datatypes:
		Init_RealVector();
		Init_RealVectorReference();
		Init_RealMatrix();
		Init_RealMatrixReference();
		Init_RegressionDataset();
		Init_UnlabeledData();
		Init_SolutionSet();
		Init_RealMatrixRow();
		Init_RealMatrixColumn();

		// Objective Functions:
		Init_BinaryCD();
		Init_ExactGradient();
		// Add friends.
		
		// Random other stuff & Algorithms:
		Init_LinearModel();
		Init_PCA();
		Init_BinaryRBM();
		Init_GaussianBinaryRBM();
		Init_RBM_Analytics();
		Init_RBMBinaryLayer();
		Init_RBMGaussianLayer();

		// Objective Function optimizers:
		Init_Steepest_Descent();
		Init_LBFGS();

		// Sample Problems:
		Init_BarsAndStripes();

		Init_Generated_Headers();

	}
}