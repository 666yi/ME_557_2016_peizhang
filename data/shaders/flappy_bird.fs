#version 410 core                                                 

in vec2 pass_texcoord; 
in vec4 pass_Color;
out vec4 color;   

uniform sampler2D birdBMP;

//uniform int pass_texture_num;
//uniform sampler2D treeBMP;

void main(void)                                                   
{   

	color = 0.3*pass_Color+0.7*texture(birdBMP, pass_texcoord);                            
//	if(pass_texture_num==0){	
//		color = 0.3*pass_Color+0.7*texture(birdBMP, pass_texcoord);
//	}
//	else if(pass_texture_num==1) {
//		color = 0.3*pass_Color+0.7*texture(treeBMP, pass_texcoord);
//	}
}                           