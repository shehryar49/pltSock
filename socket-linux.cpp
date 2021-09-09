/*
Created by Shahryar Ahmad.
Socket Module for Plutonium Programming Language

This module is not turing complete.But something is better than nothing. :)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "PltObject.h"
using namespace std;
struct Socket
{
  int socket_desc;
  string ip;
  int ipfamily;
  int connMethod;
  int port;
};

extern "C"
{
  PltObject Bind(PltObject&,PltArgs);
  PltObject Connect(PltObject&,PltArgs);
  PltObject Send(PltObject&,PltArgs);
  PltObject Recv(PltObject&,PltArgs);
  PltObject Listen(PltObject&,PltArgs);
  PltObject Accept(PltObject&,PltArgs);
  PltObject Close(PltObject&,PltArgs);
  int validateArgTypes(string e,const vector<PltObject>& args)
  {
    if(e.length()!=args.size())
      return -1;//missing arguments
    int f = 0;
    for(auto k: args)
    {
        if(k.type!=e[f])
          return f;//type mismatch for argument number f
        f+=1;
    }
    return 0;//no error
  }
  PltObject init(PltArgs args)
  {
	  PltObject ret;
	  return ret;
  }
  PltObject NewSocket(PltArgs args)
  {
    int e = validateArgTypes("ssi",args);
    if(e==-1)
      return Plt_Err(VALUE_ERROR,"3 arguments needed!");
    if(e!=0)
      return Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
    Socket* s = new Socket;
    int a,b;
    if(args[0].s=="AF_INET")
      a = AF_INET;
    if(args[1].s=="SOCK_STREAM")
      b= SOCK_STREAM;
    s->socket_desc = socket(a,b,0);
    int tr = 1;
    if (setsockopt(s->socket_desc,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1)
    {
      perror("setsockopt");
      exit(1);
    }
    s->ipfamily = a;
    s->connMethod = b;
    NativeObject obj;
    obj.ptr = (void*)s;
    obj.attr.emplace((string)"bind",&Bind);
    obj.attr.emplace((string)"connect",&Connect);
    obj.attr.emplace((string)"send",&Send);
    obj.attr.emplace((string)"recv",&Recv);
    obj.attr.emplace((string)"listen",&Listen);
    obj.attr.emplace((string)"accept",&Accept);
    obj.attr.emplace((string)"close",&Close);
    return obj;
  }
  PltObject Bind(PltObject& self,PltArgs args)
  {
    int e = validateArgTypes("si",args);
    if(e==-1)
      return Plt_Err(VALUE_ERROR,"2 arguments needed!");
    if(e!=0)
      return Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
    struct sockaddr_in server;
    NativeObject* p = (NativeObject*)self.ptr;
    Socket* s = (Socket*)p->ptr;
    server.sin_family = s->ipfamily;
    server.sin_addr.s_addr = inet_addr(args[0].s.c_str());
    server.sin_port = htons( args[1].i );
    s->port = args[0].i;
    if( bind(s->socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
	    string errMsg = strerror(errno);
      return Plt_Err(UNKNOWN_ERROR,errMsg);
    }
    PltObject ret;
    return ret;
  }
  PltObject Listen(PltObject& self,PltArgs args)
  {
    int e = validateArgTypes("i",args);
    if(e==-1)
      return Plt_Err(VALUE_ERROR,"1 argument needed!");
    if(e!=0)
      return Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
    NativeObject* p = (NativeObject*)self.ptr;
    Socket* s = (Socket*)p->ptr;
    if(listen(s->socket_desc,args[0].i) < 0)
    {
      string errMsg = strerror(errno);
      return Plt_Err(UNKNOWN_ERROR,errMsg);
    }
    PltObject ret;
    return ret;
  }
  PltObject Accept(PltObject& self,PltArgs args)
  {
    if(args.size()!=0)
      return Plt_Err(VALUE_ERROR,"0 arguments needed");
    NativeObject* p = (NativeObject*)self.ptr;
    Socket* s = (Socket*)p->ptr;
    struct sockaddr_in  client;
    int c = sizeof(client);
    int new_socket = accept(s->socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
  	if (new_socket<0)
	  {
      string errMsg = strerror(errno);
      return Plt_Err(UNKNOWN_ERROR,errMsg);
  	}
    Socket* A = new Socket;
    A->socket_desc = new_socket;
    A->port = s->port;
    A->ipfamily = s->ipfamily;
    A->connMethod = s->connMethod;
    NativeObject obj;
    obj.ptr = (void*)A;
    obj.attr.emplace((string)"bind",&Bind);
    obj.attr.emplace((string)"connect",&Connect);
    obj.attr.emplace((string)"send",&Send);
    obj.attr.emplace((string)"recv",&Recv);
    obj.attr.emplace((string)"close",&Close);
    return obj;
  }
  PltObject Connect(PltObject& self,PltArgs args)
  {
    int e = validateArgTypes("si",args);
    if(e==-1)
      return Plt_Err(VALUE_ERROR,"2 arguments needed!");
    if(e!=0)
      return Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
    NativeObject* p = (NativeObject*)self.ptr;
    Socket* s = (Socket*)p->ptr;
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(args[0].s.c_str());
  	server.sin_family = s->ipfamily;
	  server.sin_port = htons(args[1].i);
    if (connect(s->socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
  	{
      string errMsg = strerror(errno);
      return Plt_Err(UNKNOWN_ERROR,errMsg);
	  }
    PltObject ret;
    return ret;
  }
  PltObject Send(PltObject& self,PltArgs args)
  {
    int e = validateArgTypes("s",args);
    if(e==-1)
      return Plt_Err(VALUE_ERROR,"1 argument needed!");
    if(e!=0)
      return Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
    NativeObject* p = (NativeObject*)self.ptr;
    Socket* s = (Socket*)p->ptr;
    if(send(s->socket_desc , args[0].s.c_str(),args[0].s.length() , 0) < 0)
	  {
      string errMsg = strerror(errno);
      return Plt_Err(UNKNOWN_ERROR,errMsg);
	  }
    PltObject ret;
    return ret;
  }
  PltObject Recv(PltObject& self,PltArgs args)
  {
    int e = validateArgTypes("i",args);
    if(e==-1)
      return Plt_Err(VALUE_ERROR,"1 argument needed!");
    if(e!=0)
      return Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
    NativeObject* p = (NativeObject*)self.ptr;
    Socket* s = (Socket*)p->ptr;
    char msg[args[0].i+1];
    string data;
    size_t read = recv(s->socket_desc,msg,args[0].i,0);
    if( read< 0)
	  {
      string errMsg = strerror(errno);
      return Plt_Err(UNKNOWN_ERROR,errMsg);
    }
    msg[read] = 0;
    data = A;
    PltObject ret = (string)data;
    return ret;
  }
  PltObject Close(PltObject& self,PltArgs args)
  {
    if(args.size()!=0)
      return Plt_Err(VALUE_ERROR,"0 arguments needed!.");
    NativeObject* p = (NativeObject*)self.ptr;
    Socket* s = (Socket*)p->ptr;
    close(s->socket_desc);
    self = (int)0;
    PltObject ret;
    return ret;
  }
}
