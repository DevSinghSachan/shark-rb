class Optimizer
	class PCA
		class Text
			attr_reader :pca
			attr_reader :unlabeled_data
			DefaultDimensions = 5 #Number of dimensions

			def initialize(opts={})
				@standard_vector     = Optimizer::Conversion::Text.create_text_sample_set_from_samples opts[:samples]
				@unlabeled_data      = Optimizer::UnlabeledData.new Optimizer::Conversion::Text.create_Conversion_samples(:data => opts[:samples], :vector => @standard_vector.feature_vector)
				@dimensions          = [(opts[:dimensions] || DefaultDimensions) - 1, 0].max
				# normalize the data
				@pca = Optimizer::PCA.new @unlabeled_data
				@pca_encoder = Optimizer::LinearModel.new
				@pca.encoder @pca_encoder, @dimensions
				@pca_decoder = Optimizer::LinearModel.new
				@pca.decoder @pca_decoder, @dimensions
			end

			def present(cutoff=0, ordered=true, cutoff_number=false)
				eigenvectors.each do |eigenvector|
					puts "\n{"
					if ordered
						eigenvector.to_a.sort_by {|i| -i[1]}.each_with_index do |pair, k|
							if cutoff_number and k > cutoff_number then break end
							if pair[1] > cutoff
								puts "    #{pair[0]} => #{pair[1]}"
							end
						end
					else
						eigenvector.to_a.each_with_index do |pair, k|
							if cutoff_number and k > cutoff_number then break end
							if pair[1] > cutoff
								puts "    #{pair[0]} => #{pair[1]}"
							end
						end
					end
					puts "}\n"
				end
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