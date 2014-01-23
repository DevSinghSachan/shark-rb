#include "rb_RealVector.h"
rb_RealVector::rb_RealVector(RealVector _data) {
	data = _data;
}
rb_RealVector::rb_RealVector() {
}
void rb_RealVector::fill(double filling) {
	std::fill(data.begin(), data.end(), filling);
}
void rb_RealVector::remove_NaN(double replacement = 0.0) {
	for (size_t i = 0;i< data.size();i++)
		if (data(i) != data(i))
			data(i) = replacement;
}