/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

class SimNode
{
public:
   SimNode(vector<size_t> sa,size_t p) :simArray(sa),id(p) {}
   ~SimNode(){}
   
   bool operator ==(const SimNode& n) const{
      for(size_t i=0;i<simArray.size();i++)
      {
         if(simArray[i]!=n.simArray[i])
         return false;
      }
      return true;
   }
   unsigned operator ()()const
   {
      unsigned countone=0;
      for(size_t i=0;i<simArray.size();i++)
      {
         countone += bitonecount(simArray[i]);
      }
      //cout<<"countone "<<countone<<endl;
      return countone;
   }
   size_t getSimValue(unsigned i){return simArray[i];}
   size_t getId(){return id;}


   vector<size_t> simArray;
private:
   unsigned bitonecount(size_t s) const
   {
      unsigned count=0;
      while(s)
      {
         count++;
         s&=(s-1);
      }
      return count;
   }

   size_t id;
};

class FECGroup
{
public:
   FECGroup(){  _fraiged = false;}
   FECGroup(SimNode s) :_simValue(s.simArray)
   {
      _idList.push_back(s.getId()); _inList.push_back(false); //_simList.push_back(s);
      _fraiged=false;
   }
   ~FECGroup(){}
   bool isDone(){return _fraiged;}
   void setDone(){_fraiged=true;}

   void push_back(unsigned id,bool invert)
   {
     _idList.push_back(id);
     _inList.push_back(invert);

   }
   size_t getSimValue(unsigned i)const {return _simValue[i];}
   bool oneMember(){return(_idList.size()==1);}
   void push_back(SimNode& s,bool inv){
      _idList.push_back(s.getId()); 
      _inList.push_back(inv); 
   }
   size_t getId(unsigned i){return _idList[i];}
   bool getIn(unsigned i){return _inList[i];}
   size_t getSize(){return _idList.size();}
   bool printG(unsigned id)const
   {
      for(unsigned i=0;i<_idList.size();i++)
      {
         if(_idList[i]==id)
         {
            printGfind(i,_inList[i]);
            return true;
         }
      }
      return false;
   }
   void printGfind(unsigned count,bool inv) const
   {
      if(inv)
      {
         for(unsigned i=0;i<_idList.size();i++)
         {
            if(i==count) continue;
            cout<<" ";
            if(!_inList[i])
            {
               cout<<"!";
            }
            cout<<_idList[i];
         }
      }
      else{
         for(unsigned i=0;i<_idList.size();i++)
         {
            if(i==count) continue;
            cout<<" ";
            if(_inList[i])
            {
               cout<<"!";
            }
            cout<<_idList[i];
         }        
      }
   }
/*   void printValue(bool inv) const
   {
      //size_t to 1011101101
      size_t s=_simValue[_simValue.size()-1];
      if(!inv)
      {
        for(unsigned i=0;i<64;i++)
        {
           if(i%8==0&&i!=0) cout<<"_";
           if(s%2) cout<<"1";
           else cout<<"0";
           s=s/2;
        }
      }
      else{
         for(unsigned i=0;i<64;i++)
         {
            if(i%8==0&&i!=0) cout<<"_";
            if(s%2) cout<<"0";
            else cout<<"1";
            s=s/2;
         }
      }
   }
*/
   void print() const
   {
      for(unsigned i=0;i<_idList.size();i++)
      {
         cout<<" ";
         if(_inList[i])
         cout<<"!";
         cout<<_idList[i];
      }
   }
   void sort()
   {
      unsigned tmp =0;
      bool tmpbool;
      unsigned i2;
      for(unsigned i=1;i<_idList.size();i++)
      {
         for(unsigned j=0;j<i;j++)
         {
            if(_idList[i]<_idList[j])
            {
               tmp = _idList[i];
               tmpbool = _inList[i];
               i2=i;
               while(i2>j)
               {
                  _idList[i2]=_idList[i2-1];
                  _inList[i2]=_inList[i2-1];
                  i2--;
               }
               _idList[j]=tmp;
               _inList[j]=tmpbool;
            }
         }
      }
      if(_inList[0])
      {
         for(unsigned i=0;i<_inList.size();i++)
         _inList[i]=!_inList[i];
         for(unsigned i=0;i<_simValue.size();i++)
         {
            _simValue[i]=~_simValue[i];
         }
      }
   }

private:
   //vector<SimNode> _simList;
   bool _fraiged;
   vector<size_t> _idList;
   vector<bool>   _inList;
   vector<size_t> _simValue;
};   

//template <class Data>
class HashSet2
{
public:
   HashSet2(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet2() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //



   void init(size_t b) { _numBuckets = b; _buckets = new vector<SimNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<SimNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<SimNode>& operator [](size_t i) const { return _buckets[i]; }

   bool empty() const { return true; }
   // number of valid data
   size_t size() const { size_t s = 0; return s; }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const SimNode& d) const { return false; }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(SimNode& d) const { return false; }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const SimNode& d) { return false; }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   FECGroup& getFECGroup(unsigned i) { return _fecgroups[i];}
 
   unsigned getFECsize(){return _fecgroups.size();}

   bool insertSim(const SimNode& d)
   {
      if(d()>_numBuckets/2)
      {_buckets[_numBuckets-d()].push_back(d); return true;}
      _buckets[d()].push_back(d);
      return true;
   }

   size_t returnsize()
   {
      unsigned size=0;
      for(unsigned i=0;i<_numBuckets;i++)
      {
         size+=+_buckets[i].size();
      }
      return size;
   }
   int checkSimValue(SimNode& s1,FECGroup s2)
   {
      size_t f0=s1.getSimValue(0);
      size_t f1=s2.getSimValue(0);
      bool invert=false;
      if((f0^f1)==size_t(0-1))
      {
         invert=true;  
      }else if(f0!=f1)
      {
        return 3;//false
      }
      for(unsigned i=0;i<_numBuckets/64;i++)
      {
         f0=s1.getSimValue(i);
         f1=s2.getSimValue(i);
         if(invert)
         {
            if((f0^f1)!=size_t(0-1))
            return 3;
         }
         else//not invert
         {
            if(f0!=f1)
            return 3;
         }
      }
      if(invert)
      return 2;
      return 1;
   }

   void collect()
   {
      vector<FECGroup> tmpgroups;
      int a=3;
      for(unsigned i=0;i<_numBuckets;i++)
      {
        // cout<<endl<<"i:"<<i<<" ";
         for(unsigned j=0;j<_buckets[i].size();j++)
         {
         //   cout<<"j:"<<j<<" "<<endl;
            if(j==0)
            {
               FECGroup tmp1(_buckets[i][j]);
               tmpgroups.push_back(tmp1);
               continue;
            }
            for(unsigned k=0;k<tmpgroups.size();k++)
            {
               a = checkSimValue(_buckets[i][j],tmpgroups[k]);// 1==same 2==invert 3==notthesame
               if(a==2)
               {
                  tmpgroups[k].push_back(_buckets[i][j],true);
                  break;
               }
               else if(a==1)
               {
                  tmpgroups[k].push_back(_buckets[i][j],false);
                  break;
               }
            }
            if(a==3)
            {
               FECGroup tmp(_buckets[i][j]);
               tmpgroups.push_back(tmp);
            }
         }
         eliminate(tmpgroups);
         for(unsigned l=0;l<tmpgroups.size();l++)
         {
            _fecgroups.push_back(tmpgroups[l]);
         }
         tmpgroups.clear();
      }
      sortFEC();
   }//FECgroups complete!
   void sortFEC()
   {
      for(unsigned i=0;i<_fecgroups.size();i++)
         _fecgroups[i].sort();
      
      FECGroup tmp;
      unsigned i2;
      for(unsigned i=1;i<_fecgroups.size();i++)
      {
         for(unsigned j=0;j<i;j++)
         {
            if(_fecgroups[i].getId(0)<_fecgroups[j].getId(0))
            {
               tmp=_fecgroups[i];
               i2=i;
               while(i2>j)
               {
                  _fecgroups[i2]=_fecgroups[i2-1];
                  i2--;
               }
               _fecgroups[j]=tmp;
            }
         }
      }
   }
   void eliminate(vector<FECGroup>& tmpgroups)
   {
      unsigned n=0;
      while(n<tmpgroups.size())
      {
         if(tmpgroups[n].oneMember())
         {
            tmpgroups[n]=tmpgroups[tmpgroups.size()-1];
            tmpgroups.pop_back();           
         }
         else{n++;}
      }
   }


private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<SimNode>*  _buckets;
   vector<FECGroup>  _fecgroups;

   size_t bucketNum(const SimNode& d) const {
     if(d()>_numBuckets/2)
     return _numBuckets-d();
     return d(); 
     }
};




































class TaskNode
{

public:
   //TaskNode();
   TaskNode(size_t f00,size_t f11,size_t p) : _f0(f00), _f1(f11), pointer(p) {}
   ~TaskNode() {}

   bool operator == (const TaskNode& n) const { 
     if(_f0==n._f0&&_f1==n._f1)
     return true;
     return _f0==n._f1&&_f1==n._f0;
   }
   size_t operator () () const
   {
       return (_f0+_f1)/2;
   }
   const size_t& getPointer() const {return pointer; }
//   const size_t& getName() const { return _name; }
//   const size_t& getLoad() const { return _load; }
   friend ostream& operator << (ostream& os, const TaskNode& n);

private:
   size_t   _f0;
   size_t   _f1;
   size_t   pointer;
};


template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:

   private:
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { return iterator(); }
   // Pass the end
   iterator end() const { return iterator(); }
   // return true if no valid data
   bool empty() const { return true; }
   // number of valid data
   size_t size() const { size_t s = 0; return s; }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { return false; }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const { return false; }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { return false; }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   size_t insert(const Data& d) { 
     
     size_t key=d();
     for(size_t i=0;i<_buckets[key].size();i++)
     {
        if(_buckets[key][i]==d)
        return _buckets[key][i].getPointer();
     }
     _buckets[key].push_back(d);
     return 0;
   }

   bool insertSim(const Data& d)
   {
      _buckets[d()].push_back(d);
      return true;
   }

   size_t returnsize()
   {
      unsigned size=0;
      for(unsigned i=0;i<_numBuckets;i++)
      {
         size+=+_buckets[i].size();
      }
      return size;
   }
   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { return false; }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
     return (d() % _numBuckets); 
     }
};
#endif // MY_HASH_SET_H
