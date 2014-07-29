require 'rubygems'
require 'rubygems/package_task'
require 'rake/clean'

CLEAN.include("pkg", "doc", "Makefile", "*.so", "*.o")

spec = Gem::Specification.new do |s|
  s.name		= "rcaps"
  s.version		= "0.9.20"
  s.author		= "Ben Walton"
  s.email		= "bdwalton@gmail.com"
  s.homepage		= "http://rcaps.rubyforge.org/"
  s.rubyforge_project	= "http://rubyforge.org/projects/rcaps"
  s.summary    		= "A library for manipulating capabilities using the POSIX 1003.1e interfaces"
  s.date		= '2008-10-09'
  s.extensions		= ['extconf.rb']
  s.files		= %w{COPYING gpl.txt rcaps.c rcaps.h}
  s.require_path	= 'lib'
  s.has_rdoc		= true
  s.rdoc_options	= %(-x test/)
  s.test_file		= 'test/all_tests.rb'
end

task :default => [:package]

Gem::PackageTask.new(spec) do |pkg|
    pkg.need_tar = true
end

