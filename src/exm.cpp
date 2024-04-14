#include "exm.h"

namespace PeterOS {
    ExtendedManager &ExtendedManager::instance() {
        static ExtendedManager _ex_manager = ExtendedManager();
        return _ex_manager;
    }
    ExtendedManager::ExtendedManager() = default;
    ExtendedManager::~ExtendedManager() = default;
    ExtendedManager::ExtendedManager(const ExtendedManager &) = default;
    ExtendedManager &ExtendedManager::operator=(const ExtendedManager &) = default;

    // allocate PCB
    RC ExtendedManager::create(int priority){
        return priority;
    }

    RC ExtendedManager::destroy(int proc_id){return proc_id;}

    RC ExtendedManager::request(int resrc_id, int k){return resrc_id + k;}

    RC ExtendedManager::release(int resrc_id, int k){return resrc_id + k;}

    RC ExtendedManager::timeout(){return 0;}

    RC ExtendedManager::scheduler(){return 0;}

    RC ExtendedManager::init(){return 0;}
    
    RC ExtendedManager::init_default(){return 0;}
}