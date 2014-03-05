Deep Belief Networks
====================

Deep Belief Network in Ruby, using Shark (`shark-rb`) inspired by Yusuke Gomori's examples (see [references](#references-)).

**Author / Translator :** Jonathan Raiman


Implementation
--------------


This also introduces some new classes to the Shark namespace:

#### DBN

The **DBN** class is a deep belief network: a series of stacked RBMs with a Logistic Regression step at the top.


#### Logistic Regression

The **Logisitc Regression** class is a classifier with a Softmax behavior. Here's how you set it up:


	x = Shark::RealMatrix.new [
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
	        [0, 1],
	        [0, 1],
	        [0, 1]
	    ]

	classifier = Optimizer::LogisticRegression.new samples: x,
                                                   labels: y,
                                                   number_of_inputs: 6,
                                                   number_of_outputs: 2




To train it, now do the following:

	learning_rate = 0.01
	
	200.times do |epoch|s
	    classifier.train learning_rate: learning_rate
	    learning_rate *= 0.95
	    cost = classifier.negative_log_likelihood
	    puts "Training epoch #{epoch}, cost is #{cost}"
	end


You can now use it for predictions:

	classifier.predict([1, 1, 0, 0, 0, 0])
	# => #<Optimizer::RealMatrix:0x007fe931b1cde0 @data = [[0.6920317364189528, 0.30796826358104723]]>


We find that the sample `[1, 1, 0, 0, 0, 0]` seems to have come from the first 3 vectors rather than the last 3 :)



Tests
-----

run `rake test` to do unit testing on Logistic Regression and DBN (DBN tests are currently a stub).


### References :

  - Yusuke Sugomori
  [Deep Learning](https://github.com/yusugomori/DeepLearning)

  - Y. Bengio, P. Lamblin, D. Popovici, H. Larochelle: Greedy Layer-Wise
  Training of Deep Networks, Advances in Neural Information Processing
  Systems 19, 2007

  - P. Vincent, H. Larochelle, Y. Bengio, P.A. Manzagol: Extracting and
  Composing Robust Features with Denoising Autoencoders, ICML' 08, 1096-1103,
  2008

  - DeepLearningTutorials
  [DeepLearningTutorials](https://github.com/lisa-lab/DeepLearningTutorials)

  - Yusuke Sugomori: Stochastic Gradient Descent for Denoising Autoencoders,
  [Stochastic Gradient Descent for Denoising Autoencoders](http://yusugomori.com/docs/SGD_DA.pdf)