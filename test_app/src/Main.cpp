#include "io/Log.hpp"
#include "utils/Build.hpp"
#include "common/TestManager.hpp"

#include <cxxopts.hpp>

int main(int argc, char* argv[])
{
    try
    {
        cxxopts::Options options(argv[0], "Application for running Chemgine tests");
        options.add_options()
            ("u,unit", "Enables unit tests")
            ("p,perf", "Enables performance tests")
            ("f,filter", "Filters tests using the given ECMAScript regex", cxxopts::value<std::string>())
            ("log", "Sets logging level", cxxopts::value<std::string>())
            ("h,help", "Print usage information");

        const auto args = options.parse(argc, argv);
        if (argc == 1 || args.count("help"))
        {
            std::cout << options.help() << '\n';
            return 0;
        }

        std::cerr << "HERE 0\n";

        const auto logLevelStr = args.count("log") ? args["log"].as<std::string>() : "INFO";
        const auto logLevel = LogBase::parseLogType(logLevelStr);
        if(not logLevel)
        {
            Log().fatal("Failed to parse log level: '{0}'.", logLevelStr);
            CHG_UNREACHABLE();
        }
        if (not LogBase::isLogTypeEnabled(*logLevel))
        {
            Log().fatal("The specified log level: '{0}' is disabled on this build.", logLevelStr);
            CHG_UNREACHABLE();
        }
        LogBase::settings().logLevel = *logLevel;
        
        const auto filterStr = args.count("filter") ? args["filter"].as<std::string>() : ".*";
        std::regex filter(filterStr, std::regex::ECMAScript | std::regex::optimize);

        std::cerr << "HERE 1\n";
        TestManager tests(std::move(filter));

        std::cerr << "HERE 2\n";
        const auto runUnit = args["unit"].as<bool>();
        const auto runPerf = args["perf"].as<bool>();
        if (runUnit)
            tests.runUnit();
        if (runPerf)
            tests.runPerf();
    }
    catch (const cxxopts::exceptions::exception& e)
    {
        Log().fatal("Option parsing failed with error:\n{0}", e.what());
        CHG_UNREACHABLE();
    }
    catch (const std::regex_error& e)
    {
        Log().fatal("Regex creation failed with error:\n{0}", e.what());
        CHG_UNREACHABLE();
    }

	return 0;
}
