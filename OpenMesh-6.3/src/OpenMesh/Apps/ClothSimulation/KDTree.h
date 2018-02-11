#ifndef KDTREE_H
#define KDTREE_H

//------- ANN
#include <ANN/ANN.h>
#include <ANN/ANNx.h>
#include <ANN/ANNperf.h>

// globally defined type and constants
#include "GlobalTypes.h"
#include "Vector.h"

/**
 * ClosestPoint class
 *
 * class that allows efficient closest point lookup using KD-tree (ANN library)
 */
class KDTree
{
public:
	/// constructor
	KDTree();

	/// destructor
	~KDTree();

	/// build ANN KD-tree for _pts
	void init(const MyMesh& _mesh);

	/// release data
	void release();

    /// Check if there is collision
    void getClosestPoint(const Vector3& _queryVertex, Vector3& resultPoint, double& distance);

private:
	/// data points only used when ANN search is performed
	ANNpointArray * dataPoints_;

	/// kd tree search structure
	ANNkd_tree * kDTree_;
};

#endif /* KDTREE_H */