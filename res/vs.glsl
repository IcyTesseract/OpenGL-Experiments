#version 330

uniform mat4 m_proj;
uniform mat4 m_trans;

layout(location=0) in vec3 v_pos;
layout(location=1) in vec3 n_pos;
layout(location=2) in vec2 u_pos;

out vec2 u_pos_vs;

void main(void)
{
	u_pos_vs = u_pos;
    gl_Position = m_proj * m_trans * vec4(v_pos, 1.0);
}
