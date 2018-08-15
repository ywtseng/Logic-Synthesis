#include "base/main/mainInt.h"

ABC_NAMESPACE_IMPL_START

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

// export to mainInit.c
extern "C" void Lsv_Init ( Abc_Frame_t * );
extern "C" void Lsv_End  ( Abc_Frame_t * );

// command functions
static int Abc_CommandMajFind( Abc_Frame_t * , int , char ** );
static int Abc_Command1SubFind( Abc_Frame_t*, int, char**);
// external functions defined in lsv package
extern void Lsv_NtkMajFind( Abc_Ntk_t * );
extern void Lsv_NtkSubFind( Abc_Ntk_t * );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////
 
/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

void
Lsv_Init( Abc_Frame_t * pAbc )
{
   Cmd_CommandAdd( pAbc, "z LSV", "MAJ_find" , Abc_CommandMajFind , 0 );
   Cmd_CommandAdd( pAbc, "z LSV", "1subfind" , Abc_Command1SubFind, 0 );
}

void
Lsv_End( Abc_Frame_t * pAbc )
{
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

int
Abc_CommandMajFind( Abc_Frame_t * pAbc , int argc , char ** argv )
{
   Abc_Ntk_t *pNtk = Abc_FrameReadNtk(pAbc);
   if(!pNtk){
	  Abc_Print(-1,"Empty Network\n"); 
	  return 1;	  //represent this code wrong
   }
   if(Abc_NtkIsStrash(pNtk)){
	   //strash
	   Lsv_NtkMajFind(pNtk);
   }
   else{
	   //aig or default
	   pNtk = Abc_NtkStrash(pNtk,0,1,0);// find AIG
	   Lsv_NtkMajFind(pNtk);
	   Abc_NtkDelete(pNtk);
   }
   return 0;
}

//PA2 function
int
Abc_Command1SubFind( Abc_Frame_t * pAbc , int argc , char ** argv )
{
   Abc_Ntk_t *pNtk = Abc_FrameReadNtk(pAbc);
   if(!pNtk){
	  Abc_Print(-1,"Empty Network\n"); 
	  return 1;	  //represent this code wrong
   }
   if(Abc_NtkIsStrash(pNtk)){
	   //strash
	   Lsv_NtkSubFind(pNtk);
   }
   else{
	   //aig or default
	   pNtk = Abc_NtkStrash(pNtk,0,1,0);// find AIG
	   Lsv_NtkSubFind(pNtk);
	   Abc_NtkDelete(pNtk);
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

