#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;
using namespace std;

// Forward declarations
class MyEntity;
class MyCollider;

// Base class of most of the objects in our Game Engine
class MyNode 
{

protected:
    // ATTRIBUTES
    MyEntity* entity; // Entity stored in our nodes
    vector <MyNode*> children; // Childrens that store an entity inside
    MyNode* parent; // Father of the current node

    vec3 position; // Vector that stores the position 
    vec3 rotation; // Vector that stores the rotation
    vec3 scale; // Vector that stores the scaling
    mat4 transMatrix; // Transformation matrix of the node. Contains translation, rotation and scaling values.

    bool isChanged; // Boolean that enables once the position, rotation or scaling has been modified

    // COLLISIONS
    vector <MyCollider*> colliders;
    vector <MyCollider*> collisions;

public:
    MyNode(MyEntity* entity);
    ~MyNode(){}

    // BASIC FUNCTIONS
    void addChild(MyNode* child); // Adds a child to the children vector
    void removeChild(MyNode* child); // Removes a child from the children vector

    MyNode* getParent(); // Returns the current parent of this node

    MyEntity* getEntity(); // Only use when the entity is a model!!!

    void setPosition(vec3 newPosition); // Sets the position of this node
    void setRotation(vec3 newRotation); // Sets the rotation of this node
    void setScale(vec3 newScale); // Sets the scale of this node
    vec3 getPosition(); // Returns the current  position of this node
    vec3 getRotation(); // Returns the current rotation of this node
    vec3 getScale(); // Returns the current scale of this node

    mat4 translate(vec3 newPosition);
    mat4 rotate(vec3 newRotation);
    mat4 scalate(vec3 newScale);

    void setTransfMatrix(mat4 matrix); // Sets the transform matrix of this node
    mat4 getTransfMatrix(); // Returns the current transform matrix of this node

    void travel(mat4 matrix); // Travels to one of the child nodes

    mat4 calculateMatrix();

    // CUSTOM FUNCITONS
    // bool operator==(const MyNode& node1, const MyNode& node2);

    // COLLISIONS
    void addCubeCollider(char* name, MyNode* parent, float xSize, float ySize, float zSize, float xOffset, float yOffset, float zOffset);
    bool checkCollisions(MyNode* otherNode);

    vector<MyCollider*>* getCollisions();
    vector<MyCollider*>* getColliders();

    MyEntity* getMyEnt();
};