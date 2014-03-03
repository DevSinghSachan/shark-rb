module HeaderFileGenerator
	class HeaderFile
		class Method
			class InputChecker
				# could be simplified to reduce number of propositions.
				def initialize tests=[]
					@tests = tests
					@message = nil
				end

				def default_message
					@message
				end

				def tests
					@tests
				end

				def and other_test
					if other_test.is_a? Conjunction
						Conjunction.new(self.tests + other_test.tests)
					else
						Conjunction.new([self, other_test])
					end
				end

				def or other_test
					if other_test.is_a? Disjunction
						Disjunction.new(self.tests + other_test.tests)
					else
						Disjunction.new([self, other_test])
					end
				end

				def error_message(msg)
					@message = msg
					self
				end

				def rb_raise_if message=nil, indent=0
"""#{"\t"*indent}if #{self.to_s}
#{"\t"*(indent+1)}rb_raise(rb_eArgError, \"#{message ? message : default_message}\");
"""
				end

				def rb_raise_unless message=nil, indent=0
"""#{"\t"*indent}if (!#{self.to_s})
#{"\t"*(indent+1)}rb_raise(rb_eArgError, \"#{message ? message : default_message}\");
"""
				end
			end

			class Conjunction < InputChecker
				def to_s
					"(" + @tests.map {|i| i.to_s}.join(" && ") + ")"
				end
			end

			class Disjunction < InputChecker
				def to_s
					"(" + @tests.map {|i| i.to_s}.join(" || ") + ")"
				end
			end
		end
	end
end