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

### Pointers vs References

Pointers are generally preferred to references. References should be used for
optimizing the passing in of parameters and for returning from accessor
operators (e.g. ```T &Vector::operator[](size_t)```). As parameters, references
should always be const.

### Casting

Do not use C-style casts. C++ casts are more readable, and more explicit about
the type of cast being used. Do not use ```dynamic_cast``` in code building for the
GBA, as RTTI is disabled in GBA builds.

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

