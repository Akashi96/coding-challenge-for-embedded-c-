#include <iostream>
#include <map>
#include <string>
#include <iostream>
#include <functional>
#include <algorithm>
#include <vector>
#include <cstdlib>

//
// supporting tools and software
//
// Validate and test your json commands
// https://jsonlint.com/

// RapidJSON : lots and lots of examples to help you use it properly
// https://github.com/Tencent/rapidjson
//

// std::function
// std::bind
// std::placeholders
// std::map
// std::make_pair

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace std;

bool g_done = false;

//
// TEST COMMANDS
//
auto help_command = R"(
 {
  "command":"help",
  "payload": {
    "usage":"Enter json command in 'command':'<command>','payload': { // json payload of arguments }"
  }
 }
)";

auto exit_command = R"(
 {
  "command":"exit",
  "payload": {
     "reason":"Exiting program on user request."
  }
 }
)";

auto sum_command = R"(
 {
  "command":"sum",
  "payload": {
     "num1":"5", "num2": "6", "num3": "1"
  }
 }
)";

auto factorial_command = R"(
 {
  "command":"factorial",
  "payload": {
     "num1":"5", "num2":"6"
  }
 }
)";

auto sort_command = R"(
 {
  "command":"sort",
  "payload": {
     "num1":"15", "num2":"6"
  }
 }
)";

class Controller {
public:
    bool help(rapidjson::Value &payload)    // Function to execute help command
    {
        cout << "Controller::help: command: \n";
        for(rapidjson::Value::ConstMemberIterator itr = payload.MemberBegin(); itr != payload.MemberEnd(); itr++)
            {
                std::cout << itr -> value.GetString() << std::endl;
            }

        return true;
    }

    bool exit(rapidjson::Value &payload)    // Function to execute exit command
    {
        cout << "Controller::exit: command: \n";

        for(rapidjson::Value::ConstMemberIterator itr = payload.MemberBegin(); itr != payload.MemberEnd(); itr++)
            {
                std::cout << itr -> value.GetString() << std::endl;
            }
        ::exit(0);
        return true;
    }

    // implement 3-4 more commands
    
    bool sum(rapidjson::Value &payload)    // Function to execute sum command
    {
        cout << "Controller::sum: command: \n";
        int Sum = 0;
        for(rapidjson::Value::ConstMemberIterator itr = payload.MemberBegin(); itr != payload.MemberEnd(); itr++)
        {
            Sum = Sum + std::stoi(itr -> value.GetString());
        }
        std::cout << "Sum = " << Sum << std::endl;
        return true;
    }

    bool factorial(rapidjson::Value &payload)    // Function to execute factorial command
    {
        cout << "Controller::factorial: command: \n";
        int num;
        for(rapidjson::Value::ConstMemberIterator itr = payload.MemberBegin(); itr != payload.MemberEnd(); itr++)
        {
            num = std::stoi(itr -> value.GetString());
            int fact = 1;
            for(int i = num; i > 1; i--)
            {
                fact = fact * i;
            }
            std::cout << "Factorial of number " << num << "=" << fact << std::endl;
        }
        return true;
    }

    bool sort(rapidjson::Value &payload)    // Function to execute sort command
    {
        cout << "Controller::sort: command: \n";
        std::vector<int> v;
        for(rapidjson::Value::ConstMemberIterator itr = payload.MemberBegin(); itr != payload.MemberEnd(); itr++)
        {
            int num = std::stoi(itr -> value.GetString());
            v.insert(v.end(), num);
        }
        std::sort(v.begin(), v.end());
        std::cout << "Sorted List of numbers: ";
        for(auto i : v)
            std::cout << i << " ";
        std::cout << std::endl;
        return true;
    }
};

typedef std::function<bool(rapidjson::Value &)> CommandHandler;
// Bonus Question: why did I type cast this?
/* ANSWER: To create CommandHandler as an alias for std::functionbool(rapidjson::Value &)> . Also it makes up for more clear and understandable code.
std::function works as a wrapper for any callable function, thus, by typecasting it as CommandHandler, Any function that accepts "rapidjson::Value&" type 
of parameter becomes callable by the variable declared by CommandHandler.*/

class CommandDispatcher {
public:

    CommandDispatcher(Controller controller)    // Controller
    {
        // add command handlers in Controller class to CommandDispatcher using addCommandHandler
        CommandHandler handle;  // Create a handler of type CommandHandler i.e. of type std::function<bool(rapidjson::Value &)>

        handle = std::bind(&Controller::exit, controller, std::placeholders::_1);   // Use std::bind to bind the Partial Function with CommandHandler handle 
        addCommandHandler("exit", handle);      // call addCommandHandler() to add to map
        handle = std::bind(&Controller::help, controller, std::placeholders::_1);
        addCommandHandler("help", handle);
        handle = std::bind(&Controller::sum, controller, std::placeholders::_1);
        addCommandHandler("sum", handle);
        handle = std::bind(&Controller::factorial, controller, std::placeholders::_1);
        addCommandHandler("factorial", handle);
        handle = std::bind(&Controller::sort, controller, std::placeholders::_1);
        addCommandHandler("sort", handle);
    }

    virtual ~CommandDispatcher()
    {   // No implementation for virtual destructor needed in our case.

    	// Question: why is it virtual? Is it needed in this case?
        /* ANSWER: Virtual Destructors are generally useful if we perform inheritance. Whenever we delete an instance of deried class from a pointer to the base class
                    to prevent undefined behaviour, we need virtual destructor in base class. Also to ensure proper destructor sequence call we must make the base destructor
                    virtual. It would avoid memory leaks. This ensures dynamic/runtime binding of the destructor.
                    The virtual destructor is not needed in this case.*/
        
    }

    bool addCommandHandler(std::string command, CommandHandler handler)
    {
        cout << "CommandDispatcher: addCommandHandler: " << command << std::endl;
        command_handlers_[command] = handler;       // Make a <key: value> entry in map, where key= command and value= handler
        
        return true;
    }

    bool dispatchCommand(std::string command_json)
    {
        cout << "COMMAND: " << command_json << endl;
       
        Document doc;
        doc.Parse(command_json.c_str());
        rapidjson::Value& cmd = doc["command"];
        rapidjson::Value& payload = doc["payload"];
        CommandHandler valueFunc = command_handlers_.at(cmd.GetString());   // extract the CommandHandler present at index cmd.GetString()
        valueFunc(payload);     // call the function using CommandHandler valueFunc and passing rapidjson::Value &payload as a parameter
        std::cout << std::endl;

        return true;
    }

private:
    std::map<std::string, CommandHandler> command_handlers_;

    // Question: why delete these?
    /* ANSWER: We delete these, so that we can prevent them from being defined or called. Deleting of constructors provides a cleaner way of preventing 
                the compiler from generating functions that we don't want. */
    // delete unused constructors
    CommandDispatcher() = delete;
    CommandDispatcher (const CommandDispatcher&) = delete;
    CommandDispatcher& operator= (const CommandDispatcher&) = delete;

};

int main()
{
    std::cout << "COMMAND DISPATCHER: STARTED" << std::endl;

    Controller controller;                 // controller class of functions to "dispatch" from Command Dispatcher
    CommandDispatcher command_dispatcher(controller);   // Create an object of class CommandDispatcher and pass in object of class Controller as a parameter

    // command line interface for testing
    string command;
    while( ! g_done ) 
    {
        std::cout << "COMMANDS: {\"command\":\"sum\", \"payload\": {\"num1\":\"X\", \"num2\": \"Y\", \"num3\": \"Z\"}}\n"
                  << "\t{\"command\":\"factorial\", \"payload\": { \"num1\":\"X\", \"num2\":\"Y\"}}\n"
                  << "\t{\"command\":\"sort\", \"payload\": { \"num1\":\"X\", \"num2\":\"Y\"}}\n"
                  << "\t{\"command\":\"exit\", \"payload\":{\"reason\":\"User requested exit.\"}}\n"
                  << "\t{\"command\":\"help\", \"payload\": {\"usage\":\"Enter json command in 'command':'<command>','payload': { // json payload of arguments }\"}}\n"
                  << "\tenter command : ";
        getline(cin, command);
        command_dispatcher.dispatchCommand(command);
    }

    std::cout << "COMMAND DISPATCHER: ENDED" << std::endl;
    return 0;
}
