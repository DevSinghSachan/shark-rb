require_relative '../header_file_generation'

describe 'C++ HeaderFiles' do
	before(:all) do
		@header_file = HeaderFileGenerator::HeaderFile.new "filename" => "test.json", "wrapped_class" => "rb_Nil"
		@header_file_array = HeaderFileGenerator::HeaderFile.new "filename" => "test.json", "wrapped_class" => "rb_Nil"
		@vector_definition = {
				"name" => "test",
				"types" => ["std::vector<double>*"]
			}
		@regular_definition = {
				"name" => "test",
				"types" => ["array"]
			}
		@std_vector    = HeaderFileGenerator::HeaderFile::Method::CppClass.new("std::vector<double>", :pointer => true)
		@rb_RealVector = HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealVector")
		@RealVector    = HeaderFileGenerator::HeaderFile::Method::CppClass.new("RealVector")
	end

	describe 'should create & own methods' do
		before(:all) do
			@header_file.define_methods [@vector_definition]
			@method = @header_file.cpp_methods.select {|i| i.method_name == @vector_definition["name"]}.first
			@header_file_array.define_methods [@regular_definition]
			@regular_method = @header_file_array.cpp_methods.select {|i| i.method_name == @regular_definition["name"]}.first
		end

		it 'should add methods to method list' do
			@regular_method.should_not be_nil
		end

		it 'should add methods w/. pointers to method list' do
			@method.should_not be_nil
		end

		describe 'whose parameters' do
			before(:all) do
				@parameter = @method.parameters.first
				@parameter_regular = @regular_method.parameters.first
			end	

			it 'should exist' do
				@parameter_regular.should_not be_nil
				@parameter_regular.class.should == HeaderFileGenerator::HeaderFile::Method::Input
			end

			it 'should exist w/. pointers' do
				@parameter.should_not be_nil
				@parameter.class.should == HeaderFileGenerator::HeaderFile::Method::Input
			end

			describe 'have output classes that' do

				it 'should have guessed output classes' do
					@parameter_regular.output_class.should_not be_nil
					@parameter_regular.output_class.should === @RealVector
				end

				it 'should have guessed output classes w/. pointers' do
					@parameter.output_class.should_not be_nil
					@parameter.output_class.should === @std_vector
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
					@parameter_regular.compatible_classes.should_not be_empty
					@parameter_regular.compatible_classes.should include *[
						@rb_RealVector,
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealVectorReference"),
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealMatrixRow"),
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealMatrixColumn")
					]
				end				

				it 'should have some input compatible classes w/. pointers' do
					@parameter.compatible_classes.should_not be_empty
					@parameter.compatible_classes.should include *[
						@rb_RealVector,
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealVectorReference"),
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealMatrixRow"),
						HeaderFileGenerator::HeaderFile::Method::CppClass.new("rb_RealMatrixColumn")
					]
				end

				it 'should provide a converted format for a variable' do
					@parameter_regular.input_class = @parameter_regular.compatible_classes.first
					@parameter_regular.to_converted_form.should match /\*.+#{@parameter_regular.parameter_name}/
				end

				it 'should provide a converted format for a variable w/. pointer' do
					@parameter.input_class = @parameter_regular.compatible_classes.first
					@parameter.to_converted_form.should match /&.+#{@parameter_regular.parameter_name}/
				end
				
			end

		end
	end
end