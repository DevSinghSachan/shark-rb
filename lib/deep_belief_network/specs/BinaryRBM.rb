require_relative '../deep_belief_network'

describe 'Binary RBM' do

	before(:all) do

		@data = Shark::RealMatrix.new [
					[1,1,1,0,0,0],
                    [1,0,1,0,0,0],
                    [1,1,1,0,0,0],
                    [0,0,1,1,1,0],
                    [0,0,1,1,0,0],
                    [0,0,1,1,1,0]
                ]

        Shark::RNG.seed 120

		@rbm             = Shark::RBM::BinaryRBM.new 
		@rbm.set_structure visible: 6, hidden: 2
		@training_epochs = 1000
		@learning_rate   = 0.1
		@k               = 1
		@rbm.input = @data

		@training_epochs.times do |epoch|
			@rbm.contrastive_divergence learning_rate: @learning_rate,
										k: @k
		end

		@v = Shark::RealMatrix.new [
				[1, 1, 0, 0, 0, 0],
                [0, 0, 0, 1, 1, 0]
            ]

        @hbias = [0.82574995, -0.47456143]
        @vbias = [0.43333333, -0.2, 1.26666667, -1.08333333, -1.03333333, -1.28333333]
        @weight_matrix = ~Shark::RealMatrix.new(
        	[
				[ 4.21848663, -9.58985094],
				[ 0.73588344, -6.41164898],
				[ 7.20381464,  2.38420557],
				[-3.11429122,  8.65548531],
				[-3.67251191,  5.00958115],
				[-4.69418359, -2.89383999]
			]
		)
		@original_parameter_vector = (@weight_matrix.to_a.flatten + @hbias.to_a + @vbias.to_a).to_realvector
	end

	it 'should be able to reconstruct using its hidden neurons' do
		reconstruction = @rbm.reconstruct @v
		(reconstruction - [@data[0].to_a, @data[-1].to_a]).mean.should be_within(0.3).of 0
	end

	it 'should save bias assignments' do
		@rbm.hidden_neurons.bias  = @hbias
		@rbm.hidden_neurons.bias.should == @hbias
		@rbm.visible_neurons.bias  = @vbias
		@rbm.visible_neurons.bias.should == @vbias
	end

	it 'should save weight matrix assignments' do
		@rbm.weight_matrix = @weight_matrix
		@rbm.weight_matrix.should == @weight_matrix
	end

	it 'should give the right propup' do
		@rbm.propup(@v[0]).should == @rbm.sigmoid((@v[0] * ~@rbm.weight_matrix) + @rbm.hidden_neurons.bias)
		(
			@rbm.propup(@v[0]) - Shark::RealMatrix.new(
				[
					9.96921164e-01,
					6.99097005e-08
				]
			)
		).sum.should be_within(1e-06).of(0)
	end

	it 'should give the right propdown' do
		@rbm.propdown([1,1]).should == @rbm.sigmoid(([1,1] * @rbm.weight_matrix) + @rbm.visible_neurons.bias)
		(
			@rbm.propdown([1,1]) - Shark::RealMatrix.new(
				[
					7.11767532e-03,
					2.79878956e-03,
					9.99980686e-01,
					9.88545599e-01,
					5.75355536e-01,
					1.40332317e-04
				]
			)
		).sum.should be_within(1e-06).of(0)
	end

	describe 'Sampling' do
		it 'should have the same sampling using hidden units on average' do
			@hsample = Shark::RealMatrix.new [1,1]
			@samples = Shark::RealMatrix.new 10000, 6
			10_000.times do |i|
				@samples[i] = @rbm.sample_v_given_h(@hsample).last[0]
			end
			(
				@samples.mean(axis:0) - [
					6.60000000e-03,
					2.40000000e-03,
					1.00000000e+00,
					9.87200000e-01,
					5.77500000e-01,
					2.00000000e-04
				]
			).sum.should be_within(1e-02).of(0)
		end

		it 'should have the same sampling using visible units on average' do
			@samples = Shark::RealMatrix.new 10000, 2
			10_000.times do |i|
				@samples[i] = @rbm.sample_h_given_v(@v[0]).last[0]
			end
			(
				@samples.mean(axis:0) - [
					0.9978,
					0.0
				]
			).sum.should be_within(1e-02).of(0)
		end

	end

	describe 'Training' do
		it 'should update the weights similarly' do
			@parameter_evolution = Shark::RealMatrix.new 10_000, @rbm.number_of_parameters

			@rbm.hidden_neurons.bias    = [0,0]
			@rbm.visible_neurons.bias   = [0,0,0,0,0,0]
			@rbm.weight_matrix.clear

			10_000.times do |epoch|
				@rbm.contrastive_divergence input: @data,
											k: 1,
											learning_rate: 0.1
				@rbm.contrastive_divergence input: @data,
											k: 1,
											learning_rate: 0.1
				@parameter_evolution[epoch] = @rbm.parameter_vector
				@rbm.hidden_neurons.bias    = [0,0]
				@rbm.visible_neurons.bias   = [0,0,0,0,0,0]
				@rbm.weight_matrix.clear
			end

			Math.sqrt(
				(
					(
						@parameter_evolution.mean(axis:0) - [
							  7.84383352e-03,
							 -8.68810901e-02,
							  2.94655309e-01,
							  9.61819945e-03,
							 -8.79572217e-02,
							 -2.79198554e-01,
							  8.74161857e-03,
							 -8.60187567e-02,
							  2.94927168e-01,
							  9.14761942e-03,
							 -8.91207717e-02,
							 -2.78855506e-01,
							  3.08959632e-03,
							  3.15556574e-03,
							 -2.70000000e-04,
							 -3.12566667e-02,
							  9.44983333e-02,
							  5.16666667e-05,
							 -3.19416667e-02,
							 -9.53033333e-02
						]
					)**2
				).sum
			).should be_within(2e-02).of(0)
		end

		it 'should be able to update the bias' do
			h_bias = @rbm.hidden_neurons.bias.deep_copy()
			@rbm.hidden_neurons.bias += 2
			@rbm.hidden_neurons.bias.should_not == h_bias
			@rbm.hidden_neurons.bias.should == (h_bias + 2)
			v_bias = @rbm.visible_neurons.bias.deep_copy()
			@rbm.visible_neurons.bias += 2
			@rbm.visible_neurons.bias.should_not == v_bias
			@rbm.visible_neurons.bias.should == (v_bias + 2)
		end

		it 'should be able to update the weights' do
			weight_matrix = @rbm.weight_matrix.deep_copy()
			@rbm.weight_matrix += 2
			@rbm.weight_matrix().should_not == weight_matrix
			@rbm.weight_matrix().should == (weight_matrix + 2)
		end

		it 'should estimate cross-entropy cost correctly' do
			rbm = Shark::RBM::BinaryRBM.new
			training_data = Shark::RealMatrix.new [
				[1,1,1,0,0,0],
				[1,1,1,0,0,0],
				[1,0,1,0,0,0],
				[1,1,1,0,0,0],
				[0,0,1,1,1,0],
				[0,0,1,1,0,0],
				[0,0,1,1,1,0]
			]
			rbm.set_structure visible: 6, hidden: 3
			rbm.weight_matrix = Shark::RealMatrix.new [
				[-7.04423364,-5.71574815, 3.85495703, 6.54276807, 1.24076955,-4.26321733],
				[ 3.05550762, 0.43632773, 2.51919426,-3.49456278,-4.12685438,-3.22315804],
				[ 4.76580181, 1.86983584, 1.39850034,-5.46176108,-3.95184239,-2.84229769]
			]
			rbm.visible_neurons.bias = [-0.05714286,-0.51428571, 1.37142857, 0.22857143,-0.41428571,-1.04285714]
			rbm.hidden_neurons.bias = [-0.28860541,-0.13251746,-0.27677419]

			sigmoid_activation_h = rbm.sigmoid_activation_hidden training_data
			Math.sqrt(
				(
					(
						sigmoid_activation_h -
						Shark::RealMatrix.new(
							[
								[ 1.01674721e-04,  9.97208866e-01,  9.99572600e-01],
								[ 1.01674721e-04,  9.97208866e-01,  9.99572600e-01],
								[ 2.99481485e-02,  9.95688654e-01,  9.97233848e-01],
								[ 1.01674721e-04,  9.97208866e-01,  9.99572600e-01],
								[ 9.99988229e-01,  5.29996835e-03,  2.50480915e-04],
								[ 9.99959295e-01,  2.48265216e-01,  1.28683205e-02],
								[ 9.99988229e-01,  5.29996835e-03,  2.50480915e-04]
							]
						)
					)**2
				).sum
			).should be_within(1e-06).of(0)
			sigmoid_activation_v = rbm.sigmoid_activation_visible sigmoid_activation_h
			Math.sqrt(
				(
					(
						sigmoid_activation_v -
						Shark::RealMatrix.new(
							[
								[9.99570683e-01, 8.56838942e-01, 9.94942869e-01, 1.64078901e-04, 2.07608525e-04, 8.25632134e-04],
							    [9.99570683e-01, 8.56838942e-01, 9.94942869e-01, 1.64078901e-04, 2.07608525e-04, 8.25632134e-04],
							    [9.99461857e-01, 8.33917298e-01, 9.95458031e-01, 2.03094855e-04, 2.18807355e-04, 7.35495803e-04],
							    [9.99570683e-01, 8.56838942e-01, 9.94942869e-01, 1.64078901e-04, 2.07608525e-04, 8.25632134e-04],
							    [8.37789327e-04, 1.97110939e-03, 9.94727909e-01, 9.98832093e-01, 6.90745560e-01, 4.85046293e-03],
							    [1.86768596e-03, 2.24362884e-03, 9.97184100e-01, 9.97079894e-01, 4.38075890e-01, 2.14455883e-03],
							    [8.37789327e-04, 1.97110939e-03, 9.94727909e-01, 9.98832093e-01, 6.90745560e-01, 4.85046293e-03]
							]
						)
					)**2
				).sum
			).should be_within(1e-06).of(0)
			cost = rbm.get_reconstruction_cross_entropy training_data
			cost.should be_within(1e-06).of(0.520249097998)
			# one step of contrastive divergence (probabilities ignored):
			ph_sample = Shark::RealMatrix.new [
				[0,1,1],
				[0,1,1],
				[0,1,1],
				[0,1,1],
				[1,0,0],
				[1,1,0],
				[1,0,0]
			]
 			nh_means = Shark::RealMatrix.new [
 				[1.01674721e-04, 9.97208866e-01, 9.99572600e-01],
				[1.01674721e-04, 9.97208866e-01, 9.99572600e-01],
				[1.01674721e-04, 9.97208866e-01, 9.99572600e-01],
				[1.01674721e-04, 9.97208866e-01, 9.99572600e-01],
				[9.99988229e-01, 5.29996835e-03, 2.50480915e-04],
				[9.99959295e-01, 2.48265216e-01, 1.28683205e-02],
				[9.99959295e-01, 2.48265216e-01, 1.28683205e-02]
			]
			nv_samples = Shark::RealMatrix.new [
				[1,1,1,0,0,0],
				[1,1,1,0,0,0],
				[1,1,1,0,0,0],
				[1,1,1,0,0,0],
				[0,0,1,1,1,0],
				[0,0,1,1,0,0],
				[0,0,1,1,0,0]
			]

			lr = 0.1

			rbm.weight_matrix   += lr * ~(
					((~training_data) * ph_sample) -
					((~nv_samples) * nh_means)
				)
			
			rbm.visible_neurons.bias += lr * (training_data - nv_samples).mean(axis:0)
			rbm.hidden_neurons.bias  += lr * (ph_sample - nh_means).mean(axis:0)
			cost = rbm.get_reconstruction_cross_entropy training_data
			cost.should be_within(1e-06).of(0.501913898573)

		end
	end

end