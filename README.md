# pltSock

Socket module for plutonium programming language.

#HOW TO COMPILE

Compile using:

  For Linux:
  
  g++ -shared socket-linx.cpp -o socket.so -fPIC
  
  For Windows:
  
  g++ -shared socket-win.cpp -o socket.dll -lws2_32
  
And then copy the socket.so or socket.dll to modules folder.
The modules folder is present inside plutonium installation directory.
