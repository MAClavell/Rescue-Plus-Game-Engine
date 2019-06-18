#include "Material.h"

// Constructor - Set up a material
Material::Material(SimpleVertexShader * vertexShader, SimplePixelShader * pixelShader)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	SetAlpha(1);
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

// Set the alpha of this material
void Material::SetAlpha(float newAlpha)
{
	alpha = newAlpha;
}

// Get the alpha of this material
float Material::GetAlpha()
{
	return alpha;
}
