#include <corefilter/tools/RegisterTable.h>
#include <corefilter/Model.h>
#include <stack>
#include <core/Invoker.h>
#include <corefilter/tools/filterMacros.h>

#include <corefilter/IFilter.h>
#include <corefilter/filter_export.h>
#include <mutex>

RegisterTable* registerInstance_l = nullptr;


void* newFilter(std::string className)
{
	return RegisterTable::getInstance().newObjectInstance(className);
}


filter::Model* copyFilter(filter::Model* filter)
{
	if (filter == nullptr) return nullptr;
	filter::Model* nFilter = RegisterTable::getInstance().newObjectInstance(filter->getConstructorName());
	nFilter->setName(filter->getName());
	nFilter->setLevel(filter->getLevel());

	for (auto& field : getParameterNames(filter->getConstructorName()))
	{
		std::string copyField(field);
		{
			__callfunc(nFilter, "copy_" + copyField, filter);
		}
	}
	return nFilter;
}

filter::Model* copyAlgorithms(filter::Model* root)
{
	if (root == nullptr) return nullptr;
	filter::Model* newRoot = copyFilter(root);

	filter::Model* copyParent;
	filter::Model* parent = static_cast<filter::IFilter*>(root);

	std::stack<filter::Model*> heap;
	std::stack<filter::Model*> copyHeap;

	heap.push(parent);
	copyHeap.push((filter::IFilter*)newRoot);
	std::map<std::string, filter::Model *> copyChildCache; //To avoid multiple instance of same object with the same name;

	while (!heap.empty())
	{
		parent = heap.top();
		copyParent = copyHeap.top();
		heap.pop();
		copyHeap.pop();

		for (auto childMap : parent->getChildrens())
		{
			filter::Model* copyChild = nullptr;
			if (copyChildCache.find(childMap.second->getName()) != copyChildCache.end())
			{
				copyChild = copyChildCache[childMap.second->getName()];
			}
			else
			{
				copyChild = static_cast<filter::Model*>(copyFilter(childMap.second));
				copyChildCache[childMap.second->getName()] = copyChild;

			}
			copyChild->addDependencies(copyParent);

			if (! childMap.second->getChildrens().empty())
			{
				heap.push(static_cast<filter::IFilter*>(childMap.second));
				copyHeap.push(copyChild);
			}

		}
	}


	return newRoot;
}

HipeStatus freeAlgorithms(filter::Model* root)
{
	if (root == nullptr) return OK;

	filter::Model* parent = static_cast<filter::IFilter*>(root);
	std::stack<filter::Model*> heap;
	std::map<filter::Model*, int> cacheFilter;

	heap.push(parent);
	cacheFilter[parent] = 0;;

	while (!heap.empty())
	{
		parent = heap.top();
		heap.pop();
		for (auto childMap : parent->getChildrens())
		{
			cacheFilter[childMap.second] = 0;
			if (!childMap.second->getChildrens().empty())
			{
				heap.push(childMap.second);
			}
		}
	}
	for (auto &filter : cacheFilter)
	{
		delete filter.first;
	}

	cacheFilter.clear();

	return OK;
}

RegisterTable* registerInstance()
{
	static std::mutex _mutex;

	_mutex.lock();
	if (registerInstance_l == nullptr)
	{
		registerInstance_l = new RegisterTable();
	}
	_mutex.unlock();

	return registerInstance_l;
}


const std::vector<std::string> getTypes(std::string className)
{
	return RegisterTable::getInstance().getMethodNames(className);
}

const std::vector<std::string> getParameterNames(std::string className)
{
	return RegisterTable::getInstance().getVarNames(className);
}
