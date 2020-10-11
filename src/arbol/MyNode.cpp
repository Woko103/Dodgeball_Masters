
// #include <GLFW/glfw3.h>
#include <MyCollider.h>
#include <math.h>
#include <MyEntity.h>
#include <MyNode.h>

/*METHODS*/

MyNode::MyNode(MyEntity* entity)
{
    this->entity = entity;
    this->parent = nullptr;
    transMatrix = mat4(1.0f);

    position = vec3(0.0f);
    scale = vec3(1.0f);
    rotation = vec3(0.0f);

    isChanged = false;
}

void MyNode::addChild(MyNode* child)
{

    child->parent = this;
    this->children.push_back(child);

}

void MyNode::removeChild(MyNode* child)
{
    for(int i = 0; i < children.size(); ++i)
    {
        if(children.at(i) == child)
        {
            child->parent = nullptr;
            children.erase(children.begin() + i);
        }
    }
}

MyNode* MyNode::getParent()
{
    return this->parent;
}

MyEntity* MyNode::getEntity()
{
    return entity;
}

void MyNode::setPosition(vec3 newPosition)
{
    this->position = newPosition;
    isChanged = true;
}

void MyNode::setRotation(vec3 newRotation)
{
    this->rotation = newRotation;
    isChanged = true;
}

void MyNode::setScale(vec3 newScale)
{
    this->scale = newScale;
    isChanged = true;
}

glm::vec3 MyNode::getPosition()
{
    return this->position;
}

glm::vec3 MyNode::getRotation()
{
    return this->rotation;
}

glm::vec3 MyNode::getScale()
{
    return this->scale;
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%% BASIC TRANSFORMATIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

mat4 MyNode::translate(vec3 newPosition)
{
    return glm::translate(mat4(1.0f), newPosition);
}

mat4 MyNode::rotate(vec3 newRotation)
{
    newRotation.x = (newRotation.x/180.0f) * M_PI;
    newRotation.y = (newRotation.y/180.0f) * M_PI;
    newRotation.z = (newRotation.z/180.0f) * M_PI;

    return glm::rotate(mat4(1.0f), newRotation.z, glm::vec3(0, 0, 1))
           * glm::rotate(mat4(1.0f), newRotation.y, glm::vec3(0, 1, 0))
           * glm::rotate(mat4(1.0f), newRotation.x, glm::vec3(1, 0, 0));
}

mat4 MyNode::scalate(vec3 newScale)
{
    return glm::scale(mat4(1.0f), newScale);
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%% TRANSFORM MATRIX FUNCTIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void MyNode::setTransfMatrix(mat4 newMatrix)
{
    this->transMatrix = newMatrix;
}

glm::mat4 MyNode::getTransfMatrix()
{
    return this->transMatrix;
}

void MyNode::travel(mat4 acumMatrix)
{
    if (isChanged)
    {
        this->transMatrix = acumMatrix * calculateMatrix();

        for(int i = 0; i < children.size(); i++)
        {
            children.at(i)->isChanged = true;
        }

        isChanged = false;
    }

    if(entity)
        this->entity->draw(transMatrix);

    for(int i = 0; i < children.size(); i++)
    {
        children.at(i)->travel(transMatrix);
    }
}

mat4 MyNode::calculateMatrix()
{
    mat4 auxMatrix = translate(position) * rotate(rotation) *  scalate(scale);

    return auxMatrix;
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%% COLLISION METHODS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
void MyNode::addCubeCollider(char* name, MyNode* parent, float xSize, float ySize, float zSize, float xOffset, float yOffset, float zOffset)
{
    MyCollider* collider = new MyCollider(name, parent, xSize, ySize, zSize, xOffset, yOffset, zOffset);

    colliders.push_back(collider);
}

bool MyNode::checkCollisions(MyNode*  otherNode)
{
    if(!otherNode) {return false;}

    if(otherNode == this) {return false;}

    // Comprobamos si los nodos tienen colliders
    if(otherNode->colliders.size() == 0 || this->colliders.size() == 0)
    {
        return false;
    }

    for(int i=0; i<this->colliders.size(); i++)
    {
        for(int j=0; j<otherNode->colliders.size(); j++)
        {
            // Comprobamos colisiones en eje X
            float xNodeLimitOne = this->colliders.at(i)->getXOffset() + this->colliders.at(i)->getXSize()/2.0 + this->position.x;
            float xNodeLimitTwo = this->colliders.at(i)->getXOffset() - this->colliders.at(i)->getXSize()/2.0 + this->position.x;

            float xOtherLimitOne = otherNode->colliders.at(j)->getXOffset() + otherNode->colliders.at(j)->getXSize()/2.0 + otherNode->position.x;
            float xOtherLimitTwo = otherNode->colliders.at(j)->getXOffset() - otherNode->colliders.at(j)->getXSize()/2.0 + otherNode->position.x;

            if(xNodeLimitOne >= xOtherLimitTwo && xOtherLimitOne >= xNodeLimitTwo)
            {
                // Comprobamos colisiones en eje Z
                float zNodeLimitOne = this->colliders.at(i)->getZOffset() + this->colliders.at(i)->getZSize()/2.0 + this->position.z;
                float zNodeLimitTwo = this->colliders.at(i)->getZOffset() - this->colliders.at(i)->getZSize()/2.0 + this->position.z;

                float zOtherLimitOne = otherNode->colliders.at(j)->getZOffset() + otherNode->colliders.at(j)->getZSize()/2.0 + otherNode->position.z;
                float zOtherLimitTwo = otherNode->colliders.at(j)->getZOffset() - otherNode->colliders.at(j)->getZSize()/2.0 + otherNode->position.z;

                if(zNodeLimitOne >= zOtherLimitTwo && zOtherLimitOne >= zNodeLimitTwo)
                {
                    // Comprobamos colisiones en eje Y
                    float yNodeLimitOne = this->colliders.at(i)->getYOffset() + this->colliders.at(i)->getYSize()/2.0 + this->position.y;
                    float yNodeLimitTwo = this->colliders.at(i)->getYOffset() - this->colliders.at(i)->getYSize()/2.0 + this->position.y;

                    float yOtherLimitOne = otherNode->colliders.at(j)->getYOffset() + otherNode->colliders.at(j)->getYSize()/2.0 + otherNode->position.y;
                    float yOtherLimitTwo = otherNode->colliders.at(j)->getYOffset() - otherNode->colliders.at(j)->getYSize()/2.0 + otherNode->position.y;

                    if(yNodeLimitOne >= yOtherLimitTwo && yOtherLimitOne >= yNodeLimitTwo)
                    {
                        // Comprobar si el collider ya se encuentra en el vector de colisiones
                        for(int i = 0; i < collisions.size(); ++i)
                        {
                            if(collisions.at(i) == otherNode->colliders.at(j))
                                return true;
                        }

                        this->collisions.push_back(otherNode->colliders.at(j));
                        return true;
                    }
                }
            }

            // Eliminar collider del vector de colisiones cuando se deja de colisionar con el
            for(int i = 0; i < collisions.size(); ++i)
            {
                if(collisions.at(i) == otherNode->colliders.at(j))
                {
                    collisions.erase(collisions.begin() + i);
                }
            }
        }
    }
    return false;
}


vector<MyCollider*>* MyNode::getCollisions()
{
    return &collisions;
}

vector<MyCollider*>* MyNode::getColliders()
{
    return &colliders;
}

MyEntity* MyNode::getMyEnt(){
    return entity;
}