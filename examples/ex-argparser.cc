#include <iostream>
#include <ArgParser.h>
using namespace std;
using namespace Leaf;

int main(int argc, char* argv[])
{
    struct Args {
        bool  showUsage;
        bool  greeting;
        string reply;

        Args()
        : showUsage(false),
          greeting(false)
        {
        }
    };
    using Parser = ArgParser<Args>;
    Parser parser("ex-argparser", "An example of ArgParser",
                  "ex-argparser [OPTION]");

    parser.add({"-h", "--help"}, [](Args& args, Parser&) {
        args.showUsage = true;
    },
    "", "Show this help message");

    parser.add({"-g", "--greeting"}, [](Args& args, Parser&) {
        args.greeting  = true;
    },
    "", "Show a greeting message");

    parser.add({"-r", "--reply"}, [](Args& args, Parser& parser) {
        if (parser.hasNext())
            args.reply = parser.getNext();
        else
            parser.error("-r: Need a reply string");
    },
    "STRING", "Reply a given string");

    parser.parse(argc, argv);
    if (parser.hasError()) {
        cerr << "Error: " << parser.getErrorMessage() << endl;
        return EXIT_FAILURE;
    }

    const auto &args = parser.getPrivateData();
    if (args.showUsage) {
        cout << parser.generateUsage() << endl;
        return EXIT_FAILURE;
    }

    if (args.greeting) {
        cout << "Hello" << endl;
        return EXIT_SUCCESS;
    }

    if (!args.reply.empty()) {
        cout << "Your message: " << args.reply << endl;
        return EXIT_SUCCESS;
    }

    cout << parser.generateUsage() << endl;
    return EXIT_SUCCESS;
}
