#ifndef PLTOBJECT_H_
#define PLTOBJECT_H_
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

#define PltList vector<PltObject>
#define PltArgs const vector<PltObject>&
#define Dictionary std::unordered_map<PltObject,PltObject,PltObject::HashFunction>
struct PltObject;
typedef void(*Method)(PltObject*,PltObject*,int,PltObject*);

//

size_t hashList(void*);
size_t hashDict(void*);
//
enum ErrCode
{
  SYNTAX_ERROR = 1,
  TYPE_ERROR = 2,
  VALUE_ERROR = 3,
  MATH_ERROR  = 4,
  NAME_ERROR = 5,
  INDEX_ERROR = 6,
  ARGUMENT_ERROR = 7,
  UNKNOWN_ERROR = 8
};

struct FileObject
{
  FILE* fp;
  bool open;
};
struct PltObject
{
    union
    {
        void* ptr;
        float f;
        long long int l;
        int i;
    };
    char type;
    char extra;
    string s;
    PltObject()
    {
        type = 'n';
    }
    PltObject(const PltObject& other)
    {
        type = other.type;
        if(other.type=='i')
        {
           i = other.i;
        }
        else if(other.type=='o' || other.type=='v')
        {
          s = other.s;
          ptr = other.ptr;
        }
        else if(other.type=='w')
        {
          s = other.s;
          extra = other.extra;
          i = other.i;
        }
        else if(other.type=='n')
        {

        }
        else if(other.type=='p')
          ptr = other.ptr;
        else if(other.type=='r')
        {
          ptr = other.ptr;

        }
        else if(other.type=='f')
        {
           f = other.f;

        }
        else if(other.type=='q')
        {
          ptr = other.ptr;
          type = 'q';
        }
        else if(other.type=='e')
        {
           s = other.s;
           i = other.i;

        }
        else if(other.type=='b')
        {
            i = other.i;

        }
        else if(other.type=='c')
        {
                   ptr =(other.ptr);
           type = 'c';

        }
        else if(other.type=='s')
        {
           s = other.s;

        }
        else if(other.type=='u')
        {
            ptr = other.ptr;
        }
        else if(other.type=='m')
        {
            i = other.i;

        }
        else if(other.type=='j')
        {
          ptr = other.ptr;
        }
        else if(other.type=='l')
        {
            l = other.l;

        }
        else if(other.type=='a')
        {
          ptr = other.ptr;
        }
    }
    void operator=(const PltObject& other)
    {
        type = other.type;
        if(other.type=='i')
        {
           i = other.i;

        }
        else if(other.type=='o' || other.type=='v')
        {
          s = other.s;
          ptr = other.ptr;
        }
        else if(other.type=='w')
        {
          s = other.s;
          extra = other.extra;
          i = other.i;
        }
        else if(other.type=='n')
        {
        }
        else if(other.type=='p')
          ptr = other.ptr;
        else if(other.type=='r')
        {
          ptr = other.ptr;
          type = 'r';

        }
        else if(other.type=='f')
        {
           f = other.f;

        }
        else if(other.type=='e')
        {
           s = other.s;
           i = other.i;

        }
        else if(other.type=='c')
        {
           ptr = (other.ptr);
           type = 'c';

        }
        else if(other.type=='q')
        {
           ptr = other.ptr;
           type = 'q';
        }
        else if(other.type=='b')
        {
            i = other.i;

        }
        else if(other.type=='s')
        {
           s = other.s;

        }
        else if(other.type=='u')
        {
            ptr = other.ptr;
        }
        else if(other.type=='m')
        {
           i = other.i;

        }
        else if(other.type=='j')
        {
          ptr = other.ptr;
        }
        else if(other.type=='l')
        {
            l = other.l;

        }
        else if(other.type=='a')
        {
           ptr = other.ptr;
        }
    }

    bool operator==(const PltObject& other)const
    {
        if(other.type!=type)
            return false;
        if(type=='n')
        {
          return true;
        }
        else if(type=='i')
        {
          return i==other.i;
        }
        else if(type=='l')
        {
          return l==other.l;
        }
        else if(type=='f')
        {
          return f==other.f;
        }
        else if(type=='b')
        {
          return i==other.i;
        }
        else if(type=='m')
        {
          return i==other.i;
        }
        else if(type=='u')
        {
          return ((FileObject*)ptr)==((FileObject*)other.ptr);
        }
        else if(type=='s')
        {
          return other.s==s;
        }
        else if(type=='j')
        {
            return *(PltList*)ptr==*(PltList*)other.ptr;
        }
        else if(type=='q')
        {
          return false;
        }
        else if(type=='a')
        {
            return *(Dictionary*)ptr==*(Dictionary*)other.ptr;
        }
        return false;
    }
    struct HashFunction
    {
      size_t operator()(const PltObject& obj) const
      {
        size_t a = std::hash<char>()(obj.type);
        size_t b;
        size_t hashPltObject(const PltObject&);
        b = hashPltObject(obj) << 1;
        return a ^ b;
      }
  };
};
size_t hashPltObject(const PltObject&);
size_t hashList(void* p)
{
  PltList l = *(PltList*)p;
   size_t hash = l.size();
   for (auto i : l)
      hash ^= hashPltObject(i) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    return hash;
}
size_t hashDict(void* p)
{
  Dictionary d = *(Dictionary*)p;
   size_t hash = d.size();
    for (auto i : d)
      hash ^= hashPltObject(i.first)+hashPltObject(i.second) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    return hash;
}
size_t hashPltObject(const PltObject& a)
{
    char t = a.type;
    if(t=='s')
    {
        return std::hash<std::string>()(a.s);
    }
    else if(t=='i')
    {
        return std::hash<int>()(a.i);
    }
    else if(t=='l')
    {
        return std::hash<long long int>()(a.l);
    }
    else if(t=='f')
    {
        return std::hash<float>()(a.f);
    }
    else if(t=='j')
    {
        return hashList(a.ptr);
    }
    else if(t=='a')
    {
        return hashDict(a.ptr);
    }
    else if(t=='u')
    {
        return std::hash<FILE*>()(((FileObject*)a.ptr)->fp);
    }
    else if(t=='m')
    {
        return std::hash<unsigned char>()(a.i);
    }
    else if(t=='b')
    {
        return std::hash<bool>()(a.i);
    }
    else if(t=='r')
    {
      return std::hash<void*>()(a.ptr);
    }
    return 0;
}
PltObject Plt_Err(ErrCode e,string des)
{
  PltObject ret;
  ret.type = 'e';
  ret.s = des;
  ret.i = e;
  return ret;
}
inline PltObject PltObjectFromString(string s)
{
  PltObject ret;
  ret.type = 's';
  ret.s = s;
  return ret;
}
inline PltObject PltObjectFromInt(int x)
{
  PltObject ret;
  ret.type = 'i';
  ret.i = x;
  return ret;
}
inline PltObject PltObjectFromFloat(float f)
{
  PltObject ret;
  ret.type = 'f';
  ret.f = f;
  return ret;
}
inline PltObject PltObjectFromInt64(long long int x)
{
  PltObject ret;
  ret.type = 'l';
  ret.l = x;
  return ret;
}
inline PltObject PltObjectFromPointer(void* p)
{
  PltObject ret;
  ret.type = 'p';
  ret.ptr = p;
  return ret;
}
inline PltObject PltObjectFromByte(unsigned char x)
{
  PltObject ret;
  ret.type = 'm';
  ret.i = x;
  return ret;
}
inline PltObject PltObjectFromMethod(Method r)
{
  PltObject ret;
  ret.type = 'r';
  ret.ptr = (void*)r;
  return ret;
}
#endif
