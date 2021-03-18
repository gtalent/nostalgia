/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/assert.hpp>
#include <ox/std/trace.hpp>

#include "glutils.hpp"

namespace nostalgia::core::renderer {

[[nodiscard]]
ox::Result<GLuint> buildShader(GLuint shaderType, const GLchar *src, const char *shaderName) {
	auto shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		static const auto errMsgSize = 1000;
		char errMsg[errMsgSize];
		glGetShaderInfoLog(shader, errMsgSize, nullptr, errMsg);
		oxErrorf("shader compile error in {}: {}", shaderName, errMsg);
		glDeleteShader(shader);
		return OxError(1, "shader compile error");
	}
	return shader;
}

[[nodiscard]]
ox::Result<GLuint> buildVertShader(const GLchar *src, const char *shaderName) {
	return buildShader(GL_VERTEX_SHADER, src, shaderName);
}

[[nodiscard]]
ox::Result<GLuint> buildFragShader(const GLchar *src, const char *shaderName) {
	return buildShader(GL_FRAGMENT_SHADER, src, shaderName);
}

[[nodiscard]]
ox::Result<GLuint> buildShaderProgram(const GLchar *vert, const GLchar *frag) {
	GLuint prgm = 0;
	oxRequire(vs, buildVertShader(vert, "vshad"));
	if (!vs) {
		glDeleteShader(vs);
	} else {
		oxRequire(fs, buildFragShader(frag, "fshad"));
		if (!fs) {
			// cleanup shaders that were created
			glDeleteShader(fs);
		} else {
			prgm = glCreateProgram();
			if (prgm) {
				glAttachShader(prgm, vs);
				glAttachShader(prgm, fs);
				glLinkProgram(prgm);
			}
		}
	}
	return prgm;
}

void destroy(const Bufferset &bufferset) {
	glDeleteVertexArrays(1, &bufferset.vao);
	glDeleteBuffers(1, &bufferset.ebo);
	glDeleteBuffers(1, &bufferset.vbo);
}

}
