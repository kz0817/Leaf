#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <map>
#include <functional>
#include <initializer_list>

namespace Leaf
{
template <typename T>
class ArgParser {
    using OptionParser = std::function<void(T&, ArgParser&)>;
    using SimpleParser = std::function<void(void)>;

    T priv_;
    std::map<std::string, OptionParser> optParserMap_;
    OptionParser completionHook_;

    int currArgIdx_;
    int currArgc_;
    char **currArgv_;

    std::string errorMsg_;

    struct Profile {
        std::string name;
        std::string description;
        std::string synopsis;
    } profile_;

    struct Help {
        std::vector<std::string> options;
        std::string usage;
        std::string message;
    };
    std::vector<Help> optionHelps_;

    void add_(const std::string& option, OptionParser parser)
    {
        optParserMap_.insert(std::make_pair(option, parser));
    }

    void add_(const std::string& option, SimpleParser parser)
    {
        add_(option, [=](T&, ArgParser&) { parser(); });
    }

    std::string join_(
      const std::vector<std::string>& vect, const std::string& sep) const
    {
        std::stringstream ss;
        if (vect.empty())
            return "";

        const size_t lastIdx = vect.size() - 1;
        for (size_t i = 0; i < lastIdx; i++)
            ss << vect[i] << sep;
        ss << vect[lastIdx];
        return ss.str();
    }

public:
    ArgParser(
      const std::string &name = "", const std::string &desc = "",
      const std::string &synopsis = "")
    : completionHook_([](T&, ArgParser&){}),
      currArgIdx_(-1),
      currArgc_(-1),
      currArgv_(nullptr),
      profile_({name.empty() ? "[PROGRAM]" : name, desc, synopsis})
    {
    }

    virtual ~ArgParser()
    {
    }

    template <typename PARSER_TYPE>
    void add(std::initializer_list<std::string> options, PARSER_TYPE parser,
             const std::string& optionUsage = "",
             const std::string& optionDesc = "")
    {
        for (auto optIt = options.begin(); optIt != options.end(); ++optIt)
            add_(*optIt, parser);
        optionHelps_.emplace_back(Help({options, optionUsage, optionDesc}));
    }

    template <typename PARSER_TYPE>
    void add(const std::string& option, PARSER_TYPE parser,
             const std::string& optionUsage = "",
             const std::string& optionDesc = "")
    {
        add({option}, parser, optionUsage, optionDesc);
    }

    void setCompletionHook(OptionParser hook)
    {
        completionHook_ = hook;
    }

    bool hasNext(void) const
    {
        return currArgIdx_ < currArgc_ - 1;
    }

    const char* getNext(void)
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
            const std::string& opt = currArgv_[currArgIdx_];
            auto optParserItr = optParserMap_.find(opt);
            if (optParserItr != optParserMap_.end())
                (optParserItr->second)(priv_, *this);
            else
                error("Unknown option: " + opt);
            if (hasError())
                return false;
        }
        completionHook_(priv_, *this);
        return errorMsg_.empty();
    }

    void error(const std::string& msg)
    {
        errorMsg_ = msg;
    }

    bool hasError(void) const
    {
        return !errorMsg_.empty();
    }

    const std::string& getErrorMessage(void) const
    {
        return errorMsg_;
    }

    const T& getPrivateData(void) const
    {
        return priv_;
    }

    T& getPrivateData(void)
    {
        return priv_;
    }

    std::string generateUsage(void) const
    {
        constexpr auto spaces = "    ";
        std::stringstream ss;

        ss << "NAME" << std::endl;
        ss << spaces << profile_.name;
        if (!profile_.description.empty())
            ss << " -- " << profile_.description;
        ss << std::endl << std::endl;

        if (!profile_.synopsis.empty()) {
            ss << "SYNOPSIS" << std::endl;
            ss << spaces << profile_.synopsis << std::endl << std::endl;
        }

        ss << "OPTIONS" << std::endl;
        for (const auto& help: optionHelps_) {
            ss << spaces << join_(help.options, ",");
            if (!help.usage.empty())
               ss << " " << help.usage;
            ss << std::endl;
            if (!help.message.empty())
                ss << spaces << spaces << help.message << std::endl;
            ss << std::endl;
        }

        return ss.str();
    }
};

using SimpleArgParser = ArgParser<void*>;

}
