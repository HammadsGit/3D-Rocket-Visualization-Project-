#include "simple_mesh.hpp"
/*
SimpleMeshData concatenate(SimpleMeshData aM, SimpleMeshData const& aN)
{
	aM.positions.insert(aM.positions.end(), aN.positions.begin(), aN.positions.end());
	aM.colors.insert(aM.colors.end(), aN.colors.begin(), aN.colors.end());
	return aM;
}
*/

SimpleMeshData concatenate(SimpleMeshData aM, SimpleMeshData const& aN)
{
	aM.positions.insert(aM.positions.end(), aN.positions.begin(), aN.positions.end());
	aM.colors.insert(aM.colors.end(), aN.colors.begin(), aN.colors.end());
	aM.normals.insert(aM.normals.end(), aN.normals.begin(), aN.normals.end());
	return aM;
}

GLuint create_vao(SimpleMeshData const& aMeshData)
{
	//positions
	GLuint positionVBO = 0;
	glGenBuffers(1, &positionVBO); //generate the buffer pointer and give its value to positionVBO
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO); //we make sure its of type GL_ARRAY_BUFFER which all VBOs are
	glBufferData(GL_ARRAY_BUFFER, aMeshData.positions.size() * sizeof(Vec3f), aMeshData.positions.data(), GL_STATIC_DRAW); // we then add data to it

	//colors
	GLuint colorVBO;
	glGenBuffers(1, &colorVBO); //generate the buffer pointer and give its value to positionVBO
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO); //we make sure its of type GL_ARRAY_BUFFER which all VBOs are
	glBufferData(GL_ARRAY_BUFFER, aMeshData.colors.size() * sizeof(Vec3f), aMeshData.colors.data(), GL_STATIC_DRAW); // we then add data to it

	//texcoords
	GLuint texcoordsVBO = 0;
	glGenBuffers(1, &texcoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.texcoords.size() * sizeof(Vec2f), aMeshData.texcoords.data(), GL_STATIC_DRAW);

	//normals
	GLuint normalVBO = 0;
	glGenBuffers(1, &normalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.normals.size() * sizeof(Vec3f), aMeshData.normals.data(), GL_STATIC_DRAW);
	
	//CREATE VAO
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
	glVertexAttribPointer(
		3, // location = 1 in vertex shader 
		2, GL_FLOAT, GL_FALSE, //floats, not normalized to[0::1](GL FALSE) 
		0, // stride = 0 indicates that there is no padding between inputs 
		nullptr // data starts at offset 0 in the VBO. 
	);
	glEnableVertexAttribArray(3);

	//reset state
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//clean up buffers (these arent fully deleted as the VAO still holds a reference to them;

	glDeleteBuffers(1, &positionVBO);
	glDeleteBuffers(1, &normalVBO);
	glDeleteBuffers(1, &texcoordsVBO);
	glDeleteBuffers(1, &colorVBO);
	return vao;
}

