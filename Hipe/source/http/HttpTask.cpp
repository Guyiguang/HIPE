#include <HttpTask.h>
#include <HttpServer.h>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <json/JsonBuilder.h>
#include <orchestrator/Orchestrator.h>
#include <filter/data/Composer.h>
#include <core/HipeException.h>

#ifdef USE_GPERFTOOLS
#include <gperftools/heap-checker.h>
#include <assert.h>
#endif


using namespace boost::property_tree;
using namespace std;

core::Logger http::HttpTask::logger = core::setClassNameAttribute("HttpTask");


void http::HttpTask::runTask()
{
	#ifdef USE_GPERFTOOLS
	static int iteration_leak=0;
	HeapLeakChecker heap_checker("HttpTask");
	#endif
	{
		try {
			std::stringstream dataResponse;

		
			ptree treeRequest;
			ptree treeResponse;
			ptree treeResponseInfo;

			read_json(_request->content, treeRequest);
			if (treeRequest.count("command") != 0)
			{
				std::string command = treeRequest.get_child("command").get<std::string>("type");
				if (command.find("kill") != std::string::npos)
				{
					orchestrator::OrchestratorFactory::getInstance()->killall();
				}
				ptree ltreeResponse;
				ltreeResponse.add("Status", "Task has been killed");
				std::stringstream ldataResponse;
				write_json(ldataResponse, ltreeResponse);
				*_response << "HTTP/1.1 200 OK\r\n"
					<< "Access-Control-Allow-Origin: *\r\n"
					<< "Content-Type: application/json\r\n"
					<< "Content-Length: " << ldataResponse.str().length() << "\r\n\r\n"
					<< ldataResponse.str();
				HttpTask::logger << "HttpTask response has been sent";
				HttpTask::logger << ldataResponse.str();

				return;
			}

			HttpTask::logger << "Check if algorithm need to be built";
			auto json_filter_tree = json::JsonBuilder::buildAlgorithm(dataResponse, treeRequest);
			treeResponseInfo.add("Algorithm", dataResponse.str());
			dataResponse.str(std::string());

			HttpTask::logger << "Check if orchestrator need to be built";
			auto orchestrator = json::JsonBuilder::getOrBuildOrchestrator(dataResponse, treeRequest);
			treeResponseInfo.add("Orchestrator", dataResponse.str());
			dataResponse.str(std::string());

			stringstream strlog;
			strlog << "Bind algorithm ";
			strlog << json_filter_tree->getName() << " to orchestrator " << orchestrator;

			HttpTask::logger << strlog.str();

			orchestrator::OrchestratorFactory::getInstance()->bindModel(json_filter_tree->getName(), orchestrator);
			treeResponseInfo.add("Binding", "OK");
			treeResponse.add_child("Status", treeResponseInfo);

			std::stringstream status;
			write_json(status, treeResponseInfo);
			HttpTask::logger << "Response info :\n" << status.str();

			//Check if data is present
			if (treeRequest.count("data") != 0)
			{
				filter::data::Data data = filter::data::Composer::getDataFromComposer(treeRequest.get_child("data"));
			
				if (data.getType() == filter::data::IODataType::LISTIO)
				{
					filter::data::ListIOData & list_io_data = static_cast<filter::data::ListIOData&>(data);
				}

				//Start processing Algorithm with data
				filter::data::Data outputData;

				orchestrator::OrchestratorFactory::getInstance()->process(json_filter_tree->getName(), data, outputData);

				//after the process execution Data should be an OutputData type
				filter::data::OutputData & output_data = static_cast<filter::data::OutputData &>(outputData);
			
			
				treeResponse.add_child("dataResponse", output_data.resultAsJson());
			}
			write_json(dataResponse, treeResponse);
		

			*_response << "HTTP/1.1 200 OK\r\n"
						<< "Access-Control-Allow-Origin: *\r\n"
					   << "Content-Type: application/json\r\n"
					   << "Content-Length: " << dataResponse.str().length() << "\r\n\r\n"
					   << dataResponse.str();
			HttpTask::logger << "HttpTask response has been sent";
			HttpTask::logger << dataResponse.str();
		}
		catch (std::exception& e) {
			ptree treeResponse;
			treeResponse.add("Status", e.what());
			std::stringstream dataResponse;
			write_json(dataResponse, treeResponse);
			*_response << "HTTP/1.1 200 OK\r\n"
			<< "Access-Control-Allow-Origin: *\r\n"
				<< "Content-Type: application/json\r\n"
				<< "Content-Length: " << dataResponse.str().length() << "\r\n\r\n"
				<< dataResponse.str();
		}

		catch (HipeException& e) {
			ptree treeResponse;
			treeResponse.add("Status", e.what());
			std::stringstream dataResponse;
			write_json(dataResponse, treeResponse);
			*_response << "HTTP/1.1 200 OK\r\n"
				<< "Access-Control-Allow-Origin: *\r\n"
				<< "Content-Type: application/json\r\n"
				<< "Content-Length: " << dataResponse.str().length() << "\r\n\r\n"
				<< dataResponse.str();
		}
	}
#ifdef USE_GPERFTOOLS
	iteration_leak++;
	//if (iteration_leak == 10)
	{
		if (!heap_checker.NoLeaks()) assert(NULL == "heap memory leak");
	}
			
#endif
	
}
