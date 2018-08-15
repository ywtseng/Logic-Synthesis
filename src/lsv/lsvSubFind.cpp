#include "base/main/mainInt.h"
#include "sat/bsat/satSolver.h"
#include "sat/cnf/cnf.h"
#include "proof/fraig/fraig.h"
#include <vector>
using namespace std;

/* Operation
1) find fanin && fanout
Vec_Int_t * vout = &(pObj->vFanouts);
int * vinarray = Vec_IntArray(vout);
for(int i= 0;i<Vec_IntSize(vout);i++){
 	Abc_Obj_t * pi = Abc_NtkObj( pNtk, vinarray[i] );
	printf("  %2d  ",pi->Id);	
}
Abc_Obj_t* o1 = Abc_Objgates[1]( pObj );
Abc_Obj_t* o2 = Abc_Objgates[2]( pObj );

2).find complement
unsigned  f0 = pObj->fCompl0;
unsigned  f1 = pObj->fCompl1;

3).find ID
int o1_id = o1->Id;
int o2_id = o2->Id;

*/
bool
TryToFraig(Abc_Ntk_t* pNtk, Abc_Ntk_t* pNtk_v3){
	//Delcare variable
	Prove_Params_t param;
	Prove_Params_t* param_ptr = &param;
	Abc_Ntk_t* pNtk_v4 = NULL;
	// get the miter of the two networks
	pNtk_v4 = Abc_NtkMiter(pNtk, pNtk_v3, 1, 0, 0, 0);
	if (pNtk_v4 != NULL) {
		//Step 1: 
		int constant = Abc_NtkMiterIsConstant(pNtk_v4);
		if (constant == 0 || constant == 1) {
			Abc_NtkDelete(pNtk_v4);
			if(constant == 0){ return false;}
			if(constant == 1){ return true;}
		}
		//Step 2: Use SAT solver
		Prove_ParamsSetDefault(param_ptr);
		param_ptr->nItersMax = 5;
		int ivyprove = Abc_NtkIvyProve(&pNtk_v4, param_ptr);
		Abc_NtkDelete(pNtk_v4);
		if(ivyprove == 0){ return false;}
		if(ivyprove == 1){ return true;}
	}
	return false;
}

bool
TryToMerge(Abc_Ntk_t* pNtk, Abc_Ntk_t* pNtk_v2, Abc_Obj_t* f_gate, Abc_Obj_t* g_gate){
	//step 1: first fraig
	Abc_Ntk_t* pNtk_v3 = Abc_NtkStrash(pNtk_v2,0,1,0);
	Abc_NtkDelete(pNtk_v2);
	if(TryToFraig(pNtk,pNtk_v3)){
		Abc_NtkDelete(pNtk_v3); 
		return true;
	}
	//step 2: Second fraig
	vector<Abc_Obj_t*> fanout;
	vector<int> fanin_value;
	//fetch pNtk fanout value
	Abc_Obj_t* f_v1 = Abc_NtkObj(pNtk, f_gate->Id);
	Vec_Int_t * f_v1_fanout = &(f_v1->vFanouts);
	int * f_v1_fanout_array = Vec_IntArray(f_v1_fanout);
	for(int i = 0; i < Vec_IntSize(f_v1_fanout); i++){
		Abc_Obj_t * f_next = Abc_NtkObj( pNtk, f_v1_fanout_array[i] );
		if( Abc_ObjFaninId0(f_next)==f_v1->Id ){ 
			fanin_value.push_back(0); 
		}
		else{ 
			fanin_value.push_back(1); 
		}
		fanout.push_back(f_next);
	}
	//compare pNtk and pNtk_v3
	for(int i = 0; i < fanout.size(); i++){
		Abc_Obj_t* gate_fanout = fanout[i];
		if( gate_fanout->Id >= Abc_NtkObjNum(pNtk_v3) ){  continue;  }
		//compare pNtk and pNtk_v3
		Abc_Obj_t* f_v3_gate = Abc_NtkObj(pNtk_v3, gate_fanout->Id);
		if (f_v3_gate != NULL){ 
			if (fanin_value[i]==1){ Abc_ObjXorFaninC(f_v3_gate, 1); }
			else{ Abc_ObjXorFaninC(f_v3_gate, 0); }
		}
	}
	bool is_merge = TryToFraig(pNtk,pNtk_v3);
	Abc_NtkDelete(pNtk_v3); 
	return is_merge;
}


void 
Lsv_NtkSubFind(Abc_Ntk_t* pNtk){
	//Delcare variable
	int m = 0;
	int n = 0;
	Abc_Obj_t* f_gate = NULL; 
	Abc_Obj_t* g_gate = NULL; 
	vector< vector<Abc_Obj_t*> > gnode_matrix;
	vector<Abc_Obj_t*> fnode_vector;

	//Step 1: Traverse all node
	Abc_NtkForEachNode(pNtk,f_gate,m){
		vector<Abc_Obj_t*> gnode_vector;
		gnode_vector.clear();
		Abc_NtkForEachNode(pNtk,g_gate,n){
			bool is_merge = false;
			// Constraint 1:
			if(f_gate==g_gate){ continue; }
			Abc_Ntk_t* pNtk_v2 = Abc_NtkDup(pNtk);
			Abc_ObjReplace(Abc_NtkObj(pNtk_v2, f_gate->Id), Abc_NtkObj(pNtk_v2, g_gate->Id));
			// Constraint 2:
			if(Abc_NtkIsAcyclic(pNtk_v2)==false){ Abc_NtkDelete(pNtk_v2); continue; }
			// Constraint 3:
			is_merge = TryToMerge(pNtk,pNtk_v2,f_gate,g_gate);
			// Complete all constraints
			if(is_merge){ gnode_vector.push_back(g_gate); }
		}
		if(gnode_vector.size()!=0){
			gnode_matrix.push_back(gnode_vector);
			fnode_vector.push_back(f_gate);
		}
	}
	
	//Step 2: Output result
	printf("-----------------------------------------------\n");
	printf("Output Result : \n");
	for(int i = 0; i < gnode_matrix.size() ; i++){
		vector<Abc_Obj_t*> gnode_vector = gnode_matrix[i];
		Abc_Obj_t* print_fgate = fnode_vector[i];
		printf("%s:",Abc_ObjName(print_fgate) );
		for(int j=0;j<gnode_vector.size();j++){
			Abc_Obj_t* print_ggate = gnode_vector[j];
			printf("  %s",Abc_ObjName(print_ggate) );
		}
		printf("\n");
	}
	printf("Merge Number : %d \n",gnode_matrix.size());
}