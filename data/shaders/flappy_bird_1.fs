#version 410 core                                                 

in vec2 pass_texcoord; 
in vec4 pass_Color;
out vec4 color;   

uniform sampler2D treeBMP;


void main(void)                                                   
{   

	color = pass_Color;                            

}                           