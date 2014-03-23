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

        Shark::RNG.seed 10

		@rbm             = Shark::RBM::BinaryRBM.new 
		@rbm.set_structure visible: 6, hidden: 2
		@training_epochs = 1000
		@learning_rate   = 0.1
		@k               = 1

		@training_epochs.times do |epoch|
			@rbm.contrastive_divergence input: @data,
										learning_rate: @learning_rate,
										k: @k
		end

		@v = Shark::RealMatrix.new [
				[1, 1, 0, 0, 0, 0],
                [0, 0, 0, 1, 1, 0]
            ]

        @hbias = [0.82574995, -0.47456143]
        @vbias = [0.43333333, -0.2, 1.26666667, -1.08333333, -1.03333333, -1.28333333]
        @weight_matrix = ~Shark::RealMatrix.new([
									[ 4.21848663, -9.58985094],
									[ 0.73588344, -6.41164898],
									[ 7.20381464,  2.38420557],
									[-3.11429122,  8.65548531],
									[-3.67251191,  5.00958115],
									[-4.69418359, -2.89383999]
								   ])
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
			@samples = Shark::RealMatrix.new 1000, 6
			1000.times do |i|
				puts @samples[i].size
				@samples[i] << @rbm.sample_v_given_h(@hsample)
			end
			#puts 
			puts @samples.mean(axis:0)
		end
	end

	describe 'when doing Gibbs Sampling' do

		# it 'should provide consistent samples' do
		# 	samples = nil
		# 	trials = 100000

		# 	ph_sample = Shark::RealMatrix.new [
		# 										[1, 0],
		# 										[1, 0],
		# 										[1, 0],
		# 										[1, 1],
		# 										[1, 1],
		# 										[0, 1]
		# 									  ]
		# 	@rbm.weight_matrix = ~Shark::RealMatrix.new([
		# 							[ 4.21848663, -9.58985094],
		# 							[ 0.73588344, -6.41164898],
		# 							[ 7.20381464,  2.38420557],
		# 							[-3.11429122,  8.65548531],
		# 							[-3.67251191,  5.00958115],
		# 							[-4.69418359, -2.89383999]
		# 						   ])
		# 	@rbm.hidden_neurons.bias  = [0.82574995, -0.47456143]
		# 	@rbm.visible_neurons.bias = [0.43333333, -0.2, 1.26666667, -1.08333333, -1.03333333, -1.28333333]
			
		# 	trials.times do |epoch|
		# 		if epoch == 0
		# 			samples = (@rbm.gibbs_hvh ph_sample).last
		# 		else
		# 			samples += (@rbm.gibbs_hvh ph_sample).last
		# 		end
		# 	end

		# 	samples /= trials

		# 	puts samples

		# end

	end

end