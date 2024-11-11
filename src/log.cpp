#include <string>
#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>
#include <csignal>
#include <cstdarg>
#include <thread>
#include <iomanip>

#include <unistd.h>
#include <execinfo.h>
#include <glog/logging.h>

std::string globalLogPath;

class LogSink : public google::LogSink {
public:
    LogSink(const std::string& logFile) : logFile_(logFile) {
        manageLogFile();
        logStream_.open(logFile_, std::ios::app | std::ios::out);
        if (!logStream_.good()) {
            std::cerr << "Failed to open log file: " << logFile_ << std::endl;
        }
    }

    ~LogSink() { logStream_.flush(); }

    virtual void send(google::LogSeverity severity, const char* fullFileName, const char* baseFileName, int line,
                      const struct tm* tmTime, const char* message, size_t msgLen) override {
        if (logStream_.is_open()) {
            // Get current time with high resolution
            auto now = std::chrono::system_clock::now();
            auto now_us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000;

            // Format time using tm_time and microseconds
            std::ostringstream time_stream;
            time_stream << std::put_time(tmTime, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(6)
                        << now_us.count();

            // Write log entry
            logStream_ << "[" << google::LogSeverityNames[severity] << "] " << time_stream.str() << " "
                       << "threadId: " << std::dec << std::this_thread::get_id() << " " << baseFileName << ":" << line
                       << " " << std::string(message, msgLen) << std::endl;
        }
    }

private:
    std::string logFile_;
    std::ofstream logStream_;

    void manageLogFile() {
        if (::access(logFile_.c_str(), F_OK) == 0) {
            std::ifstream ifs(logFile_, std::ios::binary | std::ios::ate);
            size_t fileSize = 0;
            if (ifs.good()) {
                fileSize = ifs.tellg();
                ifs.close();
            }
            constexpr size_t maxSize = 10 * 1024 * 1024;
            if (fileSize >= maxSize) {
                ::remove(logFile_.c_str());
            }
        }
    }
};


extern "C" {

void signalHandler(int signum) {
    void* array[10];
    size_t size;

    size = backtrace(array, 10);

    char** messages = backtrace_symbols(array, size);

    std::ofstream logFile(globalLogPath.c_str(), std::ios_base::app);
    if (logFile.is_open()) {
        logFile << "Error: signal " << signum << std::endl;
        for (size_t i = 0; i < size; i++) {
            logFile << messages[i] << std::endl;
        }
        logFile.close();
    } else {
        std::cerr << "Failed to open log file: ./test.log" << std::endl;
    }

    free(messages);

    // 退出程序
    exit(signum);
}
}

void initLog(const std::string& logPath) {
    globalLogPath = logPath;
    google::InitGoogleLogging("secure_flash");
    FLAGS_logtostderr = false;
    FLAGS_alsologtostderr = false;

    FLAGS_log_prefix = true;
    FLAGS_logbufsecs = 0;
    FLAGS_max_log_size = 10;
    FLAGS_stop_logging_if_full_disk = true;

    static LogSink logSink(logPath);
    google::AddLogSink(&logSink);

    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);
}