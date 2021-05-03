/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <array>

#include <ox/std/assert.hpp>
#include <ox/std/bstring.hpp>
#include <ox/std/trace.hpp>

#include "glutils.hpp"

namespace nostalgia::core::renderer {

void deleteBuffer(GLuint b) noexcept {
	glDeleteBuffers(1, &b);
}

void deleteTexture(GLuint t) noexcept {
	glDeleteTextures(1, &t);
}

void deleteVertexArray(GLuint v) noexcept {
	glDeleteVertexArrays(1, &v);
}

template struct GLobject<deleteBuffer>;
template struct GLobject<deleteTexture, TextureBase>;
template struct GLobject<deleteVertexArray>;
template struct GLobject<glDeleteProgram>;
template struct GLobject<glDeleteShader>;

[[nodiscard]]
static ox::Result<Shader> buildShader(GLuint shaderType, const GLchar *src, const char *shaderName) noexcept {
	Shader shader(glCreateShader(shaderType));
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		ox::BString<1000> errMsg;
		glGetShaderInfoLog(shader, errMsg.cap(), nullptr, errMsg.data());
		oxErrorf("shader compile error in {}: {}", shaderName, errMsg);
		return OxError(1, "shader compile error");
	}
	return ox::move(shader);
}

[[nodiscard]]
ox::Result<Program> buildShaderProgram(const GLchar *vert, const GLchar *frag) noexcept {
	oxRequire(vs, buildShader(GL_VERTEX_SHADER, vert, "vshad"));
	oxRequire(fs, buildShader(GL_FRAGMENT_SHADER, frag, "fshad"));
	Program prgm(glCreateProgram());
	glAttachShader(prgm, vs);
	glAttachShader(prgm, fs);
	glLinkProgram(prgm);
	return ox::move(prgm);
}

}
