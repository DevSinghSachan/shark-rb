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
				@standard_vector     = Optimizer::Conversion::Text.create_text_sample_set_from_samples opts[:samples]
				@unlabeled_data      = Optimizer::UnlabeledData.new Optimizer::Conversion::Text.create_samples(:data => opts[:samples], :vector => @standard_vector.feature_vector)
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
				Optimizer::Conversion::Text.present_filters parameters, cutoff, ordered, cutoff_number
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
						Optimizer::Conversion::Text.create_text_samples_from_filters :filter => filter, :vector => @standard_vector.features
					end
				else 
					@autoencoder.parameters_to_a index
				end
			end

			def eval sample, rendered = false
				if !rendered
					@autoencoder.eval(Optimizer::Conversion::Text.create_text_sample_from_sample sample, @standard_vector.feature_vector)
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