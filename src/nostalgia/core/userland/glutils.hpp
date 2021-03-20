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

namespace nostalgia::core::renderer {

struct Texture {

	GLuint texId = 0;
	GLsizei width = 0;
	GLsizei height = 0;

	constexpr operator GLuint&() noexcept {
		return texId;
	}

	constexpr operator const GLuint&() const noexcept {
		return texId;
	}

};

struct Bufferset {
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	Texture tex;
	GLsizei eboElements = 0;
};

[[nodiscard]]
ox::Result<GLuint> buildShaderProgram(const GLchar *vert, const GLchar *frag);

void destroy(const Bufferset &bufferset);

}
