/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "myHashSet.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed



void
CirMgr::strash()
{
   DFS_List.clear();
   HashSet<TaskNode> _hashset(cirMgr->getM()*2+2);
   CirGate::setGlobalRef();

   for(size_t i=0;i<PO_IdList.size();i++)
   {
      st(cirMgr->getGate(PO_IdList[i]),_hashset);
   }

   //update DFSLIST
}


void
CirMgr::st(CirGate* _gate,HashSet<TaskNode>& set)
{
//   if(!_gate->isAig())
//      return ;
   if(_gate->isGlobalRef())
      return ;
   _gate->setRef();

   for(size_t i=0;i<_gate->getFaninsize();i++)
   {
      //cout<<_gate->getFanin(i)<<endl;
      st(cirMgr->getGate(_gate->getFanin(i)),set);
   }


   if(!_gate->isAig()){
   DFS_List.push_back(_gate);
   return ;
   }

   //cout<<_gate->getGateId()<<"not aig"<<endl;

   size_t f0=_gate->getFanin(0)*2;
   size_t f1=_gate->getFanin(1)*2;
   if(_gate->getFanininv(0))
      f0++;
   if(_gate->getFanininv(1))
      f1++;

   TaskNode _node(f0,f1,(size_t)_gate);
   size_t x = set.insert(_node);
   CirGate* _mergegate = (CirGate*)x;
   if(_mergegate!=0)
   {
      _mergegate->merge(_gate);
   }
   else{
      DFS_List.push_back(_gate);
   }

}



void
CirMgr::fraig()
{ 
   SatSolver solver;
   solver.initialize();
   ALL_List[0]->setvar(solver.newVar());
   for(unsigned i=0;i<PI_IdList.size();i++)
   {
      ALL_List[PI_IdList[i]]->setvar(solver.newVar());
   }
   for(unsigned i=0;i<ALL_List.size();i++)
   {
      if(ALL_List[i]!=0)
      {
         if(ALL_List[i]->isAig())
         {
            ALL_List[i]->setvar(solver.newVar());
         }
      }
   }
   for(unsigned i=_M;i<ALL_List.size();i++)
   {
      if(ALL_List[i]!=0)
      {
         if(ALL_List[i]->isPo())
         {
            ALL_List[i]->setvar(getGate(ALL_List[i]->getFanin(0))->getvar());
         }
      }
   }

   CirGate* _gate1;
   CirGate* _gate2;
   for(unsigned i=0;i<ALL_List.size();i++)
   {
      if(ALL_List[i]!=0)
      {
         if(ALL_List[i]->isAig())
         {
            _gate1=getGate(ALL_List[i]->getFanin(0));
            _gate2=getGate(ALL_List[i]->getFanin(1));
            solver.addAigCNF(ALL_List[i]->getvar(),_gate1->getvar(),
            ALL_List[i]->getFanininv(0),_gate2->getvar(),ALL_List[i]->getFanininv(1));
         }
      }
   }

/*   CirGate::setGlobalRef();
   for(unsigned i=0;i<PO_IdList.size();i++)
   {
     // cout<<i<<endl;
      fra(PO_IdList[i],solver);
   }*/
  for(unsigned i=0;i<_fecgroups.size();i++)
   {
      fraigg(&(_fecgroups[i]),solver);
      //cout<<i;
   }

   cirMgr->DFS_List.clear();
   CirGate::setGlobalRef();
   for(size_t i=0;i<PO_IdList.size();i++){
      dfs(getGate(PO_IdList[i]));      
   }
   //add const ?
}


void
CirMgr::fra(unsigned id,SatSolver& s)
{
   CirGate* g=getGate(id);
   if(g->isGlobalRef()) return;

   g->setGlobalRef();

   for(unsigned i=0;i<g->getFaninsize();i++)
   {
      fra(g->getFanin(i),s);
   }

   fraigg(g->getFEC(),s);
}

void 
CirMgr::fraigg(FECGroup* fecgroup,SatSolver& s)
{
   if(fecgroup==0)
   { return;}
   if(fecgroup->isDone())
   { return ;}

   bool inv;
   int tmpvar;
   for(unsigned i=0;i<fecgroup->getSize();i++)
   {
      if(getGate(fecgroup->getId(i))==0) continue;
      for(unsigned j=i+1;j<fecgroup->getSize();j++)
      {
         if(getGate(fecgroup->getId(j))==0) continue;
         tmpvar=s.newVar();
         s.addXorCNF(tmpvar,getGate(fecgroup->getId(i))->getvar(),fecgroup->getIn(i),
         getGate(fecgroup->getId(j))->getvar(),fecgroup->getIn(j));
         s.assertProperty(ALL_List[0]->getvar(), false);
         s.assumeProperty(tmpvar,true);
         if(s.assumpSolve())
         {
         //for(unsigned i=0;i<PI_IdList.size();i++)
         //{
            //cout<<s.getValue(getGate(PI_IdList[i])->getvar())<<" ";
         //}
            //cout<<endl;
         }

         else{
         //if(fecgroup->getIn(i)) cout<<"i";
        // if(fecgroup->getIn(j)) cout<<"j";
         inv = (fecgroup->getIn(i)!=fecgroup->getIn(j));
         mergefraig(fecgroup->getId(i),fecgroup->getId(j),inv);
         }
         s.assumeRelease();         
      }
   }
   fecgroup->setDone();
}
void
CirMgr::mergefraig(unsigned id1,unsigned id2,bool inv)//不一樣 inv
{
   CirGate* g1 = getGate(id1);
   CirGate* g2 = getGate(id2);
   if(g1==0||g2==0) return ;
   if(!inv)
   {
      for(unsigned i=0;i<g2->getFanoutsize();i++)
      {
         g1->setFanout(g2->getFanout(i));
         g1->setFanoutinv(g2->getFanoutinv(i));
         getGate(g2->getFanout(i))->resetFanin(id2,id1,inv);
      }
      cout<<"Fraig: "<<id1<<" merging "<<id2<<"..."<<endl;
   }
   else{
      for(unsigned i=0;i<g2->getFanoutsize();i++)
      {
         g1->setFanout(g2->getFanout(i));
         g1->setFanoutinv(!g2->getFanoutinv(i));
         getGate(g2->getFanout(i))->resetFanin(id2,id1,inv);
      }      
      cout<<"Fraig: "<<id1<<" merging !"<<id2<<"..."<<endl;
   }
   for(unsigned i=0;i<g2->getFaninsize();i++)
   {
      if(getGate(g2->getFanin(i))==0) continue;
      getGate(g2->getFanin(i))->eraseFanout(id2,g2->getFanininv(i));
   }  
   delete ALL_List[id2];
   ALL_List[id2]=0; 
}
/********************************************/
/*   Private member functions about fraig   */
/********************************************/

