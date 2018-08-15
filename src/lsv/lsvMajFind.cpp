#include "base/main/mainInt.h"

ABC_NAMESPACE_IMPL_START

/* Operation
1) find fanin && fanout
Vec_Int_t * vout = &(pObj->vFanouts);
int * vinarray = Vec_IntArray(vout);
for(int i= 0;i<Vec_IntSize(vout);i++){
 	Abc_Obj_t * pi = Abc_NtkObj( pNtk, vinarray[i] );
	printf("  %2d  ",pi->Id);	

Abc_Obj_t* o1 = Abc_Objgates[1]( pObj );
Abc_Obj_t* o2 = Abc_Objgates[2]( pObj );

2).find complement
unsigned  f0 = pObj->fCompl0;
unsigned  f1 = pObj->fCompl1;

3).find ID
int o1_id = o1->Id;
int o2_id = o2->Id;

*/

struct MAJfunction{
	int maj_gate;
	int input1;
	int input2;
	int input3;
};
struct MAJfunction maj[1000];

bool CompareGate(Abc_Obj_t* comp1, Abc_Obj_t* comp2,Abc_Obj_t* comp7,Abc_Obj_t* comp0,int case_num,int total_MAJ) {
	bool is_find_gate = false;
	if(Abc_ObjFaninNum(comp1)==0 || Abc_ObjFaninNum(comp2)==0){
		is_find_gate = false;
	}
	else{
		Abc_Obj_t* comp3 = Abc_ObjFanin0(comp1);
		Abc_Obj_t* comp4 = Abc_ObjFanin1(comp1);
		Abc_Obj_t* comp5 = Abc_ObjFanin0(comp2);
		Abc_Obj_t* comp6 = Abc_ObjFanin1(comp2);
		bool p1 = (comp3 == comp6) && (comp4 == comp5);
		bool p2 = (comp3 == comp5) && (comp4 == comp6);
		bool p3 = ((Abc_ObjFaninC0(comp1)^Abc_ObjFaninC1(comp2))&(Abc_ObjFaninC1(comp1)^Abc_ObjFaninC0(comp2)));
		bool p4 = ((Abc_ObjFaninC0(comp1)^Abc_ObjFaninC0(comp2))&(Abc_ObjFaninC1(comp1)^Abc_ObjFaninC1(comp2)));
		if(p1 && p3){ is_find_gate = true; } 
		if(p2 && p4){ is_find_gate = true; } 
	}
	//Output Result
	int output1,output2,output3;
	if(is_find_gate){
		switch(case_num){
			case 1:
				if(comp7->fCompl0 == 1){ output1 = Abc_ObjFanin0(comp7)->Id; }
				else{ output1 = (-Abc_ObjFanin0(comp7)->Id); }
			
				if(comp1->fCompl0 == 1){ output2 = (-Abc_ObjFanin0(comp1)->Id); }
				else{ output2 = (Abc_ObjFanin0(comp1)->Id); }
			
				if(comp1->fCompl1 == 1){ output3 = (-Abc_ObjFanin1(comp1)->Id); }
				else{ output3 = (Abc_ObjFanin1(comp1)->Id); }
			case 2:
				if(comp7->fCompl1 == 1){ output1 = Abc_ObjFanin1(comp7)->Id; }
				else{ output1 = (-Abc_ObjFanin1(comp7)->Id); }
			
				if(comp1->fCompl0 == 1){ output2 = (-Abc_ObjFanin0(comp1)->Id); }
				else{ output2 = (Abc_ObjFanin0(comp1)->Id); }
			
				if(comp1->fCompl1 == 1){ output3 = (-Abc_ObjFanin1(comp1)->Id); }
				else{ output3 = (Abc_ObjFanin1(comp1)->Id); }
			case 3:
				if(comp7->fCompl1 == 1){ output1 = Abc_ObjFanin1(comp7)->Id; }
				else{ output1 = (-Abc_ObjFanin1(comp7)->Id); }
			
				if(comp1->fCompl0 == 1){ output2 = (-Abc_ObjFanin0(comp1)->Id); }
				else{ output2 = (Abc_ObjFanin0(comp1)->Id); }
			
				if(comp1->fCompl1 == 1){ output3 = (Abc_ObjFanin1(comp1)->Id); }
				else{ output3 = (-Abc_ObjFanin1(comp1)->Id); }
			case 4:
				if(comp7->fCompl0 == 1){ output1 = Abc_ObjFanin0(comp7)->Id; }
				else{ output1 = (-Abc_ObjFanin0(comp7)->Id); }
			
				if(comp1->fCompl0 == 1){ output2 = (-Abc_ObjFanin0(comp1)->Id); }
				else{ output2 = (Abc_ObjFanin0(comp1)->Id); }
			
				if(comp1->fCompl1 == 1){ output3 = (-Abc_ObjFanin1(comp1)->Id); }
				else{ output3 = (Abc_ObjFanin1(comp1)->Id); }
		}
		
		//printf("%d = MAJ(%d, %d, %d)     (%d)\n", comp0->Id, output1, output2, output3,case_num);
		maj[total_MAJ].maj_gate = comp0->Id;
		maj[total_MAJ].input1 = output1;
		maj[total_MAJ].input2 = output2;
		maj[total_MAJ].input3 = output3;
	}
	return is_find_gate;
}


void Lsv_NtkMajFind(Abc_Ntk_t * pNtk) {
	Abc_Obj_t* pObj;
	int i;
	int total_MAJ = 0;
	
	Abc_AigForEachAnd(pNtk, pObj,i){
		//Sum information
		Abc_Obj_t* gates[15] ={NULL};
		gates[0] = pObj;
		
		//Solve constraint
		Vec_Int_t* gate0_fanins = &(gates[0]->vFanins);
	    if(Vec_IntSize(gate0_fanins) < 2 ){ continue; }
	    else{
			//constraint 1
			Vec_Int_t* gate1_fanins = &(Abc_ObjFanin0( gates[0] )->vFanins);
			Vec_Int_t* gate2_fanins = &(Abc_ObjFanin1( gates[0] )->vFanins);
			if(Vec_IntSize(gate1_fanins) < 2 || Vec_IntSize(gate2_fanins) < 2){ continue; }
			//constraint 2
			unsigned gates0_fCompl0 = gates[0]->fCompl0;
			unsigned gates0_fCompl1 = gates[0]->fCompl1;
			if(gates0_fCompl0==0||gates0_fCompl1==0){ continue; }
	    }
		
		//level 1 : set the gates of level 1
	    gates[1] = Abc_ObjFanin0( gates[0] );
	    gates[2] = Abc_ObjFanin1( gates[0] );
	    gates[3] = Abc_ObjFanin0( gates[1] );
	    gates[4] = Abc_ObjFanin1( gates[1] );
	    gates[5] = Abc_ObjFanin0( gates[2] );
	    gates[6] = Abc_ObjFanin1( gates[2] );
			
		//level 2 : Compare Gate
		Abc_Obj_t* fanin0 = Abc_ObjFanin0(pObj);
		Abc_Obj_t* fanin1 = Abc_ObjFanin1(pObj);
		if(CompareGate(gates[4], gates[2], gates[1],gates[0],1,total_MAJ)){ total_MAJ++; continue; }
		if(CompareGate(gates[3], gates[2], gates[1],gates[0],2,total_MAJ)){ total_MAJ++; continue; }
		if(CompareGate(gates[5], gates[1], gates[2],gates[0],3,total_MAJ)){ total_MAJ++; continue; }
		if(CompareGate(gates[6], gates[1], gates[2],gates[0],4,total_MAJ)){ total_MAJ++; continue; }
		
	}
	//Output all MAJfunction
	for(int i = 0 ;i<total_MAJ;i++){
		printf("%d = MAJ(%d, %d, %d)\n", maj[i].maj_gate, maj[i].input1, maj[i].input2, maj[i].input3);
	}
	//Output total number
	printf("Total_MAJ = %2d \n",total_MAJ);
}
////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END
