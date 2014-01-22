require 'set'
require 'ostruct'
class Optimizer
	module Autoencoder
		class Text
			attr_reader :autoencoder
			attr_reader :standard_vector
			DefaultRho = 0.01 #Sparsity parameter
			DefaultBeta = 3.0 #Regularization parameter
			DefaultLambda = 0.0002  #Weight decay paramater
			DefaultHiddenNeurons = 25 #Number of hidden neurons

			def self.create_text_sample_from_sample sample, fv
				vector = Array.new(fv.keys.length, 0.0)
				case sample
				when Array
					# each element in Array exists only once.
					sample.each do |keyword|
						vector[fv[keyword]] = 1.0
					end
				when Hash
					# keys lead to special values
					sample.keys.each do |keyword|
						vector[fv[keyword]] = sample[keyword]
					end
				else
					# some special class perhaps?
					sample.to_a.each do |keyword|
						vector[fv[keyword]] = sample[keyword]
					end
				end
				vector
			end

			# creates a set of text to convert text features to vectors
			def self.create_text_sample_set_from_samples samples
				feature_vector = Set.new
				case samples.first
				when Array
					# each element in Array exists only once.
					samples.each do |sample|
						feature_vector.merge sample
					end
				when Hash
					# keys lead to special values
					samples.each do |sample|
						feature_vector.merge sample.keys
					end
				else
					# some special class perhaps?
					samples.each do |sample|
						feature_vector.merge sample.to_a
					end
				end
				fv = {}
				feature_vector.each_with_index {|i,k| fv[i] = k}
				OpenStruct.new :feature_vector => fv, :features => feature_vector.to_a
			end

			def self.create_autoencoder_samples opts={}
				vectors = Array.new(opts[:data].length).map {Array.new(opts[:vector].keys.length, 0.0)}
				opts[:data].each_with_index do |sample, k|
					vectors[k] = Optimizer::Autoencoder::Text.create_text_sample_from_sample sample, opts[:vector]
				end
				vectors
			end

			def self.create_text_samples_from_filters opts={}
				converted_filter = Hash.new(0.0)
				opts[:filter].each_with_index do |score, k |
					converted_filter[opts[:vector][k]] = score
				end
				converted_filter
			end

			def initialize(opts={})
				@standard_vector     = Optimizer::Autoencoder::Text.create_text_sample_set_from_samples opts[:samples]
				@unlabeledData       = Optimizer::UnlabeledData.new Optimizer::Autoencoder::Text.create_autoencoder_samples(:data => opts[:samples], :vector => @standard_vector.feature_vector)
				@regression_data_set = Optimizer.regression_data_set
				@autoencoder_vectors = Optimizer.samples 
				@autoencoder = Optimizer.autoencoder :data => @autoencoder_vectors,
													 :hidden_neurons => opts[:hidden_neurons] || DefaultHiddenNeurons,
													 :rho => opts[:rho]                       || DefaultRho,
													 :lambda => opts[:lambda]                 || DefaultLambda,
													 :beta => opts[:beta]                     || DefaultBeta
			end

			def present(cutoff=0)
				parameters.each do |filter|
					puts "\n{"
					filter.each_pair do |k,v|
						if v > cutoff
							puts "    #{k} => #{v}"
						end
					end
					puts "}\n"
				end
			end

			def train
				@autoencoder.train
			end

			def error
				@autoencoder.solution
			end

			def parameters index = 0, rendered = true
				if index == 0 and rendered
					filters = @autoencoder.parameters index
					filters.map do |filter|
						Optimizer::Autoencoder::Text.create_text_samples_from_filters :filter => filter, :vector => @standard_vector.features
					end
				else 
					@autoencoder.parameters index
				end
			end

			def eval sample, rendered = false
				if !rendered
					@autoencoder.eval(Optimizer::Autoencoder::Text.create_text_sample_from_sample sample, @standard_vector.feature_vector)
				else
					@autoencoder.eval sample
				end
			end

			alias :step :train
			alias :solution :error
			alias :layers :parameters
		end
	end
end

TextAutoencoder = Optimizer::Autoencoder::Text