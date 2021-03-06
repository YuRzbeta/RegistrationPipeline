//
// Created by ferran on 19/01/16.
//

#ifndef PIPELINE_MYCOMPRESSEDOCTREE_H
#define PIPELINE_MYCOMPRESSEDOCTREE_H


#include "../IDataStructure.h"
#include "../../../external/MoreStructures/CompressedOctree/CompressedOctree.h"
#include "../../Converters/converterYago.h"
#include "../../point.h"

class myCompressedOctree : public IDataStructure{

public:

    // Elements ----------------------------------------------------------------
    CompressedOctree *cOctree;
    converterYago *cyg;
    float diagonal;


    // Methods -----------------------------------------------------------------

    myCompressedOctree();
    myCompressedOctree(vector<Point *> *P, float _diag);
    ~myCompressedOctree();

    returnData calcOneNN(Point *queryPoint, float errEps);
    returnData calcOwnNN(Point *queryPoint);
    vector<returnData> calcNneigh(Point *queryPoint, int nNeigh);

    void printStats();
};


#endif //PIPELINE_MYCOMPRESSEDOCTREE_H
