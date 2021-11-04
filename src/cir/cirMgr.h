/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "myHashSet.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr() { simulated=false;}
   ~CirMgr() {deleteAll_List(); }  

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void st(CirGate*,HashSet<TaskNode>&);
   size_t getM(){return _M;}
  
   void printFEC() const;
   void fraig();
   void fra(unsigned,SatSolver&);
   void fraigg(FECGroup*,SatSolver&);
   void mergefraig(unsigned,unsigned,bool);
   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;


   void deleteAll_List();
  // CirGate* checkUndef(unsigned);
   void readheader(ifstream&);
   void printgateunused()const;
   void printfloatingfi()const;
   void dfs(CirGate*) const;
   bool startopt(CirGate* );
   void df(CirGate*);
   CirGate* getOtherButCon(CirGate*);
   bool getOtherButConI1(CirGate*);
   bool getOtherButConI0(CirGate*);

   bool stringtosize(vector<string>&,vector<size_t>&);
   void simulate();
   void sim(unsigned);
   unsigned getPatterNum(){return _patternNum;}
   //void setFECGroups(FECGroup f){_fecgroups.push_back(f);}
   unsigned getFECGroupSize(){return _fecgroups.size();}
   FECGroup& getFECGroup(unsigned i){return _fecgroups[i];}
private:
   ofstream           *_simLog;
   vector<FECGroup>   _fecgroups;
   bool simulated;

   unsigned _M;
   unsigned _I;
   unsigned _L;
   unsigned _O;
   unsigned _A;
   
   unsigned _patternNum;
   bool startprint;

   IdList PISYM_List;
   IdList POSYM_List;

   GateList ALL_List;
   GateList UNDEF_List;
   GateList DFS_List;

   IdList PI_IdList;
   IdList PO_IdList;  

};

#endif // CIR_MGR_H
