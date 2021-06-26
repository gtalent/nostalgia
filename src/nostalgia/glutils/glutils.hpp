/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/defines.hpp>

#define GL_GLEXT_PROTOTYPES 1
#ifdef OX_OS_Darwin
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GLES3/gl3.h>
#endif

#include <ox/std/error.hpp>

namespace nostalgia::glutils {

struct Empty {};

struct TextureBase {

	GLsizei width = 0;
	GLsizei height = 0;

	constexpr TextureBase() noexcept = default;

	constexpr TextureBase(TextureBase &&tb) noexcept {
		width = tb.width;
		height = tb.height;
		tb.width = 0;
		tb.height = 0;
	}

	constexpr TextureBase &operator=(TextureBase &&tb) noexcept {
		width = tb.width;
		height = tb.height;
		tb.width = 0;
		tb.height = 0;
		return *this;
	}

};


template<auto del, typename Base = Empty>
struct GLObject: public Base {

	GLuint id = 0;

	constexpr GLObject() noexcept = default;

	explicit constexpr GLObject(GLuint id) noexcept {
		this->id = id;
	}

	constexpr GLObject(GLObject &&o) noexcept: Base(ox::move(o)) {
		id = o.id;
		o.id = 0;
	}

	~GLObject() {
		del(id);
	}

	GLObject &operator=(GLObject &&o) noexcept {
		if (this != &o) {
			del(id);
			Base::operator=(ox::move(o));
			id = o.id;
			o.id = 0;
		}
		return *this;
	}

	constexpr GLuint release() noexcept {
		auto out = id;
		id = 0;
		return out;
	}

	constexpr operator GLuint&() noexcept {
		return id;
	}

	constexpr operator const GLuint&() const noexcept {
		return id;
	}

};

void deleteBuffer(GLuint b) noexcept;
void deleteTexture(GLuint t) noexcept;
void deleteVertexArray(GLuint v) noexcept;

extern template struct GLObject<deleteBuffer>;
extern template struct GLObject<deleteTexture, TextureBase>;
extern template struct GLObject<deleteVertexArray>;
extern template struct GLObject<glDeleteProgram>;
extern template struct GLObject<glDeleteShader>;

using GLBuffer = GLObject<deleteBuffer>;
using GLShader = GLObject<glDeleteShader>;
using GLProgram = GLObject<glDeleteProgram>;
using GLTexture = GLObject<deleteTexture, TextureBase>;
using GLVertexArray = GLObject<deleteVertexArray>;

[[nodiscard]]
ox::Result<GLProgram> buildShaderProgram(const GLchar *vert, const GLchar *frag) noexcept;

}
