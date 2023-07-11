# TorSnake
This class uses the torsocks shared object to allow you to create a tcp client that's capable of being routed through tor. 

## Requirements
This requires that you have tor and torsocks installed. which can be done via `sudo apt-get install tor -y`

### Notice
This repository is experimental. I don't know exactly what the tor backend code is doing, all I know is that this code works and allows you to use .onion addresses in your socket code. <br>
<br>
The location of your libtorsocks.so file may not be the same as mine. You will need to modify the torSnake.class.h code to reflect the location on your system.<br>
<br>
I used the following command to find the location of the so on my machine : `find / -name "libtorsocks.so" 2>/dev/null` 
