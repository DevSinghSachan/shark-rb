class Optimizer
	module Autoencoder
		class Text
			attr_reader :autoencoder
			DefaultRho = 0.01 #Sparsity parameter
			DefaultBeta = 3.0 #Regularization parameter
			DefaultLambda = 0.0002  #Weight decay paramater
			DefaultHiddenNeurons = 25 #Number of hidden neurons

			# creates a set of text to convert text features to vectors
			def self.create_text_sample_set_from_samples samples
				feature_vector = Set.new
				samples.each do |sample|
					feature_vector.merge sample
				end
				fv = {}
				feature_vector.each_with_index {|i,k| fv[i] = k}
				fv
			end

			def self.create_autoencoder_samples opts={}
				vectors = Array.new(opts[:data].length, Array.new(opts[:vector].keys.length, 0.0))
				opts[:data].each_with_index do |sample, k|
					sample.each do |keyword|
						vectors[k][opts[:vector][keyword]] = 1.0
					end
				end
				vectors
			end

			def initiliaze(opts={})
				@standard_vector     = self.create_text_sample_set_from_samples opts[:samples]
				@autoencoder_vectors = self.create_autoencoder_samples :data => opts[:samples], :vector => @standard_vector
				autoencoder_opts     = {
					:data => @autoencoder_vectors,
					:hidden_neurons => DefaultHiddenNeurons,
					:rho => DefaultRho,
					:lambda => DefaultLambda,
					:beta => DefaultBeta
				}
				autoencoder_opts = opts.merge autoencoder_opts
				@autoencoder = Optimizer.autoencoder autoencoder_opts
			end

			def train
				@autoencoder.train
			end

			def error
				@autoencoder.solution
			end

			# build eval, parameters, etc...

			alias :train :step
			alias :error :solution
		end
	end
end

TextAutoencoder = Optimizer::Autoencoder::Text