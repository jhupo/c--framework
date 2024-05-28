#include <logger/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <json.h>

#include <fstream>

COMMON_BEGIN_NAMESPACE

namespace logger{

    namespace{
            const char default_logger_flush[]           = "flush";
            const char default_logger_dest[]            = "destination";
            const char default_logger_category[]        = "category";
            const char default_logger_path[]            = "path";
            const char default_logger_pattern[]         = "pattern";
            const char default_logger_file_size[]       = "fileSize";
            const char default_logger_max_file[]        = "maxFile";
            const char default_logger_level[]           = "level";
            const char default_logger_flush_interval[]  = "interval";
        }

    bool init_logger(const std::string& config, const std::string& channel)
    {
        Json::Value json;
        Json::Value channel_json;
        Json::Value flush_json;
        std::string error;
        std::ifstream infile(config.c_str());
        if(!infile.is_open()){
            std::cerr << "Failed to open logger configuration file: " << config << std::endl;
            return false;
        }
        std::string content((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
        std::shared_ptr<Json::CharReader> reader(Json::CharReaderBuilder().newCharReader());
        if(!reader->parse(content.c_str(),content.c_str() + content.size(),&json,&error)){
            std::cerr << "Failed to parse logger configuration: " << error << std::endl;
            return false;
        }

        if(json[channel].isNull()){
            std::cerr << "Failed to find channel: " << channel << std::endl;
            return false;
        }

        channel_json = json[channel];
        flush_json = json[default_logger_flush];

        spdlog::sink_ptr sinks;

        if("TextFile" == channel_json[default_logger_dest].asString()){
            sinks = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                    channel_json[default_logger_path].asString(),
                    channel_json[default_logger_file_size].asInt(),
                    channel_json[default_logger_max_file].asInt());
        }else{
            sinks = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        }
        sinks->set_pattern(channel_json[default_logger_pattern].asString());
        sinks->set_level(spdlog::level::from_str(channel_json[default_logger_level].asString()));
        auto logger = std::make_shared<spdlog::logger>(channel_json[default_logger_category].asString(),sinks);
        spdlog::flush_every(std::chrono::seconds(flush_json[default_logger_flush_interval].asInt()));
        spdlog::flush_on(spdlog::level::from_str(flush_json[default_logger_level].asString()));
        spdlog::set_default_logger(logger);
        return true;
    }

}


COMMON_END_NAMESPACE