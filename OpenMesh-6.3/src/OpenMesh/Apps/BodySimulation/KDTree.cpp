#include "KDTree.h"

KDTree::KDTree()
{
	dataPoints_ = NULL;
	kDTree_ = NULL;
}

KDTree::~KDTree()
{
	release();
}

void KDTree::release()
{
	if( kDTree_ ) {
		delete kDTree_;
		kDTree_ = NULL;
	}
	if( dataPoints_ ) {
		annDeallocPts(*dataPoints_);
		delete dataPoints_;
		dataPoints_ = NULL;
	}
}

void KDTree::init(const MyMesh& _mesh)
{
	release();

	dataPoints_ = new ANNpointArray;
	*dataPoints_ = annAllocPts(_mesh.n_vertices(), 3); // allocate data points

    MyMesh::VertexIter vIt,vBegin,vEnd;
    vBegin = _mesh.vertices_begin();
    vEnd = _mesh.vertices_end();
    int i = 0;
    for (vIt = vBegin; vIt != vEnd; ++vIt)
    {
        (*dataPoints_)[i][0] = _mesh.point(*vIt)[0];
        (*dataPoints_)[i][1] = _mesh.point(*vIt)[1];
        (*dataPoints_)[i][2] = _mesh.point(*vIt)[2];
        ++i;
    }

    // build search structure (date points, number of points, dimension of space)
    kDTree_ = new ANNkd_tree(*dataPoints_, _mesh.n_vertices(), 3);	
}

// Return index
void
KDTree::getClosestPoint(const Vector3& _queryVertex, Vector3& resultPoint, double& distance)
{

	// initialize ANN types for wrapping
	ANNpoint queryPt;							// query point
	ANNidx nnIdx[1];							// near neighbor indices
	ANNdist dists[1];							// near neighbor distances

	queryPt = annAllocPt(3);					// allocate query point

	// assign values to ANN query point
	queryPt[0] = _queryVertex[0];
	queryPt[1] = _queryVertex[1];
	queryPt[2] = _queryVertex[2];

	//ANNmin_k mink;
	//int ptsVisited=0;

    kDTree_->annkSearch(queryPt, 1, nnIdx, dists);
	//kDTree_->annkSearch(&queryPt, nnIdx, dists, 0, &mink, ptsVisited);

    annDeallocPt(queryPt);

// #ifdef DEBUG
//     std::cout << "distance is " << dists[0] << " and the point is at" << (*dataPoints_)[nnIdx[0]][0] 
// 			  << "," << (*dataPoints_)[nnIdx[0]][1] << "," << (*dataPoints_)[nnIdx[0]][2] << std::endl;
// #endif
 
    resultPoint[0] = (*dataPoints_)[nnIdx[0]][0];
	resultPoint[1] = (*dataPoints_)[nnIdx[0]][1];
	resultPoint[2] = (*dataPoints_)[nnIdx[0]][2];

	distance = dists[0];
}
