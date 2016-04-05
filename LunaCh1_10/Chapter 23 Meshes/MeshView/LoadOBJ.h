#include "MeshGeometry.h"
#include "LightHelper.h"
#include "Vertex.h"

struct OBJMaterial
{
	Material Mat;
	bool AlphaClip;
	std::string EffectTypeName;
	std::wstring DiffuseMapName;
	std::wstring NormalMapName;
};

class OBJLoader
{
public:
	bool LoadOBJ(const std::string& filename,
		std::vector<Vertex::PosNormalTexTan>& vertices,
		std::vector<USHORT>& indices,
		std::vector<MeshGeometry::Subset>& subsets,
		std::vector<OBJMaterial>& mats);

private:
	void ReadMaterials(std::ifstream& fin, UINT numMaterials, std::vector<OBJMaterial>& mats);
	void ReadSubsetTable(std::ifstream& fin, UINT numSubsets, std::vector<MeshGeometry::Subset>& subsets);
	void ReadVertices(std::ifstream& fin, UINT numVertices, std::vector<Vertex::PosNormalTexTan>& vertices);
	void ReadTriangles(std::ifstream& fin, UINT numTriangles, std::vector<USHORT>& indices);
	void ReadOBJ(std::ifstream& fin, std::vector<OBJMaterial>& mats, std::vector<MeshGeometry::Subset>& subsets, 
		std::vector<Vertex::PosNormalTexTan>& vertices, std::vector<USHORT>& indices);
};