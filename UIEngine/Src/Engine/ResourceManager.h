#pragma once
#include <memory>
#include <string>
#include <unordered_map>

template <class T>
class ResourceManager
{
private:
	std::unordered_map<std::string, std::shared_ptr<T>> Map;
	std::string Name;

	void ReleaseAll() const { Map.clear(); }

public:
	std::shared_ptr<T> Load(const std::string& filename, void* args = nullptr)
	{
		//S_ASSERT(!filename.empty(), "filename cannot be null")

		auto it = Map.find(filename);

		if (it != Map.end())
		{
			return (*it).second;
		}

		std::shared_ptr<T> resource = std::make_shared<T>(filename, args);

		Map.insert(std::make_pair(filename, resource));

		return resource;
	}

	// So far only used for shaders
	std::shared_ptr<T> Load(const std::string& filename, const std::string& secondFileName) { return Load(std::string(filename), (void*)secondFileName.c_str()); }

	std::shared_ptr<T> Load(std::shared_ptr<T> resource)
	{
		auto it = Map.find(resource->GetName());

		if (it != Map.end())
		{
			return (*it).second;
		}

		Map.insert(std::make_pair(resource->GetName(), resource));

		return resource;
	}

	bool Unload(const std::string_view filename) const
	{
		//S_ASSERT(!filename.empty(), "filename cannot be null")

		std::string FileName = filename.data();

		auto it = Map.find(FileName);

		if (it != Map.end())
		{
			Map.erase(it);
			return true;
		}

		//S_ASSERT(false, "cannot find " + FileName)
		return false;
	}

	void Initialise(const std::string_view name)
	{
		//S_ASSERT(!name.empty(), "Null m_name is not allowed")

		Name = name;
	}

	const std::string& GetName() const { return Name; }

	const int Size() const { return Map.size(); }
};
