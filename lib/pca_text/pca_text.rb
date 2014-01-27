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
				@standard_vector     = Optimizer::Conversion::Text.create_text_sample_set_from_samples opts[:samples]
				@unlabeled_data      = Optimizer::UnlabeledData.new Optimizer::Conversion::Text.create_samples(:data => opts[:samples], :vector => @standard_vector.feature_vector)
				@dimensions          = [(opts[:dimensions] || DefaultDimensions) - 1, 0].max
				@pca = Optimizer::PCA.new @unlabeled_data
				@encoder = create_encoder @dimensions
				@decoder = create_decoder @dimensions
			end

			def present(cutoff=0, ordered=true, cutoff_number=false)
				Optimizer::Conversion::Text.present_filters eigenvectors, cutoff, ordered, cutoff_number
			end

			def eigenvectors index = 0, rendered = true
				if index == 0 and rendered
					filters = @pca.eigenvectors.to_a
					filters.map do |filter|
						Optimizer::Conversion::Text.create_text_samples_from_filters :filter => filter, :vector => @standard_vector.features
					end
				else 
					@pca.eigenvectors
				end
			end
		end
	end
end

TextPCA = Optimizer::PCA::Text