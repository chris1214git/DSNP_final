/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"
#include "myHashSet.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{

public:
   CirGate() {_gateid = 0; _linenum=0; _ref=0;}
   virtual ~CirGate() {}

   unsigned getGateId() const { return _gateid;  }
   void setGateId(unsigned id){ _gateid = id;}
   unsigned getLineNo() const { return _linenum; }
   void setLineNum(unsigned num) {_linenum = num;}
   bool isGlobalRef(){return _ref==_global_ref;}
   static void setGlobalRef() {_global_ref++;}
   void setRef(){_ref = _global_ref;}

   void merge(CirGate*);

   virtual string getTypeStr() const =0;
   virtual void printGate() const =0;
   void setFanout(unsigned a){ Fanout_List.push_back(a);}
   void setFanoutinv(bool inv){ foutinv.push_back(inv);}
   void setFanin(unsigned a){ Fanin_List.push_back(a);}
   void setFanininv(bool inv){ fininv.push_back(inv);}

   unsigned getFaninsize() const{return Fanin_List.size();}
   unsigned getFanin(unsigned i) const{ return Fanin_List[i];}
   bool getFanininv(unsigned i) const{return fininv[i];}
   unsigned getFanoutsize() const{ return Fanout_List.size();}
   unsigned getFanout(unsigned i) const{return Fanout_List[i];}
   bool getFanoutinv(unsigned i) const{return foutinv[i];}
   bool eraseFanout(unsigned,bool);

   virtual void coutUNDEF() const {}

   virtual string getSymbolic() const{ return "";}
   virtual bool sweepOrNot() =0;
   virtual bool isAig() const =0;
   virtual bool isPi() const =0;
   virtual bool isPo() const =0;
   virtual bool isConst() const =0;
   virtual bool isUNDEF() const =0;

   virtual void reportGate()const =0;// const;
   void reportFanin(int level) const;// const;
   void reportFanout(int level) const;// const;
   virtual void coutdfs() const;
   //virtual bool eraseFanout(CirGate*);
   void resetFanin(unsigned,unsigned,bool);
   bool const0input();
   bool const1input();  
   bool doubleInput();
   bool doubleInputInv();   
   //{cout<<"Not Aig no other fanin!"<<endl; return NULL;}
   //virtual bool getOtherButCon(){cout<<"Not Aig no other fininv!"<<endl; return false;}  

   void addSimValue(size_t s){ _simValue.push_back(s);}
   void resetSimValue(){ _simValue.clear();}
   size_t getSimValue(unsigned i){ //if(i>=_simValue.size()) cout<<"getSimValue error"<<endl; return 0;
   return _simValue[i];}
   size_t SimSize(){return _simValue.size();}
   virtual void setSimFanin(){}
   vector<size_t> getSimArray() const{return _simValue;}
   //void printSim()const {for(size_t i=0;i<_simValue.size();i++) cout<<" "<<_simValue[i]; return;}
   void printsim() const{
      size_t a=_simValue[_simValue.size()-1];
      for(unsigned i=63;i>=0;i--)
      {
         if((a>>i)%2) cout<<"1";
         else cout<<"0";
         if(i%8==0){if(i==0) break; cout<<"_";}
      }
      cout<<endl;
   } 
   void setFEC(FECGroup* f){_fecgroup=f;}
   FECGroup* getFEC(){return _fecgroup;}
   
   void setvar(int a){_var=a;}
   int getvar(){return _var;}
   int _var;

   //bool getmerged(){return _merged;}
  // void setmerged(){_merged=true;}
private:
   void recursiveFanout(int,int,bool) ;
   void recursiveFanin(int,int,bool) ;
protected:   
   FECGroup* _fecgroup;
   //bool _merged;
   unsigned _gateid;
   unsigned _linenum;
   size_t _ref;
   static size_t _global_ref;
   vector<unsigned> Fanout_List;
   vector<bool> foutinv;
   vector<unsigned> Fanin_List;
   vector<bool> fininv;   

   vector<size_t> _simValue;
};

class PIGate: public CirGate
{
public:
   PIGate(unsigned id,unsigned lineno,unsigned ionum) {
      setGateId(id);
      setLineNum(lineno);
      _ionumber = ionum;
      _symbolic="";      
    }
   ~PIGate(){}

   virtual string getTypeStr() const{cout<<"PI  "<<_gateid; return "PI  ";}
   unsigned getionumber() const{return _ionumber;}
   void setSymbolic(string str){ _symbolic=str;}
   virtual string getSymbolic() const{ return _symbolic;}

   //virtual bool eraseFanout(CirGate*);

   virtual bool isAig() const { return false; }
   virtual bool isPi() const { return true;}
   virtual bool isPo() const { return false;}
   virtual bool isConst() const {return false;}   
   virtual bool isUNDEF() const {return false;}

   virtual void reportGate() const;
   bool sweepOrNot() { return false;}
   
   virtual void printGate() const {}
   
   private:
   unsigned _ionumber;
   string _symbolic;
};


class POGate: public CirGate
{
public:
   POGate(unsigned id,unsigned lineno,unsigned ionum) {
      setGateId(id);
      setLineNum(lineno);
      _ionumber = ionum;
      _symbolic="";
    }
   ~POGate(){}

   virtual string getTypeStr() const {cout<<"PO  "<<_gateid; return "PO  ";}
   void setSymbolic(string str){ _symbolic=str;}
   virtual string getSymbolic() const{ return _symbolic;}
   unsigned getionumber() const{return _ionumber;}


   virtual void reportGate() const;   
   virtual bool isAig() const { return false; }
   virtual bool isPi() const { return false;}
   virtual bool isPo() const { return true;}
   virtual bool isConst() const {return false;}
   virtual bool isUNDEF() const {return false;}    
   bool sweepOrNot() { return false;}   

   virtual void printGate() const {}

   virtual void setSimFanin();
private:
   unsigned _ionumber;
   string _symbolic;
};


class AIGGate: public CirGate
{
public:
   AIGGate(unsigned id=0,unsigned lineno=0) {
      setGateId(id);
      setLineNum(lineno);
    }
   ~AIGGate(){}

   virtual string getTypeStr() const{cout<<"AIG "<<_gateid; return "AIG ";}
   virtual void printGate() const {}
   virtual void reportGate() const;   
   virtual bool isAig() const { return true;}
   virtual bool isPi() const { return false;}
   virtual bool isPo() const { return false;}
   virtual bool isConst() const {return false;}
   virtual bool isUNDEF() const {return false;}    
   bool sweepOrNot() { cout<<"Sweeping: "; cout<<"AIG("<<_gateid<<") removed..."<<endl; return true;}
   
   virtual void setSimFanin();
   //virtual bool getOtherButCon();
   /*virtual CirGate* getOtherButCon()
   {
      if(Fanin_List[0]->isConst()) 
         return Fanin_List[1]; 
      return Fanin_List[0];
   }*/
private:

};


class CONSTGate: public CirGate
{
public:
   CONSTGate() {
      setGateId(0); 
      setLineNum(0);
    }
   ~CONSTGate(){}
   virtual string getTypeStr() const {cout<<"CONST"<<_gateid; return "CONST";}

   virtual void printGate() const {}
   virtual void reportGate() const;
   virtual bool isAig() const { return false;}
   virtual bool isPi() const { return false;}
   virtual bool isPo() const { return false;}
   virtual bool isConst() const {return true;}    
   virtual bool isUNDEF() const {return false;}   
   bool sweepOrNot() { return false;}

private:
};

class UNDEFGate: public CirGate
{
public:
   UNDEFGate(unsigned id) {
      setGateId(id);
      setLineNum(0);
    }
   ~UNDEFGate(){}

   virtual string getTypeStr() const {return"";}// "UNDEF";}
   virtual void coutUNDEF() const{ cout<<"*"; }

   virtual void printGate() const {}
   virtual void reportGate() const;   
   virtual bool isAig() const { return false; }
   virtual bool isPi() const { return false;}
   virtual bool isPo() const { return false;}
   virtual bool isConst() const {return false;}   
   virtual bool isUNDEF() const {return true;}    
   bool sweepOrNot() { cout<<"Sweeping: "; cout<<"UNDEF("<<_gateid<<") removed..."<<endl; return true;}
   virtual void setSimFanin();

private:
};


#endif // CIR_GATE_H

