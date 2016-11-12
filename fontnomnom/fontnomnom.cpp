#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <locale>
#include <codecvt>

#include "create.h"
#include "read.h"

static std::wstring stringToWstring(const std::string& t_str)
{
	//setup converter
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	return converter.from_bytes(t_str);
}

int main(int argc, char **argv)
{
	int ret = EXIT_FAILURE;

	if (argc < 2)
	{
		CreateTemplate(Rect(Point(), Size(100, 200)), L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmonpqrstuvwxyz.!,;:\"'#/$%?&*()_+=");
		ret = EXIT_SUCCESS;
	}
	else
	{
		std::string file = argv[1];

		if (file.substr(file.size() - 4) == ".png")
		{
			try
			{
				ReadTemplate(file);
				ret = EXIT_SUCCESS;
			}
			catch (PNG_Exception e)
			{
				std::cerr << e.GetError() << std::endl;
			}
			catch (const char *e)
			{
				std::cerr << e << " : aborting." << std::endl;
			}
		}
		else
		{
			std::ifstream ifs(file.c_str());

			if (ifs)
			{
				bool success = false;
				Json::Value root;

				{
					Json::Reader reader;
					success = reader.parse(ifs, root);
				}

				if (success && root["fontnomnom"].isObject())
				{
					Json::Value fnn = root["fontnomnom"];

					if (fnn["size"].isArray() && fnn["size"][0].isInt() && fnn["size"][1].isInt() && fnn["string"].isString())
					{					
						std::wstring ws = stringToWstring(fnn["string"].asString());				
						CreateTemplate(Rect(Point(), Size(fnn["size"][0].asInt(), fnn["size"][1].asInt())), ws);
						ret = EXIT_SUCCESS;
					}
				}
			}
		}
	}

	system("pause");
	return ret;
}