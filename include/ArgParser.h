#pragma once
#include <iostream>
#include <string>
#include <cassert>
#include <map>
#include <functional>

namespace Leaf
{
template <typename T>
class ArgParser {
    using OptionParser = std::function<void(T &, ArgParser &)>;
    T priv_;
    std::map<std::string, OptionParser> optParserMap_;
    OptionParser completionHook_;
    int currArgIdx_;
    int currArgc_;
    char **currArgv_;
    std::string errorMsg_;

public:
    ArgParser()
    : completionHook_([](T&, ArgParser &){}),
      currArgIdx_(-1),
      currArgc_(-1),
      currArgv_(nullptr)
    {
    }

    virtual ~ArgParser()
    {
    }

    void add(const std::string &option, OptionParser optParser)
    {
        optParserMap_.insert(std::make_pair(option, optParser));
    }

    void setCompletionHook(OptionParser hook)
    {
        completionHook_ = hook;
    }

    bool hasNext(void) const
    {
        return currArgIdx_ < currArgc_ - 1;
    }

    const char *getNext(void)
    {
        assert(hasNext());
        assert(currArgv_);
        currArgIdx_++;
        return currArgv_[currArgIdx_];
    }

    bool parse(int argc, char **argv)
    {
        currArgc_ = argc;
        currArgv_ = argv;
        for (currArgIdx_ = 1; currArgIdx_ < currArgc_; currArgIdx_++) {
            const std::string &opt = currArgv_[currArgIdx_];
            auto optParserItr = optParserMap_.find(opt);
            if (optParserItr != optParserMap_.end())
                (optParserItr->second)(priv_, *this);
            else
                error("Unknown option: " + opt);
            if (!errorMsg_.empty())
                return false;
        }
        completionHook_(priv_, *this);
        return errorMsg_.empty();
    }

    void error(const std::string &msg)
    {
        errorMsg_ = msg;
    }

    const std::string &getErrorMessage(void) const
    {
        return errorMsg_;
    }

    const T &getPrivateData(void) const
    {
        return priv_;
    }

    T &getPrivateData(void)
    {
        return priv_;
    }
};

}
