# Developer Handbook

## About

The purpose of the Developer Handbook is similar to that of the README. The
README should be viewed as a prerequisite to the Developer Handbook. The README
should provide information needed to build the project, which might be used by
an advanced user or a person trying to build and package the project. The
Developer Handbook should focus on information needed by a developer working on
the project.

## Project Structure

### Overview

All components have a platform indicator next to them:

    (PG) - PC, GBA
    (-G) - GBA
    (P-) - PC

* Nostalgia
  * core - platform abstraction and user I/O (PG)
    * gba - GBA implementation (-G)
    * sdl - SDL2 implementation (P-)
    * qt - Qt implementation, mostly for studio support (P-)
    * userland - common things needed by all non-bare-metal implementations (P-)
    * studio - studio plugin for core (P-)
  * player - plays the games (PG)
  * studio - makes the games (P-)
  * tools - command line tools (P-)
    * pack - packs a studio project directory into an OxFS file (P-)
  * world - defines processes map data (PG)
    * studio - studio plugin for world (P-)
* deps - project dependencies
  * Ox - Library of things useful for portable bare metal and userland code. Not really that external...
    * clargs - Command Line Args processing (P-)
    * fs - file system (PG)
    * mc - Metal Claw serialization, builds on model (PG)
    * model - Data structure modelling (PG)
    * std - Standard-ish Library with a lot missing and some things added (PG)
  * GbaStartup - GBA assembly startup code, mostly pulled from devkitPro under MPL 2.0 (-G)

## Code Base Conventions

### Formatting

* Indentation is done with tabs.
* Alignment is done with spaces.
* Opening brackets go on the same line as the thing they are opening for (if,
  while, for, try, catch, function, etc.)
* No space between function parentheses and arguments.
* Spaces between arithmetic/bitwise/logical/assignment operands and operators.
* Pointer and reference designators should be bound to the identifier name and
  not the type, unless there is not identifier name.

### Write C++, Not C

On the surface, it seems like C++ changes the way we do things from C for no
reason, but there are reasons for many of these duplications of functionality.
The C++ language designers aren't stupid, trust them or question them, but
don't ignore them.

#### Casting

Do not use C-style casts. C++ casts are more readable, and more explicit about
the type of cast being used. Do not use ```dynamic_cast``` in code building for the
GBA, as RTTI is disabled in GBA builds.

#### Library Usage

C++ libraries should generally be preferred to C libraries. C libraries are
allowed, but pay extra attention.

This example from nostalgia::core demonstrates the type of problems that can
arise from idiomatically mixed code.

```cpp
uint8_t *loadRom(const char *path) {
	auto file = fopen(path, "r");
	if (file) {
		fseek(file, 0, SEEK_END);
		const auto size = ftell(file);
		rewind(file);
		// new can technically throw, though this project considers out-of-memory to be unrecoverable
		auto buff = new uint8_t[size];
		fread(buff, size, 1, file);
		fclose(file);
		return buff;
	} else {
		return nullptr;
	}
}
```

In practice, that particular example is not something we really care about
here, but it does demonstrate that problems can arise when mixing what might be
perceived as cool old-school C-style code with lame seemingly over-complicated
C++-style code.

Here is another more concrete example observed in another project:
```cpp
int main() {
	// using malloc does not call the constructor
	std::vector<int> *list = (std::vector<int>*) malloc(sizeof(std::vector<int>));
	doStuff(list);
	// free does not call the destructor, which causes memory leak for array inside list
	free(list);
	return 0;
}
```

The code base where this was observed actually got away with this for the most
part, as the std::vector implementation used evidentally waited until the
internal array was needed before initializing and the memory was zeroed out
because the allocation occurred early in the program's execution. While the
std::vector implementation in queston worked with this code and the memory leak
is not noticable because the std::vector was meant to exist for the entire life
of the process, other classes likely will not get away with it due to more
substantial constructors and more frequent instatiations of the classes in
question.

### Pointers vs References

Pointers are generally preferred to references. References should be used for
optimizing the passing in of parameters and for returning from accessor
operators (e.g. ```T &Vector::operator[](size_t)```). As parameters, references
should always be const.

### Error Handling

Exceptions are clean and nice and gleefully encouraged in userland code running
in environments with expansive system resources, but absolutely unacceptable in
code running in restrictive bare metal environments. The GBA build has them
disabled. Exceptions cause the compiler to generate a great deal of extra code
that inflates the size of the binary. The binary size bloat is often cited as
one of the main reasons why many embedded developers prefer C to C++.

Instead throwing exceptions, all engine code must return error codes. Nostalgia
and Ox both use ```ox::Error``` to report errors. ```ox::Error``` is a struct
that has overloaded operators to behave like an integer error code, plus some
extra fields to enhance debugability. If instantiated through the ```OxError(x)```
macro, it will also include the file and line of the error. The ```OxError(x)```
macro should only be used for the initial instantiation of an ```ox::Error```.

While not strictly necessary, it is a very helpful thing to mark functions
returning ```ox::Error```s as ```[[nodiscard]]```. This will make sure no
errors are accidentally ignored.

In addition to ```ox::Error``` there is also the template ```ox::ValErr<T>```.
```ox::ValErr``` simply wraps the type T value in a struct that also includes
error information, which allows the returning of a value and an error without
resorting to output parameters.

```ox::ValErr``` can be used as follows:

```cpp
[[nodiscard]] ox::ValErr<int> foo(int i) {
	if (i < 10) {
		return i + 1; // implicitly calls ox::ValErr<T>::ValErr(T)
	}
	return OxError(1); // implicitly calls ox::ValErr<T>::ValErr(ox::Error)
}

int caller1() {
	auto v = foo(argc);
	if (v.error) {
		return 1;
	}
	std::cout << v.value << '\n';
	return 0;
}

int caller2() {
	// it is also possible to capture the value and error in their own variables
	auto [val, err] = foo(argc);
	if (err) {
		return 1;
	}
	std::cout << val << '\n';
	return 0;
}
```

Lastly, there are two macros available to help in passing ```ox::Error```s
back up the call stack, ```oxReturnError``` and ```oxThrowError```.

```oxReturnError``` is by far the more helpful of the two. ```oxReturnError```
will return an ```ox::Error``` if it is not 0 and ```oxThrowError``` will throw
an ```ox::Error``` if it is not 0. Because exceptions are disabled for GBA
builds and thus cannot be used in the engine, ```oxThrowError``` is  only really
useful at the boundry between engine libraries and Nostalgia Studio.

```cpp
void studioCode() {
	auto [val, err] = foo(1);
	oxThrowError(err);
	doStuff(val);
}

[[nodiscard]] ox::Error engineCode() {
	auto [val, err] = foo(1);
	oxReturnError(err);
	doStuff(val);
	return OxError(0);
}
```

Both macros will also take the ```ox::ValErr``` directly:

```cpp
void studioCode() {
	auto valerr = foo(1);
	oxThrowError(valerr);
	doStuff(valerr.value);
}

[[nodiscard]] ox::Error engineCode() {
	auto valerr = foo(1);
	oxReturnError(valerr);
	doStuff(valerr.value);
	return OxError(0);
}
```

### File I/O

All engine file I/O should go through nostalgia::core::Context, which should go
through ox::FileSystem. Similarly, all studio file I/O should go throuh
nostalgia::studio::Project, which should go through ox::FileSystem.

ox::FileSystem abstracts away differences between conventional storage devices
and ROM.
