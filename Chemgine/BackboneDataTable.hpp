#include "DataTable.hpp"
#include "BackboneData.hpp"

class BackboneDataTable :
	public DataTable<ComponentIdType, std::string, BackboneData>
{
public:
	BackboneDataTable();
	BackboneDataTable(const BackboneDataTable&) = delete;

	bool loadFromFile(const std::string& path);
};