require 'rb_shark'
require_relative './conversion_text/conversion_text.rb'
require_relative './autoencoder_text/autoencoder_text.rb'
require_relative './pca_text/pca_text.rb'
require_relative './binaryrbm_text/binaryrbm_text.rb'
require_relative './deep_belief_network/deep_belief_network.rb'
require_relative './datatypes/datatypes.rb'

# Notes:
# Following was modified in Gamma.h "Gamma_distribution<>(k())" to "dist()"
# double p(double x)const
# {
# 	// return std::pow(x, k()-1) * std::exp(-x / theta()) / (shark::gamma(k()) * std::pow(theta(), k())); //  CI
# 	return std::pow(x, k()-1) * std::exp(-x / theta()) / (Gamma_distribution<>(k(), theta()) * std::pow(theta(), k()));
# }

# In GlobalRNG
# Gamma distribution calls cauchy instead of gamma... should be changed.
#