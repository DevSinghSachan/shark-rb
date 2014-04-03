require_relative '../deep_belief_network'

describe 'Deep Belief Network' do

	describe 'Prediction layer' do
		before(:all) do
			Shark::RNG.seed 120

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

			@dbn = Shark::RBM::DBN.new rbm_type: Shark::RBM::RBM,
									   samples: @training_data,
									   labels: @training_labels,
									   input_size:  @training_data.size2,
									   hidden_layers:  [3, 3],
									   output_size: @training_labels.size2

		    # pre-training (TrainUnsupervisedDBN)
		    @dbn.pretrain learning_rate: @pretrain_lr, k: @k, epochs: @pretraining_epochs, verbose: false
		    # fine-tuning (DBNSupervisedFineTuning)
		    @dbn.finetune learning_rate: @finetune_lr, epochs: @finetune_epochs, verbose: false
		    @test_data = Shark::RealMatrix.new [
		    	[1, 1, 0, 0, 0, 0],
		        [0, 0, 0, 1, 1, 0],
		        [1, 1, 1, 1, 1, 0]
		    ]
		end

		it 'should have the same input size as the last RBM output size' do
			@dbn.prediction_layer.input_size.should == @dbn.sigmoid_layers.last.output_size
		end

		it 'should have the same output size as the DBN' do
			@dbn.output_size.should == @dbn.prediction_layer.output_size
		end

		it 'should label accordingly' do
			@dbn.rbm_layers[0].weight_matrix.should       == @dbn.sigmoid_layers[0].weight_matrix.to_a
			@dbn.rbm_layers[0].hbias.should               == @dbn.sigmoid_layers[0].bias
			@dbn.rbm_layers[1].weight_matrix.should       == @dbn.sigmoid_layers[1].weight_matrix.to_a
			@dbn.rbm_layers[1].hbias.should               == @dbn.sigmoid_layers[1].bias

			@dbn.predict(@test_data[0])[0].round.should == [1.0, 0.0]
			@dbn.predict(@test_data[1])[0].round.should == [0.0, 1.0]
			@dbn.predict(@test_data[0])[0][0].should > 0.8
			@dbn.predict(@test_data[1])[0][1].should > 0.8
		end

	end
end