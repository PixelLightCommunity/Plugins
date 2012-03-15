//[-------------------------------------------------------]
//[ Define helper macro                                   ]
//[-------------------------------------------------------]
#define STRINGIFY(ME) #ME


//[-------------------------------------------------------]
//[ Vertex shader source code                             ]
//[-------------------------------------------------------]
// GLSL (OpenGL 2.0 ("#version 110") and OpenGL ES 2.0 ("#version 100")) vertex shader source code, "#version" is added by hand
static const PLCore::String sVertexShaderSourceCodeGLSL = STRINGIFY(
// Attributes
attribute highp vec3 VertexPosition;	// Object space vertex position input
attribute lowp  vec2 VertexTexCoord;	// Vertex texture coordinate input
varying   lowp  vec2 VertexTexCoordVS;	// Vertex texture coordinate output

// Uniforms
uniform highp mat4 ObjectSpaceToClipSpaceMatrix;	// Object space to clip space matrix

// Programs
void main()
{
	// Calculate the clip space vertex position, lower/left is (-1,-1) and upper/right is (1,1)
	gl_Position = ObjectSpaceToClipSpaceMatrix*vec4(VertexPosition, 1);

	// In case you want to have a fullscreen quad in here, replace the line above by
//	gl_Position = vec4(VertexPosition, 1);

	// Pass through the vertex texture coordinate
	VertexTexCoordVS = VertexTexCoord;
}
);	// STRINGIFY


//[-------------------------------------------------------]
//[ Fragment shader source code                           ]
//[-------------------------------------------------------]
// GLSL (OpenGL 2.0 ("#version 110") and OpenGL ES 2.0 ("#version 100")) fragment shader source code, "#version" is added by hand
static const PLCore::String sFragmentShaderSourceCodeGLSL = STRINGIFY(
// Attributes
varying lowp vec2 VertexTexCoordVS;	// Interpolated vertex texture coordinate input from vertex shader

// Uniforms
uniform lowp sampler2D TextureMap;	// Texture map

// Programs
void main()
{
	// Fragment color = fetched interpolated texel color
	//gl_FragColor = texture2D(TextureMap, VertexTexCoordVS);
	gl_FragColor = vec4(texture2D(TextureMap, VertexTexCoordVS).b, texture2D(TextureMap, VertexTexCoordVS).g, texture2D(TextureMap, VertexTexCoordVS).r, texture2D(TextureMap, VertexTexCoordVS).a);
	// i know it says BGRA it seems to be working only this way
}
);	// STRINGIFY


//[-------------------------------------------------------]
//[ Undefine helper macro                                 ]
//[-------------------------------------------------------]
#undef STRINGIFY