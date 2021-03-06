/*
 * Texture2DArrayManager.h
 *
 *  Created on: 06.03.2013
 *      Author: nopper
 */

#ifndef TEXTURE2DARRAYMANAGER_H_
#define TEXTURE2DARRAYMANAGER_H_

#include "../../UsedLibs.h"

#include "Texture2DArray.h"

class Texture2DArrayManager
{
private:

	static Texture2DArrayManager* instance;

	std::map<std::string, Texture2DArraySP> allTextures;

private:

	Texture2DArrayManager();
	virtual ~Texture2DArrayManager();

public:

	static Texture2DArrayManager* getInstance();

	static void terminate();

	void addTexture(const std::string& key, const Texture2DArraySP& texture);

	Texture2DArraySP createTexture(const std::string& key, boost::int32_t width, boost::int32_t height, GLenum format, GLenum type, bool mipMap = true, GLint minFilter = GL_LINEAR_MIPMAP_LINEAR, GLint magFilter = GL_LINEAR, GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT);

	Texture2DArraySP createTexture(const std::string& key, GLint internalFormat, boost::int32_t width, boost::int32_t height, GLenum format, GLenum type, bool mipMap = true, GLint minFilter = GL_LINEAR_MIPMAP_LINEAR, GLint magFilter = GL_LINEAR, GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT);

};

#endif /* TEXTURE2DARRAYMANAGER_H_ */
