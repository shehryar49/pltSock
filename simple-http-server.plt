# Example HTTP Server
# Using socket module in Plutonium
import socket
socket.init() # Initialize socket module
var s = socket.NewSocket("AF_INET","SOCK_STREAM",0)
s.bind("127.0.0.1",8080)
s.listen(3)
var html = "<h1>hello,welcome to the server</h1>"
var res = "HTTP/1.1 200 OK\nServer: myserver\nContent-type: text/html\nContent-length: "+str(len(html))+"\nConnection: close\n\n"+html
while(true)
{
  var client = s.accept()
  var req = client.recv(1024)
  client.send(res)
  client.close()
}
