module HeaderFileGenerator
	class HeaderFile
		class Converter

			Conversions = {
				"Array" => {
					"RealVector" => "rb_ary_to_1d_realvector",
					"RealMatrix" => "rb_ary_to_realmatrix"
				},
				"RealVector" => {
					"Array" => "realvector_to_rb_ary"
				},
				"RealMatrix" => {
					"Array" => "realmatrix_to_rb_ary"
				},
				"Float" => {
					"double"     => "NUM2DBL",
					"int"        => "NUM2INT"
				},
				"double" => {
					"Float"      => "rb_float_new",
					"Fixnum"     => "INT2FIX"
				},
				"int" => {
					"Float"      => "rb_float_new",
					"Fixnum"     => "INT2FIX"
				}
			}

			def initialize(variable_name="")
				@name = variable_name
			end

			def self.convert(variable_name="")
				self.new variable_name
			end

			def to_s_if_ready!
				if @origin.nil? || @destination.nil? then self
				else to_s end
			end

			def from(type)
				@origin = type.to_s
				to_s_if_ready!
			end

			def to_s
				if @origin.nil? || @destination.nil? then raise ArgumentError.new "Converter needs an origin and destination type: call #from and #to." end
				Conversions.fetch(@origin).fetch(@destination) + "(#{@name})"
			rescue KeyError
				raise NotImplementedError.new "No conversion from #{@origin} to #{type}."
			end

			def to(type)
				@destination = type.to_s
				to_s_if_ready!
			end
		end
	end
end