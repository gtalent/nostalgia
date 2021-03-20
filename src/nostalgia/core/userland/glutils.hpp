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

};


template<auto del, typename Base = Empty>
struct GLobject: public Base {

	GLuint id = 0;

	constexpr GLobject() = default;

	explicit constexpr GLobject(GLuint id) {
		this->id = id;
	}

	constexpr GLobject(GLobject &&o) {
		id = o.id;
		o.id = 0;
	}

	~GLobject() {
		del(id);
	}

	GLobject &operator=(GLobject &&o) {
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

void deleteTexture(GLuint t);

extern template struct GLobject<glDeleteProgram>;
extern template struct GLobject<glDeleteShader>;
extern template struct GLobject<deleteTexture, TextureBase>;
using Shader = GLobject<glDeleteShader>;
using Program = GLobject<glDeleteProgram>;
using Texture = GLobject<deleteTexture, TextureBase>;

struct Bufferset {
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	Texture tex;
	GLsizei eboElements = 0;
};

[[nodiscard]]
ox::Result<Program> buildShaderProgram(const GLchar *vert, const GLchar *frag);

void destroy(const Bufferset &bufferset);

}
