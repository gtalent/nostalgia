/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#define GL_GLEXT_PROTOTYPES 1
#ifdef OX_OS_Darwin
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GLES3/gl3.h>
#endif

#include <ox/std/error.hpp>
#include <ox/std/trace.hpp>

namespace nostalgia::core::renderer {

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
struct GLobject: public Base {

	GLuint id = 0;

	constexpr GLobject() noexcept = default;

	explicit constexpr GLobject(GLuint id) {
		this->id = id;
	}

	constexpr GLobject(GLobject &&o): Base(ox::move(o)) {
		id = o.id;
		o.id = 0;
	}

	~GLobject() {
		del(id);
	}

	GLobject &operator=(GLobject &&o) {
		this->~GLobject();
		Base::operator=(ox::move(o));
		id = o.id;
		o.id = 0;
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

extern template struct GLobject<deleteBuffer>;
extern template struct GLobject<deleteTexture, TextureBase>;
extern template struct GLobject<deleteVertexArray>;
extern template struct GLobject<glDeleteProgram>;
extern template struct GLobject<glDeleteShader>;

using Buffer = GLobject<deleteBuffer>;
using Shader = GLobject<glDeleteShader>;
using Program = GLobject<glDeleteProgram>;
using Texture = GLobject<deleteTexture, TextureBase>;
using VertexArray = GLobject<deleteVertexArray>;

[[nodiscard]]
ox::Result<Program> buildShaderProgram(const GLchar *vert, const GLchar *frag) noexcept;

}
