#include "Cloth.h"
#include <iostream>
int stttt = 0;
Cloth::Cloth(Body* b){
    QuadMesh mesh;
    // generate vertices
    QuadMesh::VertexHandle vhandle[2601];
    int index = 0;
    for(int i = 0; i < 51; ++i){
        for(int j = 0; j < 51; ++j){
            vhandle[index] = mesh.add_vertex(QuadMesh::Point((i-25)/25.0f, 3, (j-25)/25.0f));
            index++;
        }
    }
    std::vector<QuadMesh::VertexHandle> face_vhandles;
    for(int i = 0; i < 50; ++i){
        for(int j = 0; j < 50; ++j){
            face_vhandles.clear();
            face_vhandles.push_back(vhandle[i*51+j]);
            face_vhandles.push_back(vhandle[(i+1)*51+j]);
            face_vhandles.push_back(vhandle[(i+1)*51+j+1]);
            face_vhandles.push_back(vhandle[i*51+j+1]);
            mesh.add_face(face_vhandles);
        }
    }
    clothMesh = mesh;
    body = b;
    clothMesh.request_face_normals();
    clothMesh.update_normals();
    parseCloth();
    //clothNodes[0]->setFixed(true);
    //clothNodes[51*50]->setFixed(true);
}

Cloth::Cloth(QuadMesh mesh,Body* b)
{    
    body = b;
    clothMesh = mesh;
    parseCloth();
}

void Cloth::parseCloth(){
    // creating ClothNodes
    for(QuadMesh::VertexIter v_it=clothMesh.vertices_begin(); v_it!=clothMesh.vertices_end(); ++v_it) {
        ClothNode* cn = new ClothNode(Vector3(clothMesh.point(*v_it)[0], clothMesh.point(*v_it)[1], clothMesh.point(*v_it)[2]), v_it->idx(), body, MASS);
        clothNodes.push_back(cn);
    }
    // creating face and structural and shear spring
    std::set<std::string> springSet;
    for(QuadMesh::FaceIter f_it = clothMesh.faces_begin(); f_it != clothMesh.faces_end(); ++f_it) {
        // store the four vertices in a quad
        std::vector<ClothNode*> vertices;
        std::vector<std::string> verticesIdx;
        for (QuadMesh::FaceVertexIter fv_it=clothMesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
        {
            vertices.push_back(clothNodes[fv_it->idx()]);
            verticesIdx.push_back(fv_it->idx()+"");
        }
        ClothNode* topLeft = vertices[0];
        ClothNode* topRight = vertices[1];
        ClothNode* botRight = vertices[2];
        ClothNode* botLeft = vertices[3];
        /*if(topLeft->getIndex()-botLeft->getIndex() != -1 || topRight->getIndex()-botRight->getIndex() != -1 || 
            botRight->getIndex()-botLeft->getIndex() != 51 || topRight->getIndex()-topLeft->getIndex() != 51){
            std::cout << "TopLeft" << (*topLeft).getIndex()
         << "TopRight" << (*topRight).getIndex() 
         << "BotLeft" << (*botLeft).getIndex() 
         << "BotRight" << (*botRight).getIndex() << std::endl;
     }*/
        // structural spring | use verticesIdx to eliminate duplicated spring
        addSpring(springSet, verticesIdx[0], verticesIdx[1], topLeft, topRight, STRUCT);
        addSpring(springSet, verticesIdx[0], verticesIdx[3], topLeft, botLeft, STRUCT);
        addSpring(springSet, verticesIdx[2], verticesIdx[1], botRight, topRight, STRUCT);
        addSpring(springSet, verticesIdx[2], verticesIdx[3], botRight, botLeft, STRUCT);
        // shear spring
        addSpring(springSet, verticesIdx[0], verticesIdx[2], topLeft, botRight, SHEAR);
        addSpring(springSet, verticesIdx[1], verticesIdx[3], topRight, botLeft, SHEAR);

        addFaces(topLeft, topRight, botLeft);
        addFaces(botRight, topRight, botLeft);
    }
    
    // create bending spring
    for (QuadMesh::VertexIter v_it=clothMesh.vertices_sbegin(); v_it!=clothMesh.vertices_end(); ++v_it)
    {
        for (QuadMesh::VertexVertexIter vv_it=clothMesh.vv_iter(*v_it); vv_it.is_valid(); ++vv_it) {
            // for each vertex iterate the secondary neighbor, since one vertex's four cross secondary neighbor has been
            // add as shear spring, only secondary neighbors in the same column or row will be add as bending spring.
            for (QuadMesh::VertexVertexIter v3_it=clothMesh.vv_iter(*vv_it); v3_it.is_valid(); ++v3_it) {
                if(v_it->idx() != v3_it->idx())
                    addSpring(springSet, v_it->idx() + "", v3_it->idx() + "", clothNodes[v_it->idx()], clothNodes[v3_it->idx()], BEND);
            }
        }
    }
    //printAllSprings();
    spatialFaces = std::vector<std::vector<std::vector<std::set<ClothFace*> > > >(10);
    for(int i = 0; i < 10; i++){
        spatialFaces[i] = std::vector<std::vector<std::set<ClothFace*> > >(10);
        for(int j = 0; j < 10; j++){
            spatialFaces[i][j] = std::vector<std::set<ClothFace*> >(10);
            for(int k = 0; k < 10; k++){
                spatialFaces[i][j][k]=std::set<ClothFace*>();
            }   
        }        
    }
}

Cloth::~Cloth()
{
    for(unsigned int x=0; x < clothNodes.size(); ++x)
    {
        delete clothNodes[x];
    }
}
void Cloth::spatialCloth(){
    top = std::numeric_limits<float>::min();
    bottom = std::numeric_limits<float>::max(); 
    left = std::numeric_limits<float>::max();
    right = std::numeric_limits<float>::min();
    front = std::numeric_limits<float>::min();
    back = std::numeric_limits<float>::max();
    for(unsigned int i=0; i < clothNodes.size(); ++i) {
        top = clothNodes[i]->getPosition().m_y > top ? clothNodes[i]->getPosition().m_y : top;
        bottom = clothNodes[i]->getPosition().m_y < bottom ? clothNodes[i]->getPosition().m_y : bottom;
        left = clothNodes[i]->getPosition().m_x < left ? clothNodes[i]->getPosition().m_x : left;
        right = clothNodes[i]->getPosition().m_x > right ? clothNodes[i]->getPosition().m_x : right;
        front = clothNodes[i]->getPosition().m_z > front ? clothNodes[i]->getPosition().m_z : front;
        back = clothNodes[i]->getPosition().m_z < back ? clothNodes[i]->getPosition().m_z : back;
    }
    printf( "top:%f, bottom:%f, left:%f, right:%f, front:%f, back:%f \n", top, bottom, left, right, front, back);
    float height = std::abs(top-bottom), length = std::abs(right-left), width = std::abs(front-back);
    printf( "length:%f, height:%f, width:%f\n", length, height, width);
    for(unsigned int i=0; i < clothFaces.size(); ++i) {
        int x = std::abs(clothFaces[i]->cn1->getPosition().m_x-left) / length * 10;
        int y = height == 0 ? 0 : std::abs(clothFaces[i]->cn1->getPosition().m_y-bottom) / height * 10;
        int z = std::abs(clothFaces[i]->cn1->getPosition().m_z-back) / width * 10;
        x = x == 10 ? 9: x; y = y == 10 ? 9: y; z = z == 10 ? 9: z;
        //printf("x:%d ,y:%d ,z:%d\n", x, y, z);
        spatialFaces[x][y][z].insert(clothFaces[i]);
        
        x = std::abs(clothFaces[i]->cn2->getPosition().m_x-left) / length * 10;
        y = height == 0 ? 0 : std::abs(clothFaces[i]->cn1->getPosition().m_y-bottom) / height * 10;
        z = std::abs(clothFaces[i]->cn2->getPosition().m_z-back) / width * 10;
        x = x == 10 ? 9: x; y = y == 10 ? 9: y; z = z == 10 ? 9: z;
        //printf("x:%d ,y:%d ,z:%d\n", x, y, z);
        spatialFaces[x][y][z].insert(clothFaces[i]);
        
        x = std::abs(clothFaces[i]->cn3->getPosition().m_x-left) / length * 10;
        y = height == 0 ? 0 : std::abs(clothFaces[i]->cn1->getPosition().m_y-bottom) / height * 10;
        z = std::abs(clothFaces[i]->cn3->getPosition().m_z-back) / width * 10;
        x = x == 10 ? 9: x; y = y == 10 ? 9: y; z = z == 10 ? 9: z;
        //printf("x:%d ,y:%d ,z:%d\n", x, y, z);
        spatialFaces[x][y][z].insert(clothFaces[i]);
    }
}

std::set<ClothFace*> Cloth::getSpatialFaces(Vector3 pos){
    float height = std::abs(top-bottom), length = std::abs(right-left), width = std::abs(front-back);
    int x = std::abs(pos.m_x-left) / length * 10;
    int y = height == 0 ? 0 : std::abs(pos.m_y-bottom) / height * 10;
    int z = std::abs(pos.m_z-back) / width * 10;
    x = x == 10 ? 9: x; y = y == 10 ? 9: y; z = z == 10 ? 9: z;
    return spatialFaces[x][y][z]; 

}

void Cloth::spatialClear(){
    top = std::numeric_limits<float>::min(), 
    bottom = std::numeric_limits<float>::max(), 
    left = std::numeric_limits<float>::max(),
    right = std::numeric_limits<float>::min(),
    front = std::numeric_limits<float>::max(),
    back = std::numeric_limits<float>::min();
    for(int i = 0; i < 10; ++i){
        for(int j = 0; j < 10; ++j){
            for(int k = 0; k < 10; ++k){
                spatialFaces[i][j][k].clear();
            }
        }
    }
}
void Cloth::timeStep(float deltaTime)
{
    stttt++;
    std::cout << std::endl << "Iteration: " << stttt<< std::endl;
    // Apply forces
    for(unsigned int i = 0; i < clothNodes.size(); ++i){
        clothNodes[i]->setForce(Vector3(0,0,0));
        if(i != 0 && i != 2550)clothNodes[i]->addForce(Vector3(0,-GRAVITY,0));
        clothNodes[i]->addForce(clothNodes[i]->getVelocity()*-0.0125f);
    }
    for(std::vector<Spring*>::iterator itrSpr = clothSprings.begin(); itrSpr != clothSprings.end(); ++itrSpr) {
        (*itrSpr)->ApplyForce();
    }
    //std::cout << "after force" << std::endl;
    //TODO change to Bounding Volume Hierarchy
    //spatialCloth();
    //std::cout << "after spatial" << std::endl;
    
    for(unsigned int i = 0; i < clothNodes.size(); ++i){
        //clothNodes[i]->setNeighborFaces(getSpatialFaces(clothNodes[i]->getPosition()));
        clothNodes[i]->timeStep(deltaTime);
    }
    for(std::vector<Spring*>::iterator itrSpr = clothSprings.begin(); itrSpr != clothSprings.end(); ++itrSpr) {
        (*itrSpr)->ApplyProvotDynamicInverse();
    }
    /*for(unsigned int i = 0; i < clothNodes.size(); ++i){
        clothNodes[i]->update(deltaTime);
    }*/
    //spatialClear();
  //  printAllSprings();
}

void Cloth::addSpring(std::set<std::string> &springSet,std::string p1str, std::string p2str, ClothNode* p1, ClothNode* p2, int type) { 
    std::string p1p2 = p1str+"|"+p2str, p2p1 = p2str+"|"+p1str;
    if(springSet.find(p1p2) == springSet.end() && springSet.find(p2p1) == springSet.end()){
        springSet.insert(p2p1);
        Spring* spring = new Spring(p1, p2, type);
        p1->addStructAdjacent(p2);
        //p1->addSpring(spring);
        p2->addStructAdjacent(p1);
        //p2->addSpring(spring);
        clothSprings.push_back(spring);
    }
}

void Cloth::addFaces(ClothNode* c1, ClothNode* c2, ClothNode* c3){
    ClothFace* cf = new ClothFace(c1, c2, c3);
    clothFaces.push_back(cf);
}

void Cloth::printAllNodes()
{
    for (unsigned int i = 0; i < clothNodes.size(); ++i)
    {
        printf("[%f, %f, %f] ", clothNodes[i]->getPosition().m_x, clothNodes[i]->getPosition().m_y, clothNodes[i]->getPosition().m_z);
        printf("\n");
    }
}

void Cloth::printAllSprings()
{
    for (unsigned int i = 0; i < clothSprings.size(); ++i)
    {
        printf("spring btw [%f, %f, %f] and [%f, %f, %f] with type=%d\n", 
                clothSprings[i]->cn1->getPosition().m_x, clothSprings[i]->cn1->getPosition().m_y, clothSprings[i]->cn1->getPosition().m_z, 
                clothSprings[i]->cn2->getPosition().m_x, clothSprings[i]->cn2->getPosition().m_y, clothSprings[i]->cn2->getPosition().m_z,
                clothSprings[i]->getType());
        /*if(clothSprings[i]->getType() == 0 
            && (std::abs(clothSprings[i]->cn1->getIndex() - clothSprings[i]->cn2->getIndex())==51) ||
            (std::abs(clothSprings[i]->cn1->getIndex() - clothSprings[i]->cn2->getIndex())== 1)) {
                std::cout << "Correct" << std::endl;
        } else if(clothSprings[i]->getType() == 1 
            && (std::abs(clothSprings[i]->cn1->getIndex() - clothSprings[i]->cn2->getIndex())!=52) ||
            (std::abs(clothSprings[i]->cn1->getIndex() - clothSprings[i]->cn2->getIndex())!= 49)) {
                std::cout << "Correct" << std::endl;
        } else if(clothSprings[i]->getType() == 2 
            && (std::abs(clothSprings[i]->cn1->getIndex() - clothSprings[i]->cn2->getIndex())!=51*2) ||
            (std::abs(clothSprings[i]->cn1->getIndex() - clothSprings[i]->cn2->getIndex())!= 2)) {
            std::cout << "Correct" << std::endl;
        } else {
            printf("spring btw ClothNode[%d] and ClothNode[%d] with type=%d\n", 
                clothSprings[i]->cn1->getIndex(),
                clothSprings[i]->cn2->getIndex(),
                clothSprings[i]->getType());
            
        }*/
    }
}