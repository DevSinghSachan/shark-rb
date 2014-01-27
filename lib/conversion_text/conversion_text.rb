require 'set'
require 'ostruct'
class Optimizer
	module Conversion
		class Text

			def self.create_text_sample_from_sample sample, fv
				vector = Array.new(fv.keys.length, 0.0)
				case sample
				when Array
					# each element in Array exists only once.
					sample.each do |keyword|
						vector[fv[keyword]] = 1.0
					end
				when Hash
					# keys lead to special values
					sample.keys.each do |keyword|
						vector[fv[keyword]] = sample[keyword]
					end
				else
					# some special class perhaps?
					sample.to_a.each do |keyword|
						vector[fv[keyword]] = sample[keyword]
					end
				end
				vector
			end

			# creates a set of text to convert text features to vectors
			def self.create_text_sample_set_from_samples samples
				feature_vector = Set.new
				case samples.first
				when Array
					# each element in Array exists only once.
					samples.each do |sample|
						feature_vector.merge sample
					end
				when Hash
					# keys lead to special values
					samples.each do |sample|
						feature_vector.merge sample.keys
					end
				else
					# some special class perhaps?
					samples.each do |sample|
						feature_vector.merge sample.to_a
					end
				end
				fv = {}
				feature_vector.each_with_index {|i,k| fv[i] = k}
				OpenStruct.new :feature_vector => fv, :features => feature_vector.to_a
			end

			def self.create_samples opts={}
				vectors = Array.new(opts[:data].length).map {Array.new(opts[:vector].keys.length, 0.0)}
				opts[:data].each_with_index do |sample, k|
					vectors[k] = Optimizer::Conversion::Text.create_text_sample_from_sample sample, opts[:vector]
				end
				vectors
			end

			def self.create_text_samples_from_filters opts={}
				converted_filter = Hash.new(0.0)
				opts[:filter].each_with_index do |score, k |
					converted_filter[opts[:vector][k]] = score
				end
				converted_filter
			end
		end
	end
end

TextAutoencoder = Optimizer::Autoencoder::Text