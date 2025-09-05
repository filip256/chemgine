#include "common/TestManager.hpp"
#include "data/def/Parsers.hpp"
#include "io/Log.hpp"
#include "perf/PerformanceReport.hpp"
#include "utils/Build.hpp"
#include "utils/Path.hpp"

#include <cxxopts.hpp>

int main(int argc, char* argv[])
{
    try {
        cxxopts::Options options(argv[0], "Application for running Chemgine tests");
        // clang-format off
        options.add_options()
            ("u,unit", "Enables unit tests")
            ("p,perf", "Enables performance tests and specifies report output path", cxxopts::value<std::string>())
            ("c,compare", "Compares sequentially the given comma-separated list of reports", cxxopts::value<std::string>())
            ("f,filter", "Filters tests using the given ECMAScript regex", cxxopts::value<std::string>())
            ("log", "Sets logging level", cxxopts::value<std::string>())
            ("h,help", "Print usage information");
        // clang-format on

        const auto args = options.parse(argc, argv);
        if (argc == 1 || args.count("help")) {
            std::cout << options.help() << '\n';
            return 0;
        }

        const auto logLevelStr = args.count("log") ? args["log"].as<std::string>() : "INFO";
        const auto logLevel    = LogBase::parseLogType(logLevelStr);
        if (not logLevel) {
            Log().fatal("Failed to parse log level: '{0}'.", logLevelStr);
            CHG_UNREACHABLE();
        }
        if (not LogBase::isLogTypeEnabled(*logLevel)) {
            Log().fatal("The specified log level: '{0}' is disabled on this build.", logLevelStr);
            CHG_UNREACHABLE();
        }
        LogBase::settings().logLevel = *logLevel;

        const auto filterStr = args.count("filter") ? args["filter"].as<std::string>() : ".*";
        std::regex filter(filterStr, std::regex::ECMAScript | std::regex::optimize);

        std::unique_ptr<TestManager> testManager;
        int                          returnCode = 0;

        if (args.count("unit")) {
            if (not testManager)
                testManager = std::make_unique<TestManager>(std::move(filter));

            returnCode = testManager->runUnit() ? 0 : 2;
        }

        if (args.count("perf")) {
            if (not testManager)
                testManager = std::make_unique<TestManager>(std::move(filter));

            const auto outputPath = utils::normalizePath(args["perf"].as<std::string>());
            if (outputPath.empty()) {
                Log().warn("Output path is empty, no report will be dumped.");
                testManager->runPerf();
            }
            else {
                if (const auto dir = utils::extractDirName(outputPath); dir.size())
                    utils::createDir(dir);

                const auto report = testManager->runPerf();
                report.dump(outputPath);
                Log().info("Dumped report to: '{}'.", outputPath);
            }
        }

        if (args.count("compare")) {
            const auto& compareList = args["compare"].as<std::string>();
            const auto  reportPaths = def::parse<std::vector<std::string>>(compareList);
            if (not reportPaths || reportPaths->size() < 2) {
                Log().fatal("Invalid compare report list: '{}'.", compareList);
            }

            auto* prevReportPath = &reportPaths->front();
            auto  prevReport     = PerformanceReport::fromFile(*prevReportPath);
            if (not prevReport) {
                Log().fatal("Failed to load report: '{}'.", *prevReportPath);
            }

            for (size_t i = 1; i < reportPaths->size(); ++i) {
                auto* currentReportPath = &(*reportPaths)[i];
                auto  currentReport     = PerformanceReport::fromFile(*currentReportPath);
                if (not currentReport) {
                    Log().fatal("Failed to load report: '{}'.", *currentReportPath);
                }

                Log().info("Compare '{}' -> '{}':", *prevReportPath, *currentReportPath);
                LogBase::settings().outputStream << currentReport->compare(*prevReport) << std::endl;

                prevReportPath = currentReportPath;
                prevReport.emplace(std::move(*currentReport));
            }
        }

        return returnCode;

    } catch (const cxxopts::exceptions::exception& e) {
        Log().fatal("Option parsing failed with error:\n{0}", e.what());
        CHG_UNREACHABLE();
    } catch (const std::regex_error& e) {
        Log().fatal("Regex creation failed with error:\n{0}", e.what());
        CHG_UNREACHABLE();
    }
}
