#pragma once
#include <string>

class Resource
{
	template <class T>
	friend class ResourceManager;

public:
	Resource() = default;
	virtual ~Resource(){};

	const std::string& GetResourceFileName() const { return ResourceFileName; }

	Resource(const std::string_view resourcefilename, void* args) { ResourceFileName = resourcefilename; }

	// Misc Functions
	//------------------
	void SetName(const std::string_view newName) { name = newName; }

	std::string_view GetName() const { return name; }

protected:
	Resource(const Resource&) {}

	Resource& operator=(const Resource&) { return *this; }

	// resource filename
	std::string ResourceFileName;

	// resource filename
	std::string name;
};
