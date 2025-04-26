#include "NarrativaAIPrecondition.h"

bool UNarrativaAIPrecondition::CheckPrecondition_Implementation(FActorInfoStructure InActorInfoStructure)
{
	ActorInfoStructure = InActorInfoStructure;
	/* Return false by default */
	return false;
}
