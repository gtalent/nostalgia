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
#include <OpenGL/gl.h>
#else
#include <GLES3/gl3.h>
#endif

namespace nostalgia::core::renderer {

struct Bufferset {
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	GLuint tex = 0;
	GLsizei eboElements = 0;
};

[[nodiscard]] GLuint buildShaderProgram(const GLchar *vert, const GLchar *frag);

void destroy(const Bufferset &bufferset);

}
