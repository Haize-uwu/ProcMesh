#include <Functions.hpp>

#include <ostream>
#include <raylib.h>
#include <raylib.hpp>
#include <raymath.h>
#include <vector>
#include<iostream>


#define UP Vector3{0.0f, 1.0f, 0.0f}
#define DOWN Vector3{0.0f, -1.0f, 0.0f}
#define LEFT Vector3{-1.0f, 0.0f, 0.0f}
#define RIGHT Vector3{1.0f, 0.0f, 0.0f}
#define FORWARD Vector3{0.0f, 0.0f, 1.0f}
#define BACK Vector3{0.0f, 0.0f, -1.0f}


float cellsize= 1;
Vector3 gridOffset = Vector3{0.0f, 0.0f,0.0f};
float gridSize = 1;
int subdiv = 1;
float scale = 1;

Mesh create_plane(int subdiv, Vector3 center, Vector3 direction);
Mesh unit_plane(int subdiv, int index, Vector3 center, Vector3 direction, float scale);
Mesh create_cube(int subdiv,Vector3 center, float scale);

int main(void){
    const int screenWidth = 1200;
    const int screenHeight = 750;
    raylib::InitWindow(screenWidth, screenHeight, "Mesh Gen");
    Camera camera = {{3.0f,3.0f,3.0f}, {0.0f,0.0f,0.0f}, {0.0f, 1.0f,0.0f}, 45.0f, 0};

    Vector3 origin = {0.0f,0.01f, 0.0f};
    SetTargetFPS(60);
    DisableCursor();

    
    // Mesh plane_mesh = unit_plane(gridSize, 0, Vector3Scale(LEFT, 0.5f), LEFT, scale);
    Mesh plane_mesh = create_cube(gridSize, origin, scale);
   
    Model plane = LoadModelFromMesh(plane_mesh);

    bool modelUpdate = false;
    bool toggleSurface = false;

    while(!WindowShouldClose()){
        
        if(IsKeyPressed(KEY_UP) && gridSize <31) {modelUpdate = true; gridSize++;}
        if(IsKeyPressed(KEY_DOWN) && gridSize >1){ modelUpdate = true;gridSize--;}
        if(IsKeyPressed(KEY_RIGHT)) {modelUpdate = true;scale += 0.5;}
        if(IsKeyPressed(KEY_LEFT)) {modelUpdate = true;scale -= 0.5;}
        if(IsKeyPressed(KEY_M)){toggleSurface = !toggleSurface;}
        if(modelUpdate){
            // UnloadMesh(plane_mesh);
            modelUpdate = !modelUpdate;
            // plane_mesh = unit_plane(gridSize, 0, Vector3Scale(LEFT, 0.5f), LEFT, scale);
            plane_mesh = create_cube(gridSize, origin, scale);
            plane =  LoadModelFromMesh(plane_mesh);
        }

        
        UpdateCamera(&camera, CAMERA_FREE);
        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode3D(camera);
            if(toggleSurface){
                DrawModel(plane, origin, 1.0f, PURPLE);
            }
            else{
                DrawModelWires(plane, origin, 1.0f, PURPLE);
            }
            
                // DrawSphereEx(origin, 1.0f, 32  , 32, GREEN);
                
                Vector3 direction = Vector3Normalize(UP);
                Vector3 binormal = Vector3Scale(Vector3{direction.z, direction.x, direction.y}, 1.0f / gridSize);
                Vector3 tangent = Vector3RotateByAxisAngle(binormal, direction, PI / 2.0f);
                 Vector3 tempOffset = Vector3Add(binormal, tangent);
            Vector3 offset = Vector3Add(Vector3Scale(tempOffset, -cellsize / 2.0f), origin);
             Vector3 vertexOffset = Vector3Add(Vector3Add(Vector3Scale(binormal, 0), 
                                            Vector3Scale(tangent,0)),   
                                    offset);
            DrawLine3D(origin, vertexOffset, ORANGE);
            // DrawLine3D(origin, UP, ORANGE);
            DrawLine3D(origin, Vector3Add(vertexOffset, tangent), RED);
            DrawLine3D(origin, Vector3Add(Vector3Add(vertexOffset, tangent), binormal), GREEN);
            DrawLine3D(origin, Vector3Add(vertexOffset, binormal), BLUE);

                DrawGrid(5, 1.0);

            
            EndMode3D();
        EndDrawing();
        // UnloadMesh(plane_mesh);
    }
    
    CloseWindow();
}




Mesh create_plane(int subdiv, Vector3 center, Vector3 direction){
    float vertexOffset =cellsize*0.5f;
    //tracker integers so we dont draw over existing quads
    // this is a discrete mesh implementation
    int v = 0;
    int t = 0;

    Mesh plane_mesh = {0};
    
    
    plane_mesh.vertexCount = gridSize*gridSize*4;
    std::vector<Vector3> positions(plane_mesh.vertexCount);
    std::vector<float> vertices(plane_mesh.vertexCount*3);
    plane_mesh.triangleCount = gridSize*gridSize*6;
    std::vector<unsigned short> indices(plane_mesh.triangleCount*3);
    std::vector<float> normals(plane_mesh.vertexCount*3);
    
    for(int x = 0; x < gridSize; x++){
        for(int y = 0; y < gridSize; y++){
            //to prevent the quads from being overlayed
             Vector3 cellOffset = Vector3{-x*cellsize,0.0f,-y*cellsize};
            /*  using positions here because its easier to index the whole vector but
                mesh.vertices needs a flattened array of floats so will need to move the vertices
                out of this structure.
            */ 
            positions[v] =   Vector3Add(Vector3Add(Vector3{-vertexOffset, 0.0f,-vertexOffset}, gridOffset),cellOffset);
            positions[v+1] = Vector3Add(Vector3Add(Vector3{-vertexOffset, 0.0f,vertexOffset}, gridOffset),cellOffset);
            positions[v+2] = Vector3Add(Vector3Add(Vector3{vertexOffset, 0.0f,-vertexOffset}, gridOffset),cellOffset);
            positions[v+3] = Vector3Add(Vector3Add(Vector3{vertexOffset, 0.0f,vertexOffset}, gridOffset),cellOffset);

            indices[t] = v;
            indices[t+1] = v+1;
            indices[t+2] = v+2;
            indices[t+3] = v+2;
            indices[t+4] = v+1;
            indices[t+5] = v+3;
            v+=4;
            t+=6;
        }   
    }
    std::cout << "GENERATED VERTICES" <<std::endl;
    

    for(int i =0; i<positions.size(); i++){
        //  std::cout << "Moving VERTICES:\t"<<i <<std::endl;
        vertices[i*3] = positions[i].x ;
        vertices[i*3+1] = positions[i].y;
        vertices[i*3+2] = positions[i].z ;

        normals[i*3] = direction.x;
        normals[i*3+1] = direction.y;
        normals[i*3+2] = direction.z;
    }
    
    std::cout << "MOVED VERTICES" <<std::endl;

   
    plane_mesh.vertices = vertices.data();
    plane_mesh.normals = normals.data();
    plane_mesh.indices = indices.data();

   
    

    UploadMesh(&plane_mesh, true);
    
    return plane_mesh;


}




Mesh unit_plane(int subdiv, int index, Vector3 center, Vector3 direction, float scale){
    
    //tracker integers so we dont draw over existing quads
    // this is a discrete mesh implementation
    int v = 0;
    int t = 0;

    Mesh plane_mesh = {0};
    
    
    plane_mesh.vertexCount = gridSize*gridSize*4;
    std::vector<Vector3> positions(plane_mesh.vertexCount);
    std::vector<float> vertices(plane_mesh.vertexCount*3);
    plane_mesh.triangleCount = gridSize*gridSize*6;
    std::vector<unsigned short> indices(plane_mesh.triangleCount*3);
    std::vector<float> normals(plane_mesh.vertexCount*3);
    
    // tangents and binormals to get lighting correct
    direction = Vector3Normalize(direction);
    Vector3 binormal = Vector3Scale(Vector3{direction.z, direction.x, direction.y}, 1.0f / gridSize);
    Vector3 tangent = Vector3RotateByAxisAngle(binormal, direction, PI / 2.0f);
    Vector3 tempOffset = Vector3Add(binormal, tangent);
    Vector3 offset = Vector3Add(Vector3Scale(tempOffset, -subdiv / 2.0f), center);

    for(int x = 0; x < gridSize; x++){
        for(int y = 0; y < gridSize; y++){
            //to prevent the quads from being overlayed
            Vector3 vertexOffset = Vector3Add(Vector3Add(Vector3Scale(binormal, x), 
                                            Vector3Scale(tangent,y)),   
                                    offset);
            int index_offset = 4 *(x*subdiv+y) +index;
                                    
            
            /*  using positions here because its easier to index the whole vector but
                mesh.vertices needs a flattened array of floats so will need to move the vertices
                out of this structure.
            */ 
            positions[v] =   Vector3Scale(vertexOffset, scale);
            positions[v+1] = Vector3Scale(Vector3Add(vertexOffset, tangent),scale);
            positions[v+2] = Vector3Scale(Vector3Add(Vector3Add(vertexOffset, tangent), binormal),scale);
            
            positions[v+3] = Vector3Scale(Vector3Add(vertexOffset, binormal),scale);
            

            indices[t] = v+index;
            indices[t+1] = v+index+3;
            indices[t+2] = v+index+1;
            indices[t+3] = v+index+2;
            indices[t+4] = v+index+1;
            indices[t+5] = v+index+3;

          
            v+=4;
            t+=6;
        }   
    }
    std::cout << "GENERATED VERTICES" <<std::endl;
    

    for(int i =0; i<positions.size(); i++){
        //  std::cout << "Moving VERTICES:\t"<<i <<std::endl;
        vertices[i*3] = positions[i].x ;
        vertices[i*3+1] = positions[i].y;
        vertices[i*3+2] = positions[i].z ;

        normals[i*3] = direction.x;
        normals[i*3+1] = direction.y;
        normals[i*3+2] = direction.z;
    }
    
    std::cout << "MOVED VERTICES" <<std::endl;

   
    plane_mesh.vertices = vertices.data();
    plane_mesh.normals = normals.data();
    plane_mesh.indices = indices.data();

   
    

    // UploadMesh(&plane_mesh, true);
    
    return plane_mesh;


}

Mesh create_cube(int subdiv, Vector3 center, float scale){

    Mesh cube = {0} ;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<unsigned short> indices;

    std::vector<Vector3> directions = {UP, DOWN, LEFT, RIGHT, FORWARD, BACK};

    for (int i = 0; i< directions.size(); i++){
        int index = 4*i*subdiv*subdiv;

        Mesh plane = unit_plane(subdiv,index, Vector3Scale(directions[i], 0.5f), directions[i], scale);
        cube.vertexCount += plane.vertexCount;
        cube.triangleCount += plane.triangleCount;
        vertices.insert(vertices.end(), plane.vertices, plane.vertices + plane.vertexCount*3);
        normals.insert(normals.end(), plane.normals, plane.normals+plane.vertexCount*3);
        indices.insert(indices.end(), plane.indices, plane.indices+plane.triangleCount*3);
      
        // break;
    }

   
    std::cout << cube.vertexCount <<std::endl;
    std::cout << cube.triangleCount <<std::endl;

    cube.vertices = vertices.data();
    cube.normals = normals.data();
    cube.indices = indices.data();

    UploadMesh(&cube, false);
    return cube;
}