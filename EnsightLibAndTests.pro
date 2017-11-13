TEMPLATE = subdirs

CONFIG += ordered
SUBDIRS = lib \
          test \
          read_file \
          write_file \
          interpolate_variable \
          ensight_viewer

lib.file = ensight_lib/EnsightLib.pro

test.file = ensight_lib/test/EnsightLibTest.pro
test.depends = lib

read_file.subdir = ensight_lib/examples/read_file
read_file.depends = lib

write_file.subdir = ensight_lib/examples/write_file
write_file.depends = lib

interpolate_variable.subdir = ensight_lib/examples/interpolate_variable
interpolate_variable.depends = lib

ensight_viewer.subdir = ensight_lib/examples/ensight_viewer
ensight_viewer.depends = lib
