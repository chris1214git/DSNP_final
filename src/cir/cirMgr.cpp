/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/bool
CirMgr::readCircuit(const string& fileName)
{
   unsigned idtmp=0;
   unsigned gateid=0;
   unsigned ionum=0;
   unsigned _lineNum;
   unsigned pigateid=0;   

   string str;   
   CirGate* _newgate=0;
   
   ifstream ifs(fileName.c_str());
   if (!ifs) {
      cerr <<" Cannot open design \""<<fileName<<"\""<<"!!"<<endl;
      return false;
   }
   
   readheader(ifs);
   

   for(unsigned i=0;i<_M+_O+1;i++)
   {
      ALL_List.push_back(NULL);
   }

   _lineNum = 1;
   _newgate = new CONSTGate();
   ALL_List[0]=_newgate;

   for(unsigned i=0;i<_I;i++){ //PI must be even
      getline(ifs,str);
      idtmp = atoi(str.c_str()); idtmp/=2;

      PI_IdList.push_back(idtmp);
      _newgate = new PIGate(idtmp,_lineNum+1,ionum);
      _lineNum++; 
      ionum++;

      ALL_List[idtmp]=(_newgate);
   }

   ionum=0;

   for(unsigned i=0;i<_O;i++){  
      getline(ifs,str);

      idtmp = _M+i+1;      
      PO_IdList.push_back(idtmp); 

      _newgate = new POGate(idtmp,_lineNum+1,ionum);
      _lineNum++;
      ionum++;
      ALL_List[idtmp]=(_newgate);
   }

   for(unsigned i=0;i<_A;i++){
      getline(ifs,str,' ');
      idtmp = atoi(str.c_str());
      gateid = idtmp/2;
      _lineNum++;

      _newgate = new AIGGate(gateid,_lineNum);
      ALL_List[gateid]=(_newgate);      
      //getline(ifs,str,' ');
      getline(ifs,str);
   }

   ifs.close();
   ifs.open(fileName.c_str());

   readheader(ifs);

   for(unsigned i=0;i<_I;i++){getline(ifs,str);}
   for(unsigned i=0;i<_O;i++){
      getline(ifs,str);
      idtmp = atoi(str.c_str());
      gateid = idtmp/2;

      if(getGate(gateid)==0){
         _newgate = new UNDEFGate(gateid);
         UNDEF_List.push_back(getGate(_M+i+1));
         ALL_List[gateid] = (_newgate);
         _newgate -> setFanout(_M+i+1);
         getGate(_M+i+1)->setFanin(gateid);
      }
      else{
         getGate(gateid)->setFanout(_M+i+1);
         getGate(_M+i+1)->setFanin(gateid);
      }

      if(idtmp%2){
         getGate(_M+i+1)->setFanininv(true);
         getGate(gateid)->setFanoutinv(true);         
      }
      else{
         getGate(gateid)->setFanoutinv(false); 
         getGate(_M+i+1)->setFanininv(false);
      }
   }
   //cout<<"YAYA"<<endl;

   for(unsigned i=0;i<_A;i++){
      getline(ifs,str,' ');
      idtmp = atoi(str.c_str());
      pigateid = idtmp/2;
//fanin1
      getline(ifs,str,' ');   
      idtmp = atoi(str.c_str());
      gateid = idtmp/2;
//undef process
      if(getGate(gateid)==0){
         _newgate = new UNDEFGate(gateid);
         UNDEF_List.push_back(getGate(pigateid));
         ALL_List[gateid] =(_newgate); //to be ensure!!!!!!!!!!!!!!!
      }      
      getGate(pigateid)->setFanin(gateid);
      getGate(gateid)->setFanout(pigateid);    
      
      if(idtmp%2==1){
         getGate(pigateid)->setFanininv(true);
         getGate(gateid)->setFanoutinv(true);
      }
      else  
      {
         getGate(gateid)->setFanoutinv(false);        
         getGate(pigateid)->setFanininv(false);
      }

      getline(ifs,str);
      idtmp = atoi(str.c_str()); 
      gateid = idtmp/2;

      if(getGate(gateid)==0){
         _newgate = new UNDEFGate(gateid);
         UNDEF_List.push_back(getGate(pigateid));
         ALL_List[gateid] = (_newgate); //to be ensure!!!!!!!!!!!!!!!
      }
      getGate(pigateid)->setFanin(gateid);
      getGate(gateid)->setFanout(pigateid);         

      if(idtmp%2==1){
         getGate(pigateid)->setFanininv(true);
         getGate(gateid)->setFanoutinv(true);
      }
      else {
         getGate(gateid)->setFanoutinv(false);     
         getGate(pigateid)->setFanininv(false);     
      }
   }

   char c;
   ifs.get(c);

   while(c=='i'||c=='o'){
      getline(ifs,str,' '); 
      idtmp = atoi(str.c_str()); 
      getline(ifs,str);
      if(c=='i')
         static_cast<PIGate*>(getGate(PI_IdList[idtmp]))->setSymbolic(str);   
      else
         static_cast<POGate*>(getGate(PO_IdList[idtmp]))->setSymbolic(str);      
      c='\0';
      ifs.get(c);         
   }
   ifs.close();

   CirGate::setGlobalRef();
   for(size_t i=0;i<PO_IdList.size();i++){
      dfs(getGate(PO_IdList[i]));      
   }

   return true;
}
void
CirMgr::dfs(CirGate* gate) const
{
   gate->setRef();
   for(unsigned i =0;i< gate->getFaninsize();i++)
   {
      if(!cirMgr->ALL_List[gate->getFanin(i)]->isGlobalRef())
      dfs(cirMgr->ALL_List[gate->getFanin(i)]);
   }
   cirMgr->DFS_List.push_back(gate);
}

void
CirMgr::printSummary() const
{
   unsigned A=0;
   for(unsigned i=0;i<ALL_List.size();i++)
   {
      if(ALL_List[i]!=0)
         if(ALL_List[i]->isAig())
         A++;
   }   

   cout<<endl;
   cout<<"Circuit Statistics"<<endl;
   cout<<"=================="<<endl;
   cout<<"  "<<"PI   "<<setw(9)<<right<<_I<<endl;
   cout<<"  "<<"PO   "<<setw(9)<<right<<_O<<endl;
   cout<<"  "<<"AIG  "<<setw(9)<<right<<A<<endl;
   cout<<"------------------"<<endl;
   cout<<"  "<<"Total"<<setw(9)<<_I+_O+A<<endl;
}

void
CirMgr::printNetlist() const
{
   cout<<endl;

 /*  cirMgr->DFS_List.clear();
   CirGate::setGlobalRef();
   for(size_t i=0;i<PO_IdList.size();i++){
      dfs(getGate(PO_IdList[i]));      
   }*/
    unsigned UNDEF=0;
   for(size_t i=0;i<DFS_List.size();i++)
   {
      if(DFS_List[i]->isUNDEF()) 
      {
         UNDEF++;
         continue;
      }
      cout<<"["<<i-UNDEF<<"] ";
      DFS_List[i]->coutdfs();
      cout<<endl;
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(size_t i=0;i<PI_IdList.size();i++){
      cout<<" "<<PI_IdList[i];
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(size_t i=0;i<PO_IdList.size();i++){
      cout<<" "<<PO_IdList[i];
   }   
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   
   printfloatingfi();
   printgateunused();   
   /*if(!UNDEF_List.empty())
   {  
      cout<<"Gates with floating fanin(s):";
      for(size_t i=0;i<UNDEF_List.size();i++){
         cout<<" "<<UNDEF_List[i]->getGateId();  
      }
      cout<<endl;
   }*/

}
void
CirMgr::printfloatingfi()const
{
   bool check =false;
   for(unsigned i=0;i<ALL_List.size();i++)
   {
      if(ALL_List[i]!=0)
      {
         if(ALL_List[i]->isUNDEF())
         {
            for(unsigned j=0;j<ALL_List[i]->getFanoutsize();j++)
            {
               if(!check)
               {cout<<"Gates with floating fanin(s):"; check=true;}
               cout<<" "<<ALL_List[i]->getFanout(j);
            }
         }
      }
   }
}

void
CirMgr::printgateunused()const{
   bool check=false;
   for(size_t i=0;i<ALL_List.size();i++){
      if(ALL_List[i]!=NULL)
      {
         if(ALL_List[i]->getFanoutsize()==0)
         {
            if(ALL_List[i]->isAig()||ALL_List[i]->isPi())
            {
            if(!check)
               {
                  cout<<"Gates defined but not used  :";
                  check=true;
               }      
            cout<<" "<<ALL_List[i]->getGateId();                       
            }
         }
      }   
   }
   if(check)
   cout<<endl;
}


void
CirMgr::writeAag(ostream& outfile) const
{
      /*
   outfile<<"aag "<<_M<<" "<<_I<<" "<<_L<<" "<<_O<<" "<<_A<<"\n";
   for(size_t i=0;i<PI_IdList.size();i++)
   {
      outfile<<(getGate(PI_IdList[i])->getGateId()*2)<<"\n";
   }
   for(size_t i=0;i<PO_IdList.size();i++)
   {
      if(getGate(PO_IdList[i])->getFanininv(0))
      {
         outfile<<(getGate(PO_IdList[i])->getFanin(0)->getGateId()*2+1)<<"\n";
      }
      else  outfile<<(getGate(PO_IdList[i])->getFanin(0)->getGateId()*2)<<"\n";
   }
   for(size_t i=0;i<DFS_List.size();i++)
   {
      if(DFS_List[i]->isAig())
      {
         outfile<<DFS_List[i]->getGateId()*2<<" ";
         for(size_t j=0;j<DFS_List[i]->getFaninsize();j++)
         {
            if(DFS_List[i]->getFanininv(j))
            {
               outfile<<DFS_List[i]->getFanin(j)->getGateId()*2+1;   
            }
            else outfile<<DFS_List[i]->getFanin(j)->getGateId()*2;
            if(j==0) outfile<<" ";
         }
         outfile<<"\n";
      }
   }

   for(size_t i=0;i<PI_IdList.size();i++)
   {
      if(getGate(PI_IdList[i])->getSymbolic()!="")
      {
         outfile<<"i"<<i<<" "<<getGate(PI_IdList[i])->getSymbolic()<<"\n";
      }
   }
   for(size_t i=0;i<PO_IdList.size();i++)
   {
      if(getGate(PO_IdList[i])->getSymbolic()!="")
      {
         outfile<<"o"<<i<<" "<<getGate(PO_IdList[i])->getSymbolic()<<"\n";
      }
   }   
   outfile<<"c\n"<<"I,m Batman";
   */
}


CirGate* 
CirMgr::getGate(unsigned gid) const {
   if(gid>=ALL_List.size()) return 0;
   return ALL_List[gid];
}

void
CirMgr::deleteAll_List(){   
   for(size_t i=0;i<ALL_List.size();i++){
      if(ALL_List[i])  delete ALL_List[i];        
   }
   ALL_List.clear();
}



void 
CirMgr::readheader(ifstream& ifs){
   string str;
   getline(ifs,str,' ');
   getline(ifs,str,' ');
   _M = atoi(str.c_str());
   getline(ifs,str,' ');
   _I = atoi(str.c_str());
   getline(ifs,str,' ');
   _L = atoi(str.c_str());      
   getline(ifs,str,' ');
   _O = atoi(str.c_str());
   getline(ifs,str);
   _A = atoi(str.c_str());   
}

void
CirMgr::printFECPairs() const
{
   for(unsigned i=0;i<_fecgroups.size();i++)
   {
      cout<<"["<<i<<"]";
      _fecgroups[i].print();
      cout<<endl;
   }
}
/*
void
CirMgr::writeAag(ostream& outfile) const
{
}
*/
void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
   
}

