#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <winsock2.h>
#include "PltObject.h"
#define EXPORT __declspec(dllexport)
using namespace std;

extern "C"
{
	struct Socket
	{
		SOCKET socket_desc;
		struct sockaddr_in addr;
	};
	EXPORT PltObject Bind(PltObject&, PltArgs);
	EXPORT PltObject Connect(PltObject&, PltArgs);
	EXPORT PltObject Send(PltObject&, PltArgs);
	EXPORT PltObject Recv(PltObject&, PltArgs);
	EXPORT PltObject Listen(PltObject&, PltArgs);
	EXPORT PltObject Accept(PltObject&, PltArgs);
	EXPORT PltObject Close(PltObject&, PltArgs);
	int validateArgTypes(string e, const vector<PltObject>& args)
	{
		if (e.length() != args.size())
			return -1;//missing arguments
		int f = 0;
		for (auto k : args)
		{
			if (k.type != e[f])
				return f;//type mismatch for argument number f
			f += 1;
		}
		return 0;//no error
	}
	EXPORT PltObject init(PltArgs args)
	{
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			string errMsg = "Error code: " + to_string(WSAGetLastError());
			return Plt_Err(UNKNOWN_ERROR, errMsg);
		}
		PltObject ret;
		return ret;
	}
	EXPORT PltObject NewSocket(PltArgs args)
	{
		int e = validateArgTypes("ssi", args);
		if (e == -1)
			return Plt_Err(VALUE_ERROR, "3 arguments needed!");
		if (e != 0)
			return Plt_Err(TYPE_ERROR, "Argument " + to_string(e) + " is of invalid type.");
		Socket* s = new Socket;
		int a;
		int b;
		if (args[0].s == "AF_INET")
			a = AF_INET;
		if (args[1].s == "SOCK_STREAM")
			b = SOCK_STREAM;
		//Create a socket
		if ((s->socket_desc = socket(a,b, 0)) == INVALID_SOCKET)
		{
			string errMsg = "Error code: " + to_string(WSAGetLastError());
			return Plt_Err(UNKNOWN_ERROR, errMsg);
		}
		s->addr.sin_family = a;
		NativeObject obj;
		obj.ptr = (void*)s;
		obj.attr.emplace((string)"bind", &Bind);
		obj.attr.emplace((string)"connect", &Connect);
		obj.attr.emplace((string)"send", &Send);
		obj.attr.emplace((string)"recv", &Recv);
		obj.attr.emplace((string)"listen", &Listen);
		obj.attr.emplace((string)"accept", &Accept);
		obj.attr.emplace((string)"close", &Close);
		return obj;
	}
	EXPORT PltObject Bind(PltObject& self,PltArgs args)
	{
		int e = validateArgTypes("si", args);
		if (e == -1)
			return Plt_Err(VALUE_ERROR, "2 arguments needed!");
		if (e != 0)
			return Plt_Err(TYPE_ERROR, "Argument " + to_string(e) + " is of invalid type.");
		NativeObject* p = (NativeObject*)self.ptr;
		Socket* s =(Socket*) p->ptr;
		//Prepare the sockaddr_in structure
		s->addr.sin_addr.s_addr = inet_addr(args[0].s.c_str());
		s->addr.sin_port = htons(args[1].i);
		//Bind
		if (bind(s->socket_desc, (struct sockaddr*)&(s->addr), sizeof(s->addr)) == SOCKET_ERROR)
		{
			string errMsg = "Error code: " + to_string(WSAGetLastError());
			return Plt_Err(UNKNOWN_ERROR, errMsg);
		}
		PltObject ret;
		return ret;
	}
	EXPORT PltObject Listen(PltObject& self,PltArgs args)
	{
		int e = validateArgTypes("i", args);
		if (e == -1)
			return Plt_Err(VALUE_ERROR, "1 argument needed!");
		if (e != 0)
			return Plt_Err(TYPE_ERROR, "Argument " + to_string(e) + " is of invalid type.");
		NativeObject* p = (NativeObject*)self.ptr;
		Socket* s = (Socket*)p->ptr;
		listen(s->socket_desc,args[0].i);
		PltObject ret;
		return ret;
	}
	EXPORT PltObject Connect(PltObject& self, PltArgs args)
	{
		int e = validateArgTypes("si", args);
		if (e == -1)
			return Plt_Err(VALUE_ERROR, "1 argument needed!");
		if (e != 0)
			return Plt_Err(TYPE_ERROR, "Argument " + to_string(e) + " is of invalid type.");
		NativeObject* p = (NativeObject*)self.ptr;
		Socket* s = (Socket*)p->ptr;
		s->addr.sin_addr.s_addr = inet_addr(args[0].s.c_str());
		s->addr.sin_port = htons(args[1].i);
		if (connect(s->socket_desc, (struct sockaddr*)&(s->addr), sizeof(s->addr)) < 0)
		{
			string errMsg = "Error code: " + to_string(WSAGetLastError());
			return Plt_Err(UNKNOWN_ERROR, errMsg);
		}
		PltObject ret;
		return ret;
	}
	EXPORT PltObject Accept(PltObject& self,PltArgs args)
	{
		if (args.size() != 0)
			return Plt_Err(VALUE_ERROR, "0 arguments needed");
		NativeObject* p = (NativeObject*)self.ptr;
		Socket* s = (Socket*)p->ptr;
		Socket* A = new Socket;
		int c = sizeof(A->addr);
		A->socket_desc = accept(s->socket_desc, (struct sockaddr*)&(A->addr), &c);
		if (A->socket_desc == INVALID_SOCKET)
		{
			string errMsg = "Error code: " + to_string(WSAGetLastError());
			return Plt_Err(UNKNOWN_ERROR, errMsg);
		}
		puts("Connection accepted");
		NativeObject obj;
		obj.ptr = (void*)A;
		obj.attr.emplace((string)"send", &Send);
		obj.attr.emplace((string)"recv", &Recv);
		obj.attr.emplace((string)"close", &Close);
		return obj;
	}
	EXPORT PltObject Send(PltObject& self,PltArgs args)
	{
		int e = validateArgTypes("s", args);
		if (e == -1)
			return Plt_Err(VALUE_ERROR, "1 argument needed!");
		if (e != 0)
			return Plt_Err(TYPE_ERROR, "Argument " + to_string(e) + " is of invalid type.");
		NativeObject* p = (NativeObject*)self.ptr;
		Socket* s = (Socket*)p->ptr;

		send(s->socket_desc, args[0].s.c_str(), args[0].s.length(), 0);
		PltObject ret;
		return ret;
	}
	EXPORT PltObject Recv(PltObject& self, PltArgs args)
	{
		int e = validateArgTypes("i", args);
		if (e == -1)
			return Plt_Err(VALUE_ERROR, "0 arguments needed!");
		if (e != 0)
			return Plt_Err(TYPE_ERROR, "Argument " + to_string(e) + " is of invalid type.");
		NativeObject* p = (NativeObject*)self.ptr;
		Socket* s = (Socket*)p->ptr;
		char buff[args[0].i+1];

		int read = recv(s->socket_desc,buff,args[0].i, 0);
		if (read == SOCKET_ERROR)
		{
			string errMsg = "Error code: " + to_string(WSAGetLastError());
			return Plt_Err(UNKNOWN_ERROR, errMsg);
		}
		buff[read] = 0;
		string A = buff;
		string data = A;
		PltObject ret = data;
		return ret;
	}
	EXPORT PltObject Close(PltObject& self,PltArgs args)
	{
		if (args.size() != 0)
			return Plt_Err(VALUE_ERROR, "0 arguments needed!.");
		NativeObject* p = (NativeObject*)self.ptr;
		Socket* s = (Socket*)p->ptr;
		closesocket(s->socket_desc);
		delete s;
		PltObject ret;
		return ret;
	}
}
