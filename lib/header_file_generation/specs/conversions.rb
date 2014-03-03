require_relative '../header_file_generation'

describe 'C++ HeaderFiles' do
	before(:all) do
		
		@header_file_vector = HeaderFileGenerator::HeaderFile.new "filename" => "test.json", "class" => "rb_Nil", "wrapped_class" => "Nil", "initialization" => ["InitAbstractModel()"], "dependencies" => ["models/Nil"]
		@header_file_array  = HeaderFileGenerator::HeaderFile.new "filename" => "test.json", "class" => "rb_Nil", "wrapped_class" => "Nil", "initialization" => ["InitAbstractModel<rb_Nil>()"], "cpp_dependencies" => ["extras/models/rb_abstract_model.extras"]
		@header_file_getter = HeaderFileGenerator::HeaderFile.new "filename" => "test.json", "class" => "rb_Nil", "wrapped_class" => "Nil", "initialization" => ["InitAbstractModel<rb_Nil>"]
		@header_file_setter = HeaderFileGenerator::HeaderFile.new "filename" => "test.json", "class" => "rb_Nil", "wrapped_class" => "Nil", "initialization" => ["InitAbstractModel"], "dependencies" => ["<models/Nil>"]
		
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
		@std_vector    = HeaderFileGenerator::HeaderFile::Method::CppClass.new("std::vector<double>", :pointer => true)
		@rb_RealVector = HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealVector")
		@RealVector    = HeaderFileGenerator::HeaderFile::Method::CppClass.new("RealVector")
	end

	describe 'should create & own methods' do
		before(:all) do
			@header_file_getter.define_methods [@getter_definition]
			@header_file_setter.define_methods [@setter_definition]
			@header_file_vector.define_methods [@vector_definition]
			@header_file_array.define_methods  [@array_definition]

			@getter_method  = @header_file_getter.cpp_methods.select {|i| i.method_name == @getter_definition["name"]}.first
			@vector_method  = @header_file_vector.cpp_methods.select {|i| i.method_name == @vector_definition["name"]}.first
			@array_method   = @header_file_array.cpp_methods.select  {|i| i.method_name == @array_definition["name"] }.first
			@setter_method  = @header_file_setter.cpp_methods.select {|i| i.method_name == @setter_definition["name"]}.first
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

		describe 'whose parameters' do
			before(:all) do
				@vector_parameter = @vector_method.parameters.first
				@array_parameter = @array_method.parameters.first
				@getter_parameter = @getter_method.parameters.first
			end

			it 'should exist' do
				@array_parameter.should_not be_nil
				@array_parameter.class.should == HeaderFileGenerator::HeaderFile::Method::Input
			end

			it 'should exist w/. pointers' do
				@vector_parameter.should_not be_nil
				@vector_parameter.class.should == HeaderFileGenerator::HeaderFile::Method::Input
			end

			it 'are empty when none are needed' do
				@getter_parameter.should be_nil
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
					@array_parameter.input_class = @array_parameter.compatible_classes.first
					@array_parameter.to_converted_form.should match /\*.+#{@array_parameter.parameter_name}/
				end

				it 'should provide a converted format for a variable w/. pointer' do
					@vector_parameter.input_class = @array_parameter.compatible_classes.first
					@vector_parameter.to_converted_form.should match /&.+#{@array_parameter.parameter_name}/
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

		describe 'whose output' do
			before(:all) do
				@getter_output = @getter_method.return_methodology
				@setter_output = @setter_method.return_methodology
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
		end

		it 'that can output a C++ function definition' do
			->(){ @getter_method.to_cpp_function_definition }.should_not raise_error
			->(){ @setter_method.to_cpp_function_definition }.should_not raise_error
			->(){ @vector_method.to_cpp_function_definition }.should_not raise_error
			->(){ @array_method.to_cpp_function_definition  }.should_not raise_error
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