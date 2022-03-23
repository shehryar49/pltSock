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
  bool open;

};

extern "C"
{
  void Bind(PltObject*,PltObject*,int,PltObject*);
  void Connect(PltObject*,PltObject*,int,PltObject*);
  void Send(PltObject*,PltObject*,int,PltObject*);
  void Recv(PltObject*,PltObject*,int,PltObject*);
  void SendTo(PltObject*,PltObject*,int,PltObject*);
  void RecvFrom(PltObject*,PltObject*,int,PltObject*);
  void Listen(PltObject*,PltObject*,int,PltObject*);
  void Accept(PltObject*,PltObject*,int,PltObject*);
  void Close(PltObject*,PltObject*,int,PltObject*);
  void DestroySocket(PltObject*,PltObject*,int,PltObject*);

  int validateArgTypes(string e,PltObject* args,int n)
  {
    if(e.length()!=n)
      return -1;//missing arguments
    int f = 0;
    int k = 0;
    for(k=0;k<n;k+=1)
    {
        if(args[k].type!=e[f])
          return f;//type mismatch for argument number f
        f+=1;
    }
    return 0;//no error
  }
  void init(PltObject* args,int n,PltObject* rr)
  {

  }
  void getMacros(PltObject* args,int n,PltObject* rr)
  {
    if(n!=0)
    {
      *rr = Plt_Err(ARGUMENT_ERROR,"Error zero arguments needed!");
      return;
    }
    Dictionary* d = new Dictionary;
    d->emplace(PltObjectFromString("AF_INET"),PltObjectFromInt(AF_INET));
    d->emplace(PltObjectFromString("AF_INET6"),PltObjectFromInt(AF_INET6));
    d->emplace(PltObjectFromString("SOCK_STREAM"),PltObjectFromInt(SOCK_STREAM));
    d->emplace(PltObjectFromString("SOCK_DGRAM"),PltObjectFromInt(SOCK_DGRAM));
    rr->type = 'c';
    rr->ptr = (void*)d;

  }
  void DestroySocket(PltObject* self,PltObject* args,int n,PltObject* rr)
  {
    Dictionary* d = (Dictionary*)self->ptr;
    Socket* s = (Socket*)(*d)[PltObjectFromString(".internalPTR")].ptr;
    delete s;
  }
  void NewSocket(PltObject* args,int n,PltObject* rr)
  {
    int e = validateArgTypes("iii",args,n);
    if(e==-1)
    {
      *rr = Plt_Err(VALUE_ERROR,"3 arguments needed!");
      return;
    }
    if(e!=0)
    {
      *rr = Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
       return;
    }
    Socket* s = new Socket;
    s->socket_desc = socket(args[0].i,args[1].i,0);
    int tr = 1;
    if (setsockopt(s->socket_desc,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1)
    {
      perror("setsockopt");
      exit(1);
    }
    s->ipfamily = args[0].i;
    s->connMethod = args[1].i;
    Dictionary* d = new Dictionary;
    d->emplace(PltObjectFromString(".internalPTR"),PltObjectFromPointer((void*)s));
    d->emplace(PltObjectFromString("bind"),PltObjectFromMethod(&Bind) );
    d->emplace(PltObjectFromString("connect"),PltObjectFromMethod(&Connect) );
    d->emplace(PltObjectFromString("send"),PltObjectFromMethod(&Send) );
    d->emplace(PltObjectFromString("recv"),PltObjectFromMethod(&Recv) );
    d->emplace(PltObjectFromString("listen"),PltObjectFromMethod(&Listen) );
    d->emplace(PltObjectFromString("accept"),PltObjectFromMethod(&Accept) );
    d->emplace(PltObjectFromString("close"),PltObjectFromMethod(&Close) );
    d->emplace(PltObjectFromString(".destroy"),PltObjectFromMethod(&DestroySocket) );
    if(s->connMethod==SOCK_DGRAM)
    {
      d->emplace(PltObjectFromString("sendto"),PltObjectFromMethod(&SendTo) );
      d->emplace(PltObjectFromString("recvfrom"),PltObjectFromMethod(&RecvFrom) );
    }
    rr->type = 'c';
    rr->ptr = (void*)d;
  }
  void Bind(PltObject* self,PltObject* args,int n,PltObject* rr)
  {
    int e = validateArgTypes("si",args,n);
    if(e==-1)
    {
      *rr = Plt_Err(VALUE_ERROR,"2 arguments needed!");
      return;
    }
    if(e!=0)
    {
      *rr = Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
      return;
    }
    struct sockaddr_in server;
    Dictionary* p = (Dictionary*)self->ptr;
    Socket* s = (Socket*)(*p)[PltObjectFromString(".internalPTR")].ptr;
    server.sin_family = s->ipfamily;
    server.sin_addr.s_addr = inet_addr(args[0].s.c_str());
    server.sin_port = htons( args[1].i );
    s->port = args[0].i;
    if( bind(s->socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
	    string errMsg = strerror(errno);
      *rr = Plt_Err(UNKNOWN_ERROR,errMsg);
    }
  }
  void Listen(PltObject* self,PltObject* args,int n,PltObject* rr)
  {
    int e = validateArgTypes("i",args,n);
    if(e==-1)
    {
      *rr = Plt_Err(VALUE_ERROR,"1 argument needed!");
      return;
    }
    if(e!=0)
    {
      *rr = Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
      return;
    }
    Dictionary* p = (Dictionary*)self->ptr;
    Socket* s = (Socket*)(*p)[PltObjectFromString(".internalPTR")].ptr;
    if(listen(s->socket_desc,args[0].i) < 0)
    {
      string errMsg = strerror(errno);
      *rr = Plt_Err(UNKNOWN_ERROR,errMsg);
    }
  }
  void Accept(PltObject* self,PltObject* args,int n,PltObject* rr)
  {
    if(n!=0)
    {
      *rr = Plt_Err(VALUE_ERROR,"0 arguments needed");
      return;
    }
    Dictionary* p = (Dictionary*)self->ptr;
    Socket* s = (Socket*)(*p)[PltObjectFromString(".internalPTR")].ptr;
    struct sockaddr_in  client;
    int c = sizeof(client);
    int new_socket = accept(s->socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
  	if (new_socket<0)
	  {
      string errMsg = strerror(errno);
      *rr = Plt_Err(UNKNOWN_ERROR,errMsg);
      return;
  	}
    Socket* A = new Socket;
    A->socket_desc = new_socket;
    A->port = s->port;
    A->ipfamily = s->ipfamily;
    A->connMethod = s->connMethod;
    Dictionary* d = new Dictionary;
    d->emplace(PltObjectFromString(".internalPTR"),PltObjectFromPointer((void*)A));
    d->emplace(PltObjectFromString("bind"),PltObjectFromMethod(&Bind) );
    d->emplace(PltObjectFromString("connect"),PltObjectFromMethod(&Connect) );
    d->emplace(PltObjectFromString("send"),PltObjectFromMethod(&Send) );
    d->emplace(PltObjectFromString("recv"),PltObjectFromMethod(&Recv) );
    d->emplace(PltObjectFromString("listen"),PltObjectFromMethod(&Listen) );
    d->emplace(PltObjectFromString("accept"),PltObjectFromMethod(&Accept) );
    d->emplace(PltObjectFromString("close"),PltObjectFromMethod(&Close) );
    d->emplace(PltObjectFromString(".destroy"),PltObjectFromMethod(&DestroySocket) );
    rr->type = 'c';
    rr->ptr = (void*)d;
  }
  void Connect(PltObject* self,PltObject* args,int n,PltObject* rr)
  {
    int e = validateArgTypes("si",args,n);
    if(e==-1)
    {
      *rr = Plt_Err(VALUE_ERROR,"2 arguments needed!");
      return;
    }
    if(e!=0)
    {
      *rr = Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
      return;
    }
    Dictionary* p = (Dictionary*)self->ptr;
    Socket* s = (Socket*)(*p)[PltObjectFromString(".internalPTR")].ptr;
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(args[0].s.c_str());
  	server.sin_family = s->ipfamily;
	  server.sin_port = htons(args[1].i);
    if (connect(s->socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
  	{
      string errMsg = strerror(errno);
      *rr = Plt_Err(UNKNOWN_ERROR,errMsg);
	  }
  }
  void Send(PltObject* self,PltObject* args,int n,PltObject* rr)
  {
    int e = validateArgTypes("s",args,n);
    if(e==-1)
    {
      *rr = Plt_Err(VALUE_ERROR,"1 argument needed!");
      return;
    }
    if(e!=0)
    {
      *rr = Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
       return;
    }
      Dictionary* p = (Dictionary*)self->ptr;
      Socket* s = (Socket*)(*p)[PltObjectFromString(".internalPTR")].ptr;
    if(send(s->socket_desc , args[0].s.c_str(),args[0].s.length() , 0) < 0)
	  {
      string errMsg = strerror(errno);
      *rr = Plt_Err(UNKNOWN_ERROR,errMsg);
	  }
  }
  void Recv(PltObject* self,PltObject* args,int n,PltObject* rr)
  {
    int e = validateArgTypes("i",args,n);
    if(e==-1)
    {
      *rr = Plt_Err(VALUE_ERROR,"1 argument needed!");
      return;
    }
    if(e!=0)
    {
      *rr = Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
      return;
    }
    Dictionary* p = (Dictionary*)self->ptr;
    Socket* s = (Socket*)(*p)[PltObjectFromString(".internalPTR")].ptr;
    char msg[args[0].i+1];
    string data;
    size_t read = recv(s->socket_desc,msg,args[0].i,0);
    if( read< 0)
	  {
      string errMsg = strerror(errno);
      *rr = Plt_Err(UNKNOWN_ERROR,errMsg);
      return;
    }
    msg[read] = 0;
    data = msg;
    *rr = PltObjectFromString(data);
  }
   void RecvFrom(PltObject* self,PltObject* args,int n,PltObject* rr)
  {
    int e = validateArgTypes("i",args,n);
    if(e==-1)
    {
      *rr = Plt_Err(VALUE_ERROR,"1 argument needed!");
      return;
    }
    if(e!=0)
    {
      *rr = Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
      return;
    }
    Dictionary* p = (Dictionary*)self->ptr;
    Socket* s = (Socket*)(*p)[PltObjectFromString(".internalPTR")].ptr;
    char msg[args[0].i+1];
    string data;
    unsigned int len;
    int read;
    struct sockaddr_in cliaddr;
    len = sizeof(cliaddr);  //len is value/resuslt
   
    read = recvfrom(s->socket_desc, (char *)msg, args[0].i, 
                MSG_WAITALL, ( struct sockaddr *) &cliaddr,
                &len);
    msg[read] = '\0';
    char *ip = inet_ntoa(cliaddr.sin_addr);
    string IP = ip;
    if( read< 0)
	  {
      string errMsg = strerror(errno);
      *rr = Plt_Err(UNKNOWN_ERROR,errMsg);
      return;
    }
    Dictionary* d = new Dictionary;
    data = msg;
    d->emplace(PltObjectFromString("addr"),PltObjectFromString(IP));
    d->emplace(PltObjectFromString("data"),PltObjectFromString(data));
    rr->type = 'c';
    rr->ptr = (void*)d;
  }
   void SendTo(PltObject* self,PltObject* args,int n,PltObject* rr)
  {
    int e = validateArgTypes("ssi",args,n);
    if(e==-1)
    {
      *rr = Plt_Err(VALUE_ERROR,"1 argument needed!");
      return;
    }
    if(e!=0)
    {
      *rr = Plt_Err(TYPE_ERROR,"Argument "+to_string(e)+" is of invalid type.");
       return;
    }
      Dictionary* p = (Dictionary*)self->ptr;
      Socket* s = (Socket*)(*p)[PltObjectFromString(".internalPTR")].ptr;
      int read;
      unsigned int len;
         struct sockaddr_in addr;
          memset(&addr, 0, sizeof(addr));
    // Filling server information
    addr.sin_family = s->connMethod;
    addr.sin_port = htons(args[2].i);
    addr.sin_addr.s_addr = inet_addr(args[1].s.c_str());
       
    sendto(s->socket_desc, (const char *)args[0].s.c_str(), args[0].s.length(),
        MSG_CONFIRM, (const struct sockaddr *) &(addr), 
            sizeof(addr));
  }
  void Close(PltObject* self,PltObject* args,int n,PltObject* rr)
  {
    if(n!=0)
    {
      *rr = Plt_Err(VALUE_ERROR,"0 arguments needed!.");
      return;
    }
    Dictionary* p = (Dictionary*)self->ptr;
    Socket* s = (Socket*)(*p)[PltObjectFromString(".internalPTR")].ptr;
    close(s->socket_desc);
    s->open = false;
    PltObject ret;
    *self = ret;//set socket object to nil
  }
}
