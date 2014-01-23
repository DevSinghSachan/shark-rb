#include "rb_UnlabeledData.h"
#include <limits>
using namespace shark;
using namespace std;
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