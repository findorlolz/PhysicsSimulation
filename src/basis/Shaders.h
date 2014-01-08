#pragma once

#include <string>
#include "gpu/GLContext.hpp"

typedef FW::GLContext::Program Shader;

Shader* getBasicShader()
{
	auto shader_program = new GLContext::Program(
		"#version 120\n"
		FW_GL_SHADER_SOURCE(
		attribute vec3 positionAttrib;
		uniform mat4 posToClip;		

		void main()
		{
			vec4 pos = vec4(positionAttrib, 1.0);
			gl_Position = pos * posToClip;
		}
		),
		"#version 120\n"
		FW_GL_SHADER_SOURCE(
		uniform vec4 diffuseUniform;
		void main()
		{
			vec4 diffuseColor = diffuseUniform;
			gl_FragColor = diffuseColor;
		}
		)
		);
		return shader_program;
}