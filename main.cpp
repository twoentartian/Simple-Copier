#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <filesystem>

#include "configure_file.hpp"


int main()
{
	configuration_file file;
	configuration_file::json default_config;
	default_config["src_dir"] = {"./src1", "./src2"};
	default_config["copy_ext_name"] = "plot";
	default_config["dst_dir"] = {"./dst1", "./dst2"};
	file.SetDefaultConfiguration(default_config);
	file.LoadConfiguration("config.json");
	
	auto src_dir = file.get<std::vector<std::string>>("src_dir");
	auto dst_dir = file.get<std::vector<std::string>>("dst_dir");
	auto copy_ext_name = file.get<std::string>("copy_ext_name");
	
	//check empty
	if (!src_dir.has_value())
	{
		std::cerr << "Error, empty src dor." << std::endl;
		return -1;
	}
	if (!dst_dir.has_value())
	{
		std::cerr << "Error, empty dst dor." << std::endl;
		return -1;
	}
	if (!copy_ext_name.has_value())
	{
		std::cerr << "Error, empty copy extension name." << std::endl;
		return -1;
	}
	
	//check src dir
	for (int i = 0; i < src_dir->size(); ++i)
	{
		if (!std::filesystem::exists((*src_dir)[i]))
		{
			std::cerr << "Error, src folder does not exist: " << (*src_dir)[i] << std::endl;
			return -1;
		}
	}
	
	//check dst dir
	for (int i = 0; i < dst_dir->size(); ++i)
	{
		if (!std::filesystem::exists((*dst_dir)[i]))
		{
			std::cerr << "Error, dst folder does not exist: " << (*dst_dir)[i] << std::endl;
			return -1;
		}
	}
	
	while (true)
	{
		for (int src_index = 0; src_index < src_dir->size(); ++src_index)
		{
			for (auto&fe : std::filesystem::directory_iterator((*src_dir)[src_index]))
			{
				auto fp = fe.path();
				auto file_name = fp.filename();
				if (file_name.extension().string().find(*copy_ext_name) != -1)
				{
					try
					{
						std::cout << "[INFO] Find " << file_name.string() << ", copying." << std::endl;
						std::uintmax_t src_size = std::filesystem::file_size(fp);
						
						//find dst
						std::string dest_path;
						for (int i = 0; i < dst_dir->size(); ++i)
						{
							std::filesystem::space_info si = std::filesystem::space((*dst_dir)[i]);
							if (src_size <= si.available)
							{
								dest_path = (*dst_dir)[i];
								std::cout << "[INFO] Choose dest dir " << dest_path << "." << std::endl;
								break;
							}
						}
						if (dest_path.empty())
						{
							std::cout << "[INFO] All dest paths are full, press ENTER to start next." << std::endl;
							std::string temp;
							std::getline(std::cin, temp);
						}
						
						auto dest_target = dest_path/fp.filename();
						std::filesystem::copy(fp,dest_target, std::filesystem::copy_options::overwrite_existing);
						
						//check success
						std::uintmax_t dst_size = std::filesystem::file_size(dest_target);
						
						if (dst_size == src_size)
						{
							std::cout << "[INFO] Deleting " << file_name.string() << "." << std::endl;
							std::filesystem::remove(fp);
						}
						else
						{
							std::cout << "[WARN] Size mismatch: " << file_name.string() << "." << std::endl;
						}
					}
					catch (std::exception& e)
					{
						std::cerr << "Error, failed to copy or delete " << fp << std::endl;
					}
				}
			}
		}
		

		std::cout << "[INFO] Finish iteration, wait for 1 minutes." << std::endl;
		std::this_thread::sleep_for(std::chrono::minutes(1));
		
	}
	
	return 0;
}
