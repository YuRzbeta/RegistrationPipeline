/*
 *  CONVERTER PCL
 *
 *  Conversion class between PCL data types and Pipleline data types. *
 *
 */

#ifndef CONVERTERPCL_H
#define CONVERTERPCL_H

#include <vector>

#include "../point.h"

#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/filters/filter.h>
#include <pcl/features/normal_3d.h>
#include <opencv2/opencv.hpp>



using namespace std;
using namespace pcl;

/*
 * Data structure used to return different data, out of this class.
 */
struct myPCLReturn{

    vector<int> indices;
    vector<float> dists;
};

class converterPCL {
public:

    // NO ELEMENTS ----------------------------

    // METHODS ------------------------

    converterPCL();                                 // Constructor
    ~converterPCL();                                // Destructor

    PointCloud<PointXYZ>::Ptr points2PCL
            (vector<Point*> * points);              // Convert data to PCL
    vector<Point> * PCL2points
            (PointCloud<PointXYZ>::Ptr cloud);      // Convert PCL to data
    vector<int> points2PCLindices
            (vector<Point*> * points);              // Convert data to PCL (only indices)

    PointCloud<SHOT352>::Ptr desc2SHOT
            (vector<Point*> * points);              //
    PointCloud<FPFHSignature33>::Ptr desc2FPFH(vector<Point*> * points);
    PointCloud<ShapeContext1980>::Ptr desc23DSC(vector<Point*> * points);
    PointCloud< Histogram<153> >::Ptr desc2SpinImage(vector<Point*> * points);


    PointCloud<Normal>::Ptr calcNormals(PointCloud<PointXYZ>::Ptr cloud, float radiusSearch);
    void repairNormals(PointCloud<PointXYZ>::Ptr workpoints, PointCloud<pcl::Normal>::Ptr normals);


};

#endif // CONVERTERPCL_H
