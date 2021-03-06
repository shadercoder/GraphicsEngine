/*
 * Texture2DMultisampleManager.cpp
 *
 *  Created on: 04.02.2013
 *      Author: nopper
 */

#include "TextureFactory.h"

#include "Texture2DMultisampleManager.h"

using namespace std;

using namespace boost;

Texture2DMultisampleManager* Texture2DMultisampleManager::instance;

Texture2DMultisampleManager::Texture2DMultisampleManager() :
	allTextures()
{
}

Texture2DMultisampleManager::~Texture2DMultisampleManager()
{
	auto walker = allTextures.begin();
	while (walker != allTextures.end())
	{
		walker->second.reset();

		walker++;
	}
	allTextures.clear();
}


Texture2DMultisampleManager* Texture2DMultisampleManager::getInstance()
{
	if (!instance)
	{
		instance = new Texture2DMultisampleManager();
	}

	return instance;
}

void Texture2DMultisampleManager::terminate()
{
	if (instance)
	{
		delete instance;
		instance = 0;
	}
}

void Texture2DMultisampleManager::addTexture(const string& key, const Texture2DMultisampleSP& texture)
{
	allTextures[key] = texture;
}

Texture2DMultisampleSP Texture2DMultisampleManager::createTexture(const string& key, int32_t samples, GLint internalFormat, int32_t width, int32_t height, bool fixedsamplelocations)
{
	auto walker = allTextures.find(key);

	TextureFactory textureFactory;

	if (walker == allTextures.end())
	{
		allTextures[key] = textureFactory.createTexture2DMultisample(key, samples, internalFormat, width, height, fixedsamplelocations);

		return allTextures[key];
	}

	return allTextures[key];
}


