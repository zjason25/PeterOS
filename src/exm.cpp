#include "exm.h"

#include <iostream>

namespace PeterOS {
ExtendedManager& ExtendedManager::instance() {
  static ExtendedManager _ex_manager = ExtendedManager();
  return _ex_manager;
}
ExtendedManager::ExtendedManager() = default;
ExtendedManager::~ExtendedManager() = default;
ExtendedManager::ExtendedManager(const ExtendedManager&) = default;
ExtendedManager& ExtendedManager::operator=(const ExtendedManager&) = default;

RC ExtendedManager::create(int p) {
  // error checking
  if (p >= RL_levels || p < 0) {
    std::cerr << "Cannot create process " << pid << " with priority " << p
              << std::endl;
    return -1;
  }

  if (pid > 15) {
    std::cerr << "Cannot create more than " << MAX_PROC << " processes "
              << std::endl;
    return -1;
  }

  // allocate new proc i
  PCB[pid] = new proc;
  Node<int>* proc_i = new Node<int>{pid, nullptr};
  proc* new_proc = PCB[pid];
  new_proc->state = 1;
  new_proc->p = p;

  // set parent for proc i
  if (pid == 0) {
    new_proc->parent = NULL_PROC;  // parent of process 0 is NULL
  } else {
    // find the pid of the first process on RL with the highest priority
    // (currently running process)
    int n = RL_levels - 1;
    while (n >= 0) {
      if (RL[n] != nullptr) {
        // insert proc i at the parent's children
        new_proc->parent = RL[n]->value;
        std::cout << "parent: " << new_proc->parent << std::endl;
        // delete children from oldest to newest?
        proc_i->next = PCB[new_proc->parent]->children;
        PCB[new_proc->parent]->children = proc_i;
        break;
      }
      n--;
    }
  }

  proc_i = new Node<int>{pid, nullptr};
  // place process i into RL: FIFO
  Node<int>* RL_head = RL[p];
  if (RL_head == nullptr) {
    RL[p] = proc_i;
  } else {
    while (RL_head->next != nullptr) {
      RL_head = RL_head->next;
    }
    RL_head->next = proc_i;
  }

  std::cout << "process " << pid << " created" << std::endl;
  pid++;
  scheduler();

  return 0;
}

// TODO: test release resource
int ExtendedManager::destroy(int proc_id, int rec) {
  // First locate running process
  Node<int>* rng_proc_i = nullptr;
  for (int i = RL_levels - 1; i >= 0; i--) {
    if (RL[i] != nullptr) {
      rng_proc_i = RL[i];
      break;
    }
  }
  // error checking:
  // 1.no process exists yet
  if (rng_proc_i == nullptr) {
    std::cerr << "No process exists" << std::endl;
    return -1;
  }
  // 2.invalid process id
  if (proc_id >= MAX_PROC || proc_id < INIT_PROC) {
    std::cerr << "Invalid process id" << std::endl;
    return -1;
  }
  // 3.destroying non-existent process
  proc* proc_de = PCB[proc_id];  // process to be destroyed
  if (proc_de == nullptr) {
    std::cerr << "Process " << proc_id << " does not exist" << std::endl;
    return -1;
  }
  // 4.not a running process and not a child of running process
  if (rng_proc_i->value != proc_id && proc_de->parent != rng_proc_i->value) {
    std::cerr << "Cannot destroy process " << proc_id << std::endl;
    return -1;
  }
  // 5.init process destroying itself
  if (proc_id == INIT_PROC) {
    std::cerr << "Cannot destroy init process" << std::endl;
    return -1;
  }

  proc* rng_proc = PCB[rng_proc_i->value];  // the running process

  // destroy all proc i's children and grandchildren
  Node<int>* child = nullptr;
  proc* cur_proc = nullptr;

  // destroying a child process
  if (proc_de->parent == rng_proc_i->value) {
    child = proc_de->children;
    cur_proc = proc_de;
    // destroying a running process
  } else {
    child = rng_proc->children;
    cur_proc = rng_proc;
  }
  int num_proc = 1;  // the process itself
  Node<int>* nextChild = nullptr;
  while (child != nullptr) {
    nextChild = child->next;
    num_proc += destroy(child->value, 1);
    child = nextChild;
  }

  // remove proc j from its parent's list of children
  proc* parent = PCB[cur_proc->parent];
  child = parent->children;
  Node<int>* prev = nullptr;
  Node<int>* cur = child;
  Node<int>* next = nullptr;
  while (cur != nullptr) {
    next = cur->next;
    if (cur->value == proc_id) {
      if (prev == nullptr) {
        parent->children = cur->next;
      } else {
        prev->next = cur->next;
      }
      delete cur;
      cur = nullptr;
      break;
    }
    prev = cur;
    cur = next;
  }

  // remove proc j from RL
  prev = nullptr;
  cur = RL[cur_proc->p];
  while (cur != nullptr) {
    next = cur->next;
    if (cur->value == proc_id) {
      if (prev == nullptr) {
        RL[cur_proc->p] = next;
      } else {
        prev->next = next;
      }
      delete cur;
      cur = nullptr;
      break;
    }
    prev = cur;
    cur = next;
  }

  // // TODO: test after rq and rl
  // // release all resources held by proc j
  // if (cur_proc->resources != nullptr) {
  //   std::cout << "removing resources";
  //   rsrc_unit* rsrcs = cur_proc->resources->value;
  //   while (rsrcs != nullptr) {
  //     Node<rsrc*> resource = RCB[rsrcs->index];
  //     resource.value->state += rsrcs->units_requested;
  //     // remove from waitlist
  //     Node<w_proc*>* head = resource.value->waitlist;
  //     Node<w_proc*>* prev = nullptr;
  //     while (head != nullptr) {
  //       if (head->value->proc_id == proc_id) {
  //         if (prev == nullptr) {
  //           resource.value->waitlist = head->next;
  //         } else {
  //           prev->next = head->next;
  //         }
  //         delete head;
  //         break;
  //       }
  //       head = head->next;
  //     }
  //   }
  // }

  delete PCB[proc_id];
  PCB[proc_id] = nullptr;

  // if not recursive call
  if (!rec) {
    std::cout << num_proc << " processes destroyed" << std::endl;
    scheduler();
    return 0;
  }

  return num_proc;
}

// TODO: test
RC ExtendedManager::request(int resrc_id, int k) {
  // First locate running process
  Node<int>* rng_proc_i = nullptr;
  for (int i = RL_levels - 1; i >= 0; i--) {
    if (RL[i] != nullptr) {
      rng_proc_i = RL[i];
      break;
    }
  }
  // Error checking:
  // 1.no process exists yet
  if (rng_proc_i == nullptr) {
    std::cerr << "No process exists" << std::endl;
    return -1;
  }

  // 2.Resource does not exist
  rsrc* resource = RCB[resrc_id];
  if (resource == nullptr) {
    std::cerr << "Resource does not exist" << std::endl;
    return -1;
  }
  // 3.Invalid resource id
  if (resrc_id > MAX_RESRC || resrc_id < INIT_PROC) {
    std::cerr << "Invalid resource id" << std::endl;
    return -1;
  }
  // 4.Invalid resource amount
  if (k <= INIT_PROC) {
    std::cerr << "Invalid resource amount" << std::endl;
    return -1;
  }

  // allocate the resource
  proc* rng_proc = PCB[rng_proc_i->value];
  if (resource->state >= k) {
    // subtract the allocated units from resource i
    resource->state -= k;

    // insert (r, k) into proc i's resources
    rsrc_unit* rsrc_units = new rsrc_unit{resrc_id, k};
    Node<rsrc_unit*>* new_rsrc = new Node<rsrc_unit*>{rsrc_units, nullptr};
    Node<rsrc_unit*>* cur = rng_proc->resources;
    if (cur == nullptr) {
      rng_proc->resources = new_rsrc;
    } else {
      while (cur->next != nullptr) {
        cur = cur->next;
      }
      cur->next = new_rsrc;
    }
    std::cout << "Allocated " << k << " units of Resource " << resrc_id
              << " to Process " << rng_proc_i->value << std::endl;
  } else {
    // block process
    rng_proc->state = 0;

    // remove proc from RL
    Node<int>* RL_head = RL[rng_proc->p];
    int i = RL_head->value;
    RL[rng_proc->p] = RL[rng_proc->p]->next;
    delete RL_head;

    // insert (i, k) into resource r's waitlist
    w_proc* wt_data = new w_proc{rng_proc_i->value, k};
    Node<w_proc*>* wt_proc = new Node<w_proc*>{wt_data, nullptr};
    Node<w_proc*>* wtlist_head = resource->waitlist;
    if (wtlist_head == nullptr) {
      resource->waitlist = wt_proc;
    } else {
      while (wtlist_head->next != nullptr) {
        wtlist_head = wtlist_head->next;
      }
      wtlist_head->next = wt_proc;
    }
    std::cout << "Placing process " << i << " in waitlist " << resrc_id
              << std::endl;

    scheduler();
  }
  return 0;
}

RC ExtendedManager::release(int resrc_id, int k) {
  scheduler();
  return resrc_id + k; 
  }

RC ExtendedManager::timeout() {
  Node<int>* rng_proc_i = nullptr;
  int level = -1;
  for (int i = RL_levels - 1; i >= 0; i--) {
    if (RL[i] != nullptr) {
      rng_proc_i = RL[i];
      level = i;
      break;
    }
  }
  if (rng_proc_i == nullptr) {
    std::cerr << "No process exitst" << std::endl;
    return -1;
  }

  // move head to RL to the end
  if (RL[level]->next != nullptr) {
    RL[level] = RL[level]->next;

    Node<int>* cur = RL[level];
    while (cur->next != nullptr) {
      cur = cur->next;
    }
    cur->next = rng_proc_i;
    rng_proc_i->next = nullptr;
  }
  
  scheduler();
  return 0;
}

RC ExtendedManager::scheduler() {
  Node<int>* rng_proc_i = nullptr;
  for (int i = RL_levels - 1; i >= 0; i--) {
    if (RL[i] != nullptr) {
      rng_proc_i = RL[i];
      break;
    }
  }
  if (rng_proc_i == nullptr) {
    std::cerr << "No process exitst" << std::endl;
    return -1;
  }
  std::cout << "process " << rng_proc_i->value << " running" << std::endl;
  return 0; 
}

RC ExtendedManager::init(int n, int u0, int u1, int u2, int u3) {
  std::cout << "initializing\n";
  if (this->init_status) {
    reset();
  }

  // initialize n priority levels in ready list
  this->RL = new Node<int>*[n];
  this->RL_levels = n;

  for (int i = 0; i < n; ++i) {
    RL[i] = nullptr;
  }

  // initialize inventory for each of four resources
  int units[] = {u0, u1, u2, u3};
  for (int i = 0; i < MAX_RESRC; i++) {
    rsrc* new_rsrc = new rsrc;
    new_rsrc->inventory = units[i];
    new_rsrc->state = new_rsrc->inventory;
    RCB[i] = new_rsrc;
  }

  // create Process 0
  create(INIT_PROC);
  this->init_status = 1;

  return 0;
}

RC ExtendedManager::init_default() {
  // initialize system with default values
  return init(DEFAULT_N_UNITS, DEFAULT_U0, DEFAULT_U1, DEFAULT_U2, DEFAULT_U3);
}

void ExtendedManager::reset() {
  for (int i = 0; i < MAX_PROC; i++) {
    if (PCB[i] != nullptr) {
      Node<int>* child = PCB[i]->children;
      Node<int>* cur_chld = child;
      while (cur_chld != nullptr) {
        child = cur_chld->next;
        delete cur_chld;
        cur_chld = nullptr;
        cur_chld = child;
      }
      Node<rsrc_unit*>* rsrc = PCB[i]->resources;
      Node<rsrc_unit*>* cur_rsrc = rsrc;
      while (cur_rsrc != nullptr) {
        rsrc = cur_rsrc->next;
        delete cur_rsrc;
        cur_rsrc = nullptr;
        cur_rsrc = rsrc;
      }
      delete PCB[i];
      PCB[i] = nullptr;
    }
  }

  for (int i = 0; i < MAX_RESRC; ++i) {
    if (RCB[i] != nullptr) {
      Node<w_proc*>* wtlist = RCB[i]->waitlist;
      Node<w_proc*>* cur_lst = wtlist;
      while (cur_lst != nullptr) {
        wtlist = cur_lst->next;
        delete cur_lst;
        cur_lst = nullptr;
        cur_lst = wtlist;
      }
      delete RCB[i];
      RCB[i] = nullptr;
    }
  }

  if (RL != nullptr) {
    for (int i = 0; i < RL_levels; ++i) {
      if (RL[i] != nullptr) {
        delete RL[i];
        RL[i] = nullptr;
      }
    }
    delete RL;
    RL = nullptr;
  }

  this->pid = 0;
  this->RL_levels = -1;
}

void ExtendedManager::print_RL() {
  std::cout << "P" << std::endl;
  for (int i = 0; i < RL_levels; i++) {
    Node<int>* proc_i = RL[i];
    Node<int>* cur = proc_i;
    std::cout << i << " Head: ";
    while (cur != nullptr) {
      if (cur->next == nullptr) {
        std::cout << "process " << cur->value;
      } else {
        std::cout << "process " << cur->value << " -> ";
      }
      cur = cur->next;
    }
    std::cout << std::endl;
  }
}

void ExtendedManager::print_parent(int i) {
  if (i == 0) {
    std::cout << "process " << i << "'s parent: NULL" << std::endl;
  } else {
    std::cout << "process " << i << "'s parent: " << PCB[i]->parent
              << std::endl;
  }
}

void ExtendedManager::print_children(int i) {
  std::cout << "process " << i << "'s children: ";
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

}  // namespace PeterOS