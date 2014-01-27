#include "rb_RealMatrix.h"
using namespace shark;
rb_RealMatrix::rb_RealMatrix(RealMatrix _data) {
	data = _data;
}
rb_RealMatrix::rb_RealMatrix() {
}
RealVector rb_RealMatrix::get_row(int i) {
	/*RealVector vec;
	row(data, i) = vec;*/
	return row(data, i);
}
RealVector rb_RealMatrix::get_column(int i) {
	/*RealVector vec;
	 = vec;*/
	return column(data, i);
}

void rb_RealMatrix::fill(double filling) {
	std::fill(data.storage(), data.storage()+(data.size1()*data.size2()), filling);
}