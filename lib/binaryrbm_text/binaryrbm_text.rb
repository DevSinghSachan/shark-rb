class Optimizer
	module RBM
		class Text
			attr_reader :rbm
			attr_reader :standard_vector
			attr_reader :unlabeled_data
			DefaultK = 1 #Contrastive divergence number of steps: CD-k
			DefaultMomentum = 0.01 #SteepestDescent momentum
			DefaultLearningRate = 0.1 #SteepestDescent learning rate
			DefaultHiddenStates = 25 #RBM number of hidden states

			def initialize(opts={})
				@standard_vector     = Optimizer::Conversion::Text.text_sample_set_from_samples opts[:samples]
				@unlabeled_data      = Optimizer::UnlabeledData.new Optimizer::Conversion::Text.samples(:data => opts[:samples], :vector => @standard_vector.feature_vector)
				@rbm = Optimizer::RBM::BinaryRBM.new
				@rbm.set_structure :hidden => [(opts[:hidden_states] || DefaultHiddenStates), 1].max,
								   :visible => @standard_vector.features.length
				@cd                      = Optimizer::BinaryCD.new  @rbm
				@cd.k                    = opts[:k] || DefaultK
				@cd.data                 = @unlabeled_data
				@optimizer               = Shark::Algorithms::SteepestDescent.new
				@optimizer.momentum      = opts[:momentum]      || DefaultMomentum
				@optimizer.learning_rate = opts[:learning_rate] || DefaultLearningRate
				initialize_rbm
			end

			def step
				@optimizer.step @cd
			end

			def initialize_rbm
				weights = Shark::RealVector.new @rbm.number_of_parameters
			    weights.size.times do |i|
			        weights[i] = Random.rand(0.2) - 0.1 # uniform sampling between -0.1 and +0.1
			    end
			    @rbm.parameter_vector = weights
			end

			def present(cutoff=0, ordered=true, cutoff_number=false)
				Optimizer::Conversion::Text.present parameters(true), cutoff, ordered, cutoff_number
			end

			def parameters rendered = true
				basis_vectors = Array.new(@rbm.number_of_hidden_neurons)
				basis_vectors.each_with_index do |vector,k|
					vector[k] = Array.new(@rbm.number_of_hidden_neurons, 0.0)
					vector[k][k] = 1.0
				end
				filters = eval basis_vectors
				if rendered
					filters.map do |filter|
						Optimizer::Conversion::Text.text_samples_from_filters :filter => filter,
						                                                             :vector => @standard_vector.features
					end
				else
					filters
				end
			end
		end
		alias :step :train
	end
end

TextBinaryRBM = Optimizer::RBM::Text