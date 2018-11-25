#include <iostream>
#include <string>
#include <vector>
#include <cppcutter.h>
#include <ArgParser.h>
using namespace std;
using namespace Leaf;

template <typename T>
auto cutterMessage(ArgParser<T>& parser)
{
    return cut_message("%s", parser.getErrorMessage().c_str());
}

template <typename T, typename... OPTIONS>
void doParse(ArgParser<T>& parser, OPTIONS... options)
{
    constexpr int argc = sizeof...(options) + 1;
    char *argv[argc] = {const_cast<char *>("command")};
    const char *optionArray[] = { options... };
    for (int i = 1; i < argc; i++)
        argv[i] = const_cast<char *>(optionArray[i-1]);
    parser.parse(argc, argv);
}

namespace test_argparser {

void test_no_args(void)
{
    struct Args {};
    ArgParser<Args> parser;
    doParse(parser);
    cut_assert_false(parser.hasError());
}

void test_parse_one_arg(void)
{
    struct Args {
        int x;
        Args() : x(0) {}
    };
    ArgParser<Args> parser;
    parser.add("-x", [](Args& arg, ArgParser<Args>& parser) {
        arg.x = 3;
    });
    cppcut_assert_equal(0, parser.getPrivateData().x);
    doParse(parser, "-x");
    cut_assert_false(parser.hasError());
    cppcut_assert_equal(3, parser.getPrivateData().x);
}

void test_getNext(void)
{
    struct Args {
        int x;
        Args() : x(0) {}
    };
    ArgParser<Args> parser;
    parser.add("-x", [](Args& arg, ArgParser<Args>& parser) {
        cut_assert_true(parser.hasNext());
        arg.x = atoi(parser.getNext());
    });
    cppcut_assert_equal(0, parser.getPrivateData().x);
    doParse(parser, "-x", "5");
    cut_assert_false(parser.hasError());
    cppcut_assert_equal(5, parser.getPrivateData().x);
}

void test_parse_two_args(void)
{
    struct Args {
        int x, y;
        Args() : x(0), y(0) {}
    };
    ArgParser<Args> parser;
    parser.add("-x", [](Args& arg, ArgParser<Args>& parser) {
        arg.x = 10;
    });
    parser.add("-y", [](Args& arg, ArgParser<Args>& parser) {
        arg.y = 20;
    });
    cppcut_assert_equal(0, parser.getPrivateData().x);
    cppcut_assert_equal(0, parser.getPrivateData().y);
    doParse(parser, "-x", "-y");
    cut_assert_false(parser.hasError());
    cppcut_assert_equal(10, parser.getPrivateData().x);
    cppcut_assert_equal(20, parser.getPrivateData().y);
}

void test_add_without_args(void)
{
    int x = 0;
    ArgParser<void *> parser;
    parser.add("-x", [&]() {
        x = 7;
    });
    doParse(parser, "-x");
    cut_assert_false(parser.hasError());
    cppcut_assert_equal(7, x);
}

void test_completionHook(void)
{
    struct Args {
        int x;
        Args() : x(0) {}
    };
    ArgParser<Args> parser;
    parser.add("-x", [](Args& arg, ArgParser<Args>&) {
        arg.x = 3;
    });
    parser.setCompletionHook([](Args& arg, ArgParser<Args>&) {
        arg.x = 5;
    });
    doParse(parser, "-x");
    cut_assert_false(parser.hasError());
    cppcut_assert_equal(5, parser.getPrivateData().x);
}

void test_SimpleArgParser(void)
{
    int x = 0;
    SimpleArgParser parser;
    parser.add("-x", [&]() { x = 3; });
    doParse(parser, "-x");
    cut_assert_false(parser.hasError());
    cppcut_assert_equal(3, x);
}

void test_error(void)
{
    SimpleArgParser parser;
    parser.add("-x", [&]() {
        parser.error("Test-Error");
    });
    doParse(parser, "-x");
    cut_assert_true(parser.hasError());
    cppcut_assert_equal(string("Test-Error"), parser.getErrorMessage());
}

void test_multiple_options_for_ArgParser(void)
{
    for (const auto& option: {"-x", "--x-long-style"}) {
        struct Args {
            int x;
            Args(): x(0) {}
        };
        ArgParser<Args> parser;
        parser.add({"-x", "--x-long-style"}, [&](Args& arg, ArgParser<Args>&) {
            arg.x = 3;
        });
        doParse(parser, option);
        cut_assert_false(parser.hasError(), cutterMessage(parser));
        cppcut_assert_equal(3, parser.getPrivateData().x);
    }
}

void test_multiple_options_for_SimpleArgParser(void)
{
    for (const auto& option: {"-x", "--x-long-style"}) {
        int x = 0;
        SimpleArgParser parser;
        parser.add({"-x", "--x-long-style"}, [&]() { x = 3; });
        doParse(parser, option);
        cut_assert_false(parser.hasError(), cutterMessage(parser));
        cppcut_assert_equal(3, x);
    }
}

void test_generateUsage(void)
{
    SimpleArgParser parser("TP", "A Test Program",
                           "test-prog [-abnxh]");
    parser.add({"-a", "--apple"}, [](){}, "NUMBER",
                "A number of apples");
    parser.add("-b", [](){}, "NUMBER",
                "A number of bananas");
    parser.add({"-n", "--name"}, [](){}, "[NAME]",
                "The name of the owner (Default: Taro)");
    parser.add("-x", [](){});
    parser.add({"-h", "--help"}, [](){}, "",
                "A help message is shown");

    const string expected =
    "NAME\n"
    "    TP -- A Test Program\n"
    "\n"
    "SYNOPSIS\n"
    "    test-prog [-abnxh]\n"
    "\n"
    "OPTIONS\n"
    "    -a,--apple NUMBER\n"
    "        A number of apples\n"
    "\n"
    "    -b NUMBER\n"
    "        A number of bananas\n"
    "\n"
    "    -n,--name [NAME]\n"
    "        The name of the owner (Default: Taro)\n"
    "\n"
    "    -x\n"
    "\n"
    "    -h,--help\n"
    "        A help message is shown\n"
    "\n"
    ;
    cppcut_assert_equal(expected, parser.generateUsage());
}

} // test_argparser
