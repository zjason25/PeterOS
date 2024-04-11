#include "../include/exm.h"

namespace PeterOS {
    // ExtendedManager::ExtendedManager() = default();
    // ExtendedManager::~ExtendedManager() = default();

    // allocate PCB
    RC ExtendedManager::create(unsigned priority){
        return priority;
    }

    RC ExtendedManager::destroy(unsigned proc_id){return proc_id;}

    RC ExtendedManager::request(unsigned resrc_id, unsigned k){return resrc_id + k;}

    RC ExtendedManager::release(unsigned resrc_id, unsigned k){return resrc_id + k;}

    RC ExtendedManager::timeout(){return 0;}

    RC ExtendedManager::scheduler(){return 0;}

    RC ExtendedManager::init(){return 0;}
    
    RC ExtendedManager::init_default(){return 0;}
}