#ifndef CLOSESTPOINT_H
#define CLOSESTPOINT_H

//------- ANN
#include <ANN/ANN.h>
#include <ANN/ANNx.h>
#include <ANN/ANNperf.h>

// globally defined type and constants
#include "GlobalType.h"

/**
 * ClosestPoint class
 *
 * class that allows efficient closest point lookup using KD-tree (ANN library)
 */
class ClosestPoint
{
public:
	/// constructor
	ClosestPoint();

	/// destructor
	~ClosestPoint();

	/// build ANN KD-tree for _pts
    // TODO: convert to use OpenMesh data structure
	void init(const MyMesh& mesh);

	/// release data
	void release();

	/// retrieve closest point of query
	void getClosestPoint(const MyMesh::Point &_queryVertex,
			     MyMesh::Point &resultPoint, double &distance);

      private:
	/// data points only used when ANN search is performed
	ANNpointArray * dataPoints_;

	/// kd tree search structure
	ANNkd_tree * kDTree_;
};

#endif /* CLOSESTPOINT_H */
