class Fixnum
	def to_word
		word = self.to_s
		case word[-1]
		when "1"
			word + "st"
		when "2"
			word + "nd"
		when "3"
			word + "rd"
		else
			word + "th"
		end
	end
end

module HeaderFileGenerator
	class HeaderFile
		class Method
			class OverloadedInput < HeaderFileGenerator::HeaderFile::Method::Input
				attr_accessor :chosen_overloading
				attr_reader :output_classes

				def initialize(opts={})
					@type = opts[:type].map {|i| i.to_sym}
					@position = opts[:position]
					@converted = false
					@method = opts[:method]
					@output_classes = opts[:output_classes]
					@chosen_overloading = nil
				end

				def compatible_classes
					klass_finder = ->(specific_class) {
						classes = CppClass.can_convert_to(specific_class)
						if classes.empty? then raise CppError.new "No compatible C++ conversion classes for #{specific_class}."
						else classes end
					}
					def klass_finder.for klass
						self.call(klass)
					end
					klass_finder
				end

				def convert_and_embed_with_overloading(all_params, remaining_params, calling_methodology, indent=0)
					overloaded_input = @output_classes[@chosen_overloading]
					if requires_conversion?
						cpp = ""
						compatible_classes.for(overloaded_input).each_with_index do |cpp_class, k|
							cpp += (k==0 ? "#{"\t"*indent}if" : " else if") + " (" + (matches_classes [cpp_class] ).to_s + ") " + "{\n"
							cpp += cpp_class.convert_into_class self, indent+1
							self.input_class = cpp_class
							if remaining_params.length > 0
								cpp += remaining_params.first.convert_and_embed all_params, remaining_params[1..(remaining_params.length-1)], calling_methodology, indent+1
							else
								cpp += (calling_methodology.call(indent+1) + "\n")
							end
							cpp += ("\t"*indent+"}")
						end
						cpp += "\n"
						cpp
					else
						if remaining_params.length > 0
							remaining_params.first.convert_and_embed all_params, remaining_params[1..(remaining_params.length-1)], calling_methodology, indent
						else
							calling_methodology.call(indent) + "\n"
						end
					end
				end

				def check indent=1
					msgs = []
					Disjunction.new(@type.map {|i| test = check_for_type(i); msgs << test.default_message; test}).error_message(
						msgs.map {|i| i[0..i.length-2]}.join('\n OR ')
						).rb_raise_if(nil, indent)
				end

				def comment_on_overloaded_test indent
					"#{"\t"*indent}// Checking whether overloaded input matches #{(chosen_overloading+1).to_word} possibility: #{@output_classes[@chosen_overloading]}.\n"
				end

				def convert_and_embed(all_params, remaining_params, calling_methodology, indent=0)
					cpp = ""
					@output_classes.each_with_index do |overloaded_input, input_num|
						cpp += (input_num==0 ? "#{"\t"*indent}if" : " else if") + " (" + (matches_classes compatible_classes.for(overloaded_input) ).to_s + ") " + "{\n"
						self.chosen_overloading = input_num
						cpp += comment_on_overloaded_test indent+1
						cpp += convert_and_embed_with_overloading all_params, remaining_params, calling_methodology, indent + 1
						self.chosen_overloading = nil
						cpp += "\n"
						cpp += ("\t"*indent+"}")
					end
					cpp
				end

				def to_converted_form
					if chosen_overloading.nil? then raise RuntimeError.new "#{output_name} of types #{@type} was used before an overloaded type class could be assigned." end
					to_converted_form_for_type @type[chosen_overloading], @output_classes[chosen_overloading]
				end
			end
		end
	end
end