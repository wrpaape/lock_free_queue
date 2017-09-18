all: init_build
	cmake --build build --target all

test: init_build
	cmake --build build --target test

verbose_test: init_build
	cmake --build build --target verbose_test

build_test: init_build
	cmake --build build --target build_test

verbose_build_test: init_build
	cmake --build build --target verbose_build_test

help: init_build
	cmake --build build --target help

init_build:
	cmake -E make_directory build
	cmake -E chdir build cmake $(CURDIR)

clean:
	cmake -E chdir build make clean
	cmake -E remove_directory build
