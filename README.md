# redis-like-server

This is a redis-like server, that implements a part of redis's command.
Done as a project for "Architecture Interne des Systèmes d'Exploitation" For High Performance Computing Master at Paris Saclay University

## Dependencies
You will need libcjson library to load and save the database, you can install it by running the following command for Ubuntu/Debian based

1. Run the command line : 
```bash
sudo apt install libcjson-dev 
``````

You will also need CMake if you don't already have it 

```bash
sudo apt install cmake 
``````
## Compiling
1. To compile the project :
```bash
mkdir build && cd build
cmake ..
make
``````

## Execution
1. Start the server and specify a port number (eg : 6565)
```bash
./server <PORT> 
``````
2. Open Netcat on another terminal and connect to the running server : 
```bash
nc localhost <PORT>
``````
You can open as many netcat sessions as you like to connect to the server, a new thread is created to handle each client

3. On you Netcat session you can type HELP to see all supported commands
```bash
HELP
``````
To see the description of a specific command 
```bash
HELP <COMMAND>
``````
