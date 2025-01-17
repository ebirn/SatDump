#include "dataset.h"
#include "nlohmann/json_utils.h"
#include "common/utils.h"

namespace satdump
{
    void ProductDataSet::save(std::string path)
    {
        nlohmann::json json_ojb;
        json_ojb["satellite"] = satellite_name;
        json_ojb["timestamp"] = timestamp;
        json_ojb["products"] = products_list;

        saveJsonFile(path + "/dataset.json", json_ojb);
    }

    void ProductDataSet::load(std::string path)
    {
        nlohmann::json json_ojb;

        if (path.find("http") == 0)
        {
            std::string res;
            if (perform_http_request(path, res))
                throw std::runtime_error("Could not download from : " + path);
            json_ojb = nlohmann::json::parse(res);
        }
        else
        {
            json_ojb = loadJsonFile(path);
        }

        satellite_name = json_ojb["satellite"].get<std::string>();
        timestamp = json_ojb["timestamp"].get<double>();
        products_list = json_ojb["products"].get<std::vector<std::string>>();
    }
}
