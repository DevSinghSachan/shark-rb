# require_relative '../LogisticRegression'


# describe 'Logistic Regression' do

# 	before(:all) do

# 		Shark::RNG.seed 123

# 		# training data
# 	    @training_data = Shark::RealMatrix.new [
# 	        [1,1,1,0,0,0],
# 	        [1,0,1,0,0,0],
# 	        [1,1,1,0,0,0],
# 	        [0,0,1,1,1,0],
# 	        [0,0,1,1,0,0],
# 	        [0,0,1,1,1,0]
# 	    ]
# 	    @training_labels = Shark::RealMatrix.new [
# 	        [1, 0],
# 	        [1, 0],
# 	        [1, 0],
# 	        [0, 1],
# 	        [0, 1],
# 	        [0, 1]
# 	    ]

# 	    # construct LogisticRegression
# 	    @classifier = Optimizer::LogisticRegression.new samples: @training_data,
# 						                                labels: @training_labels,
# 						                                number_of_inputs: @training_data.number_of_columns,
# 						                                number_of_outputs: @training_labels.number_of_columns

# 		@training_epochs = 200
# 		@learning_rate = 0.01
# 	    # # test
# 	    @test_data = Shark::RealMatrix.new [
# 	        [1, 1, 0, 0, 0, 0],
# 	        [0, 0, 0, 1, 1, 0],
# 	        [1, 1, 1, 1, 1, 0]
# 	    ]
# 	end

# 	before(:all) do
# 		@costs = []
# 	    @training_epochs.times do |i|
# 	        @classifier.train learning_rate: @learning_rate
# 	        @learning_rate *= 0.95
# 	        @costs << @classifier.negative_log_likelihood
# 	    end
# 	end

# 	describe 'Training error' do

# 		it 'should decrease with every epoch' do
# 			@costs.each_with_index do |cost, i|
# 				if i > 0 then @costs[i-1].should > cost end
# 			end
# 		end
# 	end

# 	describe 'Prediction' do

# 		it 'should give predictions for Arrays, Realvectors, and Matrices' do
# 			->(){@classifier.predict(@test_data[0].to_a)}.should_not raise_error
# 			->(){@classifier.predict @test_data[0]      }.should_not raise_error
# 			->(){@classifier.predict @test_data         }.should_not raise_error
# 		end

# 		it 'should return a label prediction for each given row' do
# 			@classifier.number_of_outputs.should                     == @training_labels.number_of_columns
# 			@classifier.number_of_inputs.should                      == @training_data.number_of_columns
# 			@classifier.predict(@test_data).number_of_columns.should == @training_labels.number_of_columns
# 			@classifier.predict(@test_data).number_of_rows.should    == @test_data.number_of_rows
# 			@classifier.predict(@test_data).class.should             == Shark::RealMatrix
# 		end

# 		it 'should label accordingly' do
# 			@classifier.predict(@test_data[0])[0].round.should == [1.0, 0.0]
# 			@classifier.predict(@test_data[1])[0].round.should == [0.0, 1.0]
# 		end
# 	end

# 	describe 'In a DBN' do

# 		before(:all) do
# 			@dbn = Shark::RBM::DBN.new samples: @training_data,
# 			                       labels: @training_labels,
# 			                       input_size:  @training_data.size2,
# 			                       hidden_layers: [3, 3],
# 			                       output_size: @training_labels.size2
# 			@dbn.rbm_layers[0].weight_matrix = [
# 				[-7.04423364,-5.71574815, 3.85495703, 6.54276807, 1.24076955,-4.26321733],
# 				[ 3.05550762, 0.43632773, 2.51919426,-3.49456278,-4.12685438,-3.22315804],
# 				[ 4.76580181, 1.86983584, 1.39850034,-5.46176108,-3.95184239,-2.84229769]
# 			]
# 			@dbn.rbm_layers[0].hidden_neurons.bias  = [-0.28860541,-0.13251746,-0.27677419]
# 			@dbn.rbm_layers[0].visible_neurons.bias = [-0.05714286, -0.51428571, 1.37142857, 0.22857143, -0.41428571, -1.04285714]
# 			@dbn.rbm_layers[1].weight_matrix = [
# 				[ 5.06809146,-1.19230379,-4.65362493],
# 				[ 4.69813234,-0.87270286,-4.53287803],
# 				[-6.87076436, 4.84307439, 5.72700937]
# 			]
# 			@dbn.rbm_layers[1].hidden_neurons.bias  = [0.04541275, -0.00311371, -0.52491982]
# 			@dbn.rbm_layers[1].visible_neurons.bias = [0.02857143, 0.88571429, 0.21428571]
# 			@dbn.sigmoid_layers[0].input = @training_data
# 			@dbn.sigmoid_layers[1].input = @dbn.sigmoid_layers[0].sample_h_given_v
# 			@dbn.finetune learning_rate: 0.1, epochs: @training_epochs
# 		end

# 		it 'should peform well for a well conditioned DBN' do
# 			@dbn.predict(@test_data[0])[0].round.should == [1.0, 0.0]
# 			@dbn.predict(@test_data[1])[0].round.should == [0.0, 1.0]
# 			@dbn.predict(@test_data[0])[0][0].should > 0.8
# 			@dbn.predict(@test_data[1])[0][1].should > 0.8
# 		end
# 	end

# end