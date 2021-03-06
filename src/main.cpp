#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <string.h>

#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

#include "json/json.h"
#include "scene.hpp"
#include "image.hpp"

using namespace std;

/****************************************************************************/
// Compiler Vars
/****************************************************************************/

#define NUM_THREADS 4

/****************************************************************************/
// Structs
/****************************************************************************/

struct ThreadInfo
{
    int id;
    Scene* scene;
    Image* image;
    int pixelStart;
    int pixelEnd;
};

/****************************************************************************/
/* Helper Functions */
/****************************************************************************/

void usage()
{
    cout << "Usage: rt <scene.json> <outfile> width height" << endl;
}

/****************************************************************************/
/* JSON Scene Parsing Functions */
/****************************************************************************/

Material* getMaterialByName(string name, vector<Material*> materials)
{
    if(name == "")
    {
        return NULL;
    }
    
    for(int i = 0; i < materials.size(); i++)
    {
        if(materials[i]->name == name)
        {
            return materials[i];
        }
    }
    
    cout << "[WARNING] Could not find material with name \""<<name<<"\"" << endl;
    
    return NULL;
}

Colour parseColour(Json::Value j_colour)
{
    if(j_colour.size() == 0)
    {
        return Colour(0, 0, 0);
    }
    
    return Colour(j_colour[0].asDouble(), j_colour[1].asDouble(), j_colour[2].asDouble());
}

Point3D parsePoint3D(Json::Value j_point3D)
{
    if(j_point3D.size() == 0)
    {
        return Point3D(0, 0, 0);
    }
    
    return Point3D(j_point3D[0].asDouble(), j_point3D[1].asDouble(), j_point3D[2].asDouble());
}

Vector3D parseVector3D(Json::Value j_vector3D, double defaultVal)
{
    if(j_vector3D.size() == 0)
    {
        return Vector3D(defaultVal, defaultVal, defaultVal);
    }
    
    return Vector3D(j_vector3D[0].asDouble(), j_vector3D[1].asDouble(), j_vector3D[2].asDouble());
}

SceneObj* parseSceneObj(Json::Value j_sceneObj, vector<Material*> materials)
{
    SceneObj* sceneObj;
    
    string typeStr = j_sceneObj.get("type", "none").asString();
    if(typeStr == "none")
    {
        sceneObj = new SceneObj();
    }
    else if(typeStr == "sphere")
    {
        sceneObj = new SphereObj();
    }
    else if(typeStr == "cube")
    {
        sceneObj = new CubeObj();
    }
    else if(typeStr == "mesh")
    {
        sceneObj = new MeshObj();
        
        Json::Value j_verticies = j_sceneObj["verticies"];
        Json::Value j_faces = j_sceneObj["faces"];
        
        for(int i = 0; i < j_verticies.size(); i++)
        {
            ((MeshObj*)sceneObj)->verts.push_back(parsePoint3D(j_verticies[i]));
        }
        
        ((MeshObj*)sceneObj)->faces.resize(j_faces.size());
        for(int i = 0; i < j_faces.size(); i++)
        {
            for(int j = 0; j < j_faces[i].size(); j++)
            {
                ((MeshObj*)sceneObj)->faces[i].push_back(j_faces[i][j].asInt());
            }
        }
        
        ((MeshObj*)sceneObj)->calculateTriangles();
    }
    
    Json::Value j_rotate = j_sceneObj["rotate"];
    
    sceneObj->name = j_sceneObj.get("name", "unknown").asString();
    sceneObj->material = getMaterialByName(j_sceneObj.get("material", "").asString(), materials);
    sceneObj->translate(parseVector3D(j_sceneObj["translate"], 0));
    if(j_rotate.size() != 0)
        sceneObj->rotate(j_rotate["axis"].asString(), j_rotate["angle"].asDouble());
    sceneObj->scale(parseVector3D(j_sceneObj["scale"], 1));
    
    Json::Value j_children = j_sceneObj["children"];
    for(int i = 0; i < j_children.size(); i++)
    {
        sceneObj->children.push_back(parseSceneObj(j_children[i], materials));
    }
    
    return sceneObj;
}

vector<Light*> parseLights(Json::Value j_lights, vector<Material*> materials)
{
    vector<Light*> lights(j_lights.size(), NULL);
    
    for(int i = 0; i < j_lights.size(); i++)
    {
        Light* light = new Light();
        
        Json::Value j_falloff = j_lights[i]["falloff"];
        Json::Value j_intensity = j_lights[i]["intensity"];
        
        light->name = j_lights[i].get("name", "").asString();
        light->falloff = new double[3];
        light->falloff[0] = j_falloff[0].asDouble();
        light->falloff[1] = j_falloff[1].asDouble();
        light->falloff[2] = j_falloff[2].asDouble();
        light->intensity = j_intensity.asDouble();
        light->position = parsePoint3D(j_lights[i]["position"]);
        light->colour = parseColour(j_lights[i]["colour"]);
        light->size = j_lights[i]["size"].asInt();
        light->type = j_lights[i]["type"].asString();
        
        if(light->type == "cube")
        {
            light->plain = j_lights[i]["plain"].asString();
        }
        
        lights[i] = light;
    }
    
    return lights;
}

vector<Material*> parseMaterials(Json::Value j_materials)
{
    vector<Material*> materials(j_materials.size(), NULL);
    
    for(int i = 0; i < j_materials.size(); i++)
    {
        Material* material = new Material();
        
        Json::Value j_diffused = j_materials[i]["diffused"];
        Json::Value j_specular = j_materials[i]["specular"];
        
        material->name = j_materials[i].get("name", "").asString();
        material->diffused = Colour(j_diffused[0].asDouble(), j_diffused[1].asDouble(), j_diffused[2].asDouble());
        material->specular = Colour(j_specular[0].asDouble(), j_specular[1].asDouble(), j_specular[2].asDouble());
        material->shininess = j_materials[i].get("shininess", 0).asInt();
        material->reflectivity = j_materials[i].get("reflectivity", 0).asDouble();
        
        materials[i] = material;
    }
    
    return materials;
}

Scene* parseScene(Json::Value j_scene)
{
    Scene* scene = new Scene();
    
    Json::Value j_eyePos = j_scene["eye_position"];
    Json::Value j_viewDir = j_scene["view_direction"];
    Json::Value j_upDir = j_scene["up_direction"];
    Json::Value j_field_of_view = j_scene["field_of_view"];
    Json::Value j_materials = j_scene["materials"];
    Json::Value j_lights = j_scene["lights"];
    Json::Value j_root = j_scene["root"];
    
    scene->eyePos = Point3D(j_eyePos[0].asFloat(), j_eyePos[1].asFloat(), j_eyePos[2].asFloat());
    scene->viewDir = Vector3D(j_viewDir[0].asFloat(), j_viewDir[1].asFloat(), j_viewDir[2].asFloat());
    scene->upDir = Vector3D(j_upDir[0].asFloat(), j_upDir[1].asFloat(), j_upDir[2].asFloat());
    scene->fieldOfView = j_field_of_view.asFloat();
    scene->materials = parseMaterials(j_materials);
    scene->lights = parseLights(j_lights, scene->materials);
    scene->root = parseSceneObj(j_root, scene->materials);
    
    return scene;
}

Scene* getScene(string sceneJsonFile)
{
    filebuf fb;
    if (!fb.open (sceneJsonFile.c_str(), ios::in))
    {
        cout << "[Error] Could not open scene file: " << sceneJsonFile << endl;
        return NULL;
    }
    
    istream is(&fb);
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( is, root );
    if ( !parsingSuccessful )
    {
        std::cout  << "[Error] Failed to parse json\n"
        << reader.getFormattedErrorMessages();
        
        return NULL;
    }
    
    return parseScene(root["scene"]);
}

/****************************************************************************/
// Rendering
/****************************************************************************/

void* render(void* arg)
{
    ThreadInfo* tInfo = (ThreadInfo*) arg;
    tInfo->scene->raytrace(tInfo->image, tInfo->pixelStart, tInfo->pixelEnd);
    pthread_exit(NULL);
}

void renderScene(Scene* scene, string outfile, int width, int height)
{
    pid_t rc;
    pthread_t threads[NUM_THREADS];
    void *status;
    
    // Initialize the scene
    scene->init(width, height);
    
    // Create the image to write pixel data to
    Image* image = new Image(width, height, 3);
    
    // Get the number of pixel each thread will have to render
    int leftOverPixels = (width*height) % NUM_THREADS;
    int pixelChunks = (width*height - leftOverPixels)/NUM_THREADS;
    
    // Spawn each thread to work on their respect pixel chunk
    for(int i=0; i < NUM_THREADS; i++ )
    {
        ThreadInfo *tInfo = new ThreadInfo();
        tInfo->id = i+1;
        tInfo->scene = scene;
        tInfo->image = image;
        tInfo->pixelStart = i * pixelChunks;
        tInfo->pixelEnd = tInfo->pixelStart + pixelChunks;
        
        // If this is the last thread then it needs to render the left over pixels as a result
        // of width*height not being a nice number
        if(i == NUM_THREADS - 1)
        {
            tInfo->pixelEnd += leftOverPixels;
            
            cout << "Main thread starting, rending pixels from " << tInfo->pixelStart << " to " << (tInfo->pixelEnd + leftOverPixels) << endl;
            cout << "All threads started, waiting for rendering to complete... (This may take a while)" << endl;
            
            tInfo->scene->raytrace(tInfo->image, tInfo->pixelStart, tInfo->pixelEnd + leftOverPixels);
            break;
        }
        
        cout << "Starting thread " << tInfo->id << ", rending pixels from " << tInfo->pixelStart << " to " << tInfo->pixelEnd << endl;
        
        // Spawn the thread
        rc = pthread_create(&threads[i], NULL, render, tInfo);
        if (rc)
        {
            cout << "[Error] Unable to create thread, " << rc << endl;
            exit(-1);
        }
    }
    
    // Main thread waits for all the other threads to finish
    for(int i=0; i < NUM_THREADS - 1; i++ )
    {
        rc = pthread_join(threads[i], &status);
        if (rc)
        {
            cout << "[Error] Unable to join, " << rc << endl;
            exit(-1);
        }
    }
    
    cout << "Finished rendering, saving image" << endl;
    
    // Once all threads have completed render their part of the scene save the image
    image->savePng(outfile);
}

/****************************************************************************/
/* Entry Point */
/****************************************************************************/

int main(int argc, char* argv[])
{
    string scenefile = "";
    string outfile = "";
    int width = 0;
    int height = 0;
    
    if(argc < 5)
    {
        usage();
        return -1;
    }
    
    // Get the commandline args
    scenefile = argv[1];
    outfile = argv[2];
    width = atoi(argv[3]);
    height = atoi(argv[4]);
    
    // Parse the json to create the scene
    Scene* scene = getScene(scenefile);
    if(scene == NULL)
    {
        cout << "[Error] Failed parsing scene file." << endl;
        return -1;
    }
    
    cout << "Rendering scene:" << endl;
    cout << " -- scene: " << scenefile << endl;
    cout << " -- out: " << outfile << endl;
    cout << " -- width: " << width << endl;
    cout << " -- height: " << height << endl;
    cout << endl;
    
    // Start rendering
    renderScene(scene, outfile, width, height);
}
