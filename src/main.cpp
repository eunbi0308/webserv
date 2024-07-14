#include "Webserv.hpp"

//volatile?? prevents compiler optimization??
//The volatile keyword tells the compiler that the value of the variable can change at any time,
//even outside the control of the program (for example, due to hardware or a signal handler).
//This prevents the compiler from optimizing away accesses to the variable,
//ensuring that every read or write to the variable actually occurs in the program's flow
//For instance in a for loop it might change anytime.
volatile sig_atomic_t gSignal = 0;

void handle_signal(int signal) {
    (void)signal;
	gSignal = 1;
}

void setup_signal_handler() 
{
    //process performs any necessary cleanup operations before exiting
    signal(SIGINT, handle_signal);

    //the process has the freedom to choose how it handles the signal. It can perform cleanup operations, bette roption
    signal(SIGTERM, handle_signal); 
}

int main(int ac, char **av)
{
    HttpResponse	Response;

    if (ac > 3){
        std::cout << "too many argument" << std::endl;
        return 1;
    }
    setup_signal_handler();
    try{
        Config config;
        // Declare a unique_ptr without initializing
        //std::unique_ptr<ServerManager> serverManager;
        //ServerManager serverManager(config);
        
        // Later, assign a dynamically allocated object to it
        // The std::make_unique function creates a new instance of ServerPool on the heap
        // and returns a std::unique_ptr<ServerPool> that owns this instance
        //serverManager = std::make_unique<ServerManager>(&config);
        if (ac == 2)
            config.setConfigFile(av[1]);
        config.parseConfig();
        ServerManager serverManager(config);
        serverManager.startServerManager(serverManager); 
        serverManager.startPoll();
        
        // if (Request.readRequest(fd) == true)
        // {
        //     Request.setReqServer(config.getServerList());
        //     Request.checkLocations();
        //     Response.setRequest(&Request);
        //     Response.checkMethod();
        //     std::cout << Response.getContent() << std::endl;
        // }
        
    }
    catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}