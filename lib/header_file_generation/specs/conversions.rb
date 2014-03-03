require_relative '../header_file_generation'

describe 'C++ HeaderFiles' do
	before(:all) do
		
		@header_file_vector = HeaderFileGenerator::HeaderFile.new "filename" => "test.json", "class" => "rb_Nil", "wrapped_class" => "Nil", "initialization" => ["InitAbstractModel()"], "dependencies" => ["models/Nil"]
		@header_file_array  = HeaderFileGenerator::HeaderFile.new "filename" => "test.json", "class" => "rb_Nil", "wrapped_class" => "Nil", "initialization" => ["InitAbstractModel<rb_Nil>()"], "cpp_dependencies" => ["extras/models/rb_abstract_model.extras"]
		@header_file_getter = HeaderFileGenerator::HeaderFile.new "filename" => "test.json", "class" => "rb_Nil", "wrapped_class" => "Nil", "initialization" => ["InitAbstractModel<rb_Nil>"]
		@header_file_setter = HeaderFileGenerator::HeaderFile.new "filename" => "test.json", "class" => "rb_Nil", "wrapped_class" => "Nil", "initialization" => ["InitAbstractModel"], "dependencies" => ["<models/Nil>"]
		@header_file_overloaded = HeaderFileGenerator::HeaderFile.new "filename" => "test.json", "class" => "rb_Nil", "wrapped_class" => "Nil"
		@header_file_specific = HeaderFileGenerator::HeaderFile.new "filename" => "test.json", "class" => "rb_Nil", "wrapped_class" => "Nil"
		@header_file_specific_two_sized = HeaderFileGenerator::HeaderFile.new "filename" => "test.json", "class" => "rb_Nil", "wrapped_class" => "Nil"
		
		@vector_definition = {
				"name" => "test",
				"types" => ["std::vector<double>*"]
			}
		@array_definition = {
				"name" => "test",
				"types" => ["array"]
			}
		@getter_definition = {
				"name" => "test",
				"type" => "double"
			}
		@setter_definition = {
				"name" => "test",
				"types" => ["double"]
			}
		@overloaded_definition = {
			"name" => "test",
			"types" => [["double", "array"]]
		}
		@specific_definition = {
			"name" => "test",
			"overload" => [
				{
					"types" => ["double"],
					"type" => "double"
				},
				{
					"types" => ["array"],
					"type" => "array"
				}
			]
		}
		@specific_definition_two_sizes = {
			"name" => "test",
			"overload" => [
				{
					"types" => ["double"],
					"type" => "double"
				},
				{
					"types" => ["array", "double"],
					"type" => "array"
				}
			]
		}
		@std_vector    = HeaderFileGenerator::HeaderFile::Method::CppClass.new("std::vector<double>", :pointer => true)
		@rb_RealVector = HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealVector")
		@RealVector    = HeaderFileGenerator::HeaderFile::Method::CppClass.new("RealVector")
	end

	describe 'should create & own methods' do
		before(:all) do
			@header_file_getter.define_methods      [@getter_definition]
			@header_file_setter.define_methods      [@setter_definition]
			@header_file_vector.define_methods      [@vector_definition]
			@header_file_array.define_methods       [@array_definition]
			@header_file_overloaded.define_methods  [@overloaded_definition]
			@header_file_specific.define_methods    [@specific_definition]
			@header_file_specific_two_sized.define_methods [@specific_definition_two_sizes]

			@getter_method     = @header_file_getter.cpp_methods.select        {|i| i.method_name == @getter_definition["name"]}.first
			@vector_method     = @header_file_vector.cpp_methods.select        {|i| i.method_name == @vector_definition["name"]}.first
			@array_method      = @header_file_array.cpp_methods.select         {|i| i.method_name == @array_definition["name"] }.first
			@setter_method     = @header_file_setter.cpp_methods.select        {|i| i.method_name == @setter_definition["name"]}.first
			@specific_method   = @header_file_specific.cpp_methods.select      {|i| i.method_name == @specific_definition["name"]}.first
			@overloaded_method = @header_file_overloaded.cpp_methods.select    {|i| i.method_name == @overloaded_definition["name"]}.first
			@specific_two_sizes = @header_file_specific_two_sized.cpp_methods.select {|i| i.method_name == @specific_definition_two_sizes["name"]}.first
			@methods = [@getter_method, @vector_method, @array_method, @setter_method, @specific_method, @overloaded_method, @specific_two_sizes]
		end

		it 'and add those methods to method list' do
			@array_method.should_not be_nil
		end

		it 'and add those methods w/. pointers to method list' do
			@vector_method.should_not be_nil
		end

		it 'and add those methods with no parameters to method list' do
			@getter_method.should_not be_nil
		end

		it 'and add those methods with overloaded parameters to method list' do
			@overloaded_method.should_not be_nil
		end

		it 'and add those methods with specific overloading parameters to method list' do
			@specific_method.should_not be_nil
			@specific_two_sizes.should_not be_nil
		end

		describe 'whose parameters' do
			before(:all) do
				@vector_parameter     = @vector_method.parameters.first
				@array_parameter      = @array_method.parameters.first
				@getter_parameter     = @getter_method.parameters.first
				@overloaded_parameter = @overloaded_method.parameters.first
				@specific_parameter   = @specific_method.parameters.first
				@specific_two_sizes_parameter = @specific_two_sizes.parameters.first
			end

			it 'should exist' do
				@array_parameter.should_not be_nil
				@array_parameter.class.should == HeaderFileGenerator::HeaderFile::Method::Input
			end

			it 'should exist w/. pointers' do
				@vector_parameter.should_not be_nil
				@vector_parameter.class.should == HeaderFileGenerator::HeaderFile::Method::Input
			end

			it 'should exist w/. overloading' do
				@overloaded_parameter.should_not be_nil
				@overloaded_parameter.class.should == HeaderFileGenerator::HeaderFile::Method::OverloadedInput
			end

			it 'are empty when none are needed' do
				@getter_parameter.should be_nil
				
			end

			it 'are empty when using overloading (families)' do
				@specific_parameter.should be_nil
				@specific_two_sizes_parameter.should be_nil
			end

			describe 'have output classes that' do

				it 'should have guessed output classes' do
					@array_parameter.output_class.should_not be_nil
					@array_parameter.output_class.should === @RealVector
				end

				it 'should have guessed output classes w/. pointers' do
					@vector_parameter.output_class.should_not be_nil
					@vector_parameter.output_class.should === @std_vector
				end

			end

			describe 'have input classes that' do

				it 'should find a conversion to RealVector from Ruby\'s RealVector' do
					HeaderFileGenerator::HeaderFile::Converter.can_convert(@rb_RealVector).to(@RealVector).should be_true
				end

				it 'should find compatible classes for conversion to RealVector' do
					HeaderFileGenerator::HeaderFile::Method::CppClass.can_convert_to(@RealVector).should_not be_empty
				end

				it 'should find a conversion to std::vector<double> from Ruby\'s RealVector' do
					HeaderFileGenerator::HeaderFile::Converter.can_convert(@rb_RealVector).to(@std_vector).should be_true
				end

				it 'should find compatible classes for conversion to std::vector<double>' do
					HeaderFileGenerator::HeaderFile::Method::CppClass.can_convert_to(@std_vector).should_not be_empty
				end

				it 'should have some input compatible classes' do
					@array_parameter.compatible_classes.should_not be_empty
					@array_parameter.compatible_classes.should include *[
						@rb_RealVector,
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealVectorReference"),
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealMatrixRow"),
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealMatrixColumn")
					]
				end				

				it 'should have some input compatible classes w/. pointers' do
					@vector_parameter.compatible_classes.should_not be_empty
					@vector_parameter.compatible_classes.should include *[
						@rb_RealVector,
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealVectorReference"),
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealMatrixRow"),
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealMatrixColumn")
					]
				end

				it 'should provide a converted format for a variable' do
					@array_parameter.input_class = @array_parameter.compatible_classes.select {|i| i.ruby? }.first
					@array_parameter.to_converted_form.should match /\*.+#{@array_parameter.parameter_name}/
				end

				it 'should provide a converted format for a variable w/. pointer' do
					@vector_parameter.input_class = @vector_parameter.compatible_classes.select {|i| i.type != "Array"}.first
					@vector_parameter.to_converted_form.should match /&.+#{@vector_parameter.parameter_name}/
				end

				it 'should find reasonable compatible classes' do
					inp = HeaderFileGenerator::HeaderFile::Method::Input.new(
						type: :array,
						output_class: HeaderFileGenerator::HeaderFile::Method::CppClass.guess_from_type(:array),
						position: 0)
					inp.compatible_classes.should include *[
						@rb_RealVector,
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealVectorReference"),
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealMatrixRow"),
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealMatrixColumn"),
						HeaderFileGenerator::HeaderFile::Method::CppClass::RubyArray
					]
					inp.compatible_classes.should_not include *[
						HeaderFileGenerator::HeaderFile::Method::CppClass::Ruby2DArray,
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealMatrix"),
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealMatrixReference")
					]
				end
				
			end

		end

		describe 'whose parameter families' do

			it 'should correspond to the number of overloadings' do
				@specific_two_sizes.parameter_families.length.should == @specific_definition_two_sizes["overload"].length
				@specific_method.parameter_families.length.should == @specific_definition["overload"].length
			end


			it 'should get a scan argument if there are variable argument amounts' do
				@specific_two_sizes.input_parameters.should match /number_of_arguments/
				@specific_two_sizes.to_cpp_function_definition.should match /rb_scan_args/
			end

			it 'should not get a scan argument if there is a static argument amount' do
				@specific_method.input_parameters.should_not match /number_of_arguments/
				@specific_method.to_cpp_function_definition.should_not match /rb_scan_args/
			end
		end

		describe 'whose output' do
			before(:all) do
				@getter_output = @getter_method.return_methodology @getter_method.parameters
				@setter_output = @setter_method.return_methodology @setter_method.parameters
				@specific_method.observed_family    = 0
				@specific_method_output_1           = @specific_method.return_methodology @specific_method.parameter_families.first
				@specific_method.observed_family    = 1
				@specific_method_output_2           = @specific_method.return_methodology @specific_method.parameter_families.last
				@specific_two_sizes.observed_family = 0
				@specific_method_multisize_output_1 = @specific_two_sizes.return_methodology @specific_two_sizes.parameter_families.first
				@specific_two_sizes.observed_family = 1
				@specific_method_multisize_output_2 = @specific_two_sizes.return_methodology @specific_two_sizes.parameter_families.last
			end

			it 'should not be nil' do
				@getter_output.should_not be_nil
				@setter_output.should_not be_nil
			end

			it 'should be converted to Ruby values' do
				@getter_output.should_not match /return self/
			end

			it 'should return self when no return type is provided' do
				@setter_output.should match /return self/
			end

			it 'should depend on which family is current active' do
				@specific_method_output_1.should_not match           "wrap_pointer<rb_RealVector>"
				@specific_method_multisize_output_1.should_not match "wrap_pointer<rb_RealVector>"
				@specific_method_output_2.should match               "wrap_pointer<rb_RealVector>"
				@specific_method_multisize_output_2.should match     "wrap_pointer<rb_RealVector>"
			end
		end

		it 'that can output a C++ function definition' do
			@methods.each do |method|
				->(){method.to_cpp_function_definition}.should_not raise_error
			end
		end

		it 'that can output a C++ function definition with balanced squiggly brackets' do
			@methods.each do |method|
				str = method.to_cpp_function_definition
				str.count("{").should == str.count("}")
			end
		end
	end

	describe 'should create an initialization function' do
		it 'that should create an initialization function' do
			[
				@header_file_vector,
				@header_file_vector,
				@header_file_getter,
				@header_file_setter
			].each do |header|
				header.generate_init_function.should match /#{header.init_function_name}/
			end
		end

		it 'that should create extra initializations functions' do
			[
				@header_file_vector,
				@header_file_vector,
				@header_file_getter,
				@header_file_setter
			].each do |header|
				header.generate_init_function.should match /#{header.init_functions.first["()"] ? header.init_functions.first : (header.init_functions.first + "()")}/
			end
		end
	end

	describe 'should have dependencies' do
		it 'that are injected to the header file (.h)' do
			@header_file_vector.h_file_dependencies.should match /#{'"'+@header_file_vector.dependencies.first+'"'}/
			@header_file_setter.h_file_dependencies.should match /#{@header_file_setter.dependencies.first}/
			@header_file_getter.h_file_dependencies.should be_empty
			@header_file_array.h_file_dependencies.should be_empty
		end

		it 'that are injected to the C++ file (.cpp)' do
			@header_file_array.cpp_file_dependencies.should match /#{'"'+@header_file_array.cpp_dependencies.first+'"'}/
			[
				@header_file_vector,
				@header_file_vector,
				@header_file_getter,
				@header_file_setter
			].each do |header|
				header.cpp_file_dependencies.should match /#{header.cpp_class.to_s+'.h'}/
				header.cpp_file_dependencies.should match /#{"extras/utils/rb_pointer_wrapping.extras"}/
			end
		end
	end
end