class Optimizer
	module RBM
		class CDBN < Shark::RBM::DBN
			def create_rbms_from_layers hidden_layer_array
				# construct multi-layer
				@rbm_layers     = Array.new(hidden_layer_array.length)
				@sigmoid_layers = Array.new(hidden_layer_array.length)
				hidden_layer_array.each_with_index do |layer_size, k|
					input_size         = k == 0 ? @input_size : hidden_layer_array[k-1]
					# construct sigmoid_layer

					if k==0
						layer_input = @input
					else
						layer_input = @sigmoid_layers[k-1].sample_h_given_v()
					end

					@sigmoid_layers[k] = HiddenLayer.new input_size: input_size,
														 output_size: layer_size,
														 activation: :sigmoid,
														 input: layer_input,
														 tranpose: (k > 0 and @rbm_type == Shark::RBM::BinaryRBM)
					# construct rbm_layer
					@rbm_layers[k]       = (k == 0 ? Shark::RBM::CRBM : @rbm_type).new
					@rbm_layers[k].input = layer_input
					@rbm_layers[k].set_structure hidden: layer_size,
												 visible: input_size
					@rbm_layers[k].initialize_random_uniform 1.0 / input_size
					@sigmoid_layers[k].parameters = @rbm_layers[k].weight_matrix
					@sigmoid_layers[k].bias       = @rbm_layers[k].hbias
				end
			end
		end
	end
end