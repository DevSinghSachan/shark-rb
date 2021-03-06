Shark Machine Learning Library
==============================

tl;dr : The extensibility of Shark and the beauty of Ruby.

Sparse Autoencoder
------------------

### What is a Sparse Autoencoder? ###

A sparse autoencoder is a 1 hidden layer Neural Network trained to output its input while limiting the mean activation of the hidden layer for each sample. In doing so the inner hidden neurons will partition the input space to best reproduce the input and limit simultaneous activation, thereby creating "activation filters" which serve as "building blocks" to reproduce a given input (e.g. filter 1 and filter 2 activate jointly to produce an image that is best understood as the sum of filter 1 and filter 2. With text we could imagine a set of stories that share the same beginning and end, but different middles. A filter may exist to reproduce all beginnings, and another for all endings, and then several will take on the role of middles).

### What is Shark? ###

[Shark](http://image.diku.dk/shark/) is a C++ library that implements many Machine learnign algorithms as well as some pretty extensible Neural Net building blocks. One of their suggested neural nets is a [sparse autoencoder](http://www.stanford.edu/class/cs294a/handouts.html) similar to the one built in MatLab for the Stanford course CS294A on unsupervised learning.

### Why Ruby? ###

While Ruby is not a typical AI language, it is rather practical for interfacing with web-based content sources. In particular Twitter, Yelp, WikiPedia, and other *Big Data* sources. Unsupervised learning algorithms are well suited for these large scale learning tasks due to the impracticality of hand labeling these humongous datasets. Thus this is how this rubygem was born: give a simple Ruby interface to a sparse autoencoder that is C/C++ fast.

**author (translator/binder):** Jonathan Raiman

## Installation ##
To install this package run `git clone` on this repo. Once the repo is downloaded navigate into it and:

	rake -T

This will display a set of possible actions from this rubygem. Run `rake install` to `make` and link this gem to your Ruby installation.

### Dependencies ###

You must have the following installed to install this gem:

	* Boost
	* Blas
	* Shark
	* Ruby 1.9.3*

**(you can probably get away with a more recent one, but this one has such great support, why would you want to use something else?)*

## Usage ##

Here's the good part. Usage with text is dead simple:

	require 'shark-rb'

	samples = []

	samples << ["Joe", "Mary", "Sam"]

	samples << ["Sam", "Kent", "Harvey"]

	# etc...

	encoder = TextAutoencoder.new :samples => samples, :hidden_neurons => 10

	100.times do |i|
		encoder.train
		puts "Error at iteration #{i+1} is: #{encoder.error}"
	end

	encoder.present
	#=> shows hidden neuron filters (clusters of features that are useful to recreate the input)

Principal Component Analysis
----------------------------

Here we find the principal components of a set of vectors and try to reduce them by taking the largest **k** eigenvalued eigenvectors. Here's what the authors of shark have to say about it:

>Principal component analysis (PCA), also known as Karhunen-Loeve transform, is arguably the most fundamental technique in unUnsupervised learning. It is frequently used for (linear) dimensionality reduction, (lossy) data compression, feature extraction, and data visualization. Let us consider a set of *l* data points.

In particular let us show how we can reproduce [Shark's PCA tutorial](http://image.diku.dk/shark/sphinx_pages/build/html/rest_sources/tutorials/algorithms/pca.html) on Eigenfaces taken from the [Cambridge face data](http://www.cl.cam.ac.uk/research/dtg/attarchive/facedatabase.html):

### Reading in the data ###

First, let us read in the data. There is a function for recursively scanning a directory for images in pgm format and reading them in:


	images = Shark.import_pgm "#{Dir.home}/Desktop/Cambridge_FaceDB"


### Models and learning algorithm ###

Doing a PCA is as simple as:

	pca = Shark::PCA.new images

Karhunen-Loeve transformations are affine linear models. For encoding data to an m-dimensional subspace we use:


	m = 299
	enc = Shark::LinearModel.new
	pca.encoder enc, m

The last line encodes (i.e., represents in the PCA coordinate system) the whole image database.

We can easily map from the m dimensional space back to the original n dimensional space by the optimal linear reconstruction (the decoder):


	dec = Shark::LinearModel.new
	pca.decoder dec, m


For instance, let us reconstruct the following first image using just the first m=300 components. Then we write:


	sampleImage = 0

	dec.eval(encodedImages[0]).to_pgm :width => 92, :height => 112, :path => "facesReconstruction#{sampleImage}-#{m}.pgm"


Alternatively, to keep with the Shark syntax:


	Shark.export_pgm :image => dec.eval(encodedImages[0]),
					 :width => 92,
					 :height => 112,
					 :path => "facesReconstruction#{sampleImage}-#{m}.pgm"


For those that were curious about the missing step that encoded the images:


	encodedImages = enc.eval images

	decodedImages = dec.eval encodedImages  # this can go on forever!


uBLAS Datatypes: RealMatrix, RealVector, Rows, Columns, and reference wrappers
------------------------------------------------------------------------------

How would you like to play around with `uBlas` vectors and matrices in Ruby? I know I do.

Here's how you do it:


	a = [1, 2].to_realvector

	b = [5, 7].to_realvector

	c = a + b
	#  => #<Optimizer::RealVector:0x007fbec9093398 @data = [6.0, 9.0]>


What about transposes? Good question!


	mat = Shark::RealMatrix.new [[2, 3, 1], [4, 5, 6]]

	a = [1, 2].to_realvector

	mat * a
	# => ArgumentError: For matrix product A*B incompatible number of A columns (3) and B rows (2) for multiplication.

	~mat * a
	# => #<Optimizer::RealVector:0x007fbec902e948 @data = [10.0, 13.0, 13.0]> 
	# where ~ is the transpose operator.

	mat.transpose * a
	# => #<Optimizer::RealVector:0x007fbec902e948 @data = [10.0, 13.0, 13.0]> 
	# also available as a method!


What can we cast?

	c = [2, 3].to_realvector

	a = [1, 2] + c
	# => #<Optimizer::RealVector:0x007fbed902e948> @data = [3.0, 5.0]>


Reponse: any array can be cast during a sum to a RealVector for convenience!

#### Advanced Matrix Selections ####

We can obtain rows and columns from a matrix in reference form:

	mat = Shark::RealMatrix.new [[2, 3, 1], [4, 5, 6]]

	mat.row 0
	# => #<Optimizer::RealMatrixRow:0x007fbdc9839f80 @data = [2.0, 3.0, 1.0]> 

	mat.rows
	# => [#<Optimizer::RealMatrixRow:0x007f9a2111a0b8 @data = [2.0, 3.0, 1.0]>, #<Optimizer::RealMatrixRow:0x007f9a2111a068 @data = [4.0, 5.0, 6.0]>] 

	mat.col 0
	# => <Optimizer::RealMatrixColumn:0x007fbdc9833e00 @data = [2.0, 4.0]> 

	mat.row(0) == mat[0]
	# => true

Compare a row or vector with a Ruby Array:

	mat.row(0) == [2, 3, 1]
	# => true

Check for stricter equality:

	mat.row(0).eql? mat[0]
	# => true

	mat.row(0).to_a.to_realvector == mat[0]
	# => true

	mat.row(0).to_a.to_realvector.eql? mat[0]
	# => false


##### Matrix and Vector sums #####

In Numpy there's a nice feature for matrices with directed sums:

	mat = numpy.array( [[1,2], [10, 20]])

	mat.sum
	#=> 33

	numpy.sum(mat, axis=0)
	#=> array([11, 22])

	numpy.sum(mat, axis=1)
	#=> array([ 3, 30])


Can we replicate it here? Yes!

	mat = Shark::RealMatrix.new [[1,2], [10, 20]]

	mat.sum
	# => 33.0

	mat.sum :axis => 0
	# => #<Optimizer::RealVector:0x007f980b8590f8 @data = [11.0, 22.0]> 

	mat.sum :axis => 1
	# => #<Optimizer::RealVector:0x007f980b898b40 @data = [3.0, 30.0]>


And finally all these methods can also be accessed as follows for convenience:

	mat.sum 1
	# => #<Optimizer::RealVector:0x007f980b898b40 @data = [3.0, 30.0]>
	mat.sum 0
	# => #<Optimizer::RealVector:0x007f980b8590f8 @data = [11.0, 22.0]> 
	mat.sum :rows
	# => #<Optimizer::RealVector:0x007f980b873638 @data = [3.0, 30.0]>
	mat.sum :columns
	# => #<Optimizer::RealVector:0x007f980b8590f8 @data = [11.0, 22.0]>
	mat.sum :axis => :columns
	# => #<Optimizer::RealVector:0x007f980b8590f8 @data = [11.0, 22.0]>
	mat.sum :axis => :rows
	# => #<Optimizer::RealVector:0x007f980b873638 @data = [3.0, 30.0]>
	mat.sum :direction => :rows
	# => #<Optimizer::RealVector:0x007f980b873638 @data = [3.0, 30.0]>
	# etc...


For vectors, this is much more basic:

	vec = [1, 2, 3].to_realvector

	vec.sum
	# => 6.0

##### Matrix and Vector products #####

Remember how matlab has the `.*` operator? Or how numpy uses `numpy.dot(A, B)` or and `A * B` differently?

Well in this library dot product is considered to be the default, thus `*` is the dot product (numpy: `numpy.dot`, Matlab: `*`),
and the Hadamard product (element wise product) is used as follows:

	a = Shark::RealMatrix.new(10, 20) {|i| 1.0}

	b = Shark::RealMatrix.new(10, 20) {|i| 2.0}

	c = a * b
	# => ArgumentError: For matrix product A*B incompatible number of A columns (20) and B rows (10) for multiplication.

	# c = a.hadamard(b)
	# or c = a.element_wise_product(b)

	c = a.o b
	# => #<Optimizer::RealMatrix:0x007fe9c40ed048 @data = [[2.0, 2.0, 2.0, 2.0, ... etc... ]]>

While the dot product is used as follows:

	a = Shark::RealMatrix.new(10, 20) {|i| 1.0}
	b = Shark::RealMatrix.new(20, 10) {|i| 2.0}

	c = a * b
	#<Optimizer::RealMatrix:0x007fe9c4042ee0 @data = [[40.0, 40.0, 40.0, 40.0, ... etc ...]]>

	c.size
	# => [10, 10]

#### Matrix and Vector Initialization ###

Okay, so **numpy** has beautiful syntax like this:

	binomial_matrix = numpy.random.binomial(size=shape,
	                                           n=1,
	                                           p=probabilities)


If you are curious the line above returns a matrix or vector of the dimensions specified by `shape`
and the probabilities given in `probabilities`. Of course this means that initializing random
vectors is easy and readable.

In our case, while we may one day incorporate Numpy syntax, we have the following strategy. We first obtain a random number generator with a binomial distribution:


	bin = Shark::RNG::Binomial.new
	bin.p = 0.7
	bin.n = 1

Then we sample from it during the initialization of our matrix:

	Shark::RealMatrix.new(10, 10) do |i,j|
		bin.sample
	end

Of course to duplicate the numpy functionality we need to vary the probability according to where we
are in the matrix. We can do this as follows:

	bin = Shark::RNG::Binomial.new
	bin.n = 1

	probabilities = [
		[0.1, 0.2, 0.3],
		[0.2, 0.9, 0.4]
	]

	mat = Shark::RealMatrix.new(2, 3) do |i,j|
		bin.p = probabilites[i][j]
		bin.sample
	end

Of course the same is valid for a vector:

	vec = Shark::RealVector.new(10) {|i| bin.p=probabilites[i][0]; bin.sample }


Random Number Generators
------------------------


Shark has many Random Number Generator distributions. You can access most of them as follows:


	Shark::RNG::Bernoulli
	Shark::RNG::Binomial
	Shark::RNG::Cauchy
	Shark::RNG::DiffGeometric
	Shark::RNG::DiscreteUniform
	Shark::RNG::Dirichlet
	Shark::RNG::DiscreteUniform
	Shark::RNG::Erlang
	Shark::RNG::Geometric
	Shark::RNG::LogNormal
	Shark::RNG::NegExponential
	Shark::RNG::Normal
	Shark::RNG::Poisson
	Shark::RNG::TruncatedExponential
	Shark::RNG::Uniform
	Shark::RNG::Weibull

Each has a `sample` method that replaces the `operator()` method from C++. Their methods can be found on the Shark site:


* [Bernoulli](http://image.diku.dk/shark/doxygen_pages/html/classshark_1_1_bernoulli.html)
* [Binomial](http://image.diku.dk/shark/doxygen_pages/html/classshark_1_1_binomial.html)
* etc...


Softmax Model
-------------

The [Softmax](http://image.diku.dk/shark/doxygen_pages/html/classshark_1_1_softmax.html) model is wrapped as follows:

	soft = Shark::Softmax.new
	soft.set_structure 3

	soft.eval [1, 2, 3]
	# => #<Optimizer::RealVector:0x007fc20b00fd00 @data = [0.09003057317038046, 0.24472847105479767, 0.6652409557748219]>

Support for better constructors is on its way (so that the `set_structure` step can be avoided) :) !


Binary Restricted Boltzmann Machines
------------------------------------

Let us go off the [Binary RBM tutorial given on the Shark website](http://image.diku.dk/shark/sphinx_pages/build/html/rest_sources/tutorials/algorithms/binary_rbm.html):

#### Introduction ####

Shark has a module for training restricted Boltzmann machines (RBMs) **[Hinton2007](http://image.diku.dk/shark/sphinx_pages/build/html/rest_sources/tutorials/algorithms/rbm_module.html#hinton2007)** **[Welling2007](http://image.diku.dk/shark/sphinx_pages/build/html/rest_sources/tutorials/algorithms/rbm_module.html#welling2007)**. All corresponding header files are located in the subdirectory `<SHARK_SRC_DIR>/include/shark/Unsupervised/RBM/`. We will assume that you already read the introduction to the RBM module The RBM Module.

In the following, we will train and evaluate a Binary RBM using Contrastive Divergence (CD-1) learning on a toy example. We choose this example as a starting point because its setup is quite common, and we provide a set of predefined types for it for convenience.

The example file for this tutorial can be found in `BinaryRBM.cpp.`

#### Contrastive Divergence learning - Code ####

First, we need to include the following files:

	
	require 'shark-rb'
	include Shark::Problems
	include Shark::RBM
	include Shark::RBM::Analytics # for negative log likelihood


As an example problem, we consider one of the predefined benchmark problems in `RBM/Problems/`, namely, the Bars-and-Stripes data set:


	problem = BarsAndStripes.new
	data = problem.data


Now we can create the RBM. We have to define how many input variables (visible units/observable variables) our RBM shall have. This depends on the data set from which we want to learn, since the number of visible neurons has to correspond to the dimensionality of the training data. Further, we have to choose how many hidden neurons (latent variables) we want. Also, to construct the RBM, we need to choose a random number generator. Since RBM training is time consuming, we might later want to start several trials in separate instances. In this setup, being able to choose a random number generator is crucial. But now, let’s construct the beast:


	numberOfHidden = 32 #hidden units of the rbm
	numberOfVisible = problem.input_dimension #visible units of the inputs

	#create rbm with simple binary units:
	rbm = BinaryRBM.new
	rbm.set_structure :hidden => numberOfHidden, :visible => numberOfVisible
	# or to keep with Shark syntax: rbm.set_structure numberOfVisible, numberOfHidden


Using the RBM, we can now construct the k-step Contrastive Divergence error function. Since we want to model Hinton’s famous algorithm we will set k to 1. Throughout the library we use the convention that all kinds of initialization of the structure must be set before calling `data=(val)`. This allows the gradients to adjust their internal structures. For CD-k this is not crucial, but you should get used to it before trying more elaborate gradient approximators:


	cd = Shark::BinaryCD.new rbm # you can also replace Shark::BinaryCD by Shark::ExactGradient, however it is much slower.
	cd.k = 1
	cd.data = data # which we obtained earlier using: "problem.data"


The RBM optimization problem is special in the sense that the error function can not be evaluated exactly for more complex problems than trivial toy problems, and the gradient can only be estimated. This is reflected by the fact that all RBM derivatives have the Flag `HAS_VALUE` (this is a Shark class property that is not exposed to Ruby yet -- this is why `Shark::Algorithms::LBFGS` will not work here.) deactivated. Thus, most optimizers will not be able to optimize it. One which is capable of optimizing it is the GradientDescent algorithm, which we will use in the following
	
	optimizer = Shark::Algorithms::SteepestDescent.new
	optimizer.momentum = 0.0
	optimizer.learning_rate = 0.1

Since our problem is small, we can actually evaluate the negative log-likelihood. So we use it at the end to evaluate our training success after training several trials

	numIterations = 1000 #iterations for training
	numTrials = 10 #number of trials for training
	meanResult = 0.0
	numTrials.times do |trial|
		initializeWeights rbm
		optimizer.init cd
		numIterations.times do |iteration|
			optimizer.step cd
		end

		# evaluate exact likelihood after training. this is only possible for small problems!
		likelihood = Shark::RBM::Analytics.negative_log_likelihood rbm, data

		print trial, " ", likelihood, "\n"

		meanResult += likelihood
	end

	meanResult /= numTrials


Now we can print the results as usual with:


	print "RESULTS: ", "\n"
	print "======== ", "\n"
	print "mean negative log likelihood: ", meanResult, "\n"


and the result will read something like:


	RESULTS:
	========
	mean log likelihood: 192.544


PS: there's a missing step we find the in the tutorial file:

	def initializeWeights rbm
		weights = Shark::RealVector.new(rbm.number_of_parameters) {|i| Random.rand(0.2) - 0.1}
		rbm.parameter_vector = weights
	end


Dimension Reduction comparisions
--------------------------------


Suppose we want to reduce the number of dimensions in a dataset. To illustrate this dimensionality reduction problem we will consider a bag of words dataset we convert to vectors.

As an example we will take some word associations between certain European countries, their past colonies, and some tidbits about China:

	samples = []

	samples << ["Paris", "France", "Napoleon"]
	samples << ["Napoleon", "Corsica"]
	samples << ["Coffee", "Caribeean"]
	samples << ["Colonies", "Napoleon"]
	samples << ["Paris", "Culture"]
	samples << ["Paris", "Tennis", "Roland-Garros", "France"]
	samples << ["Napoleon", "War"]
	samples << ["Napoleon", "General"]
	samples << ["China", "Hun"]
	samples << ["China", "Import-Export"]
	samples << ["Caribeean", "Import-Export"]
	samples << ["Russia", "Waterloo", "Napoleon", "France"]
	samples << ["Russia", "Tsar", "France", "Catherine the Great"]
	samples << ["Russia", "Nabokov"]
	samples << ["Colonies", "USA", "Napoleon", "Independence"]
	samples << ["Founding Fathers", "USA", "Independence"]

Once our examples are loaded in we can now proceed to apply:

	1. A sparse AutoEncoder

	2. Principal Component Analysis

Each "bag" in our samples can be represented by a vector in the common vector space where each word represents one basis vector.

Using this methodology we can then compare the two approaches:

	pca = TextPCA.new :samples => samples, :dimensions => 5
	autoencoder = TextAutoencoder.new :samples => samples, :hidden_neurons => 5

Each hidden neuron in our Autoencoder represents a "filter" which acts as a new dimension in our reduced space.

The results can then be compared as follows:

	# we first need to train the autoencoder
	100.times {autoencoder.train}

	# then we can observe the results:

	autoencoder_filters = autoencoder.parameters
	pca_filters = pca.parameters


	pca_filters[0]
	# => {
			"Paris"               => 0.3794550119360657,
			"France"              => 0.5941384854054892,
			"Napoleon"            => 0.2663832982265656,
			"Corsica"             => 0.025642724623351328,
			"Coffee"              => 0.05277653020188849,
			"Caribeean"           => 0.10339719720976602,
			"Colonies"            => -0.05118444157142263,
			"Culture"             => 0.0915330355433201,
			"Tennis"              => 0.17427168375950267,
			"Roland-Garros"       => 0.17427168375950267,
			"War"                 => 0.025642724623351328,
			"General"             => 0.025642724623351314,
			"China"               => 0.10339719720976599,
			"Hun"                 => 0.052776530201888466,
			"Import-Export"       => 0.10124133401575505,
			"Russia"              => 0.40180576217652103,
			"Waterloo"            => 0.12698927329469128,
			"Tsar"                => 0.17922723571805216,
			"Catherine the Great" => 0.17922723571805216,
			"Nabokov"             => 0.09558925316377753,
			"USA"                 => -0.06922484786608529,
			"Independence"        => -0.06922484786608529,
			"Founding Fathers"    => -0.013798900830750069
		}

	autoencoder_filters[0]
	# => {
			"Independence"        => 3.588175898266438,
			"USA"                 => 3.566360086125802,
			"Colonies"            => 1.286890962118011,
			"Founding Fathers"    => 1.2152712816877504,
			"Napoleon"            => -0.4019045745182443,
			"Roland-Garros"       => -0.7611325630369179,
			"Tennis"              => -0.7818230801150218,
			"Catherine the Great" => -0.7889429645085315,
			"Tsar"                => -0.8193810829552926,
			"Waterloo"            => -0.8250753794403964,
			"Coffee"              => -0.9219751572340426,
			"Culture"             => -0.9439164440582949,
			"Hun"                 => -0.9559185527755326,
			"Nabokov"             => -0.9915334642270374,
			"General"             => -1.0309468249908464,
			"Corsica"             => -1.0389895602259018,
			"War"                 => -1.0435156595642454,
			"China"               => -1.1431966341707245,
			"Import-Export"       => -1.1716929258881106,
			"Caribeean"           => -1.1771240188468906,
			"Russia"              => -1.1892642828543816,
			"France"              => -1.2709889156956518,
			"Paris"               => -1.3169276240654713
		}

You can now compare how dimensions (of mixtures of features in the autoencoder's filters) are represented by both methodologies. There you go!

## Issues ##

Unexplored territory.
