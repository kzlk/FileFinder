#pragma once
#include "stdafx.h"
#define MAX_THREAD 8
namespace fs = std::filesystem;

class thread_pool
{
public:

	explicit thread_pool(int threads);
    thread_pool();
    ~thread_pool();

    void execute(std::function <void()> func);

protected:

    void threadEntry(int i);
    std::mutex lock_; 
    std::condition_variable condVar_;
    bool shutdown_ = false;

    std::queue <std::function <void()>> jobs_;

    std::vector <std::thread> threads_;


};

class finder_engine: public thread_pool
{
public:
    finder_engine();
    ~finder_engine();

     std::vector<std::string>
        getListSubDir(std::string const& dir, std::string const& app_name);

    static inline std::vector<std::string> getListOfDrives();

    inline int get_num_thread();

    std::string get_path_by_name(std::string const& app_name);

    

private:
    std::string path = "NoFound";
    int available_thread = 0;
    void Finder(std::string const& dir, std::string const& name, std::string& result);
    std::atomic<bool> exit_thread_flag{ false };
   
    
};

