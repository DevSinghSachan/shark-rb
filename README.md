Sparse Autoencoder
==================

### What is a Sparse Autoencoder? ###

A sparse autoencoder is a 1 hidden layer Neural Network trained to output its input while limiting the mean activation of the hidden layer for each sample. In doing so the inner hidden neurons will partition the input space to best reproduce the input and limit simultaneous activation, thereby creating "activation filters" which serve as "building blocks" to reproduce a given input (e.g. filter 1 and filter 2 activate jointly to produce an image that is best understood as the sum of filter 1 and filter 2. With text we could imagine a set of stories that share the same beginning and end, but different middles. A filter may exist to reproduce all beginnings, and another for all endings, and then several will take on the role of middles).

### What is a Shark? ###

[Shark](http://image.diku.dk/shark/) is a C++ library that implements many Machine learnign algorithms as well as some pretty extensible Neural Net building blocks. One of their suggested neural nets is a [sparse autoencoder](http://www.stanford.edu/class/cs294a/handouts.html) similar to the one built in MatLab for the Stanford course CS294A taught by Richard Socher / Andrew Ng / and others from the deep-learning community there.

### Why Ruby? ###

While Ruby is not a typical AI language, it is rather practical for interfacing with web-based content sources. In particular Twitter, Yelp, WikiPedia, and other *Big Data* sources. Unsupervised learning algorithms are well suited for these large scale learning tasks due to the impracticality of hand labeling these humongous datasets. Thus this is how this rubygem was born: give a simple Ruby interface to a sparse autoencoder that is C/C++ fast.

Other implementations built without Shark are built, but they lack the extensibility offered here (in particular stacked autoencoders offer great learning possibilites and are responsible for the "deep" in "deep learning").

**author:** Jonathan Raiman

## Installation ##
To install this package run `git clone` on this repo. Once the repo is downloaded navigate into it and:

	rake -T

This will display a set of possible actions from this rubygem. Run `rake install` to `make` and link this gem to your Ruby installation.

### Dependencies ###

You must have the following installed to install this gem:

	* Boost
	* Blas
	* Shark
	* Ruby 1.9.3 (you can probably get away with a more recent one, but this one has such great support, why would you want to use something else?)

## Usage ##

Here's the good part. Usage with text is dead simple:

	require 'shark-rb'

	samples = [

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

## Issues ##

	??

## Next steps ##

It would be great to create Lisp, Java, and other language bindings to this package. The C++ part is done, and bindings are easy to create in Ruby (and I hear the same is true about Lisp and Python).

Also Shark offers many other algorithms that are interesting to experiment with (SVM and others).


