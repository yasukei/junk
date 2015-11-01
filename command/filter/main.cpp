#include <iostream>
#include <boost/program_options.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

namespace po = boost::program_options;
namespace io = boost::iostreams;

static bool __parse_args(
	int ac,
	const char* av[],
	io::filtering_istream& in,
	io::filtering_ostream& out
	)
{
	po::options_description desc("Allowed options");
	po::variables_map vm;

	desc.add_options()
		("help,h", "procedure help message")
		("input-file,i", po::value<std::string>(), "input file")
		("output-file,o", po::value<std::string>(), "output file")
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

		if (vm.count("input-file"))
		{
			in.push(io::file_descriptor_source(vm["input-file"].as<std::string>()));
		}
		else
		{
			in.push(std::cin);
		}

		if (vm.count("output-file"))
		{
			out.push(io::file_descriptor_sink(vm["output-file"].as<std::string>()));
		}
		else
		{
			out.push(std::cout);
		}
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}

	return true;
}

int main(int ac, const char* av[])
{
	io::filtering_istream in;
	io::filtering_ostream out;

	if (! __parse_args(ac, av, in, out))
	{
		return EXIT_FAILURE;
	}

	io::copy(in, out);

	return EXIT_SUCCESS;
}

