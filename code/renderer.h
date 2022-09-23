#ifndef _VOXELENG_RENDERER_
#define _VOXELENG_RENDERER_
#include <ostream>
#include "vertex_array.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "shader.h"


namespace VoxelEng {

	//////////////
	//Functions.//
	//////////////

	/*
	Clear all previous thrown OpenGL errors prior to this function's call.
	WARNING. Must be called in a thread with valid OpenGL context.
	*/
	void GLEraseErrors();

	/*
	Debug OpenGL thrown errors. Should be used in conjunction with GLEraseErrors().
	The last three parameters should be used to help locate the error when debugging.
	For example, if you suspect that an OpenGL error is being thrown in file "main.cpp", inside
	the function "fantasticFunction()" at line "4213", then you should pass these information as these
	said parameters when you later put a call to GLCheckErrors() to see if you were right.
	This helps the programmer to identify multiple calls to GLCheckErrors() if you
	have them scattered through the code.
	WARNING. Must be called in a thread with valid OpenGL context.
	*/
	void GLCheckErrors(std::ostream& os, const char* file, const char* function, unsigned int line);


	////////////
	//Classes.//
	////////////

	/*
	Abstraction of the renderer responsible of
	rendering everything in the game.
	*/
	class renderer {

	public:

		// Other methods.

		/*
		Binds to the corresponding OpenGL context of the thread from where this method is being called
		the three parameters passed to it
		and later performs a glDrawElements call using the indexBuffer object.
		WARNING. Must be called in a thread with valid OpenGL context.
		*/
		void draw3D(const indexBuffer& ib) const;

		/*
		Draws 'count' triangles (search for GL_TRIANGLES in the official OpenGL documentation)
		using glDrawArrays() in a 3D coordinate space.
		WARNING. Must be called in a thread with valid OpenGL context.
		*/
		void draw3D(int count) const;

		/*
		Draws 'count' triangles (search for GL_TRIANGLES in the official OpenGL documentation)
		using glDrawArrays() in a 2D coordinate space.
		WARNING. Must be called in a thread with valid OpenGL context.
		*/
		void draw2D(int count) const;

		/*
		Clears the renderer to prepare for the next frame .
		Makes a call to glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT), see the
		official OpenGL documentation for more info.
		WARNING. Must be called in a thread with valid OpenGL context.
		*/
		void clear() const;

	private:



	};

}

#endif