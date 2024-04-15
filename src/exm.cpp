#include <iostream>
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

    RC ExtendedManager::create(int p){
        // error checking
        if (p >= RL_levels) {
            std::cerr << "Cannot create process " << pid << " with priority " << p << std::endl;
            return -1; 
        }
        // 1. allocate PCB
        // 2. place process i into RL

        proc* new_proc = new proc;
        new_proc->state = 1;
        // parent of process 0 is NULL
        if (pid == 0) {
            new_proc->parent = -1;
        }
        // find the pid of the process with the highest priority on RL
        else {
            int n = RL_levels - 1;
            while (n >= 0) {
                if (RL[n] != nullptr) {
                    new_proc->parent = RL[n]->value;
                    break; 
                }
                n--;
            }
        }
        new_proc->p = p;
        // children and resources are null;
        PCB[pid] = new_proc;
        Node<int>* proc_i = new Node<int>(p);
        Node<int>* head = RL[pid];
        Node<int>* cur = head;
        while (cur != nullptr) {
            cur = cur->next;
        }
        cur = proc_i;

        std::cout << "process " << pid << " created" << std::endl;
        pid++;

        return 0;
    }

    RC ExtendedManager::destroy(int proc_id){return proc_id;}

    RC ExtendedManager::request(int resrc_id, int k){return resrc_id + k;}

    RC ExtendedManager::release(int resrc_id, int k){return resrc_id + k;}

    RC ExtendedManager::timeout(){
        std::cout << "Timeout" << std::endl;
        return 0;
    }

    RC ExtendedManager::scheduler(){return 0;}
    
    // RC ExtendedManager::clearAll(){return 0;}

    RC ExtendedManager::init(int n, int u0, int u1, int u2, int u3){
        /* 
        1. initialize n priority levels in ready list
        2. initialize inventory for each of four resources
        3. create Process 0
        */

        RL = new Node<int>*[n];

        int units[] = {u0, u1, u2, u3};
        for (int i = 0; i < MAX_RESRC; i++) {
            rsrc* new_rsrc = new rsrc;
            new_rsrc->inventory = units[i];
            new_rsrc->state = new_rsrc->inventory;
            RCB[i].value = new_rsrc;
        }
        RL_levels = n; 
        create(INIT_PROC);
        return 0;
    }

    
    RC ExtendedManager::init_default(){
        return init(DEFAULT_N_UNITS, DEFAULT_U0, DEFAULT_U1, DEFAULT_U2, DEFAULT_U3);
    }
}