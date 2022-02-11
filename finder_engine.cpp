#include "finder_engine.h"

finder_engine::finder_engine()
= default;

thread_pool::thread_pool(const int threads) : shutdown_(false)
{

    for (int i = 0; i < threads; ++i)
        threads_.emplace_back(std::bind(&thread_pool::threadEntry, this, i));
}

thread_pool::thread_pool()
= default;

thread_pool::~thread_pool()
{

    {

        std::unique_lock <std::mutex> l(lock_);
        shutdown_ = true;

    }

    condVar_.notify_all();

    for (auto& thread : threads_)
        thread.join();

    threads_.clear();
    stopped_ = true;
    std::cout << "**************Please Wait******************" << std::endl;

}

void thread_pool::execute(std::function<void()> func)
{
    std::unique_lock <std::mutex> l(lock_);
    jobs_.emplace(std::move(func));
    condVar_.notify_one();
}

void thread_pool::threadEntry(int i)
{
    std::function <void(void)> job;

    while (true)
    {
        {
            std::unique_lock <std::mutex> l(lock_);

            while (!shutdown_ && jobs_.empty())
                condVar_.wait(l);
            if (jobs_.empty())
            {

                return;
            }

            job = std::move(jobs_.front());
            jobs_.pop();
        }

        job();
    }

};

 std::vector<std::string> finder_engine::getListOfDrives()
{
    std::vector<std::string> arrayOfDrives;
    char* szDrives = new char[MAX_PATH]();
    if (GetLogicalDriveStringsA(MAX_PATH, szDrives))
        for (int i = 0; i < 100; i += 4)
            if (szDrives[i] != static_cast<char>(0))
                arrayOfDrives.push_back(std::string{ szDrives[i],szDrives[i + 1],szDrives[i + 2] });
    delete[] szDrives;
    return arrayOfDrives;
}

void finder_engine::Finder(std::string const& dir, std::string const& name, std::string& result)
{
    if (!exit_thread_flag)
    {
        std::vector<fs::path> pathes{ fs::recursive_directory_iterator(dir, fs::directory_options::skip_permission_denied), {} }; //,  


        for (auto& p : pathes)
        {
            if (!exit_thread_flag)
            {
                std::cout << "ID -> " << std::this_thread::get_id() << "    | ";
                _cputws(p.wstring().c_str());
                _cputws(L"\n");

                if (p.has_extension() && (p.filename() == name))

                {

                    exit_thread_flag = true;
                    result = p.string();

                }

            }

        }
    }
}

std::vector<std::string>
finder_engine::getListSubDir(std::string const& dir, std::string const& app_name)
{
    const std::string DB = "DATEBASE.txt";

    std::vector<std::string> base;

    std::vector<fs::path>
        pathes{ fs::directory_iterator(dir, fs::directory_options::skip_permission_denied), {} };

    for (auto& p : pathes)
    {
        if (p.has_extension() && ((p.filename().string() == app_name)))
        {
            base.clear();
            base.push_back(p.string());
            return base;
        }
        else if (!p.has_extension())
            base.push_back(p.string());

    }
    return base;
}

int finder_engine::get_num_thread()
{
    int available_thread = 0;
    if (const auto thread_count = std::thread::hardware_concurrency(); thread_count > MAX_THREAD)
        available_thread = MAX_THREAD;
    else available_thread = thread_count;
    return  available_thread;
} 

std::string finder_engine::get_path_by_name(std::string const& app_name)
{
   const std::vector<std::string> lg_d = getListOfDrives();
    std::string path = "NoFound";
    std::vector<std::string> list_root;


    for (int i = 0; i < lg_d.size(); i++)
    {
        std::vector<std::string> buf = getListSubDir(lg_d[i], app_name);
        std::vector<std::string>::const_iterator it = buf.begin();
        while (it != buf.end())
        {
            list_root.push_back(*it);
            ++it;
        }

    }

    thread_pool p (get_num_thread());

    int i = 0;

    while (i < list_root.size())
    {

        p.execute(std::bind([&path, app_name, list_root, i, this]()
            {
                if (!exit_thread_flag)
                {
	                const std::vector<fs::path> pathes{ fs::recursive_directory_iterator(list_root[i], fs::directory_options::skip_permission_denied), {}}; //,  


                    for (auto& p : pathes)
                    {
                        if (!exit_thread_flag)
                        {
                            std::cout << "ID -> " << std::this_thread::get_id() << "    | ";
                            _cputws(p.wstring().c_str());
                            _cputws(L"\n");

                            if (p.has_extension() && (p.filename() == app_name))

                            {

                                exit_thread_flag = true;
                                path = p.string();

                            }

                        }

                    }
                }

            }));

        i++;
    }

    p.~thread_pool();

    //TODO if need check firstly root disk, and after another

    /*if(path == "NoFound")
    {
        j++;
        if(j > lg_d.size())
        {
            return path;
        }
        std::cout << "App Not Found on drive: " << lg_d[j-1] << "\n Trying drive: " << lg_d[j] << std::endl;
        std::cin.get();
        std::cin.get();
        std::cout << "Enter to search on other Drive" << std::endl;
        return  get_path_by_name(app_name);
    }*/

    return path;
}