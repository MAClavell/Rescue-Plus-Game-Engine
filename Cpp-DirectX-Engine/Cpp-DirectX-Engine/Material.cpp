#include "Material.h"

// Constructor - Set up a material
Material::Material(SimpleVertexShader * vertexShader, SimplePixelShader * pixelShader)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	specularity = 0;
}

// Release all data in the material
Material::~Material()
{ }

// Get this materials vertex shas=der
SimpleVertexShader* Material::GetVertexShader()
{
	return vertexShader;
}

// Get this materials pixel shader
SimplePixelShader* Material::GetPixelShader()
{
	return pixelShader;
}

// Set this materials surface color
void Material::SetSurfaceColor(DirectX::XMFLOAT4 color)
{
	surfaceColor = color;
}

// Get this materials surface color
DirectX::XMFLOAT4 Material::GetSurfaceColor()
{
	return surfaceColor;
}

void Material::SetSpecularity(float spec)
{
	specularity = spec;
}

float Material::GetSpecularity()
{
	return specularity;
}
