#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <stb_image.h>

class Texture
{
public: // Methods
	Texture();
	Texture(std::string filename);
	void ReadFromDisk();
	void LoadToGL();

public: // fields
	unsigned int ID = 0;
	std::string name;
	int width, height;

private: // fields
	unsigned char* data;
	int nrChannels;
};

