#include <iostream>
#include <vector>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
//#include <boost/iostreams/stream.hpp>
//#include <boost/iostreams/copy.hpp>
//#include <boost/iostreams/filtering_stream.hpp>
//#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

#include <ncurses.h>

namespace po = boost::program_options;
//namespace io = boost::iostreams;

static unsigned int __strtoui(
	const std::string& str
	)
{
	std::string s(str);
	std::stringstream ss;
	unsigned int ret;

	boost::remove_erase_if(s, [](char c) { return std::isspace(c); });

	if (s.compare(0, 2, "0x") == 0)
	{
		ss << std::hex << str;
	}
	else if (s.compare(0, 1, "0") == 0)
	{
		ss << std::oct << str;
	}
	else
	{
		ss << std::dec << str;
	}
	ss >> ret;
	return ret;
}

static bool __parse_args(
	int ac,
	const char* av[],
	std::vector<char>& input_vector
	)
{
	po::options_description desc("Allowed options");
	po::variables_map vm;

	desc.add_options()
		("help,h", "procedure help message")
		("input-file,i", po::value<std::string>(), "input file")
		//("output-file,o", po::value<std::string>(), "output file")
		("begin,b", po::value<std::string>(), "begin")
		("end,e", po::value<std::string>(), "end")
		;

	try
	{
		po::store(po::parse_command_line(ac, av, desc), vm);
		po::notify(vm);

		if (vm.count("help"))
		{
			std::cout << desc << "\n";
			return false;
		}

		{
			std::string begin_str, end_str;
			unsigned int begin, end;

			begin_str = vm.count("begin") ? vm["begin"].as<std::string>() : "0";
			end_str =
				vm.count("end") ?
				vm["end"].as<std::string>() :
				std::string(boost::lexical_cast<std::string>(UINT_MAX));
			begin = __strtoui(begin_str);
			end = __strtoui(end_str);

			std::ifstream ifs(vm["input-file"].as<std::string>().c_str());
			ifs.seekg(begin);
			for(
				unsigned int index = begin;
				(index < end) && !(ifs.eof());
				index++)
			{
				input_vector.push_back(ifs.get());
			}
		}
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}

	return true;
}

class Editor
{
public:
	Editor(void) {};
	~Editor(void) {};

	void run(std::vector<char>& input_vector)
	{
		initscr();
		noecho();
		cbreak();

		keypad(stdscr, true);
		wtimeout(stdscr, 100);

		//BOOST_FOREACH(char &c, input_vector)
		std::vector<char>::iterator it = input_vector.begin();
		int index = 0;
		for ( ;
				it != input_vector.end();
				it++, index++)
		{
			//addch(c);
			if ((index % 2) == 0)
			{
				printw(" ");
			}
			if ((index % 8) == 0)
			{
				printw("\n");
			}
			printw("%x", (char)(*it));
			break;
		}

		while (true)
		{
			char ch = getch();
			if (ch == 'q')
			{
				break;
			}

			//mvprintw(10, 10, "%s", input_vector);
			//addch(ch);
			//addch(KEY_UP);
			refresh();
		}
		endwin();
	}
};

int main(int ac, const char* av[])
{
	std::vector<char> input_vector;

	if (! __parse_args(ac, av, input_vector))
	{
		return EXIT_FAILURE;
	}

	//for(unsigned int i = 0; i < input_vector.size(); i++)
	//{
	//	std::cout << input_vector[i];
	//}

	Editor e;
	e.run(input_vector);

	return EXIT_SUCCESS;
}

