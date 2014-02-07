require 'set'
require 'ostruct'
class Optimizer
	module Conversion
		class Text

			def self.text_sample_from_sample sample, fv
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
			def self.text_sample_set_from_samples samples
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

			def self.present filters, cutoff=0, ordered=true, cutoff_number=false
				filters.each do |filter|
					puts "\n{"
					if ordered
						filter.to_a.sort_by {|i| -i[1]}.each_with_index do |pair, k|
							if cutoff_number and k > cutoff_number then break end
							if pair[1] > cutoff
								puts "    #{pair[0]} => #{pair[1]}"
							end
						end
					else
						filter.to_a.each_with_index do |pair, k|
							if cutoff_number and k > cutoff_number then break end
							if pair[1] > cutoff
								puts "    #{pair[0]} => #{pair[1]}"
							end
						end
					end
					puts "}\n"
				end
			end

			def self.samples opts={}
				vectors = Array.new(opts[:data].length).map {Array.new(opts[:vector].keys.length, 0.0)}
				opts[:data].each_with_index do |sample, k|
					vectors[k] = Optimizer::Conversion::Text.text_sample_from_sample sample, opts[:vector]
				end
				vectors
			end

			def self.text_samples_from_filters opts={}
				converted_filter = Hash.new(0.0)
				opts[:filter].each_with_index do |score, k |
					converted_filter[opts[:vector][k]] = score
				end
				converted_filter
			end
		end
	end
end