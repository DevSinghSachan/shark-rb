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

					@sigmoid_layers[k] = HiddenLayer.new input_size: input_size,
														 output_size: layer_size,
														 activation: :sigmoid,
														 input: (k==0 ? @input : @sigmoid_layers[k-1].sample_h_given_v)
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
				@labels         = opts[:labels]
				create_rbms_from_layers opts[:hidden_layers]
				create_logistic_regression_layer
			end

			def create_logistic_regression_layer
				@log_layer = Shark::LogisticRegression.new samples: @sigmoid_layers[-1].sample_h_given_v,
														   label: @labels,
														   number_of_inputs: @sigmoid_layers[-1].number_of_outputs,
														   number_of_outputs: @output_size
			end

			def pretrain opts={}
				data_present!
				layer_input = nil
				@rbm_layers.each_with_index do |layer, l| # layer-wise
					# initialize the layer
					cd = Optimizer::BinaryCD.new layer
					cd.k = opts[:k]
					# collect previous activations, or input data
					if l == 0
						layer_input = @input
					else
						layer_input = @sigmoid_layers[l-1].sample_h_given_v layer_input
					end

					cd.data = layer_input.to_unlabeled_data
					# optimize this layer:
					optimizer = Shark::Algorithms::SteepestDescent.new
					optimizer.momentum      = opts[:momentum] || 0.0
					optimizer.learning_rate = opts[:learning_rate]

					# print "l                        => ", l, "\n"
					# print "layer.weight_matrix.size => ", layer.weight_matrix.size, "\n"
					# print "layer_input.size         => ", layer_input.size, "\n"


					optimizer.init cd

					# may need to be modified...
					# costly methodology, should have unlabeleddata everywhere instead...
					# print "layer_input.size                => ", layer_input.size, "\n"
					# print "layer.number_of_visible_neurons => ", layer.number_of_visible_neurons, "\n"
					# print "layer.number_of_hidden_neurons  => ", layer.number_of_hidden_neurons, "\n"

					opts[:epochs].times do |epoch| # training epochs
						optimizer.step cd
					end
				end
			end

			def predict x
				layer_input = x
				@sigmoid_layers.each_with_index do |layer, l|
					layer_input = layer.output layer_input
				end

				@log_layer.predict(layer_input)
				# TODO: write this (Feb 13th 2014)
			end

			def finetune opts={}
				# requires supervised labels and log layer for logistic regression (Feb 13th 2014)
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

			alias :"input=" :"data="
			alias :input :data
			alias :"unlabeled_data=" :"data="
			alias :unlabeled_data :data
		end
	end
end

def test_dbn pretrain_lr=0.1, pretraining_epochs=1000, k=1, finetune_lr=0.1, finetune_epochs=200

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

    dbn = Shark::RBM::DBN.new samples: x, labels: y, input_size:  6, hidden_layers:  [3, 3], output_size: 2
    # so support for RNG inputs yet

    # pre-training (TrainUnsupervisedDBN)
    dbn.pretrain learning_rate: pretrain_lr, k: 1, epochs: pretraining_epochs
    
    # fine-tuning (DBNSupervisedFineTuning)
    dbn.finetune learning_rate: finetune_lr, epochs: finetune_epochs

    # test
    x = Shark::RealMatrix.new [
    	[1, 1, 0, 0, 0, 0],
        [0, 0, 0, 1, 1, 0],
        [1, 1, 1, 1, 1, 0]
    ]

    puts dbn.predict(x)
end