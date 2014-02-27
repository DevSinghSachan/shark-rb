# Loads mkmf which is used to make makefiles for Ruby extensions
require 'mkmf'
extension_name = 'rb_shark'
dir_config(extension_name)

have_header("ruby.h")
have_library("shark")
have_library("blas")
have_library("boost_thread-mt")
have_library("boost_filesystem-mt")
have_library("boost_system-mt")
have_library("boost_serialization-mt")
$libs = append_library($libs, "stdc++")

have_type("RealVector", "shark/Data/Dataset.h")

# $srcs = Dir.glob(File.dirname(__FILE__) + "/*.cpp") +  Dir.glob(File.dirname(__FILE__) + "/*/*.cpp")

create_makefile(extension_name)