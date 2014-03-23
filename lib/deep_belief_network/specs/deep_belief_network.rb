require_relative '../deep_belief_network'

describe 'Deep Belief Network' do
	before(:all) do
		Shark::RNG.seed 42

		@training_data = Shark::RealMatrix.new [
			[1,1,1,0,0,0],
			[1,1,1,0,0,0],
	        [1,0,1,0,0,0],
	        [1,1,1,0,0,0],
	        [0,0,1,1,1,0],
	        [0,0,1,1,0,0],
	        [0,0,1,1,1,0]
	    ]
	    @training_labels = Shark::RealMatrix.new [
	    	[1, 0],
	    	[1, 0],
	        [1, 0],
	        [1, 0],
	        [0, 1],
	        [0, 1],
	        [0, 1]
	    ]

		@pretrain_lr        = 0.1
		@finetune_lr        = 0.1
		@k                  = 1
		@pretraining_epochs = 1000
		@finetune_epochs    = 200

	    @dbn = Shark::RBM::DBN.new samples: @training_data,
	                               labels: @training_labels,
	                               input_size:  6,
	                               hidden_layers: [3, 3],
	                               output_size: 2

	    # no support for RNG inputs yet

	    # pre-training (TrainUnsupervisedDBN)
	    @dbn.pretrain learning_rate: @pretrain_lr, k: @k, epochs: @pretraining_epochs

	    # fine-tuning (DBNSupervisedFineTuning)
	    @dbn.finetune learning_rate: @finetune_lr, epochs: @finetune_epochs
 
 		puts "at the end =>"
	    puts @dbn.rbm_layers[1].weight_matrix
	    puts @dbn.rbm_layers[1].visible_neurons.bias
	    puts @dbn.rbm_layers[1].hidden_neurons.bias

	    # test
	    @test_data = Shark::RealMatrix.new [
	    	[1, 1, 0, 0, 0, 0],
	        [0, 0, 0, 1, 1, 0],
	        [1, 1, 1, 1, 1, 0]
	    ]
	end

	describe 'RBM layers' do

	end

	describe 'Prediction layer' do

		it 'should have the same input size as the last RBM output size' do
			@dbn.prediction_layer.input_size.should == @dbn.sigmoid_layers.last.output_size
		end

		it 'should have the same output size as the DBN' do
			@dbn.output_size.should == @dbn.prediction_layer.output_size
		end

		it 'should label accordingly' do
			puts @dbn.predict(@test_data[0])[0].to_a.inspect
			@dbn.predict(@test_data[0])[0].round.should == [1.0, 0.0]
			@dbn.predict(@test_data[1])[0].round.should == [0.0, 1.0]
			@dbn.predict(@test_data[0])[0][0].should > 0.8
			@dbn.predict(@test_data[1])[1][0].should > 0.8
		end

	end
end