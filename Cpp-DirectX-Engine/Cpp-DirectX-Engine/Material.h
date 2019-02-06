#pragma once
#include "SimpleShader.h"

// --------------------------------------------------------
// A material definition.
//
// Holds data (shaders) for a material
// --------------------------------------------------------
class Material
{
private:
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

public:
	// --------------------------------------------------------
	// Constructor - Set up a material
	//
	// vertexShader - The vertex shader this material uses
	// pixelShader - The pixel shader this material uses
	// --------------------------------------------------------
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader);
	
	// --------------------------------------------------------
	// Release all data in the material
	// --------------------------------------------------------
	~Material();

	// --------------------------------------------------------
	// Get this materials vertex shas=der
	// --------------------------------------------------------
	SimpleVertexShader* GetVertexShader();

	// --------------------------------------------------------
	// Get this materials pixel shader
	// --------------------------------------------------------
	SimplePixelShader* GetPixelShader();

};

