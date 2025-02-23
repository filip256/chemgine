#include "DataStore.hpp"
#include "PathUtils.hpp"
#include "Log.hpp"

#include <cxxopts.hpp>

int main(int argc, char* argv[])
{
    try
    {
        cxxopts::Options options(argv[0], "Application for running Chemgine tests");
        options.add_options()
            ("input", "Input file", cxxopts::value<std::string>())
            ("o,output", "Output file", cxxopts::value<std::string>())
            ("p,pretty", "Prettifies the output")
            ("log", "Sets logging level", cxxopts::value<std::string>())
            ("h,help", "Print usage information");
        options.parse_positional({ "input" });

        const auto args = options.parse(argc, argv);
        if (args.count("help"))
        {
            std::cout << options.help() << '\n';
            return 0;
        }

        const auto logLevelStr = args.count("log") ? args["log"].as<std::string>() : "INFO";
        if (const auto logLevel = LogBase::parseLogType(logLevelStr))
            LogBase::logLevel = *logLevel;
        else
        {
            Log().fatal("Failed to parse log level: '{0}'.", logLevelStr);
            return 1;
        }

        if (not args.count("input"))
        {
            Log().fatal("Missing input file.");
            return 1;
        }

        DataStore dataStore;
        Accessor<>::setDataStore(dataStore);
        const auto inputFile = args["input"].as<std::string>();
        if (not dataStore.load(inputFile))
        {
            Log().fatal("Failed to load file: '{0}'.", inputFile);
            return 1;
        }

        if (not args.count("output"))
        {
            Log().info("Not output file was specified, dump skipped.");
            return 0;
        }

        const auto outputFile = args["output"].as<std::string>();
        Utils::createDir(Utils::extractDirName(outputFile));

        const auto prettify = args["pretty"].as<bool>();
        dataStore.dump(outputFile, prettify);
            
        Log().info("Dumped output to file: '{0}'.", outputFile);
    }
    catch (const cxxopts::exceptions::exception& e)
    {
        Log().fatal("Option parsing failed.\n{0}", e.what());
        return 1;
    }

    return 0;
}
