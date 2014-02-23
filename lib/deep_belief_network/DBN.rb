class Optimizer
	module RBM
		class DBN
			# Deep Belief Network.

			def create_rbms_from_layers hidden_layer_array

				# construct multi-layer
				@rbm_layers = Array.new(hidden_layer_array.length)
				@sigmoid_layers = Array.new(hidden_layer_array.length)
				hidden_layer_array.each_with_index do |layer_size, k|
					input_size         = k == 0 ? @input_size : hidden_layer_array[k-1]
					# construct sigmoid_layer
					@sigmoid_layers[k] = HiddenLayer.new :input_size => @input_size,
						:output_size => layer_size,
						:activation => 
						:input => k==0 ? @input : @sigmoid_layers[k-1].sample_h_given_v
					# construct rbm_layer
					@rbm_layers[k]     = Shark::RBM::BinaryRBM.new
					@rbm_layers[k].set_structure hidden: layer_size,
												 visible: input_size
					@sigmoid_layers[k].parameters = @rbm_layers[k].weight_matrix
					@sigmoid_layers[k].bias       = @rbm_layers[k].hidden_neurons.bias
				end

				# TODO: add log layer too for logistic regression (Feb 13th 2014).
			end

			def initialize(opts={})
				@input          = opts[:samples]
				@input_size     = opts[:input_size]
				@output_size    = opts[:output_size]
				create_rbms_from_layers opts[:hidden_layers]
			end

			def data_present!
				raise StandardError.new "Cannot pretrain without data." if @unlabeled_data.nil?
			end

			def pretrain opts={}
				data_present!
				layer_input = []
				@rbm_layers.length.times do |i| # layer-wise
					opts[:epochs].times do |epoch| # training epochs
						@rbm_layers[0..i].each_with_index do |layer, l| # layer input
							# initialize the layer
							cd = Optimizer::BinaryCD.new layer
							cd.k = opts[:k]

							# collect previous activations, or input data
							if l == 0
								layer_input = @unlabeled_data
							else
								layer_input = @sigmoid_layers[l-1].sample_h_given_v layer_input
							end
							# may need to be modified...
							cd.data = layer_input

							# optimize this layer:
							optimizer = Shark::Algorithms::SteepestDescent.new
							optimizer.momentum      = opts[:momentum]
							optimizer.learning_rate = opts[:learning_rate]
							optimizer.init cd
							optimizer.step cd
						end
					end
				end
			end

			def predict
				# TODO: write this (Feb 13th 2014)
			end

			def finetune
				# requires supervised labels and log layer for logistic regression (Feb 13th 2014)
			end
		end
	end
end