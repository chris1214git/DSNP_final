/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
size_t CirGate::_global_ref = 1;

void
PIGate::reportGate() const
{
   cout<<"================================================================================"<<endl;
   cout<<"= PI("<<getGateId()<<")";
   if(_symbolic!="")
   cout<<"\""<<_symbolic<<"\"" ;
   cout<<", line "<<getLineNo(); cout<<endl;
   cout<<"= FECs:";//<<endl;
   for(size_t i=0;i<cirMgr->getFECGroupSize();i++)
   {
      if(cirMgr->getFECGroup(i).printG(_gateid)) {break;}
   }
   cout<<endl;     
   cout<<"= Value: ";printsim();
   cout<<"================================================================================"<<endl;
}
void
POGate::reportGate() const
{
   cout<<"================================================================================"<<endl;
   cout<<"= PO("<<getGateId()<<")";
   if(_symbolic!="")
   cout<<"\""<<_symbolic<<"\"" ;
   cout<<", line "<<getLineNo(); cout<<endl;
   cout<<"= FECs:";//<<endl;
   for(size_t i=0;i<cirMgr->getFECGroupSize();i++)
   {
      if(cirMgr->getFECGroup(i).printG(_gateid)) {break;}
   //cirMgr->getFECGroup(i).print();
   }
   cout<<endl;     
   cout<<"= Value: ";printsim();
   cout<<"================================================================================"<<endl;
}
void
AIGGate::reportGate() const
{
   cout<<"================================================================================"<<endl;
   cout<<"= AIG("<<getGateId()<<")";
   cout<<", line "<<getLineNo(); cout<<endl;
   cout<<"= FECs:";//<<endl;
   for(size_t i=0;i<cirMgr->getFECGroupSize();i++)
   {
      if(cirMgr->getFECGroup(i).printG(_gateid)) {break;}
   }
   cout<<endl;     
   cout<<"= Value: ";printsim();
   cout<<"================================================================================"<<endl;
}
void
UNDEFGate::reportGate() const
{
   cout<<"================================================================================"<<endl;
   cout<<"= UNDEF("<<getGateId()<<")";
   cout<<", line "<<getLineNo(); cout<<endl;
   cout<<"= FECs:"<<endl;
   cout<<endl;     
   cout<<"= Value: ";printsim();
   cout<<"================================================================================"<<endl;
}
void
CONSTGate::reportGate() const
{
   cout<<"================================================================================"<<endl;
   cout<<"= CONST("<<getGateId()<<")";
   cout<<", line "<<getLineNo(); cout<<endl;
   cout<<"= FECs:";
   for(size_t i=0;i<cirMgr->getFECGroupSize();i++)
   {
      if(cirMgr->getFECGroup(i).printG(_gateid)) { break;}
   }
   cout<<endl;     
   cout<<"= Value: ";printsim();
   cout<<"================================================================================"<<endl;
}




void
CirGate::reportFanin(int level) const//const
{
   assert (level >= 0);
   _global_ref++;
   cirMgr->getGate(_gateid)->recursiveFanin(level,0,false);
}

void
CirGate::recursiveFanin(int level,int spacetime,bool inverted) {
   
   for(int i=0;i<spacetime;i++)
      cout<<"  ";
   if(inverted) cout<<"!";

   cout<<getTypeStr(); cout<<_gateid;   

   if(_ref==_global_ref)
   {
      if(level==0){}
      else{ cout<<"(*)"; }
      cout<<endl;
      return;
   }   
   cout<<endl;

   _ref = _global_ref;


   if(level==0) return;

   for(size_t i=0;i<getFaninsize();i++)
   {
      cirMgr->getGate(getFanin(i)) ->recursiveFanin(level-1,spacetime+1,getFanininv(i));
   }
}


void
CirGate::reportFanout(int level) const//const
{
   assert (level >= 0);
   _global_ref+=1;
   cirMgr->getGate(_gateid)->recursiveFanout(level,0,false);   
}

void
CirGate::recursiveFanout(int level,int spacetime,bool inverted){
   
   for(int i=0;i<spacetime;i++)
      cout<<"  ";
   if(inverted) cout<<"!";
   cout<<getTypeStr(); cout<<_gateid;   

   if(_ref==_global_ref)
   {
      if(level==0){}
      else{ cout<<"(*)"; }
      cout<<endl;
      return;
   }   
   cout<<endl;

   _ref = _global_ref;


   if(level==0) return;

   for(size_t i=0;i<getFanoutsize();i++)
   {
      //if()
      //cout<<getFanout(i)<<endl;
      cirMgr->getGate(getFanout(i))->recursiveFanout(level-1,spacetime+1,getFanoutinv(i));
      //Fanout_List[i]->recursiveFanout(level-1,spacetime+1,foutinv[i]);
   }
}

void
CirGate::coutdfs()const
{
   //cout<<getTypeStr()<<_gateid;//<<" ";
   getTypeStr();
   for(unsigned i=0;i<getFaninsize();i++)
   {
      cout<<" ";
      if(getFanininv(i)) cout<<"!";
      cirMgr->getGate(getFanin(i))->coutUNDEF();//"*"
      cout<<cirMgr->getGate(Fanin_List[i])->_gateid;
      //if(i==0 && getFaninsize()>1) cout<<" ";
   }
   if(getSymbolic()!="")
   {
      cout<<" (";
      cout<<getSymbolic();
      cout<<")";
   }
}

bool
CirGate::eraseFanout(unsigned u,bool inv)
{
   for(size_t i=0;i<Fanout_List.size();i++)
   {
      if(u==Fanout_List[i])
      {
         if(foutinv[i]==inv)
         {
            Fanout_List[i]=Fanout_List[Fanout_List.size()-1];
            foutinv[i]=foutinv[foutinv.size()-1];
            Fanout_List.pop_back();
            foutinv.pop_back();
            return true;           
         }
      }
   }
   //cerr<<"eraseFanout Error!"<<endl;
   return false;
}

void
CirGate::resetFanin(unsigned u1,unsigned u2,bool invert)
{
   //bool check = false;
   if(!invert)
   {
      for(size_t i=0;i<Fanin_List.size();i++)
      {
         if(Fanin_List[i]==u1)
         {
            Fanin_List[i]=u2;
            //check = true;
         }
      }
   }
   else{
      for(size_t i=0;i<Fanin_List.size();i++)
      {
         if(Fanin_List[i]==u1)
         {
            Fanin_List[i]=u2;
            fininv[i]= !fininv[i];
            //check = true;
         }
      }      
   }

   //if(!check)
   //cout<<"reset Fanin error!"<<endl;
}

bool
CirGate::const0input()
{
   for(size_t i=0;i<Fanin_List.size();i++)
   {
      if(cirMgr->getGate(Fanin_List[i])->isConst())
      {
         if(!fininv[i])
         return true;
      }
   }
   return false;
}

bool
CirGate::const1input()
{
   for(size_t i=0;i<Fanin_List.size();i++)
   {
      if(cirMgr->getGate(Fanin_List[i])->isConst())
      {
         if(fininv[i])
         return true;
      }
   }
   return false;
}

bool
CirGate::doubleInput()
{
   if(Fanin_List[0]==Fanin_List[1])
   {
     if(fininv[0]==fininv[1])
        return true;
   }
   return false;
}

bool
CirGate::doubleInputInv()
{
   if(Fanin_List[0]==Fanin_List[1])
   {
     if(fininv[0]!=fininv[1])
        return true;
   }
   return false;
}

void
CirGate::merge(CirGate* _gate)
{
   size_t id=_gate->getGateId();
   cout<<"Strashing: "<<_gateid<<" merging "<<id<<"..."<<endl;

   for(size_t i=0;i<_gate->getFanoutsize();i++)
   {
      cirMgr->getGate(_gate->getFanout(i))->resetFanin(id,_gateid,false);
      Fanout_List.push_back(_gate->getFanout(i));
      foutinv.push_back(_gate->getFanoutinv(i));
   }

   cirMgr->getGate(_gate->getFanin(0))->eraseFanout(id,_gate->getFanininv(0));
   cirMgr->getGate(_gate->getFanin(1))->eraseFanout(id,_gate->getFanininv(1));   
}

void 
AIGGate::setSimFanin()
{
   size_t f0;
   size_t f1;
   bool in0=fininv[0];
   bool in1=fininv[1];
   
   size_t gateSim;
   
   for(unsigned i=0;i<cirMgr->getPatterNum()/64;i++)
   {
      if(in0) f0=~ cirMgr->getGate(Fanin_List[0])->getSimValue(i);
      else f0=cirMgr->getGate(Fanin_List[0])->getSimValue(i);
      if(in1) f1=~cirMgr->getGate(Fanin_List[1])->getSimValue(i);
      else f1=cirMgr->getGate(Fanin_List[1])->getSimValue(i);

      gateSim=f0&f1;
      _simValue.push_back(gateSim);
   }
//*   cout<<endl<<f0<<" "<<f1<<" ";
//   cout<<_gateid<<"gateSim:"<<gateSim<<endl;
}

void 
POGate::setSimFanin()
{
   size_t f0;
   bool in0=fininv[0];

   for(unsigned i=0;i<cirMgr->getPatterNum()/64;i++)
   {
      if(in0) f0=~cirMgr->getGate(Fanin_List[0])->getSimValue(i);
      else f0=cirMgr->getGate(Fanin_List[0])->getSimValue(i);
      
      _simValue.push_back(f0);
   }
}

void
UNDEFGate::setSimFanin()
{
   for(unsigned i=0;i<cirMgr->getPatterNum()/64;i++)
   {
      _simValue.push_back(size_t(0));
   }
}

//void

/*
bool
AIGGate::getOtherButCon()
{
   if(Fanin[0]->isConst()) return fininv[1]; 
   else if(Fanin[1]->isConst()) return fininv[0];
   else cout<<"No Const Fanin!"<<endl;
   return false;   
}


void
POGate::switchFanin(CirGate* _gate,CirGate* _gate2,bool inv)//>getOtherButCon())
{
   if(Fanin!=_gate)
   {
      cout<<"switch PO Fanin error"<<endl;
      return;
   }
   Fanin = _gate2;
   fininv = inv;
}


void
AIGGate::switchFanin(CirGate* _gate,CirGate* _gate2,bool inv)//>getOtherButCon())
{
   if(Fanin[0]==_gate)
   {
      Fanin[0] = _gate2;
      fininv[0] = inv;
   }
   else if(Fanin[1]==_gate)
   {
      Fanin[1] = _gate2;
      fininv[1] = inv;
   }
   else{
      cout<<"switch AIG Fanin error"<<endl;
   }
}

*/









/*
CirGate*
CirGate::getothergate(CirGate* _gate)
{
   for(size_t i=0;i<Fanin_List.size();i++)
   {
      if(Fanin_List[i]!=_gate)
      {
         return Fanin_List[i];
      }
   }
   return NULL;
}

void
CirGate::replace_gate1(CirGate* _gate)
{
   for(size_t i=0;i<Fanin_List.size();i++)
   {
      if(Fanin_List[i]!=_gate)
      {
         Fanin_List[i];
      }
   }
   return NULL;   
}*/