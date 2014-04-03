class Optimizer
	module RBM
		class DBN
			class TrainingError < ArgumentError
			end

			# Deep Belief Network.

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
														 input: layer_input
					# construct rbm_layer
					@rbm_layers[k]     = Shark::RBM::BinaryRBM.new
					@rbm_layers[k].input = layer_input
					@rbm_layers[k].set_structure hidden: layer_size,
												 visible: input_size
					@rbm_layers[k].initialize_random_uniform 1.0 / input_size
					@sigmoid_layers[k].parameters = @rbm_layers[k].weight_matrix
					@sigmoid_layers[k].bias       = @rbm_layers[k].hidden_neurons.bias
				end
			end

			def create_logistic_regression_layer
				@log_layer = LogisticRegression.new samples: @sigmoid_layers[-1].sample_h_given_v(),
                                            		labels: @labels,
                                            		number_of_inputs: @sigmoid_layers.last.output_size,
                                            		number_of_outputs: @output_size
			end

			def initialize(opts={})
				@input          = opts[:samples]
				@input_size     = opts[:input_size]
				@output_size    = opts[:output_size]
				@labels         = opts[:labels]
				create_rbms_from_layers opts[:hidden_layers]
				create_logistic_regression_layer
			end

			def pretrain opts={}
				data_present!
				layer_input = nil
				# prev_input = nil
				@rbm_layers.each_with_index do |layer, l| # layer-wise
					# initialize the layer
					# cd = Optimizer::BinaryCD.new layer
					# cd.k = opts[:k]
					# collect previous activations, or input data
					if l == 0
						layer_input = @input
					else
						layer_input = @sigmoid_layers[l-1].sample_h_given_v layer_input
					end

					# in the original this does not exist, yet the result is not inverted? why, who knows?
					@sigmoid_layers[l].input = layer_input

					# opts[:epochs].times do |epoch| # training epochs
					# 	layer.contrastive_divergence input: layer_input,
					# 								 k: opts[:k],
					# 								 learning_rate: opts[:learning_rate],
					# 								 verbose: opts[:verbose]
					# end

					cd = Optimizer::BinaryCD.new layer
					cd.k = opts[:k]
					cd.data = layer_input.to_unlabeled_data
					# # optimize this layer:
					optimizer = Shark::Algorithms::SteepestDescent.new
					optimizer.momentum      = opts[:momentum] || 0.0
					optimizer.learning_rate = opts[:learning_rate]
					optimizer.init cd
					opts[:epochs].times do |epoch| # training epochs
						# replace by contrastive divergence
						optimizer.step cd
						# puts "Pre-training layer #{l}, epoch #{epoch}, cost #{layer.get_reconstruction_cross_entropy layer_input}"
					end
				end
			end

			def predict x
				layer_input = x
				@sigmoid_layers.each do |layer|
					layer_input = layer.output layer_input
				end
				@log_layer.predict(layer_input)
			end

			def finetune opts={}
				layer_input = @sigmoid_layers.last.sample_h_given_v()
				# train prediction layer
				lr = opts[:learning_rate] || 0.01
				@log_layer.input = layer_input
				(opts[:epochs] || 100).times do |epoch|
					@log_layer.train learning_rate: lr, verbose: opts[:verbose]
					lr *= 0.95
				end
			end

			def data_present!
				raise TrainingError.new "Cannot pretrain without data. #data=(data)" if @input.nil?
			end

			def data=(new_data)
				@input = new_data
			end

			def data
				@input
			end

			attr_reader :sigmoid_layers
			attr_reader :rbm_layers
			
			attr_reader :log_layer
			alias :prediction_layer :log_layer

			alias :"input=" :"data="
			alias :input :data

			alias :"unlabeled_data=" :"data="
			alias :unlabeled_data :data

			attr_reader :output_size
			alias :number_of_outputs :output_size

			attr_reader :input_size
	        alias :number_of_inputs :input_size
		end
	end
end

def test_dbn pretrain_lr=0.1, pretraining_epochs=1000, k=1, finetune_lr=0.1, finetune_epochs=200
	# Shark::RNG.seed 120

	x = Shark::RealMatrix.new [
		[1,1,1,0,0,0],
		[1,1,1,0,0,0],
		[1,0,1,0,0,0],
		[1,1,1,0,0,0],
		[0,0,1,1,1,0],
		[0,0,1,1,0,0],
		[0,0,1,1,1,0]
	]
	
	y = Shark::RealMatrix.new [
		[1, 0],
		[1, 0],
		[1, 0],
		[1, 0],
		[0, 1],
		[0, 1],
		[0, 1]
	]

	dbn = Shark::RBM::DBN.new samples: x, labels: y, input_size:  x.size2, hidden_layers:  [3, 3], output_size: y.size2
	# so support for RNG inputs yet

	# pre-training (TrainUnsupervisedDBN)
	dbn.pretrain learning_rate: pretrain_lr, k: 1, epochs: pretraining_epochs, verbose: false
	
	# fine-tuning (DBNSupervisedFineTuning)
	dbn.finetune learning_rate: finetune_lr, epochs: finetune_epochs, verbose: false

	# test
	x = Shark::RealMatrix.new [
		[1, 1, 0, 0, 0, 0],
	    [0, 0, 0, 1, 1, 0],
	    [1, 1, 1, 1, 1, 0]
	]
	puts dbn.predict(x)
end