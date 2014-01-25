#include "rb_PCA.h";
using namespace shark;
using namespace std;
void rb_PCA::train(LinearModel<RealVector> model, UnlabeledData<RealVector> const& inputs) {
	pca->train(model, inputs);
}
void rb_PCA::setWhitening(bool whitening) {
	pca->setWhitening(whitening);
};
bool rb_PCA::whitening(bool whitening) {
	return pca->m_whitening;
};

void rb_PCA::setData(UnlabeledData<RealVector> const & inputs) {
	pca->setData(inputs);
}
rb_PCA::rb_PCA() {
}
rb_PCA::rb_PCA(bool whitening) {
	pca = new PCA(whitening);
}
rb_PCA::rb_PCA(UnlabeledData<RealVector> const &inputs, bool whitening) {
	pca = new PCA(inputs, whitening);
}
void rb_PCA::encoder(LinearModel<RealVector> model, size_t m) {
	pca->encoder(model, m);
}
void rb_PCA::decoder(LinearModel<RealVector> model, size_t m) {
	pca->decoder(model, m);
}
RealVector const& rb_PCA::eigenvalues() {
	return pca->eigenvalues();
};
double rb_PCA::eigenvalue(size_t i) {
	return pca->eigenvalue(i);
};
RealMatrix const& rb_PCA::eigenvectors() {
	return pca->eigenvectors();
};
RealVector const& rb_PCA::mean() {
	return pca->mean();
};