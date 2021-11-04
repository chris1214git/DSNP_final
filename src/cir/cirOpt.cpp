/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed

//update floating list
// 
void
CirMgr::sweep()
{   
   CirGate::setGlobalRef();
   DFS_List.clear();
   for(size_t i=0;i<PO_IdList.size();i++)
   {
      dfs(getGate(PO_IdList[i]));      
   }

   for(size_t i=0;i<ALL_List.size();i++)
   {
      if(ALL_List[i]!=NULL)
      {
         if(!ALL_List[i]->isGlobalRef())
         {
           if(ALL_List[i]->sweepOrNot()){  //const gate =>eraseFanout   
           for(size_t j=0;j<ALL_List[i]->getFaninsize();j++)
           {
              if(getGate(ALL_List[i]->getFanin(j))!=0)
                 getGate(ALL_List[i]->getFanin(j))->eraseFanout(i,ALL_List[i]->getFanininv(j));
              //~~~~~~~~stay unfinished
           }  
           delete ALL_List[i]; //UNDEF or AIG
           ALL_List[i] = 0;
           }
         }
      }
   }

   UNDEF_List.clear();//順序重要嗎？
   for(size_t i=0;i<ALL_List.size();i++)
   {
      if(ALL_List[i]!=0)
      if(ALL_List[i]->isUNDEF())
      {
        for(unsigned j=0;j<ALL_List[i]->getFanoutsize();j++)
        {
           UNDEF_List.push_back(getGate(ALL_List[i]->getFanout(j)));
          
        }
         /*
         for(size_t j=0;j<ALL_List[i]->getFanoutsize();j++)
         {
           UNDEF_List.push_back(getGate(ALL_List[i]->getFanout(j)));
         }*/
      }
   }
}

void
CirMgr::optimize()
{
   if(simulated) {
      cout<<"Error: circuit has been simulated!! Do \"CIRFraig\" first!!"<<endl;
      return;
   }
   DFS_List.clear();

   CirGate::setGlobalRef();
   for(size_t i=0;i<PO_IdList.size();i++)
   {
      df(getGate(PO_IdList[i]));
   }
}

void
CirMgr::df(CirGate* _gate)
{
   if(_gate->isGlobalRef())
      return ;
   _gate->setRef();
   for(size_t i=0;i<_gate->getFaninsize();i++)
   {
      df(getGate(_gate->getFanin(i)));
   }
   if(startopt(_gate))
   DFS_List.push_back(_gate);
}


bool
CirMgr::startopt(CirGate* _gate)
{
   //const 0
   //bool inv=false;
   CirGate* _gate2;
   CirGate* tmpgate;
   unsigned id=_gate->getGateId();
   bool inv;
   if(!_gate->isAig())
      return true;
   if(_gate->const0input())
   {
      for(size_t i=0;i<_gate->getFanoutsize();i++)
      {
         ALL_List[0]->setFanout(_gate->getFanout(i));
         ALL_List[0]->setFanoutinv(_gate->getFanoutinv(i));
         getGate(_gate->getFanout(i))->resetFanin(id,0,false);//don' invert original fininv
      }
      ALL_List[0]->eraseFanout(id,false);
      _gate2 = getOtherButCon(_gate);//const0 input //even gate2 is const should return 
      inv = getOtherButConI0(_gate);
      _gate2->eraseFanout(id,inv);//even gate2 is const
      
      cout<<"Simplifying: 0 merging "<<id<<"..."<<endl;
      delete ALL_List[id];
      ALL_List[id] = 0;
      return false;
   }
   else if(_gate->const1input())
   {  
      ALL_List[0]->eraseFanout(id,true); //erase once

      _gate2 = getOtherButCon(_gate);  //x or x! or 1
      inv = getOtherButConI1(_gate);
      _gate2->eraseFanout(id,inv);
      if(!inv)
      {
         for(size_t i=0;i<_gate->getFanoutsize();i++)
         {
            getGate(_gate->getFanout(i))->resetFanin(id,_gate2->getGateId(),false);//don't change fininv
            _gate2->setFanout(_gate->getFanout(i));
            _gate2->setFanoutinv(_gate->getFanoutinv(i));
         }
      }      
      else //inv
      {
         for(size_t i=0;i<_gate->getFanoutsize();i++)
         {
            getGate(_gate->getFanout(i))->resetFanin(id,_gate2->getGateId(),true); //change fininv
            _gate2->setFanout(_gate->getFanout(i));
            _gate2->setFanoutinv(!_gate->getFanoutinv(i));
         }         
      }
      cout<<"Simplifying: "<<_gate2->getGateId()<<" merging ";
      if(inv) 
      cout<<"!";
      cout<<id<<"..."<<endl;
      delete ALL_List[id];
      ALL_List[id] = 0;
      return false;            
   }
   else if(_gate->doubleInput())
   {
      tmpgate = getGate(_gate->getFanin(0));//one of two inputs
      if(!_gate->getFanininv(0))//two input the same without invert
      {
         for(size_t i=0;i<_gate->getFanoutsize();i++)
         {
            tmpgate->setFanout(_gate->getFanout(i));//set is ok
            tmpgate->setFanoutinv(_gate->getFanoutinv(i));
            getGate(_gate->getFanout(i))->resetFanin(id,tmpgate->getGateId(),false);//reset every gate
         }
      tmpgate->eraseFanout(id,false);
      tmpgate->eraseFanout(id,false);
      }
      else{
         for(size_t i=0;i<_gate->getFanoutsize();i++)
         {
            tmpgate->setFanout(_gate->getFanout(i));//set is ok
            tmpgate->setFanoutinv(!_gate->getFanoutinv(i));
            getGate(_gate->getFanout(i))->resetFanin(id,tmpgate->getGateId(),true);//reset every gate
         }      
      tmpgate->eraseFanout(id,true);
      tmpgate->eraseFanout(id,true);          
      }



      cout<<"Simplifying: "<<_gate->getFanin(0)<<" merging ";
      if(_gate->getFanininv(0))
      cout<<"!";
      cout<<id<<"..."<<endl;
      delete ALL_List[id];
      ALL_List[id] = 0;
      return false;      
   }
   else if(_gate->doubleInputInv())
   {
      tmpgate=getGate(_gate->getFanin(0));
      for(size_t i=0;i<_gate->getFanoutsize();i++)
      {
         getGate(_gate->getFanout(i))->resetFanin(id,0,false);
         ALL_List[0]->setFanout(_gate->getFanout(i));
         ALL_List[0]->setFanoutinv(_gate->getFanoutinv(i));
      }
      tmpgate->eraseFanout(id,true);
      tmpgate->eraseFanout(id,false);
      cout<<"Simplifying: 0 merging "<<id<<"..."<<endl;
      delete ALL_List[id];
      ALL_List[id] = 0;
      return false;      
   }  
   return true;
}

CirGate* 
CirMgr::getOtherButCon(CirGate* _gate)
{
   assert(_gate->getFaninsize()==2);
   if(_gate->getFanin(0)==0)
      return ALL_List[_gate->getFanin(1)];
   return ALL_List[_gate->getFanin(0)];
}

bool 
CirMgr::getOtherButConI1(CirGate* _gate)//input must exist const1 and no const0
{
   if(_gate->getFanin(0)==0) //input const gate must be 1
   {
      return _gate->getFanininv(1);
   }
   return _gate->getFanininv(0);
}

bool
CirMgr::getOtherButConI0(CirGate* _gate)
{
  if(_gate->getFanin(0)==0)
  {
     if(_gate->getFanininv(0)==false)
     {
        return _gate->getFanininv(1);
     }
  }
  return _gate->getFanininv(0);
}
      //DFS_List.push_back(_gate);

/*
   CirGate::setGlobalRef();
   for(size_t i=0;i<PO_IdList.size();i++){
      opt(getGate(PO_IdList[i]));      
   }
   gate->setRef();
   for(unsigned i =0;i< gate->getFaninsize();i++)
   {
      if(!gate->getFanin(i)->isGlobalRef())
      dfs(gate->getFanin(i));
   }
   DFS_List.push_back(gate);

   
   
   //(a)
   CirGate* _gate,_gate2;
   bool optimizing=true;
   while(optimizing)
   {
      optimizing = false;
      for(size_t i=0;i<ALL_List[0]->getFanoutsize();i++)
      {
         if(ALL_List[0]->getFanout(i)->isAig())  
         optimizing = true;
      }
   }

   for(size_t i=0;i<ALL_List[0]->getFanoutsize();i++)
   {
      _gate = ALL_List[0]->getFanout(i); // PO or AIG
      if(!ALL_List[0]->getFanoutinv(i)) //input is const 0~~~    const0 fanout,gate fanouts'fanin, delete gate,
      {
         //delete fanout of the other fanin of gate
         _gate->getOtherConst()->eraseFanout(_gate);
         //add fanout of const0
         for(size_t j=0;j<_gate->getFanoutsize();j++)
         {
            ALL_List[0]->setFanout(_gate->getFanout(j));
            ALL_List[0]->setFanoutinv(_gate->getFanoutinv(j));
         }
         //change fanin of _gate's fanouts to const0
         for(size_t k=0;k<_gate->getFanoutsize();k++)
         {
            for(size_t l=0;l<_gate->getFanout(k)->getFaninsize();l++)  //PO or AIG
            {
               if(_gate->getFanout(k)->getFanin(l)==_gate)
               {
                  if(l==0)
                  {
                     _gate->getFanout(k)->setFanin(ALL_List[0]);
                     _gate->getFanout(k)->setFanininv(false);                     
                  }
                     _gate->getFanout(k)->setFanin2(ALL_List[0]);
                     _gate->getFanout(k)->setFanininv2(false);                        
               }
               else{
                  cout<<"optimize const0 error"<<endl;
               }
            }
         }
      }
      else{ //const1
      //erase fanout of const0
      ALL_List[0]->eraseFanout(_gate);
      //connect the fanout of the other fanin
      switchFanout(_gate,_gate->getOtherButCon(),_gate->getOtherInv());      
      //connect fanin to the other ~
      for(size_t j=0,j<_gate->getFanoutsize();j++)
      {
         _gate->getFanout(j)->switchFanin(_gate,_gate->getOtherButCon());
      }
      //Fanout should be everygate  PO AIG
      //delete _gate

   }

   for(size_t i=0;i<ALL_List.size();i++)
   {
      if(ALL_List[i]->isAig())
      {
         checkFaninSame(ALL_List[i]);
         checkFaninInv(ALL_List[i]);

      }
   }
}

bool
CirMgr::checkFaninSame(AIGGate* _gate)
{
   if(_gate->getFanin(0)!=_gate->getFanin(1))
   return false;
      if(_gate->getFanininv(0)==_gate->getFanininv(1))
      return true;
   return false;
}

bool
CirMgr::checkFaninInve(AIGGate* _gate)
{
   if(_gate->getFanin(0)!=_gate->getFanin(1))
   return false;

   if(_gate->getFanininv(0)!=_gate->getFanininv(1))
   return true;
   return false;
}


void
CirMgr::switchFanout(_gate,_gate2)//->getOtherButCon();   
{
   bool exist=false;
   for(size_t i=0;i<_gate->getFanoutsize();i++)
   {
      for(size_t j=0;j<_gate2->getFanoutsize();j++)
      {
         if(_gate->getFanout(i)==_gate2->getFanout(j))
         exist=true;
         break;
      }
      if(!exist)
      {
         _gate2->setFanout(_gate->getFanout(i));
         _gate2->setFanoutinv(_gate->getFanoutinv(i));
      }
   }
}
*/



/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
/*
(a) If one of the fanins of an
AND gate is a constant 1, this AND gate can be removed and replaced by the
other fanin, (b) If one of the fanins of an AND gate is a constatnt 0, this AND
gate can be removed and replaced by the constant 0 (Note: If the list of fanouts
of the other fanin becomes empty, and if this other fanin is an AIG or PI, it will
be added to the list of unused gates), (c) If both fanins of an AND gate are the
same, this AND gate can be removed and replaced by its fanin, (d) If one of the
fanins of an AND gate is inverse to the other fanin, this AND gate can be
removed and replaced by a constant 0.
*/