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

    RC ExtendedManager::create(int p) {
        // error checking
        if (p >= RL_levels) {
            std::cerr << "Cannot create process " << pid << " with priority " << p << std::endl;
            return -1; 
        }
        // 1. allocate PCB
        // 2. place process i into RL
        // 3. set parent

        proc* new_proc = new proc;
        Node<int>* proc_i = new Node<int>{pid, nullptr};
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
                    std::cout << "parent: " << RL[n]->value << std::endl;
                    Node<int>* chld_head = PCB[RL[n]->value]->children;
                    Node<int>* cur = chld_head;
                    Node<int>* prev = nullptr;
                    while (cur != nullptr) {
                        prev = cur;
                        cur = cur->next;
                    }
                    if (prev == nullptr) {
                        PCB[RL[n]->value]->children = proc_i;
                    }
                    else {
                        prev->next = proc_i;
                    }
                    break; 
                }
                n--;
            }
        }

        new_proc->p = p;
        // children and resources are null;
        PCB[pid] = new_proc;
        Node<int>* head = RL[p];
        Node<int>* cur = head;
        Node<int>* prev = nullptr;
        while (cur != nullptr) {
            prev = cur;
            cur = cur->next;
        }
        if (prev == nullptr) {
            RL[p] = proc_i;
        } else {
            prev->next = proc_i;
        }
        
        std::cout << "process " << pid << " created" << std::endl;
        print_RL();
        print_children(pid);
        pid++;
        

        return 0;
    }

    // TODO: test
    RC ExtendedManager::destroy(int proc_id) {
        proc* process = PCB[proc_id];
        // destroy all proc j's children and grandchildren
        if (process->children != nullptr) {
            Node<int>* child = process->children;
            while (child != nullptr) {
                destroy(child->value);
                child = child->next;
            }
        }
        // remove proc j from its parent's list of children
        Node<int>* prev = nullptr;
        Node<int>* cur = nullptr;
        if (process->parent != -1) {
            proc* parent = PCB[process->parent];
            Node<int>* child = parent->children;
            prev = nullptr;
            cur = child;
            while (cur != nullptr) {
                if (cur->value == proc_id) {
                    if (prev == nullptr) {
                        child = cur->next;
                    }
                    else {
                        prev->next = cur->next;
                    }
                    break;
                }
                cur = cur->next;
            }
        }
        // remove proc j from RL
        Node<int>* RL_level = RL[process->p];
        prev = nullptr;
        cur = RL_level;
        while (cur != nullptr) {
            if (cur->value == proc_id) {
                if (prev == nullptr) {
                    RL_level = cur->next;
                }
                else {
                    prev->next = cur->next;
                }
                break;
            }
            cur = cur->next;
        }

        // release all resources held by proc j
        if (process->resources != nullptr) {
            rsrc_unit* rsrcs = process->resources->value;
            while (rsrcs != nullptr) {
                Node<rsrc*> resource = RCB[rsrcs->index];
                // shouldn't be nullptr
                if (resource.value != nullptr) {
                    resource.value->inventory += rsrcs->units_requested;
                    // remove from waitlist
                    Node<w_proc*>* waitlist = resource.value->waitlist;
                    Node<w_proc*>* head = waitlist;
                    Node<w_proc*>* prev = nullptr;
                    while (head != nullptr) {
                        if (head->value->proc_id == proc_id) {
                            if (prev == nullptr) {
                                waitlist = head->next;
                            }
                            else {
                                prev->next = head->next;
                            }
                            break;
                        }
                    }
                }
            }
        }
        delete PCB[proc_id];
        std::cout << "n processes destroyed" << std::endl;

        return 0;
    }

    // TODO: test
    RC ExtendedManager::request(int resrc_id, int k) {
        Node<rsrc*> resource = RCB[resrc_id];
        if (resource.value->inventory < k) {
            std::cerr << "Not enough units in Resource " << resrc_id << std::endl;
            return -1;
        }

        // locate the running process with highest priority
        int cur_level = RL_levels - 1;
        Node<int>* head = RL[cur_level];
        while (cur_level >= 0) {
            if (head != nullptr) {
                head = RL[cur_level];
                break;
            }
            cur_level--;
        }
        // shouldn't happen; just in case
        if (cur_level < 0) {
            std::cerr << "error requesting resource" << std::endl;
            return -1;
        }

        // allocate the resource
        proc* cur_proc = PCB[head->value];
        rsrc_unit* rsrc_requested = new rsrc_unit;
        rsrc_requested->index = resrc_id;
        rsrc_requested->units_requested = k;
        Node<rsrc_unit*>* new_rsrc_unit = new Node<rsrc_unit*>{rsrc_requested, nullptr};
        Node<rsrc_unit*>* head_rsrc = cur_proc->resources;
        if (head_rsrc == nullptr) {
            cur_proc->resources = new_rsrc_unit;
        }
        else {
            Node<rsrc_unit*>* prev = nullptr;
            while (head_rsrc != nullptr) {
                prev = head_rsrc;
                head_rsrc = head_rsrc->next;
            }
            prev->next = new_rsrc_unit;
        }

        std::cout << "Resource " << resrc_id << " allocated " << k << " units" << std::endl;
        return 0;
    }

    RC ExtendedManager::release(int resrc_id, int k) {
        
        return resrc_id + k;
    
    }

    RC ExtendedManager::timeout() {
        std::cout << "Timeout" << std::endl;
        return 0;
    }

    RC ExtendedManager::scheduler() {return 0;}
    
    // RC ExtendedManager::clearAll(){return 0;}

    RC ExtendedManager::init(int n, int u0, int u1, int u2, int u3) {
        /* 
        1. initialize n priority levels in ready list
        2. initialize inventory for each of four resources
        3. create Process 0
        */

        this->RL = new Node<int>*[n];

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
    
    RC ExtendedManager::init_default() {
        return init(DEFAULT_N_UNITS, DEFAULT_U0, DEFAULT_U1, DEFAULT_U2, DEFAULT_U3);
    }

    void ExtendedManager::print_RL() {
        std::cout << "P" << std::endl;
        for (int i = 0; i < RL_levels; i++) {
            Node<int>* proc_i = RL[i];
            std::cout << i << " Head: ";
            while (proc_i != nullptr) {
                if (proc_i->next == nullptr) {
                    std::cout << "process " << proc_i->value;
                } else {
                    std::cout << "process " << proc_i->value << " -> ";
                }
                proc_i = proc_i->next;
            }
            std::cout << std::endl;
        }
    }

    void ExtendedManager::print_parent(int i) {
        std::cout << "process " << i << "'s parent: " << PCB[i]->parent << std::endl;
    }

    void ExtendedManager::print_children(int i) {
        std::cout << "process " << i << "'s parent: ";
        Node<int>* head = PCB[i]->children;
        while (head != nullptr) {
            if (head->next == nullptr) {
                std::cout << head->value;
            } else {
                std::cout << head->value << " -> ";
            }
            head = head->next;
        }
        std::cout << std::endl;
    }

}