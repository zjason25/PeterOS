#include "exm.h"
#include <vector>
#include <iostream>

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
    
    // RC ExtendedManager::clearAll(){return 0;}

    RC ExtendedManager::init(int n, int u0, int u1, int u2, int u3){
        /* 
        1. initialize n priority levels in ready list
        2. initialize inventory for each of four resources
        3. create Process 0
        */

        RL = new Node<int>*[n];

        std::vector<int> units = {u0, u1, u2, u3};
        for (int i = 0; i < MAX_RESRC; i++) {
            rsrc* new_rsrc = new rsrc;
            new_rsrc->inventory = units[i];
            new_rsrc->state = new_rsrc->inventory;
            RCB[i].value = new_rsrc;
        }

        // debug
        // for (int i = 0; i < MAX_RESRC; i++) {
        //     std::cout << "Initialized: " << RCB[i].value->state << " units at Resource " << i << std::endl;
        // }

        create(0); // create Process 0



        return 0;
    }

    
    RC ExtendedManager::init_default(){
        return init(DEFAULT_N_UNITS, DEFAULT_U0, DEFAULT_U1, DEFAULT_U2, DEFAULT_U3);
    }
}