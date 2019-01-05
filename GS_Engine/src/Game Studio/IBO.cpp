#include "IBO.h"

#include "glad.h"

#include "GL.h"

IBO::IBO(const void * Data, unsigned int Count)
{
	GS_GL_CALL(glGenBuffers(1, & RendererObjectId));
	Bind();
	GS_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, Count * sizeof(unsigned int), Data, GL_STATIC_DRAW));

	//IndexArray = Indices;

	IndexCount = Count;
}

IBO::~IBO()
{
	GS_GL_CALL(glDeleteBuffers(1, & RendererObjectId));
}

void IBO::Bind() const
{
	GS_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererObjectId));
}
