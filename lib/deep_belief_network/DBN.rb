class Optimizer
	module RBM
		class BinaryRBM
			def sigmoid x
				((-x).exp + 1.0).inverse
			end

			def get_reconstruction_cross_entropy input
				pre_sigmoid_activation_h = input * ~weight_matrix + hidden_neurons.bias
				sigmoid_activation_h     = sigmoid(pre_sigmoid_activation_h)
				
				pre_sigmoid_activation_v = sigmoid_activation_h * weight_matrix + visible_neurons.bias
				sigmoid_activation_v     = sigmoid(pre_sigmoid_activation_v)

				- (
						(
							input * (~sigmoid_activation_v).log +
						    (-input + 1.0) * ~(-sigmoid_activation_v + 1.0).log
					    ).sum(axis:1)
				).mean
			end

			def propup v
				pre_sigmoid_activation = v * (~weight_matrix) + hidden_neurons.bias
				return sigmoid(pre_sigmoid_activation)
			end

			def sample_h_given_v v0_sample
				h1_mean   = propup v0_sample
				binomial = Shark::RNG::Binomial.new
				binomial.n = 1
				# TODO: simplify Binomials (15th March 2014)
				if h1_mean.is_a? Shark::RealMatrix
					h1_sample = Shark::RealMatrix.new(h1_mean.size1, h1_mean.size2) do |i, j|
						binomial.p = h1_mean[i,j]
						binomial.sample
					end
					[h1_mean, h1_sample]
				else
					h1_sample = Shark::RealVector.new(h1_mean.size) do |i|
						binomial.p = h1_mean[i]
						binomial.sample
					end
					[h1_mean, h1_sample]
				end
			end

			def sample_v_given_h h0_sample
				v1_mean = self.propdown h0_sample
				binomial = Shark::RNG::Binomial.new
				binomial.n = 1
				if v1_mean.is_a? Shark::RealMatrix
					v1_sample = Shark::RealMatrix.new(v1_mean.size1, v1_mean.size2)
					v1_mean.each_cell_with_index do |cell, i, j|
						binomial.p = cell
						v1_sample[i,j] = binomial.sample
					end
					[v1_mean, v1_sample]
				else
					v1_sample = Shark::RealVector.new(v1_mean.size)
					v1_mean.each_with_index do |cell, i|
						binomial.p = cell
						v1_sample[i] = binomial.sample
					end
					[v1_mean, v1_sample]
				end
			end

			def contrastive_divergence opts={}
				opts = {:k => 1, :learning_rate => 0.1}.merge opts
				k, lr, input = opts[:k], opts[:learning_rate], opts[:input]
				raise TrainingError.new "Cannot perform contrastive divergence without data. #contrastive_divergence {:input => <data>}" if input.nil?
				*, ph_sample = sample_h_given_v opts[:input]
				chain_start = ph_sample

				nv_means, nv_samples, nh_means, nh_samples = [nil, nil, nil, nil]

				k.times do |step|
					if step == 0
						nv_means, nv_samples, nh_means, nh_samples = gibbs_hvh chain_start
					else
						nv_means, nv_samples, nh_means, nh_samples = gibbs_hvh nh_samples
					end
				end
				# puts nv_samples, (input - nv_samples).mean(axis:0)
				# puts "***"
				# puts (((~input) * ph_sample) - ((~nv_samples) * nh_means))
				# puts "***"
				self.weight_matrix   += lr * (~input * ph_sample - ~nv_samples * nh_means)
				visible_neurons.bias += lr * (input - nv_samples).mean(axis:0)
				hidden_neurons.bias  += lr * (ph_sample - nh_means).mean(axis:0)
				# raise StandardError.new "hell"
			end

			def propdown h
				pre_sigmoid_activation = h * weight_matrix + visible_neurons.bias
				sigmoid(pre_sigmoid_activation)
			end

			def gibbs_hvh h0_sample
				v1_mean, v1_sample = sample_v_given_h h0_sample
				h1_mean, h1_sample = sample_h_given_v v1_sample

				[
					v1_mean,
					v1_sample,
					h1_mean,
					h1_sample
				]
			end

			def reconstruct v
				h = sigmoid( v * ~weight_matrix + hidden_neurons.bias)
				reconstructed_v = sigmoid(h * weight_matrix + visible_neurons.bias)
				return reconstructed_v
			end

			class TrainingError < ArgumentError
			end
		end

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

			def create_logistic_regression_layer
				@log_layer = LogisticRegression.new samples: @sigmoid_layers[-1].sample_h_given_v,
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

					opts[:epochs].times do |epoch| # training epochs
						layer.contrastive_divergence input: layer_input,
													 learning_rate: opts[:learning_rate]
					end

					# cd.data = layer_input.to_unlabeled_data
					# # optimize this layer:
					# optimizer = Shark::Algorithms::SteepestDescent.new
					# optimizer.momentum      = opts[:momentum] || 0.0
					# optimizer.learning_rate = opts[:learning_rate]

					# # print "l                        => ", l, "\n"
					# # print "layer.weight_matrix.size => ", layer.weight_matrix.size, "\n"
					# # print "layer_input.size         => ", layer_input.size, "\n"


					# optimizer.init cd

					# # may need to be modified...
					# # costly methodology, should have unlabeleddata everywhere instead...
					# # print "layer_input.size                => ", layer_input.size, "\n"
					# # print "layer.number_of_visible_neurons => ", layer.number_of_visible_neurons, "\n"
					# # print "layer.number_of_hidden_neurons  => ", layer.number_of_hidden_neurons, "\n"

					# opts[:epochs].times do |epoch| # training epochs
					# 	# replace by contrastive divergence
					# 	optimizer.step cd
					# 	# print "(#{epoch}, #{l}) layer.weight_matrix => \n", layer.weight_matrix.reduce([]) {|i,v| [v.to_a.map {|i| sprintf('%.2f', i).to_f}.inspect] + i}.join("\n"), "\n"
					# 	# cost = layer.get_reconstruction_cross_entropy(layer_input)
					# 	# print 'Pre-training layer %d, epoch %d, cost %f' % [l, epoch, cost], "\n"
					# end
				end
			end

			def predict x
				layer_input = x
				@sigmoid_layers.each_with_index do |layer, l|
					layer_input = layer.output layer_input
				end
				@log_layer.predict(layer_input)
			end

			def finetune opts={}
				layer_input = @sigmoid_layers.last.sample_h_given_v
				# train prediction layer
				lr = opts[:learning_rate] || 0.01
				(opts[:epochs] || 200).times do |epoch|
					@log_layer.train learning_rate: lr, input: layer_input
					lr *= 0.95
					# cost = @log_layer.negative_log_likelihood
			  #       print 'Training epoch %d, cost is %f' % [epoch, cost], "\n"
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