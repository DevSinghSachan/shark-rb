#include "rb_RegressionDataset.h"

using namespace std;
using namespace shark;

#include "rb_pointer_wrapping.extras"

// Or create fake samples:
shark::UnlabeledData<shark::RealVector> test_regressionset_getSamples(int numSamples, int height, int width) {
	// Read images
	shark::UnlabeledData<shark::RealVector> images;
	import_csv(images, "../data/images.csv");

	const unsigned int w = 512;//width of loaded image
	const unsigned int h = 512;//height of loaded image

	unsigned int n = images.numberOfElements(); // number of images
	std::cout << "Found " << n << " images of size " << w << "x" << h << std::endl;

	// Create the samples at random
	// Important notes: Since the images are in csv format, the width and
	// height is hardcoded. Because width = height we only have one integer
	// distribution below.
	
	// Sample equal amount of patches per image
	int patchesPerImg = numSamples / n;
	typedef shark::UnlabeledData<shark::RealVector>::element_range::iterator ElRef;
	
	// Create patches
	std::vector<shark::RealVector> patches;
	for (ElRef it = images.elements().begin(); it != images.elements().end(); ++it) {
		for (int i = 0; i < patchesPerImg; ++i) {
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

rb_RegressionDataset::rb_RegressionDataset(int numSamples, int height, int width) {
	// Read the data
	shark::UnlabeledData<shark::RealVector> samples = test_regressionset_getSamples(numSamples, height, width);
	cout << "Getting regression dataset," << endl;
	cout << "Generated : " << samples.numberOfElements() << " patches." << endl;
	data = RegressionDataset(samples, samples);
	visibleSize = height * width;
}

// If labels and data are the same thing.
rb_RegressionDataset::rb_RegressionDataset(VALUE rb_unlabeled_data) {
	Check_Type(rb_unlabeled_data, T_DATA);

	rb_UnlabeledData *s;
	Data_Get_Struct(rb_unlabeled_data, rb_UnlabeledData, s);

	shark::UnlabeledData<shark::RealVector> samples = s->data;

	data = RegressionDataset(samples, samples);
	visibleSize = samples.numberOfElements() > 0 ? samples.element(0).size() : 0;
}

// Import labels and data separately
rb_RegressionDataset::rb_RegressionDataset(VALUE rb_unlabeled_data, VALUE rb_labels) {
	Check_Type(rb_unlabeled_data, T_DATA);
	Check_Type(rb_labels, T_DATA);

	rb_UnlabeledData *s, *l;

	Data_Get_Struct(rb_unlabeled_data, rb_UnlabeledData, s);
	Data_Get_Struct(rb_labels,         rb_UnlabeledData, l);

	shark::UnlabeledData<shark::RealVector> samples = s->data,
											labels  = l->data;

	data = RegressionDataset(samples, labels);
	visibleSize = samples.numberOfElements() > 0 ? samples.element(0).size() : 0;
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

VALUE method_regressionset_get_visible_size(VALUE self) {
	rb_RegressionDataset *s;
	Data_Get_Struct(self, rb_RegressionDataset, s);
	return INT2FIX(s->visibleSize);
}

VALUE method_regressionset_get_labels(VALUE self) {
	rb_RegressionDataset *s;
	Data_Get_Struct(self, rb_RegressionDataset, s);
	return wrap_pointer<rb_UnlabeledData>(
		rb_optimizer_unlabeleddata_klass,
		new rb_UnlabeledData((s->data).labels())
		);
}
VALUE method_regressionset_get_inputs(VALUE self) {
	rb_RegressionDataset *s;
	Data_Get_Struct(self, rb_RegressionDataset, s);
	return wrap_pointer<rb_UnlabeledData>(
		rb_optimizer_unlabeleddata_klass,
		new rb_UnlabeledData((s->data).inputs())
		);
}

VALUE method_regressionset_get_size(VALUE self) {
	rb_RegressionDataset *s;
	Data_Get_Struct(self, rb_RegressionDataset, s);
	return INT2FIX((s->data).numberOfElements());
}



void Init_RegressionDataset () {

	// Shark Regression sets for supervisied / labeled learning:
	rb_define_method(rb_optimizer_regressionset_klass, "visible_size",   (rb_method)method_regressionset_get_visible_size, 0);
	rb_define_method(rb_optimizer_regressionset_klass, "labels",         (rb_method)method_regressionset_get_labels, 0);
	rb_define_method(rb_optimizer_regressionset_klass, "inputs",         (rb_method)method_regressionset_get_inputs, 0);
	rb_define_method(rb_optimizer_regressionset_klass, "length",         (rb_method)method_regressionset_get_size,0);
	rb_define_method(rb_optimizer_regressionset_klass, "size",           (rb_method)method_regressionset_get_size,0);
	rb_define_method(rb_optimizer_regressionset_klass, "elements",       (rb_method)method_regressionset_get_inputs,0);
	rb_define_method(rb_optimizer_regressionset_klass, "to_a",           (rb_method)method_regressionset_get_inputs,0);
	rb_define_singleton_method(rb_optimizer_klass, "regression_dataset", (rb_method)method_regressionset_create,-1);

}

