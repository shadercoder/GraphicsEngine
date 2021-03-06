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

TextureFactory::TextureFactory() :
		autoInternalFloat(false), autoInternalInteger(true), floatBitsPerPixel(BitsPerPixel16), integerBitsPerPixel(BitsPerPixel8)
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

GLenum TextureFactory::gatherInternalFormat(GLenum format, GLenum type) const
{
	if (type == GL_FLOAT || type == GL_HALF_FLOAT)
	{
		if (autoInternalFloat)
		{
			return format;
		}

		if (format == GL_RGB)
		{
			switch (floatBitsPerPixel)
			{
				case BitsPerPixel8:
					glusLogPrint(GLUS_LOG_WARNING, "No 8 bit floating point internal format. Using automatic format.");
				break;
				case BitsPerPixel16:
					return GL_RGB16F;
				break;
				case BitsPerPixel32:
					return GL_RGB32F;
				break;
			}

			return GL_RGB;
		}
		else if (format == GL_RGBA)
		{
			switch (floatBitsPerPixel)
			{
				case BitsPerPixel8:
					glusLogPrint(GLUS_LOG_WARNING, "No 8 bit floating point internal format. Using automatic format.");
				break;
				case BitsPerPixel16:
					return GL_RGBA16F;
				break;
				case BitsPerPixel32:
					return GL_RGBA32F;
				break;
			}

			return GL_RGBA;
		}

		return format;
	}

	if (autoInternalInteger)
	{
		return format;
	}

	if (format == GL_RGB)
	{
		switch (integerBitsPerPixel)
		{
			case BitsPerPixel8:
				return GL_RGB8;
			break;
			case BitsPerPixel16:
				return GL_RGB16;
			break;
			case BitsPerPixel32:
				if (type == GL_BYTE || type == GL_SHORT || type == GL_INT)
				{
					return GL_RGB32I;
				}
				else
				{
					return GL_RGB32UI;
				}
			break;
		}

		return GL_RGB;
	}
	else if (format == GL_RGBA)
	{
		switch (integerBitsPerPixel)
		{
			case BitsPerPixel8:
				return GL_RGBA8;
			break;
			case BitsPerPixel16:
				return GL_RGBA16;
			break;
			case BitsPerPixel32:
				if (type == GL_BYTE || type == GL_SHORT || type == GL_INT)
				{
					return GL_RGBA32I;
				}
				else
				{
					return GL_RGBA32UI;
				}
			break;
		}

		return GL_RGBA;
	}

	return format;
}

Texture1DSP TextureFactory::createTexture1D(const string& identifier, int32_t width, GLenum format, GLenum type, const uint8_t* pixels, uint32_t sizeOfData, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
{
	return Texture1DSP(new Texture1D(identifier, gatherInternalFormat(format, type), width, format, type, pixels, sizeOfData, mipMap, minFilter, magFilter, wrapS, wrapT, anisotropic));
}

Texture1DSP TextureFactory::createTexture1D(const string& identifier, GLint internalFormat, int32_t width, GLenum format, GLenum type, const uint8_t* pixels, uint32_t sizeOfData, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
{
	return Texture1DSP(new Texture1D(identifier, internalFormat, width, format, type, pixels, sizeOfData, mipMap, minFilter, magFilter, wrapS, wrapT, anisotropic));
}

Texture1DArraySP TextureFactory::createTexture1DArray(const string& identifier, int32_t width, GLenum format, GLenum type, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
{
	return Texture1DArraySP(new Texture1DArray(identifier, gatherInternalFormat(format, type), width, format, type, mipMap, minFilter, magFilter, wrapS, wrapT, anisotropic));
}

Texture1DArraySP TextureFactory::createTexture1DArray(const string& identifier, int internalFormat, int32_t width, GLenum format, GLenum type, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
{
	return Texture1DArraySP(new Texture1DArray(identifier, internalFormat, width, format, type, mipMap, minFilter, magFilter, wrapS, wrapT, anisotropic));
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

Texture2DSP TextureFactory::createTexture2D(const string& identifier, int32_t width, int32_t height, GLenum format, GLenum type, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
{
	return Texture2DSP(new Texture2D(identifier, gatherInternalFormat(format, type), width, height, format, type, nullptr, 0, mipMap, minFilter, magFilter, wrapS, wrapT, anisotropic));
}

Texture2DSP TextureFactory::createTexture2D(const string& identifier, GLint internalFormat, int32_t width, int32_t height, GLenum format, GLenum type, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
{
	return Texture2DSP(new Texture2D(identifier, internalFormat, width, height, format, type, nullptr, 0, mipMap, minFilter, magFilter, wrapS, wrapT, anisotropic));
}

Texture2DArraySP TextureFactory::createTexture2DArray(const string& identifier, int32_t width, int32_t height, GLenum format, GLenum type, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
{
	return Texture2DArraySP(new Texture2DArray(identifier, gatherInternalFormat(format, type), width, height, format, type, mipMap, minFilter, magFilter, wrapS, wrapT, anisotropic));
}

Texture2DArraySP TextureFactory::createTexture2DArray(const string& identifier, GLint internalFormat, int32_t width, int32_t height, GLenum format, GLenum type, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
{
	return Texture2DArraySP(new Texture2DArray(identifier, internalFormat, width, height, format, type, mipMap, minFilter, magFilter, wrapS, wrapT, anisotropic));
}

Texture2DMultisampleSP TextureFactory::createTexture2DMultisample(const string& identifier, int32_t samples, GLint internalFormat, int32_t width, int32_t height, bool fixedsamplelocations) const
{
	return Texture2DMultisampleSP(new Texture2DMultisample(identifier, samples, internalFormat, width, height, fixedsamplelocations));
}

TextureCubeMapSP TextureFactory::createTextureCubeMap(const string& identifier, const string& posX, const string& negX, const string& posY, const string& negY, const string& posZ, const string& negZ, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
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

TextureCubeMapSP TextureFactory::createTextureCubeMap(const string& identifier, int32_t width, int32_t height, GLenum format, GLenum type, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
{
	return TextureCubeMapSP(new TextureCubeMap(identifier, gatherInternalFormat(format, type), width, height, format, type, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 0, mipMap, minFilter, magFilter, wrapS, wrapT, anisotropic));
}

TextureCubeMapSP TextureFactory::createTextureCubeMap(const string& identifier, GLint internalFormat, int32_t width, int32_t height, GLenum format, GLenum type, bool mipMap, GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT, float anisotropic) const
{
	return TextureCubeMapSP(new TextureCubeMap(identifier, internalFormat, width, height, format, type, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 0, mipMap, minFilter, magFilter, wrapS, wrapT, anisotropic));
}

bool TextureFactory::isAutoInternalFloat() const
{
	return autoInternalFloat;
}

void TextureFactory::setAutoInternalFloat(bool autoInternalFloat)
{
	this->autoInternalFloat = autoInternalFloat;
}

bool TextureFactory::isAutoInternalInteger() const
{
	return autoInternalInteger;
}

void TextureFactory::setAutoInternalInteger(bool autoInternalInteger)
{
	this->autoInternalInteger = autoInternalInteger;
}

enum FormatDepth TextureFactory::getFloatBitsPerPixel() const
{
	return floatBitsPerPixel;
}

void TextureFactory::setFloatBitsPerPixel(enum FormatDepth floatBitsPerPixel)
{
	this->floatBitsPerPixel = floatBitsPerPixel;
}

enum FormatDepth TextureFactory::getIntegerBitsPerPixel() const
{
	return integerBitsPerPixel;
}

void TextureFactory::setIntegerBitsPerPixel(enum FormatDepth integerBitsPerPixel)
{
	this->integerBitsPerPixel = integerBitsPerPixel;
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

bool TextureFactory::saveTexture2D(const Texture2DSP texture2D) const
{
	return saveImage(texture2D->getIdentifier(), texture2D->getPixelData());
}

bool TextureFactory::saveTextureCubeMap(const TextureCubeMapSP textureCubeMap) const
{
	string postFix;

	for (int32_t i = 0; i < 6; i++)
	{
		switch (i)
		{
			case 0:
				postFix = "_posX";
			break;
			case 1:
				postFix = "_negX";
			break;
			case 2:
				postFix = "_posY";
			break;
			case 3:
				postFix = "_negY";
			break;
			case 4:
				postFix = "_posZ";
			break;
			case 5:
				postFix = "_negZ";
			break;
		}

		saveImage(textureCubeMap->getIdentifier() + postFix, textureCubeMap->getPixelData(i));
	}

	return true;
}
