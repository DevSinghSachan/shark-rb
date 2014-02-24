begin
	require 'jeweler'
	Jeweler::Tasks.new do |gemspec|
		gemspec.name     = "shark-rb"
		gemspec.summary  = "Shark C++ library bindings in Ruby."
		gemspec.email    = "jraiman@mit.edu"
		gemspec.homepage = "http://github.com/JonathanRaiman/shark_rb"
		gemspec.authors  = ["Jonathan Raiman"]
		# gemspec.platform = Gem::Platform::CURRENT
		gemspec.description = "Shark C++ library bindings in Ruby implementing RealVector, LabeledData, UnlabeledData, neural nets and much more!"
		gemspec.license = "MIT"
	end
rescue LoadError
	puts "Jeweler not available. Install it with: gem install jeweler"
end



desc 'Generate Header Files'
task :header do
	require './lib/header_file_generation/header_file_generation.rb'
	HeaderFileGenerator.generate_header_files
	`git add -A`
	`git commit -m "new header files"`
	`cd #{File.dirname(__FILE__) + "/ext/"} && ruby extconf.rb && cd ..`
	Rake::Task["gemspec:generate"].reenable
	Rake::Task["gemspec:generate"].invoke
end
