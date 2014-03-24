require_relative '../LogisticRegression'


describe 'Logistic Regression' do

	before(:all) do

		Shark::RNG.seed 123

		# training data
	    @training_data = Shark::RealMatrix.new [
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
	        [0, 1],
	        [0, 1],
	        [0, 1]
	    ]

	    # construct LogisticRegression
	    @classifier = Optimizer::LogisticRegression.new samples: @training_data,
						                                labels: @training_labels,
						                                number_of_inputs: @training_data.number_of_columns,
						                                number_of_outputs: @training_labels.number_of_columns

		@training_epochs = 200
		@learning_rate = 0.01
	    # # test
	    @test_data = Shark::RealMatrix.new [
	        [1, 1, 0, 0, 0, 0],
	        [0, 0, 0, 1, 1, 0],
	        [1, 1, 1, 1, 1, 0]
	    ]
	end

	before(:all) do
		@costs = []
	    @training_epochs.times do |i|
	        @classifier.train learning_rate: @learning_rate
	        @learning_rate *= 0.95
	        @costs << @classifier.negative_log_likelihood
	    end
	end

	describe 'Training error' do

		it 'should decrease with every epoch' do
			@costs.each_with_index do |cost, i|
				if i > 0 then @costs[i-1].should > cost end
			end
		end
	end

	describe 'Prediction' do

		it 'should give predictions for Arrays, Realvectors, and Matrices' do
			->(){@classifier.predict(@test_data[0].to_a)}.should_not raise_error
			->(){@classifier.predict @test_data[0]      }.should_not raise_error
			->(){@classifier.predict @test_data         }.should_not raise_error
		end

		it 'should return a label prediction for each given row' do
			@classifier.number_of_outputs.should                     == @training_labels.number_of_columns
			@classifier.number_of_inputs.should                      == @training_data.number_of_columns
			@classifier.predict(@test_data).number_of_columns.should == @training_labels.number_of_columns
			@classifier.predict(@test_data).number_of_rows.should    == @test_data.number_of_rows
			@classifier.predict(@test_data).class.should             == Shark::RealMatrix
		end

		it 'should label accordingly' do
			@classifier.predict(@test_data[0])[0].round.should == [1.0, 0.0]
			@classifier.predict(@test_data[1])[0].round.should == [0.0, 1.0]
		end
	end

end