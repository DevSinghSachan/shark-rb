require_relative '../deep_belief_network'

describe 'Continuous Restricted Boltzmann Machines' do

	before(:all) do
		Shark::RNG.seed 120
		# CRBM does not use Shark::RNG internally, this could change however.

		@training_epochs = 1000
		@learning_rate = 0.1
		@k = 1
		@training_data = Shark::RealMatrix.new [
			[0.4, 0.5, 0.5, 0.0, 0.0, 0.0],
            [0.5, 0.3, 0.5, 0.0, 0.0, 0.0],
            [0.4, 0.5, 0.5, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.5, 0.3, 0.5, 0.0],
            [0.0, 0.0, 0.5, 0.4, 0.5, 0.0],
            [0.0, 0.0, 0.5, 0.5, 0.5, 0.0]
        ]

        @rbm = Shark::RBM::CRBM.new visible: @training_data.size2, hidden: 5, input: @training_data

        @training_epochs.times do |epoch|
        	@rbm.contrastive_divergence k: @k,
        								learning_rate: @learning_rate
        end

        @test_data = Shark::RealMatrix.new([
        	[0.5, 0.5, 0.0, 0.0, 0.0, 0.0],
        	[0.0, 0.0, 0.0, 0.5, 0.5, 0.0]
        ])
	end

	it 'should reconstruct an input' do
		(
			(
				@rbm.reconstruct(@test_data) -
				Shark::RealMatrix.new([
					[3.65761559e-01, 4.10708295e-01, 5.80115576e-01, 5.12511004e-13, 1.44232348e-12, 2.26866890e-22],
					[1.39610833e-16, 3.13601718e-16, 5.11645646e-01, 2.17370849e-01, 6.55021414e-01, 1.31867033e-26]
				])
			)**2
		).sum.should be_within(1e06).of(0)
	end
end