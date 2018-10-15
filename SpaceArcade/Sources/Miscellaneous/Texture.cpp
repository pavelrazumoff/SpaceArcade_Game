#include "texture.h"

Texture2D::Texture2D()
	: Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_CLAMP_TO_EDGE),
	Wrap_T(GL_CLAMP_TO_EDGE), Wrap_R(GL_CLAMP_TO_EDGE), Filter_Min(GL_LINEAR_MIPMAP_LINEAR), Filter_Max(GL_LINEAR)
{
}

void Texture2D::GenerateTexture(const GLchar *file, bool gamma)
{
	int width, height, nrComponents;
	unsigned char *data = stbi_load(file, &width, &height, &nrComponents, 0);

	this->Width = width;
	this->Height = height;

	glGenTextures(1, &this->ID);
	glBindTexture(GL_TEXTURE_2D, this->ID);

	if (data)
	{
		if (nrComponents == 1)
		{
			this->Internal_Format = this->Image_Format = GL_RED;
		}
		else if (nrComponents == 3)
		{
			this->Internal_Format = gamma ? GL_SRGB : GL_RGB;
			this->Image_Format = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			this->Internal_Format = gamma ? GL_SRGB_ALPHA : GL_RGBA;
			this->Image_Format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, this->ID);
		glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << file << std::endl;
		stbi_image_free(data);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::GenerateCubemap(std::vector<const GLchar*> faces, bool gamma)
{
	glGenTextures(1, &this->ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->ID);
	int width, height, nrChannels;

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i], &width, &height, &nrChannels, 0);

		if (i == 0)
		{
			this->Width = width;
			this->Height = height;
		}

		if (data)
		{
			if (nrChannels == 1)
			{
				this->Internal_Format = this->Image_Format = GL_RED;
			}
			else if (nrChannels == 3)
			{
				this->Internal_Format = gamma ? GL_SRGB : GL_RGB;
				this->Image_Format = GL_RGB;
			}
			else if (nrChannels == 4)
			{
				this->Internal_Format = gamma ? GL_SRGB_ALPHA : GL_RGBA;
				this->Image_Format = GL_RGBA;
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, this->Wrap_S);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, this->Wrap_T);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, this->Wrap_R);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, this->Filter_Max);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Texture2D::BindTexture() const
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
}

void Texture2D::BindCubemap() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->ID);
}