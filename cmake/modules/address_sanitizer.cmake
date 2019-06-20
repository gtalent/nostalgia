# This file belongs Nick Overdijk, and is from https://github.com/NickNick/wubwubcmake
# The MIT License (MIT)
# 
# Copyright (c) 2013 Nick Overdijk
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.option(USE_ASAN "Enable Address Sanitizer, if your compiler supports it" ON)

option(USE_ASAN "Enable Address Sanitizer, if your compiler supports it" OFF)
if(USE_ASAN)
	include(CheckCXXSourceCompiles)
	# If the compiler understands -fsanitize=address, add it to the flags (gcc since 4.8 & clang since version 3.2)
	set(CMAKE_REQUIRED_FLAGS_BAK "${CMAKE_REQUIRED_FLAGS}")
	set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -fsanitize=address")
	CHECK_CXX_SOURCE_COMPILES("int main() { return 0; }" FLAG_FSANA_SUPPORTED)
	set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_BAK}")

	if(FLAG_FSANA_SUPPORTED)
		set(asan_flag "-fsanitize=address")
	else(FLAG_FSANA_SUPPORTED)
		# Alternatively, try if it understands -faddress-sanitizer (clang until version 3.2)
		set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -faddress-sanitizer")
		CHECK_CXX_SOURCE_COMPILES("int main() { return 0; }" FLAG_FASAN_SUPPORTED)
		set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_BAK}")

		if(FLAG_FASAN_SUPPORTED)
			set(asan_flag "-faddress-sanitizer")
		endif(FLAG_FASAN_SUPPORTED)
	endif(FLAG_FSANA_SUPPORTED)

	if(FLAG_FSANA_SUPPORTED OR FLAG_FASAN_SUPPORTED)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${asan_flag}")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${asan_flag}")
		set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${asan_flag}")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${asan_flag}")
	endif()

endif(USE_ASAN)
