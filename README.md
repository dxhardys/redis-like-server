# redis-like-server

This is a redis-like server, that implements a part of redis's command.
Done as a project for "Architecture Interne des Systèmes d'Exploitation" - CHPS - Paris Saclay

install the libcjson-dev lib 

For Debian/Ubuntu :
sudo apt install libcjson-dev   

make 

./server [PORT]

use netcat to locally connect to the server 

nc localhost [PORT]
