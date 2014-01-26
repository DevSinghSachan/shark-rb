#include "rb_PCA.h"

using namespace shark;
using namespace std;
void rb_PCA::train(LinearModel<RealVector> &model, UnlabeledData<RealVector> const& inputs) {
	trainer.train(model, inputs);
}
void rb_PCA::setWhitening(bool whitening) {
	m_whitening = whitening;
	trainer.setWhitening(whitening);
};
bool rb_PCA::whitening() {
	return m_whitening;
};

void rb_PCA::setData(UnlabeledData<RealVector> const & inputs) {
	trainer.setData(inputs);
}
rb_PCA::rb_PCA(): m_whitening(false) {
}
rb_PCA::rb_PCA(bool whitening): m_whitening(whitening) {
	trainer = new PCA(whitening);
}
rb_PCA::rb_PCA(UnlabeledData<RealVector> const &inputs, bool whitening): m_whitening(whitening) {
	trainer = new PCA(inputs, whitening);
}
void rb_PCA::encoder(LinearModel<> & model, size_t m) {
	trainer.encoder(model, m);
}
void rb_PCA::decoder(LinearModel<> & model, size_t m) {
	trainer.decoder(model, m);
}
RealVector const& rb_PCA::eigenvalues() {
	return trainer.eigenvalues();
};
double rb_PCA::eigenvalue(size_t i) {
	return trainer.eigenvalue(i);
};
RealMatrix const& rb_PCA::eigenvectors() {
	return trainer.eigenvectors();
};
RealVector const& rb_PCA::mean() {
	return trainer.mean();
};