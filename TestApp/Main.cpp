#include "Common/TestManager.hpp"
#include "Log.hpp"

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
        if (args.count("help"))
        {
            std::cout << options.help() << '\n';
            return 0;
        }

        const auto logLevelStr = args.count("log") ? args["log"].as<std::string>() : "INFO";
        if (const auto logLevel = LogBase::parseLogLevel(logLevelStr))
            LogBase::logLevel = *logLevel;
        else
        {
            Log().fatal("Failed to parse log level: '{0}'.", logLevelStr);
            return 1;
        }

        const auto filterStr = args.count("filter") ? args["filter"].as<std::string>() : ".*";
        std::regex filter(filterStr, std::regex::ECMAScript | std::regex::optimize);

        TestManager tests(std::move(filter));

        const auto runUnit = args["unit"].as<bool>();
        const auto runPerf = args["perf"].as<bool>();
        if (runUnit)
            tests.runUnit();
        if (runPerf)
            tests.runPerf();
    }
    catch (const cxxopts::exceptions::exception& e)
    {
        Log().fatal("Option parsing failed.\n{0}", e.what());
        return 1;
    }
    catch (const std::regex_error& e)
    {
        Log().fatal("Regex creation failed.\n{0}", e.what());
        return 1;
    }

	return 0;
}
