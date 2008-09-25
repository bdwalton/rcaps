#!/usr/bin/env ruby

require 'mkmf'

dir_config("cap")

if !find_header("sys/capability.h")
  puts "No sys/capability.h found..."
elsif !find_library("cap", "cap_init")
  puts "No libcap found..."
else
  create_makefile("rcaps")
end
