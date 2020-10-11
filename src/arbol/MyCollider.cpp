#include <MyCollider.h>

MyCollider::MyCollider(char* name, MyNode* parent, float xSize, float ySize, float zSize, float xOffset, float yOffset, float zOffset)
{
    this->name = name;

    this->parent = parent;

    this->xSize = xSize;
    this->ySize = ySize;
    this->zSize = zSize;

    this->xOffset = xOffset;
    this->yOffset = yOffset;
    this->zOffset = zOffset;
}

void MyCollider::draw(mat4 matrix)
{
}