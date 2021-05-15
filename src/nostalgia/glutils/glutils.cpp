/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/bstring.hpp>
#include <ox/std/trace.hpp>

#include "glutils.hpp"

namespace nostalgia::glutils {

void deleteBuffer(GLuint b) noexcept {
	glDeleteBuffers(1, &b);
}

void deleteTexture(GLuint t) noexcept {
	glDeleteTextures(1, &t);
}

void deleteVertexArray(GLuint v) noexcept {
	glDeleteVertexArrays(1, &v);
}

template struct GLObject<deleteBuffer>;
template struct GLObject<deleteTexture, TextureBase>;
template struct GLObject<deleteVertexArray>;
template struct GLObject<glDeleteProgram>;
template struct GLObject<glDeleteShader>;

[[nodiscard]]
static ox::Result<GLShader> buildShader(GLuint shaderType, const GLchar *src, const char *shaderName) noexcept {
	GLShader shader(glCreateShader(shaderType));
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
ox::Result<GLProgram> buildShaderProgram(const GLchar *vert, const GLchar *frag) noexcept {
	oxRequire(vs, buildShader(GL_VERTEX_SHADER, vert, "vshad"));
	oxRequire(fs, buildShader(GL_FRAGMENT_SHADER, frag, "fshad"));
	GLProgram prgm(glCreateProgram());
	glAttachShader(prgm, vs);
	glAttachShader(prgm, fs);
	glLinkProgram(prgm);
	return ox::move(prgm);
}

}
