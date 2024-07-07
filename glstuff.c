PFNGLCREATESHADERPROGRAMVPROC glCreateShaderProgramv;
PFNGLGENPROGRAMPIPELINESPROC glGenProgramPipelines;
PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLPROGRAMUNIFORM1IVPROC glProgramUniform1iv;
PFNGLUSEPROGRAMSTAGESPROC glUseProgramStages;

void glstuff_get_procs()
{
	glCreateShaderProgramv = (void*) wglGetProcAddress("glCreateShaderProgramv");
	glGenProgramPipelines = (void*) wglGetProcAddress("glGenProgramPipelines");
	glBindProgramPipeline = (void*) wglGetProcAddress("glBindProgramPipeline");
	glGetProgramInfoLog = (void*) wglGetProcAddress("glGetProgramInfoLog");
	glProgramUniform1iv = (void*) wglGetProcAddress("glProgramUniform1iv");
	glUseProgramStages = (void*) wglGetProcAddress("glUseProgramStages");
}

char *vertSource =
	"#version 430\n"
	"layout (location=0) in vec2 i;"
	"out vec2 p;"
	"out gl_PerVertex"
	"{"
	"vec4 gl_Position;"
	"};"
	"void main() {"
	"gl_Position=vec4(p=i,0.,1.);"
	"}"
;
