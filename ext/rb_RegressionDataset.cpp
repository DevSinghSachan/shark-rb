#include "rb_RegressionDataset.h"
#include "rb_UnlabeledData.h"
using namespace std;
using namespace shark;

// Or create fake samples:
shark::UnlabeledData<shark::RealVector> test_regressionset_getSamples(int numSamples, int height, int width) {
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

rb_RegressionDataset::rb_RegressionDataset(int numSamples, int height, int width) {
	// Read the data
	shark::UnlabeledData<shark::RealVector> samples = test_regressionset_getSamples(numSamples, height, width);
	cout << "Getting regression dataset," << endl;
	cout << "Generated : " << samples.numberOfElements() << " patches." << endl;
	data = RegressionDataset(samples, samples);
	visibleSize = height * width;
}

// If labels and data are the same thing.
rb_RegressionDataset::rb_RegressionDataset(VALUE self, VALUE rb_unlabeled_data) {
	Check_Type(rb_unlabeled_data, T_DATA);

	rb_UnlabeledData *s;
	Data_Get_Struct(rb_unlabeled_data, rb_UnlabeledData, s);

	shark::UnlabeledData<shark::RealVector> samples = s->data;

	data = RegressionDataset(samples, samples);
	visibleSize = samples.numberOfElements() > 0 ? samples.element(0).size() : 0;
}

// Import labels and data separately
rb_RegressionDataset::rb_RegressionDataset(VALUE self, VALUE rb_unlabeled_data, VALUE rb_labels) {
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