/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <bitset>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include "myHashSet.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
   if(simulated) {
     cout<<"Error: circuit has been simulated!! Do \"CIRFraig\" first!!"<<endl;
     return;
   }
   simulated =true;

   vector<size_t> sizetArray;
   size_t x;
   unsigned pattern;

   if(_I<6) {pattern=1;}
   else if(_I<11){pattern=2;}
   else if(_I<100){pattern=5*_I;}
   else {pattern=8*_I;}
   _patternNum=pattern*64;
//
   for(size_t j=0;j<pattern;j++)
   {
      for(size_t i=0;i<_I;i++)
      {
         x=(size_t(rnGen(INT_MAX))<<32)+rnGen(INT_MAX);
         sizetArray.push_back(x);
      }

      for(size_t i=0;i<_I;i++)
      {
         getGate(PI_IdList[i])->addSimValue(sizetArray[i]);
      }
      ALL_List[0]->addSimValue(0);
      sizetArray.clear();
   }

   simulate();//set everygate to correct simvalue

   cout<<_patternNum<<" patterns simulated."<<endl;
   if(_patternNum%64!=0)
   {
      _patternNum=(_patternNum/64+1)*64;
   }
   HashSet2 hashSim(_patternNum);
   SimNode snode(ALL_List[0]->getSimArray(),0);
   hashSim.insertSim(snode);

   for(unsigned i=0;i<DFS_List.size();i++)
   {
      if(DFS_List[i]->isAig())
      {
         SimNode snode(DFS_List[i]->getSimArray(),DFS_List[i]->getGateId());
         hashSim.insertSim(snode);        
      }
   }
/*
   for(size_t i=0;i<ALL_List.size();i++)
   {
      if(ALL_List[i]!=0)
      {
         if(ALL_List[i]->isAig())
         {
            //cout<<"gateid:"<<i<<" ";
            //ALL_List[i]->printSim();cout<<endl;//cout<<" aaaa"; cout
            SimNode snode(ALL_List[i]->getSimArray(),i);
            hashSim.insertSim(snode);
         }
      }
   }
  */
   hashSim.collect();//FEC groups is OK!!!!
   _fecgroups.clear();
   for(unsigned i=0;i<hashSim.getFECsize();i++)
   {
      _fecgroups.push_back(hashSim.getFECGroup(i));
   }
}

void
CirMgr::fileSim(ifstream& patternFile)
{
   if(simulated) {
      cout<<"Error: circuit has been simulated!! Do \"CIRFraig\" first!!"<<endl;
      return ;
   }
   simulated =true;
   string str;
   vector<string> strArray;
   vector<size_t> sizetArray;
   int count=0;   

   for(size_t i=0;i<_I;i++)
   {
      strArray.push_back("");
      sizetArray.push_back(0);
   }   
//////////////////////////////////////////////////////////////////read
   while(getline(patternFile,str))
   {
      if(str.size()!=_I)//str size is _I
      {
         cout<<"Error: Pattern("<<str<<") length("<<str.size()<<") does not match the number of inputs(";
         cout<<_I<<") in a circuit!!"<<endl;
         return;
      }


      for(size_t i=0;i<_I;i++)
      {
         strArray[i]=strArray[i]+str.substr(i,1);
      }
      _patternNum++;
      count++;
      
      if(count==64)//string has 64bits
      {   
         if(!stringtosize(strArray,sizetArray))
         return;
         
         count=0;
         for(size_t i=0;i<_I;i++)
         {
            getGate(PI_IdList[i])->addSimValue(sizetArray[i]);
         }
         ALL_List[0]->addSimValue(0);

         for(size_t i=0;i<_I;i++)
         {
         strArray[i]="";
         }   
      }
   }

   if(count<64&&count>0)
   {
      str="";
      for(size_t i=0;i<64-count;i++)
      {
         str=str+"0";
         //_patternNum++;
      }
      for(size_t i=0;i<_I;i++)
      {
         strArray[i]=strArray[i]+str;
      }
      if(!stringtosize(strArray,sizetArray))
      return;
      for(size_t i=0;i<_I;i++)
      {
         //cout<<sizetArray[i]<<" ";     
         getGate(PI_IdList[i])->addSimValue(sizetArray[i]);
      }
      ALL_List[0]->addSimValue(0);
   }

   cout<<_patternNum<<" patterns simulated."<<endl;
   if(_patternNum%64!=0)
   {
      _patternNum=(_patternNum/64+1)*64;
   }

   simulate();//set everygate to correct simvalue

   HashSet2 hashSim(_patternNum);
   SimNode snode(ALL_List[0]->getSimArray(),0);
   hashSim.insertSim(snode);

   for(unsigned i=0;i<DFS_List.size();i++)
   {
      if(DFS_List[i]->isAig())
      {
         SimNode snode(DFS_List[i]->getSimArray(),DFS_List[i]->getGateId());
         hashSim.insertSim(snode);        
      }
   }
/*
   for(size_t i=0;i<ALL_List.size();i++)
   {
      if(ALL_List[i]!=0)
      {
         if(ALL_List[i]->isAig())
         {
            //cout<<"gateid:"<<i<<" ";
            //ALL_List[i]->printSim();cout<<endl;//cout<<" aaaa"; cout
            SimNode snode(ALL_List[i]->getSimArray(),i);
            hashSim.insertSim(snode);
         }
      }
   }
  */
   hashSim.collect();//FEC groups is OK!!!!
   _fecgroups.clear();
   for(unsigned i=0;i<hashSim.getFECsize();i++)
   {
      _fecgroups.push_back(hashSim.getFECGroup(i));
   }
   for(unsigned i=0;i<_fecgroups.size();i++)
   {
      for(unsigned j=0;j<_fecgroups[i].getSize();j++)
      {
         getGate(_fecgroups[i].getId(j))->setFEC(&_fecgroups[i]);
      }
   }
   
//////////////////////////////////////////////////


/*
      if(count==64)
      {
         simulate();
      }
      

      strarray.push_back(str);
      if(count==63)
      {
         count=0;
         putSimValue(strarray);
         strarray.clear();
      }
*/

}

void 
CirMgr::simulate()
{
   CirGate::setGlobalRef();
   for(size_t i=0;i<PO_IdList.size();i++)
   {
      sim(PO_IdList[i]);
   }
}

void
CirMgr::sim(unsigned id)
{
   if(getGate(id)->isGlobalRef())
   return;
   getGate(id)->setRef();
   for(size_t i=0;i<getGate(id)->getFaninsize();i++)
   {
      sim(getGate(id)->getFanin(i));
   }
   getGate(id)->setSimFanin();
}

bool
CirMgr::stringtosize(vector<string>& strArray,vector<size_t>& siArray)
{
   size_t x=0;
   size_t si=0;

   for(size_t i=0;i<_I;i++)
   {
      for(size_t j=0;j<64;j++)
      {
         if(strArray[i][j]=='1')
         {
            si=(size_t(1)<<j);
            x+=si;        
         }
         else if(strArray[i][j]!='0')
         {
            cout<<"Error: Pattern("<<strArray[i]<<") contains a non- 0/1 character(‘ "
            <<strArray[i][j]<<" ’ )."<<endl;
            return false;
         }
      }
      siArray[i]=x;
      //cout<<"x: "<<x<<endl;
      x=0;
   }
   return true;
}

/*

class TmpGroup
{
public:
   TmpGroup(vector<SimNode> SNs) :tmpgroup(SNs){}
   ~TmpGroup(){}

   void seperate()
   {
      bool insert=false;
      for(unsigned j=0;j<tmpgroup.size();j++)
      {
         for(unsigned i=0;i<fecgroups.size();i++)
         {
            if(tmpgroup[j].getSimValue()==fecgroups[i].getSimValue())
            {
               fecgroups[i].push_back(tmpgroup[j]);
               insert=true;
               break;
            }
         }
         if(!insert)
         {
            FECGroup fecgroup(tmpgroup[j].getId(),tmpgroup[j].getSimValue());
            fecgroups.push_back(fecgroup);
         }
      }

      eliminate();


   }
   void eliminate()
   {
      for(unsigned i=0;i<fecgroups.size();i++)
      {
         if(fecgroups[i].oneMember())
         {
            
         }
      }
   }
private:
   vector<FECGroup> fecgroups;
   vector<SimNode>  tmpgroup;
};
*/