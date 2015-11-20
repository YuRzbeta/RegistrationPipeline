#ifndef TRIMESH_H
#define TRIMESH_H
/*
Szymon Rusinkiewicz
Princeton University

TriMesh.h
Class for triangle meshes.
*/

#include "Vec.h"
#include "Color.h"
#include <vector>
#include <limits>

// OJU!!!! aixo s'hauria de treure junt amb els couts i endl
using namespace std;

class TriMesh {
public:
	//
	// Types
	//
	struct Face {
		int v[3];

		Face() {}
		Face(const int &v0, const int &v1, const int &v2)
			{ v[0] = v0; v[1] = v1; v[2] = v2; }
		Face(const int *v_)
			{ v[0] = v_[0]; v[1] = v_[1]; v[2] = v_[2]; }
		int &operator[] (int i) { return v[i]; }
		const int &operator[] (int i) const { return v[i]; }
		operator const int * () const { return &(v[0]); }
		operator const int * () { return &(v[0]); }
		operator int * () { return &(v[0]); }
		int indexof(int v_) const
		{
			return (v[0] == v_) ? 0 :
			       (v[1] == v_) ? 1 :
			       (v[2] == v_) ? 2 : -1;
		}
	};

	class BBox {
	public:
		point min, max;
		bool valid;

		// Construct as empty
		BBox() : min(point(std::numeric_limits<float>::max(),
		                   std::numeric_limits<float>::max(),
		                   std::numeric_limits<float>::max())),
		         max(point(-std::numeric_limits<float>::max(),
		                   -std::numeric_limits<float>::max(),
		                   -std::numeric_limits<float>::max())),
			 valid(false)
			{}

		// Initialize to one point or two points
		BBox(const point &p) : min(p), max(p), valid(true)
			{}
		BBox(const point &min_, const point &max_) :
			min(min_), max(max_), valid(true)
			{}

		// Mark invalid
		void clear()
		{
			min = point(std::numeric_limits<float>::max(),
		                    std::numeric_limits<float>::max(),
		                    std::numeric_limits<float>::max());
			max = point(-std::numeric_limits<float>::max(),
		                    -std::numeric_limits<float>::max(),
		                    -std::numeric_limits<float>::max());
			valid = false;
		}

		// Return center point and (vector) diagonal
		point center() const { return 0.5f * (min+max); }
		vec size() const { return max - min; }

		// Grow a bounding box to encompass a point
		BBox &operator += (const point &p)
			{ min.min(p); max.max(p); valid = true; return *this; }
		BBox &operator += (const BBox &b)
			{ min.min(b.min); max.max(b.max); valid = true; return *this; }

		// The following appear to be necessary for Visual Studio,
		// despite the fact that the operators shouldn't need
		// to be friends...
		friend const TriMesh::BBox operator + (const TriMesh::BBox &b, const point &p);
		friend const TriMesh::BBox operator + (const point &p, const TriMesh::BBox &b);
		friend const TriMesh::BBox operator + (const TriMesh::BBox &b1, const TriMesh::BBox &b2);
	};

	struct BSphere {
		point center;
		float r;
		bool valid;
		BSphere() : valid(false)
			{}
	};

	//
	// Enums
	//
	enum TstripRep { TSTRIP_LENGTH, TSTRIP_TERM };
	enum { GRID_INVALID = -1 };
	enum StatOp { STAT_MIN, STAT_MAX, STAT_MEAN, STAT_MEANABS,
		STAT_RMS, STAT_MEDIAN, STAT_STDEV, STAT_TOTAL };
	enum StatVal { STAT_VALENCE, STAT_FACEAREA, STAT_ANGLE,
		STAT_DIHEDRAL, STAT_EDGELEN, STAT_X, STAT_Y, STAT_Z };

	//
	// Constructor
	//
	TriMesh() : grid_width(-1), grid_height(-1), flag_curr(0)
		{}

	//
	// Members
	//

	// The basics: vertices and faces
	std::vector<point> vertices;
	std::vector<Face> faces;

	// Triangle strips
	std::vector<int> tstrips;

	// Grid, if present
	std::vector<int> grid;
	int grid_width, grid_height;

	// Other per-vertex properties
	std::vector<Color> colors;
	std::vector<float> confidences;
	std::vector<unsigned> flags;
	unsigned flag_curr;

	// Computed per-vertex properties
	std::vector<vec> normals;
	std::vector<vec> pdir1, pdir2;
	std::vector<float> curv1, curv2;
	std::vector< Vec<4,float> > dcurv;
	std::vector<vec> cornerareas;
	std::vector<float> pointareas;

	// Bounding structures
	BBox bbox;
	BSphere bsphere;

	// Connectivity structures:
	//  For each vertex, all neighboring vertices
	std::vector< std::vector<int> > neighbors;
	//  For each vertex, all neighboring faces
	std::vector< std::vector<int> > adjacentfaces;
	//  For each face, the three faces attached to its edges
	//  (for example, across_edge[3][2] is the number of the face
	//   that's touching the edge opposite vertex 2 of face 3)
	std::vector<Face> across_edge;

	//
	// Compute all this stuff...
	//
	void need_tstrips();
	void convert_strips(TstripRep rep);
	void unpack_tstrips();
	void triangulate_grid();
	void need_faces()
	{
		if (!faces.empty())
			return;
		if (!tstrips.empty())
			unpack_tstrips();
		else if (!grid.empty())
			triangulate_grid();
	}
	void need_normals();
	void need_pointareas();
	void need_curvatures();
	void need_dcurv();
	void need_bbox();
	void need_bsphere();
	void need_neighbors();
	void need_adjacentfaces();
	void need_across_edge();

	//
	// Delete everything
	//
	void clear()
	{
		vertices.clear(); faces.clear(); tstrips.clear();
		grid.clear(); grid_width = grid_height = -1;
		colors.clear(); confidences.clear();
		flags.clear(); flag_curr = 0;
		normals.clear(); pdir1.clear(); pdir2.clear();
		curv1.clear(); curv2.clear(); dcurv.clear();
		cornerareas.clear(); pointareas.clear();
		bbox.valid = bsphere.valid = false;
		neighbors.clear(); adjacentfaces.clear(); across_edge.clear();
	}

	//
	// Input and output
	//
protected:
	static bool read_helper(const char *filename, TriMesh *mesh);
public:
	static TriMesh *read(const char *filename);
	bool write(const char *filename);


	//
	// Useful queries
	//

	// Is vertex v on the mesh boundary?
	bool is_bdy(int v)
	{
//cout<<"trimesh::is_bdy "<<v<<endl;
		if (neighbors.empty()) need_neighbors();
//cout<<"trimesh::is_bdy 2"<<v<<endl;
		if (adjacentfaces.empty()) need_adjacentfaces();
//cout<<"trimesh::is_bdy 3"<<v<<endl;
		return neighbors[v].size() != adjacentfaces[v].size();
	}

	// Centroid of face f
	vec centroid(int f)
	{
		if (faces.empty()) need_faces();
		return (1.0f / 3.0f) *
			(vertices[faces[f][0]] +
			 vertices[faces[f][1]] +
			 vertices[faces[f][2]]);
	}

	// Normal of face f
	vec trinorm(int f)
	{
		if (faces.empty()) need_faces();
		return ::trinorm(vertices[faces[f][0]], vertices[faces[f][1]],
			vertices[faces[f][2]]);
	}

	// Angle of corner j in triangle i
	float cornerangle(int i, int j)
	{
		if (faces.empty()) need_faces();
		const point &p0 = vertices[faces[i][j]];
		const point &p1 = vertices[faces[i][(j+1)%3]];
		const point &p2 = vertices[faces[i][(j+2)%3]];
		return acos((p1 - p0) DOT (p2 - p0));
	}

	// Dihedral angle between face i and face across_edge[i][j]
	float dihedral(int i, int j)
	{
		if (across_edge.empty()) need_across_edge();
		if (across_edge[i][j] < 0.0f) return 0.0f;
		vec cr = trinorm(i) CROSS trinorm(across_edge[i][j]);
		vec edge = vertices[faces[i][(j+2)%3]] -
		           vertices[faces[i][(j+1)%3]];
		return len(cr) * sgn(cr DOT edge);
	}

	// Statistics
	float stat(StatOp op, StatVal val);
	float feature_size();

	//
	// Debugging
	//

	// Debugging printout, controllable by a "verbose"ness parameter
	static int verbose;
	static void set_verbose(int);
	static void (*dprintf_hook)(const char *);
	static void set_dprintf_hook(void (*hook)(const char *));
	static void dprintf(const char *format, ...);

	// Same as above, but fatal-error printout
	static void (*eprintf_hook)(const char *);
	static void set_eprintf_hook(void (*hook)(const char *));
	static void eprintf(const char *format, ...);

};


// Nonmember bbox operators
inline const TriMesh::BBox operator + (const TriMesh::BBox &b, const point &p)
{
	return TriMesh::BBox(b) += p;
}

inline const TriMesh::BBox operator + (const point &p, const TriMesh::BBox &b)
{
	return TriMesh::BBox(b) += p;
}

inline const TriMesh::BBox operator + (const TriMesh::BBox &b1, const TriMesh::BBox &b2)
{
	return TriMesh::BBox(b1) += b2;
}

#endif
