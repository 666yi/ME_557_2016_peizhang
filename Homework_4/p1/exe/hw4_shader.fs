#version 410 core                                                 
                                                                  
in vec4 pass_Color; 
in vec2 pass_texcoord;                                              
out vec4 color; 
uniform sampler2D tiger;                                                  
void main(void)                                                   
{                                                                 
//color = texture(tiger, pass_texcoord);    
		color = 0.2*pass_Color+0.8*texture(tiger, pass_texcoord); 
} 