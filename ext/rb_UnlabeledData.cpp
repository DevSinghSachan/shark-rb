#include "rb_UnlabeledData.h"
rb_UnlabeledData::rb_UnlabeledData(UnlabeledData<RealVector> _data) {
	data = _data;
}
rb_UnlabeledData::rb_UnlabeledData() {
	//data = shark::UnlabeledData<shark::RealVector>();
}
std::vector<shark::RealVector> rb_UnlabeledData::input () {
	std::vector<shark::RealVector> my_input(data.numberOfElements());
	for (size_t i=0; i<data.numberOfElements(); i++)
	{
		RealVector output(data.element(i));
		my_input[i] = output;
	}
	return my_input;
}