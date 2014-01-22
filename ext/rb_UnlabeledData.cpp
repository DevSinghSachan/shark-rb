#include "rb_UnlabeledData.h"
rb_UnlabeledData::rb_UnlabeledData(UnlabeledData<RealVector> _data) {
	data = _data;
}
rb_UnlabeledData::rb_UnlabeledData() {
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

void rb_UnlabeledData::remove_NaN() {
	for (UnlabeledData::element_iterator pos=data.elemBegin();pos!= data.elemEnd();++pos) {
		std::cout<<*pos<<" ";
		Set::element_reference ref=*pos;
		ref*=2;
		std::cout<<*pos<<std::endl;
}