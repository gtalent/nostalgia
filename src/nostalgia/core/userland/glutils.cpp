/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <array>

#include <ox/std/assert.hpp>
#include <ox/std/trace.hpp>

#include "glutils.hpp"

namespace nostalgia::core::renderer {

[[nodiscard]]
static ox::Result<GLuint> buildShader(GLuint shaderType, const GLchar *src, const char *shaderName) {
	const auto shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		std::array<char, 1000> errMsg;
		glGetShaderInfoLog(shader, errMsg.size(), nullptr, errMsg.data());
		oxErrorf("shader compile error in {}: {}", shaderName, errMsg.data());
		glDeleteShader(shader);
		return OxError(1, "shader compile error");
	}
	return shader;
}

[[nodiscard]]
ox::Result<GLuint> buildShaderProgram(const GLchar *vert, const GLchar *frag) {
	ox::Result<GLuint> out;
	oxRequire(vs, buildShader(GL_VERTEX_SHADER, vert, "vshad"));
	auto [fs, fserr] = buildShader(GL_FRAGMENT_SHADER, frag, "fshad");
	if (!fserr) {
		auto prgm = glCreateProgram();
		glAttachShader(prgm, vs);
		glAttachShader(prgm, fs);
		glLinkProgram(prgm);
		out = prgm;
	} else {
		out = fserr;
	}
	glDeleteShader(fs);
	glDeleteShader(vs);
	return out;
}

void destroy(const Bufferset &bufferset) {
	glDeleteVertexArrays(1, &bufferset.vao);
	glDeleteBuffers(1, &bufferset.ebo);
	glDeleteBuffers(1, &bufferset.vbo);
}

}
