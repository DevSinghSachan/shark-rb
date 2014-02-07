class Optimizer
	class PCA
		class Text
			attr_reader :pca
			attr_reader :unlabeled_data
			attr_reader :encoder
			attr_reader :decoder
			attr_reader :dimensions
			DefaultDimensions = 5 #Number of dimensions

			def create_encoder dimensions
				encoder = Optimizer::LinearModel.new
				@pca.encoder encoder, dimensions
				encoder
			end

			def encode sample, rendered = false
				if !rendered
					@autoencoder.encoder.eval(Optimizer::Conversion::Text.text_sample_from_sample sample, @standard_vector.feature_vector)
				else
					@autoencoder.encoder.eval sample
				end
			end

			def decode sample, render=true
				decoded = @autoencoder.decoder.eval sample
				if render
					Optimizer::Conversion::Text.text_samples_from_filters :filter => decoded.to_a, :vector => @standard_vector.features
				else
					decoded
				end
			end

			def create_decoder dimensions
				decoder = Optimizer::LinearModel.new
				@pca.decoder decoder, dimensions
				decoder
			end

			def dimensions=(val)
				if val != @dimensions and (val.instance_of? Fixnum or val.instance_of? Float)
					@dimensions = val
					@encoder = create_encoder @dimensions
					@decoder = create_decoder @dimensions
				end
				self
			end

			def initialize(opts={})
				@standard_vector     = Optimizer::Conversion::Text.text_sample_set_from_samples opts[:samples]
				@unlabeled_data      = Optimizer::UnlabeledData.new Optimizer::Conversion::Text.samples(:data => opts[:samples], :vector => @standard_vector.feature_vector)
				@dimensions          = [(opts[:dimensions] || DefaultDimensions), 1].max
				@pca = Optimizer::PCA.new @unlabeled_data
				@encoder = create_encoder @dimensions
				@decoder = create_decoder @dimensions
			end

			def present(cutoff=0, show_offset=false, ordered=true, cutoff_number=false)
				Optimizer::Conversion::Text.present parameters(0, show_offset, true), cutoff, ordered, cutoff_number
			end

			def parameters index = 0, show_offset=false, rendered = true
				if index == 0 and rendered
					if show_offset
						offset = decoder.offset
						filters = (~decoder.matrix).to_realvectors
						filters.map do |filter|
							Optimizer::Conversion::Text.text_samples_from_filters :filter => (filter + offset).to_a, :vector => @standard_vector.features
						end
					else
						filters = (~decoder.matrix).to_realvectors
						filters.map do |filter|
							Optimizer::Conversion::Text.text_samples_from_filters :filter => filter.to_a, :vector => @standard_vector.features
						end
					end
				else
					(~decoder.matrix).to_realvectors
				end
			end

			def eigenvectors
				@pca.eigenvectors
			end
		end
	end
end

TextPCA = Optimizer::PCA::Text