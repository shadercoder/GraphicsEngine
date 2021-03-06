/*
 * TextureFactory.cpp
 *
 *  Created on: 31.05.2011
 *      Author: Norbert Nopper
 */

#include <IL/il.h>
#include <IL/ilu.h>

#include "TextureFactory.h"

using namespace std;
using namespace boost;

TextureFactory::TextureFactory() : TextureFactoryBase()
{
	ilInit();
	iluInit();

	ilEnable(IL_CONV_PAL);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
}

TextureFactory::~TextureFactory()
{
}

GLuint TextureFactory::loadImage(const string& filename, string& identifier) const
{
	ILuint imageName = 0;

	string strippedFilename = filename;

	ILint found;

	ilGenImages(1, &imageName);
	ilBindImage(imageName);

	ILboolean success = ilLoadImage((const ILstring)strippedFilename.c_str());

	if (!success)
	{
		found = strippedFilename.find_last_of("/");

		if (found != -1)
		{
			strippedFilename = strippedFilename.substr(found + 1);

			success = ilLoadImage((const ILstring)strippedFilename.c_str());
		}
		else
		{
			found = strippedFilename.find_last_of("\\");

			if (found != -1)
			{
				strippedFilename = strippedFilename.substr(found + 1);

				success = ilLoadImage((const ILstring)strippedFilename.c_str());
			}
		}
	}

	if (success)
	{
		ILinfo imageInfo;

		iluGetImageInfo(&imageInfo);

		if (imageInfo.Format == IL_BGR)
		{
			ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
		}
		else if (imageInfo.Format == IL_BGRA)
		{
			ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		}

		found = strippedFilename.find_last_of(".");

		if (found != -1)
		{
			identifier = strippedFilename.substr(0, found);
		}
		else
		{
			identifier = strippedFilename;
		}

		return imageName;
	}
	else
	{
		ilBindImage(0);
		ilDeleteImages(1, &imageName);

		glusLogPrint(GLUS_LOG_ERROR, "Texture not found %s", strippedFilename.c_str());
	}

	return 0;
}

Texture2DSP TextureFactory::loadTexture2D(const string& filename, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
{
	Texture2DSP texture2D;

	string identifier;

	ILuint imageName = loadImage(filename, identifier);

	if (imageName)
	{
		ILinfo imageInfo;

		iluGetImageInfo(&imageInfo);

		glusLogPrint(GLUS_LOG_DEBUG, "Creating texture: %s", filename.c_str());

		texture2D = Texture2DSP(new Texture2D(identifier, gatherInternalFormat(imageInfo.Format, imageInfo.Type), imageInfo.Width, imageInfo.Height, imageInfo.Format, imageInfo.Type, imageInfo.Data, imageInfo.SizeOfData, mipMap, minFilter, magFilter, wrapS, wrapT, anisotropic));

		ilBindImage(0);
		ilDeleteImages(1, &imageName);
	}

	return texture2D;
}

TextureCubeMapSP TextureFactory::loadTextureCubeMap(const string& identifier, const string& posX, const string& negX, const string& posY, const string& negY, const string& posZ, const string& negZ, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
{
	TextureCubeMapSP textureCubeMap;

	ILuint imageName[6];
	ILinfo imageInfo[6];

	string dummy;

	for (GLenum cubeMapSide = GL_TEXTURE_CUBE_MAP_POSITIVE_X; cubeMapSide <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; cubeMapSide++)
	{
		string filename;

		switch (cubeMapSide)
		{
			case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
				filename = posX;
			break;
			case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
				filename = negX;
			break;
			case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
				filename = posY;
			break;
			case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
				filename = negY;
			break;
			case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
				filename = posZ;
			break;
			case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
				filename = negZ;
			break;
		}

		imageName[cubeMapSide - GL_TEXTURE_CUBE_MAP_POSITIVE_X] = loadImage(filename, dummy);

		if (imageName[cubeMapSide - GL_TEXTURE_CUBE_MAP_POSITIVE_X])
		{
			iluGetImageInfo(&imageInfo[cubeMapSide - GL_TEXTURE_CUBE_MAP_POSITIVE_X]);

			if (cubeMapSide > GL_TEXTURE_CUBE_MAP_POSITIVE_X)
			{
				if (imageInfo[cubeMapSide - GL_TEXTURE_CUBE_MAP_POSITIVE_X].SizeOfData != imageInfo[cubeMapSide - 1 - GL_TEXTURE_CUBE_MAP_POSITIVE_X].SizeOfData)
				{
					ilBindImage(0);
					ilDeleteImages(cubeMapSide - GL_TEXTURE_CUBE_MAP_POSITIVE_X, imageName);

					return textureCubeMap;
				}
			}
		}
		else
		{
			if (cubeMapSide > GL_TEXTURE_CUBE_MAP_POSITIVE_X)
			{
				ilBindImage(0);
				ilDeleteImages(cubeMapSide - GL_TEXTURE_CUBE_MAP_POSITIVE_X, imageName);
			}

			return textureCubeMap;
		}
	}

	glusLogPrint(GLUS_LOG_DEBUG, "Creating cube texture: %s", posX.c_str());

	textureCubeMap = TextureCubeMapSP(new TextureCubeMap(identifier, gatherInternalFormat(imageInfo[0].Format, imageInfo[0].Type), imageInfo[0].Width, imageInfo[0].Height, imageInfo[0].Format, imageInfo[0].Type, imageInfo[0].Data, imageInfo[1].Data, imageInfo[2].Data, imageInfo[3].Data, imageInfo[4].Data, imageInfo[5].Data, imageInfo[0].SizeOfData, mipMap, minFilter, magFilter, wrapS, wrapT, anisotropic));

	ilBindImage(0);
	ilDeleteImages(6, imageName);

	return textureCubeMap;
}

TextureCubeMapSP TextureFactory::loadTextureCubeMap(const string& filename, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
{
	TextureCubeMapSP textureCubeMap;

	ILinfo imageInfo[6];

	string identifier;

	ILuint imageName = loadImage(filename, identifier);

	if (imageName)
	{
		GLint activeFace = 0;

		for (GLenum cubeMapSide = GL_TEXTURE_CUBE_MAP_POSITIVE_X; cubeMapSide <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; cubeMapSide++)
		{
			ilBindImage(imageName);

			ilActiveFace(activeFace);

			// This avoids modifying the faces on the y-axes
			if (activeFace < 2 || activeFace > 3)
			{
				iluMirror();
				iluFlipImage();
			}

			iluGetImageInfo(&imageInfo[cubeMapSide - GL_TEXTURE_CUBE_MAP_POSITIVE_X]);

			if (cubeMapSide > GL_TEXTURE_CUBE_MAP_POSITIVE_X)
			{
				if (imageInfo[cubeMapSide - GL_TEXTURE_CUBE_MAP_POSITIVE_X].SizeOfData != imageInfo[cubeMapSide - 1 - GL_TEXTURE_CUBE_MAP_POSITIVE_X].SizeOfData)
				{
					ilBindImage(0);
					ilDeleteImages(1, &imageName);

					return textureCubeMap;
				}
			}

			activeFace++;

			// This switches the faces for the z-axes
			if (activeFace == 4)
			{
				activeFace = 5;
			}
			else if (activeFace == 5)
			{
				activeFace = 6;
			}
			else if (activeFace == 6)
			{
				activeFace = 4;
			}
		}

		glusLogPrint(GLUS_LOG_DEBUG, "Creating cube texture: %s", filename.c_str());

		textureCubeMap = TextureCubeMapSP(new TextureCubeMap(identifier, gatherInternalFormat(imageInfo[0].Format, imageInfo[0].Type), imageInfo[0].Width, imageInfo[0].Height, imageInfo[0].Format, imageInfo[0].Type, imageInfo[0].Data, imageInfo[1].Data, imageInfo[2].Data, imageInfo[3].Data, imageInfo[4].Data, imageInfo[5].Data, imageInfo[0].SizeOfData, mipMap, minFilter, magFilter, wrapS, wrapT, anisotropic));

		ilBindImage(0);
		ilDeleteImages(1, &imageName);
	}

	return textureCubeMap;
}

//
// Saving
//

bool TextureFactory::saveImage(const string& identifier, const PixelData& pixelData) const
{
	ILuint imageName = 0;
	ILboolean result;
	ILubyte numChannels = 3;

	if (pixelData.getFormat() == GL_RGBA)
	{
		numChannels = 4;
	}

	// If HDR format, force to three channels.
	if (pixelData.getType() == GL_FLOAT || pixelData.getType() == GL_HALF_FLOAT)
	{
		numChannels = 3;
	}

	ilGenImages(1, &imageName);
	ilBindImage(imageName);

	// Format and type should match OpenGL
	ilTexImage(pixelData.getWidth(), pixelData.getHeight(), 0, numChannels, pixelData.getFormat(), pixelData.getType(), pixelData.getPixels());

	ilEnable(IL_FILE_OVERWRITE);

	if (pixelData.getType() == GL_FLOAT || pixelData.getType() == GL_HALF_FLOAT)
	{
		glusLogPrint(GLUS_LOG_DEBUG, "Saving HDR texture: %s", identifier.c_str());

		result = ilSave(IL_HDR, (identifier + ".hdr").c_str());
	}
	else
	{
		glusLogPrint(GLUS_LOG_DEBUG, "Saving TGA texture: %s", identifier.c_str());

		result = ilSave(IL_TGA, (identifier + ".tga").c_str());
	}

	ilBindImage(0);
	ilDeleteImages(1, &imageName);

	return static_cast<bool>(result);
}
