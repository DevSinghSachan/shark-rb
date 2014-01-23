#include "rb_RealMatrix.h"
rb_RealMatrix::rb_RealMatrix(RealMatrix _data) {
	data = _data;
}
rb_RealMatrix::rb_RealMatrix() {
}
void rb_RealMatrix::fill(double filling) {
	std::fill(data.storage(), data.storage()+(data.size1()*data.size2()), filling);
}