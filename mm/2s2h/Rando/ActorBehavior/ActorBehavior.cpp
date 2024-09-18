#include "ActorBehavior.h"

void Rando::InitActorBehavior(bool isRando) {
    ActorBehavior::InitEnBoxBehavior(isRando);
    ActorBehavior::InitEnElforgBehavior(isRando);
    ActorBehavior::InitEnItem00Behavior(isRando);
}
