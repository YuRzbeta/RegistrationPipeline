#ifndef PIPELINE_GRIDTREE_H
#define PIPELINE_GRIDTREE_H

#define TOLERANCE 0.0000000001

#include "myPoint.h"
#include "Cell.h"
#include <ANN/ANNperf.h>
#include <vector>


using namespace std;

class GridTree
{

    int slotsPerDimension;
    float thrsKdtree;
    string DStype;
    float diagonal;

    vector< vector<double> > limits; // three rows (x,y,z) and two columns(min,max), keeps the information on limits in each dimension

    vector<vector<vector<Cell *> > > grid;

    double tol; // tolerance to prevent numerical representation errors

    int nPoints;

    int meanPoints;                         // Mean of points of not empty cells.
    int notEmptyCells;

public:

    GridTree(vector<Point *> *vec, string _DStype, float _diagonal);
    ~GridTree();

    void kdtreezation();                        // creartes kdtree in each cell with nPoints > certain number.
    int getNumElems();
    int getSlotsPerDimension();
    float getMeanHeight();
    void calcMeanPoints();                      // Compute mean of points of not empty cells.

    int findSlot(double val, char type, bool checkOutOfBounds=false, bool squared=false); // type=x,y,z returns the slot (for the givenn) where value val falls into. "checkOutOfBounds" indictes if we get out of bonds querys back IN bounds or if we throw an exception.

    vector<int> slotsTouched(double min, double max, char type, bool squared=false); // returns minimum and maximum slots touched by an interval in a dimension x,y o z (indicated by type)

    vector<myPoint *> neighbors(Point *p, double eps); // returns all neigbors at distance at most eps from p, if it finds p it does not return it
    Point * oneNeighbor(Point *p, double eps); // returns all neigbors at distance at most eps from p, if it finds p it does not return it

};

#endif
