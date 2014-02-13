class Optimizer
	module RBM
		class HiddenLayer
			# TODO: implement this in C++ perhaps?
			attr_accessor :parameters
			attr_reader :number_of_inputs
			attr_reader :number_of_outputs

			def initialize n_inputs, n_outputs
				@number_of_inputs = inputs
				@number_of_outputs = outputs
				@parameters = Shark::RealVector.new(0)
			end

			def sigmoid x
				1.0 / (1.0 + Math.exp(x))
			end

			def output inputlayer, weights, bias
				sigmoid(inputlayer*weights + bias)
			end

			def biasvalue index
				## some special Shark operation
				0.0
			end

			def sample_h_given_v inputlayer
				binomial = Shark::Binomial.new
				solution = []
				@number_of_outputs.times do |i|
					solution = binomial.sample 1, output(input, @parameters[i], @parameters[biasvalue(i)])
				end

				# TODO: create this function according to https://github.com/yusugomori/DeepLearning/blob/master/cpp/DBN.cpp
				Shark::UnlabeledData.new([])
			end
		end

		class DBN
			# Deep Belief Network.

			def create_rbms_from_layers hidden_layer_array

				hidden_layer_array.each_with_index do |layer_size, k|
					input_size         = k == 0 ? @input_size : hidden_layer_array[k-1]
					@sigmoid_layers[k] = HiddenLayer.new input_size, layer_size
					@rbm_layers[k]     = Shark::RBM::BinaryRBM.new
					@rbm_layers[k].set_structure :hidden => layer_size,
												 :visible => input_size
					@rbm_layers[k].link_parameters_to @sigmoid_layers[k].parameters
					# ties both bias and non-bias parameters together.
				end

				# TODO: add log layer too for logistic regression (Feb 13th 2014).
			end

			def initialize(opts={})
				@input_size     = opts[:input_size]
				@output_size    = opts[:output_size]
				@hidden_layers  = []
				@sigmoid_layers = []
				create_rbms_from_layers opts[:hidden_layers]
			end

			def pretrain opts={}
				layer_input = []
				@rbm_layers.length.times do |i|
					opts[:epochs].times do |epoch|
						@rbm_layers[0..i].each_with_index do |layer, l|
							# initialize the layer
							cd = Optimizer::BinaryCD.new layer
							cd.k = opts[:k]

							# collect previous activations, or input data
							if l == 0
								layer_input = @unlabeled_data
							else
								layer_input = @hidden_layers[l-1].sample_h_given_v layer_input
							end
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