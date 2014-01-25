require 'set'
require 'ostruct'
class Optimizer
	module Autoencoder
		class Text
			attr_reader :autoencoder
			attr_reader :standard_vector
			attr_reader :regression_dataset
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

			def normalize_data
				meanvec = @unlabeled_data.mean
				@unlabeled_data -= meanvec # would be better with -= operator
				# Remove outliers outside of +/- 3 standard deviations
				# and normalize to [0.1, 0.9]
				pstd = @unlabeled_data.variance.sqrt * @num_std_deviations
				@unlabeled_data.truncate_and_rescale(-pstd, pstd, @min_value, @max_value)
				@unlabeled_data.remove_NaN
			end

			def initialize(opts={})
				@num_std_deviations = opts[:std_deviations].nil? ? 3 : opts[:std_deviations]
				@min_value = opts[:min_value].nil? ? 0.1 : opts[:min_value]
				@max_value = opts[:max_value].nil? ? 0.9 : opts[:max_value]
				@standard_vector     = Optimizer::Autoencoder::Text.create_text_sample_set_from_samples opts[:samples]
				@unlabeled_data       = Optimizer::UnlabeledData.new Optimizer::Autoencoder::Text.create_autoencoder_samples(:data => opts[:samples], :vector => @standard_vector.feature_vector)
				# normalize the data
				normalize_data
				@regression_dataset = Optimizer.regression_dataset @unlabeled_data, @unlabeled_data
				@unlabeled_data = nil
				@autoencoder = Optimizer.autoencoder :data => @regression_dataset,
													 :hidden_neurons => opts[:hidden_neurons] || DefaultHiddenNeurons,
													 :rho => opts[:rho]                       || DefaultRho,
													 :lambda => opts[:lambda]                 || DefaultLambda,
													 :beta => opts[:beta]                     || DefaultBeta
			end

			def present(cutoff=0, ordered=true, cutoff_number=false)
				parameters.each do |filter|
					puts "\n{"
					if ordered
						filter.to_a.sort_by {|i| -i[1]}.each_with_index do |pair, k|
							if cutoff_number and k > cutoff_number then break end
							if pair[1] > cutoff
								puts "    #{pair[0]} => #{pair[1]}"
							end
						end
					else
						filter.to_a.each_with_index do |pair, k|
							if cutoff_number and k > cutoff_number then break end
							if pair[1] > cutoff
								puts "    #{pair[0]} => #{pair[1]}"
							end
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
					filters = @autoencoder.parameters_to_a index
					filters.map do |filter|
						Optimizer::Autoencoder::Text.create_text_samples_from_filters :filter => filter, :vector => @standard_vector.features
					end
				else 
					@autoencoder.parameters_to_a index
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