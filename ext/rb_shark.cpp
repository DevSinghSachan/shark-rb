#include "Optimizer.h"
#include "rb_RealVector.h"
#include "rb_UnlabeledData.h"

using namespace std;
using namespace shark;

VALUE rb_sym_new(const char *s) {
	return ID2SYM(rb_intern(s));
}

VALUE rb_optimizer_klass               = rb_define_class("Optimizer", rb_cObject);
VALUE rb_optimizer_samples_klass       = rb_define_class("Optimizer::Samples", rb_cObject);
VALUE rb_optimizer_realvector_klass    = rb_define_class("Optimizer::RealVector", rb_cObject);
VALUE rb_optimizer_unlabeleddata_klass = rb_define_class("Optimizer::UnlabeledData", rb_cObject);
VALUE rb_optimizer_regressionset_klass = rb_define_class("Optimizer::RegressionDataset", rb_cObject);

template<class Obtype> void delete_objects(Obtype *ptr){
	// ptr.cleanUp();
	delete ptr;
}

template<class Obtype> VALUE wrap_pointer(VALUE klass,Obtype *ptr){ //wrap)c++)obje
	return Data_Wrap_Struct(klass,0,delete_objects<Obtype>,ptr);
}

template<class Obtype> VALUE alloc_ob(VALUE self)
{
	return wrap_pointer<Obtype>(self,new Obtype());//optimizer, function, model, data, loss));//, error));
}

std::vector<shark::RealVector> rb_ary_to_realvector(VALUE ary) {
	int width = RARRAY_LEN(ary);
	int height = RARRAY_LEN(rb_ary_entry(ary, 0));
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

VALUE method_unlabeleddata_to_ary (VALUE self) {
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);
	return stdvector_realvector_to_rb_ary(s->input());
}

VALUE method_unlabeleddata_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE dataset;
	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"01",
		&dataset);
	if (TYPE(dataset) == T_ARRAY) {
		return wrap_pointer<rb_UnlabeledData>(
			rb_optimizer_unlabeleddata_klass,
			new rb_UnlabeledData(rb_ary_to_unlabeleddata(dataset))
		);
	} else {
		return wrap_pointer<rb_UnlabeledData>(
			rb_optimizer_unlabeleddata_klass,
			new rb_UnlabeledData(rb_ary_to_unlabeleddata(rb_ary_new()))
		);
	}
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

VALUE method_unlabeleddata_length (VALUE self) {
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);
	return INT2FIX((s->data).numberOfElements());
}

VALUE method_realvector_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self {
	VALUE dataset;
	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"01",
		&dataset);
	if (TYPE(dataset) == T_ARRAY) {
		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector(rb_ary_to_realvector(dataset))
			);
	} else {
		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector(rb_ary_to_realvector(rb_ary_new()))
			);
	}
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

VALUE method_unlabeleddata_shift (VALUE self, VALUE shift_vector) {
	Check_Type(shift_vector, T_DATA);
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);

	rb_RealVector *v;
	Data_Get_Struct(shift_vector, rb_RealVector, v);
	s->data = transform(s->data, shark::Shift(-(v->data)));
	return self;
}

VALUE method_realvector_multiply (VALUE self, VALUE multiplier) {
	if (TYPE(multiplier) == T_FLOAT || TYPE(multiplier) == T_FIXNUM) {
		rb_RealVector *s;
		Data_Get_Struct(self, rb_RealVector, s);
		s->data = NUM2DBL(multiplier)*(s->data);
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

VALUE method_realvector_divide (VALUE self, VALUE multiplier) {
	if (TYPE(multiplier) == T_FLOAT || TYPE(multiplier) == T_FIXNUM) {
		rb_RealVector *s;
		Data_Get_Struct(self, rb_RealVector, s);
		s->data = (s->data) / NUM2DBL(multiplier);
	} else {
		rb_raise(rb_eArgError, "Can only multiply RealVector by a number");
	}
	return self;
}


VALUE method_unlabeleddata_posshift (VALUE self, VALUE shift_vector) {
	Check_Type(shift_vector, T_DATA);
	rb_UnlabeledData *s;
	Data_Get_Struct(self, rb_UnlabeledData, s);

	rb_RealVector *v;
	Data_Get_Struct(shift_vector, rb_RealVector, v);
	s->data = transform(s->data, shark::Shift((v->data)));
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

VALUE method_unlabeleddata_truncate_and_rescale (VALUE self, VALUE minX, VALUE minY, VALUE newMin, VALUE newMax) {
	Check_Type(minX, T_DATA);
	Check_Type(minY, T_DATA);
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

std::vector<shark::RealMatrix> rb_ary_to_realmatrix(VALUE ary) {

	std::vector<shark::RealMatrix> matrices;

	if (TYPE(rb_ary_entry(ary, 0)) == T_ARRAY) {
		// 2D array
		int width = RARRAY_LEN(ary);
		int height = RARRAY_LEN(rb_ary_entry(ary, 0));
		shark::RealMatrix matrix(height, width);
		for (int i=0;i<height;i++)
			for (int j=0;j<width;j++)
				matrix(j,i) = NUM2DBL(rb_ary_entry(rb_ary_entry(ary, j), i));
		matrices.push_back(matrix);
	} else {
		// 1D array
		int height = RARRAY_LEN(ary);
		shark::RealMatrix matrix(1, height);
		for (int i=0;i<height;i++)
			matrix(0,i) = NUM2DBL(rb_ary_entry(ary, i));
		matrices.push_back(matrix);
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
/*static VALUE method_create_realvector(VALUE self, VALUE data) {
	Check_Type(data, T_ARRAY);
	// shark::UnlabeledData<shark::RealVector> unlabeled_data = rb_ary_to_unlabeleddata(data);
	return wrap_pointer<rb_UnlabeledData>(
		rb_optimizer_unlabeleddata_klass,
		new rb_UnlabeledData(rb_ary_to_unlabeleddata(data))
		);
}*/

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

static VALUE method_regressionset_get_elements(VALUE self) {
	rb_RegressionDataset *s;
	Data_Get_Struct(self, rb_RegressionDataset, s);
	std::vector<shark::RealVector> inputData = s->input();
	return stdvector_realvector_to_rb_ary(inputData);
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
			Samples *s;
			Data_Get_Struct(rb_data, Samples, s);
			data = s->data;
			visibleSize = s-> visibleSize;
		}
	} else if (number_of_arguments > 1) {
		rb_raise(rb_eArgError, "Autoencoder takes 1 argument: Optimizer::Samples, or a Hash or parameters:\n  {\n    :data => Optimizer::Samples,\n    :beta => 3.0,\n    :rho => 0.01,\n    :lambda => 0.0002,\n    :hidden_neurons => 25\n  }");
	}
	
	Optimizer *my_optimizer = new Optimizer(visibleSize, numhidden, rho, beta, lambda);
	my_optimizer->setData(data);

	return wrap_pointer<Optimizer>(self, my_optimizer);
}

static VALUE method_autoencode_layer_matrices(int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
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

	std::vector<shark::RealMatrix> samples = rb_ary_to_realmatrix(sample);

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

		// Ruby methods for samples
		// <deprecated>
		// rb_define_singleton_method(rb_optimizer_klass, "create_unlabeled_data", (rb_method)method_create_unlabeleddata, 1);
		// </deprecated>
		rb_define_singleton_method(rb_optimizer_klass, "getSamples", (rb_method)method_get_samples,-1);
		rb_define_singleton_method(rb_optimizer_klass, "samples", (rb_method)method_create_samples,1);
		rb_define_singleton_method(rb_optimizer_samples_klass, "getSamples", (rb_method)method_get_samples,-1);
		rb_define_singleton_method(rb_optimizer_samples_klass, "samples", (rb_method)method_create_samples,1);

		rb_define_method(rb_optimizer_samples_klass, "visible_size", (rb_method)method_get_visible_size, 0);
		rb_define_method(rb_optimizer_samples_klass, "length", (rb_method)method_samples_get_size,0);
		rb_define_method(rb_optimizer_samples_klass, "size", (rb_method)method_samples_get_size,0);
		rb_define_method(rb_optimizer_samples_klass, "elements", (rb_method)method_samples_get_elements,0);
		rb_define_method(rb_optimizer_samples_klass, "to_a", (rb_method)method_samples_get_elements,0);

		rb_define_method(rb_optimizer_realvector_klass, "sqrt", (rb_method)method_realvector_get_sqrt,0);
		rb_define_method(rb_optimizer_realvector_klass, "to_a", (rb_method)method_realvector_to_ary, 0);
		rb_define_method(rb_optimizer_realvector_klass, "*", (rb_method)method_realvector_multiply, 1);
		rb_define_method(rb_optimizer_realvector_klass, "/", (rb_method)method_realvector_divide, 1);
		rb_define_method(rb_optimizer_realvector_klass, "length", (rb_method)method_realvector_length, 0);
		rb_define_method(rb_optimizer_realvector_klass, "-@", (rb_method)method_realvector_negate,0);
		rb_define_method(rb_optimizer_realvector_klass, "initialize", (rb_method)method_realvector_initialize,0);

		rb_define_method(rb_optimizer_regressionset_klass, "visible_size", (rb_method)method_get_visible_size, 0);
		rb_define_method(rb_optimizer_regressionset_klass, "length", (rb_method)method_regressionset_get_size,0);
		rb_define_method(rb_optimizer_regressionset_klass, "size", (rb_method)method_regressionset_get_size,0);
		rb_define_method(rb_optimizer_regressionset_klass, "elements", (rb_method)method_regressionset_get_elements,0);
		rb_define_method(rb_optimizer_regressionset_klass, "to_a", (rb_method)method_regressionset_get_elements,0);

		rb_define_method(rb_optimizer_unlabeleddata_klass, "length", (rb_method)method_unlabeleddata_length, 0);
		rb_define_method(rb_optimizer_unlabeleddata_klass, "elements", (rb_method)method_unlabeleddata_to_ary,0);
		rb_define_method(rb_optimizer_unlabeleddata_klass, "to_a", (rb_method)method_unlabeleddata_to_ary,0);
		rb_define_method(rb_optimizer_unlabeleddata_klass, "mean", (rb_method)method_unlabeleddata_mean,0);
		rb_define_method(rb_optimizer_unlabeleddata_klass, "variance", (rb_method)method_unlabeleddata_variance,0);
		rb_define_method(rb_optimizer_unlabeleddata_klass, "shift", (rb_method)method_unlabeleddata_shift,1);
		rb_define_method(rb_optimizer_unlabeleddata_klass, "-", (rb_method)method_unlabeleddata_shift,1);
		rb_define_method(rb_optimizer_unlabeleddata_klass, "+", (rb_method)method_unlabeleddata_posshift,1);
		rb_define_method(rb_optimizer_unlabeleddata_klass, "truncate_and_rescale", (rb_method)method_unlabeleddata_truncate_and_rescale,4);
		rb_define_method(rb_optimizer_unlabeleddata_klass, "initialize", (rb_method)method_unlabeleddata_initialize, -1);

		/*rb_define_method(rb_optimizer_samples_klass, "truncate", (rb_method)method_samples_truncate,0);
		rb_define_method(rb_optimizer_samples_klass, "truncate_and_rescale", (rb_method)method_samples_truncate_and_rescale,0);
		
		*/
	}
}