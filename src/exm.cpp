#include <iostream>
#include <string>
#include "exm.h"


namespace PeterOS {
ExtendedManager& ExtendedManager::instance() {
  static ExtendedManager _ex_manager = ExtendedManager();
  return _ex_manager;
}
ExtendedManager::ExtendedManager() {
  openLogFile("output.txt");
};
ExtendedManager::~ExtendedManager() {
   if (outFile.is_open()) {
    outFile.close(); // Ensure the file is closed on destruction
    }
};
ExtendedManager::ExtendedManager(const ExtendedManager&) = delete;
ExtendedManager& ExtendedManager::operator=(const ExtendedManager&) = delete;

// check
RC ExtendedManager::create(int p) {
  // error checking
  if (p >= RL_levels || p < 0) {
    std::cerr << "Cannot create process " << pid << " with priority " << p
              << std::endl;
    log("-1");
    return -1;
  }

  if (pid > 0 && p == 0) {
    std::cerr << "Cannot create process at priority 0 " << std::endl;
    log("-1");
    return -1;
  }

  if (pid > 15) {
    std::cerr << "Cannot create more than " << MAX_PROC << " processes "
              << std::endl;
    log("-1");
    return -1;
  }

  // allocate new proc i
  PCB[pid] = new proc;
  proc* new_proc = PCB[pid];
  new_proc->ready = 1;
  new_proc->p = p;
  Node<int>* proc_i = new Node<int>{pid, nullptr};

  // set parent for proc i
  if (pid == 0) {
    new_proc->parent = NULL_PROC;
  } else {
    Node<int>* rng_proc_i = nullptr;
    for (int i = RL_levels - 1; i >= 0; i--) {
      if (RL[i] != nullptr) {
        rng_proc_i = RL[i];
        break;
      }
    }
    if (rng_proc_i == nullptr) {
      std::cerr << "error" << std::endl;
      log("-1");
      return -1;
    }
    new_proc->parent = rng_proc_i->value;

    // insert proc i into parent's list of children: FIFO
    if (PCB[new_proc->parent]->children == nullptr) {
      PCB[new_proc->parent]->children = proc_i;
    } else {
      Node<int>* children = PCB[new_proc->parent]->children;
      while (children->next != nullptr) {
        children = children->next;
      }
      children->next = proc_i;
    }
  }

  // place process i into RL: FIFO
  proc_i = new Node<int>{pid, nullptr};
  if (RL[p] == nullptr) {
    RL[p] = proc_i;
  } else {
    Node<int>* RL_head = RL[p];
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

bool ExtendedManager::isValidDestroy(int proc_id) {
  // error checking:
  // 1.invalid process id
  if (proc_id >= MAX_PROC || proc_id <= INIT_PROC) {
    std::cerr << "Invalid process id" << std::endl;
    log("-1");
    return false;
  }
  // 2.destroying non-existent process
  proc* proc_de = PCB[proc_id];  // process to be destroyed
  if (proc_de == nullptr) {
    std::cerr << "Process " << proc_id << " does not exist" << std::endl;
    log("-1");
    return false;
  }
  // locate running process id
  Node<int>* rng_proc_i = nullptr;
  for (int i = RL_levels - 1; i >= 0; i--) {
    if (RL[i] != nullptr) {
      rng_proc_i = RL[i];
      break;
    }
  }
  // 3.not a running process and not a child of running process
  if (rng_proc_i->value != proc_id && proc_de->parent != rng_proc_i->value) {
    std::cerr << "Cannot destroy process " << proc_id << std::endl;
    log("-1");
    return false;
  }

  return true;
}

// TODO: test release resource
int ExtendedManager::destroy(int proc_id, int &rec) {
  
  proc* cur_proc = PCB[proc_id];

  // recursively destroy proc i's children
  Node<int>* child = cur_proc->children;

  int num_proc = 1;  // the process itself
  Node<int>* nextChild = nullptr; 
  while (child != nullptr) {
    nextChild = child->next;
    rec = 1;
    num_proc += destroy(child->value, rec);
    child = nextChild;
  }

  // remove proc i from its parent's list of children
  proc* parent = PCB[cur_proc->parent];
  child = parent->children;
  Node<int>* prev = nullptr;
  Node<int>* cur = child;
  Node<int>* next = nullptr;
  while (cur != nullptr) {
    next = cur->next;
    if (cur->value == proc_id) {
      if (prev == nullptr) {
        parent->children = next;
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

  // remove proc i from RL
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

  // release all resources held by proc i
  if (cur_proc->resources != nullptr) {
    std::cout << "Removing resources\n";
    Node<rsrc_unit*>* cur_rsrc = cur_proc->resources;
    while (cur_rsrc != nullptr) {
      rsrc_unit* unit = cur_rsrc->value;
      release(proc_id, unit->index, unit->units_requested); // need to fix
      cur_rsrc = cur_rsrc->next;
    }
  }
  
  delete PCB[proc_id];
  PCB[proc_id] = nullptr;

  // if it's a recursive call, only return the number of proc destroyed
  if (rec) {
    rec = 0;
    return num_proc;
  }
  
  std::cout << num_proc << " processes destroyed" << std::endl;
  scheduler();
  return 0;
}

// TODO: test
RC ExtendedManager::request(int resrc_id, int k ) {
  // Error checking
  // Invalid resource id or resource amount
  if (resrc_id >= MAX_RESRC || resrc_id < INIT_PROC || k <= INIT_PROC) {
    std::cerr << "error" << std::endl;
    log("-1");
    return -1;
  }
  // Resource does not exist
  rsrc* resource = RCB[resrc_id];
  if (resource == nullptr) {
    std::cerr << "Resource does not exist" << std::endl;
    log("-1");
    return -1;
  }

  Node<int>* rng_proc_i = nullptr;
  for (int i = RL_levels - 1; i >= 0; i--) {
    if (RL[i] != nullptr) {
      rng_proc_i = RL[i];
      break;
    }
  }
  // No process exists on RL
  if (rng_proc_i == nullptr) {
    std::cerr << "No process exists" << std::endl;
    log("-1");
    return -1;
  }
  // Init process cannot request rsrc
  if (PCB[rng_proc_i->value]->parent == NULL_PROC) {
    std::cerr << "Init proc cannot request resource" << std::endl;
    log("-1");
    return -1;
  }

  // prevents a process from being blocked forever
  Node<rsrc_unit*>* rsrc = PCB[rng_proc_i->value]->resources;
  int cur_rsrc = 0;
  while (rsrc != nullptr) {
    rsrc_unit* unit = rsrc->value;
    if (unit->index == resrc_id) {
      cur_rsrc += unit->units_requested;
      break;
    }
    rsrc = rsrc->next;
  }
  if (k + cur_rsrc > resource->inventory) {
    std::cerr << "Invalid resource amount" << std::endl;
    log("-1");
    return -1;
  }
  // End of error checking


  // allocate the resource
  proc* rng_proc = PCB[rng_proc_i->value];
  if (resource->state >= k) {
    resource->state -= k;

  // insert (r, k) into proc i's resources: FIFO
  rsrc = rng_proc->resources;
    // insert at head
  if (rsrc == nullptr) {
    rsrc_unit* rsrc_units = new rsrc_unit{resrc_id, k};
    rng_proc->resources = new Node<rsrc_unit*>{rsrc_units, nullptr};
  } else {
    // updating existing node
    while (rsrc->next != nullptr) {
      rsrc_unit* unit = rsrc->value;
      if (unit->index == resrc_id) {
        unit->units_requested += k;
        break;
      }
      rsrc = rsrc->next;
    }
    // insert at tail
    if (rsrc->next == nullptr) {
      rsrc_unit* rsrc_units = new rsrc_unit{resrc_id, k};
      rsrc->next = new Node<rsrc_unit*>{rsrc_units, nullptr};
    }
  }
  std::cout << "Allocated " << k << " units of Resource " << resrc_id
            << " to Process " << rng_proc_i->value << std::endl;
  } else {
    // block process
    rng_proc->ready = 0;

    // remove proc from RL
    Node<int>* RL_head = RL[rng_proc->p];
    int i = RL_head->value;
    RL[rng_proc->p] = RL[rng_proc->p]->next;
    delete RL_head;

    // insert (i, k) into resource r's waitlist: FIFO
    w_proc* wt_data = new w_proc{i, k};
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

  }
  scheduler(); // placed here on purpose to output RL head to output.txt
  return 0;
}

int ExtendedManager::isValidRelease(int resrc_id, int k) {
  // used for commandline call to release()
  if (k <= INIT_PROC) {
    std::cerr << "Invalid amount to release" << std::endl;
    log("-1");
    return -1;
  }

  Node<int>* rng_proc_i = nullptr;
  for (int i = RL_levels - 1; i >= 0; i--) {
    if (RL[i] != nullptr) {
      rng_proc_i = RL[i];
      break;
    }
  }

  if (rng_proc_i == nullptr) {
    std::cerr << "No process exitst on RL" << std::endl;
    log("-1");
    return -1;
  }

  Node<rsrc_unit*>* rsrcs = PCB[rng_proc_i->value]->resources;
  if (rsrcs == nullptr) {
    std::cerr << "Current running process " << rng_proc_i->value << " does not any Resource " << std::endl;
    log("-1");
    return -1;
  }

  rsrc_unit* unit = nullptr;
  while (rsrcs != nullptr) {
    rsrc_unit* cur_unit = rsrcs->value;
    if (cur_unit->index == resrc_id) {
      unit = cur_unit;
      break;
    }
    rsrcs = rsrcs->next;
  }
  
  if (unit == nullptr) {
    std::cerr << "Current running process " << rng_proc_i->value << " does not hold Resource " << resrc_id << std::endl;
    log("-1");
    return -1;
  }

  if (unit->units_requested < k) {
    std::cerr << "Invalid amount to release" << std::endl;
    log("-1");
    return -1;
  }

  return rng_proc_i->value; // returns proc_id of running process
}

// TODO: test
RC ExtendedManager::release(int proc_id, int resrc_id, int k) {
  Node<rsrc_unit*>* rsrcs = PCB[proc_id]->resources;

  Node<rsrc_unit*>* prev = nullptr;
  Node<rsrc_unit*>* cur = rsrcs; 
  while (cur != nullptr) {
    if (cur->value->index == resrc_id) {
      if (cur->value->units_requested == k) {
        if (prev == nullptr) {
          PCB[proc_id]->resources = cur->next;
        } else {
          prev->next = cur->next;
        }
        delete cur->value;
        delete cur;
        cur = nullptr;
      } else {
        cur->value->units_requested -= k;
      }
      break;
    }
    prev = cur;
    cur = cur->next;
  }

  // update RCB
  rsrc* resource = RCB[resrc_id];
  resource->state += k;

  // unblock process from waitlist
  Node<w_proc*>* wtlist = resource->waitlist;
  Node<w_proc*>* prev_list = nullptr;
  while (wtlist != nullptr) {
    w_proc* wt_proc = wtlist->value;
    int proc_i = wt_proc->proc_id;
    int units = wt_proc->units_requested;
    if (units <= resource->state) {
      resource->state -= units;
      proc* cur_proc = PCB[proc_i];

      // insert (r, k) into proc i's resources
      Node<rsrc_unit*>* rsrc = cur_proc->resources;
      if (rsrc == nullptr) {
        rsrc_unit* rsrc_units = new rsrc_unit{resrc_id, units};
        Node<rsrc_unit*>* new_rsrc = new Node<rsrc_unit*>{rsrc_units, nullptr};
        cur_proc->resources = new_rsrc;
      } else {
        while (rsrc->next != nullptr) {
          // if the proc i already has some resrc i, just add to it
          rsrc_unit* unit = rsrc->value;
          if (unit->index == resrc_id) {
            unit->units_requested += units;
            break;
          }
          rsrc = rsrc->next;
        }
        if (rsrc->next == nullptr) {
          rsrc_unit* rsrc_units = new rsrc_unit{resrc_id, units};
          Node<rsrc_unit*>* new_rsrc = new Node<rsrc_unit*>{rsrc_units, nullptr};
          rsrc->next = new_rsrc;
        }
      }

      cur_proc->ready = 1;

      // remove (j, k) from waitlist
      if (prev_list == nullptr) {
        resource->waitlist = resource->waitlist->next;
      } else {
        prev_list->next = wtlist->next;
      }
      delete wtlist;

      // insert j into RL
      Node<int>* RL_level = RL[cur_proc->p];
      if (RL_level == nullptr) {
        RL[cur_proc->p] = new Node<int>{proc_i, nullptr};
      } else {
        while (RL_level->next != nullptr) {
          RL_level = RL_level->next;
        }
        RL_level = new Node<int>{proc_i, nullptr};
      }

      prev_list = wtlist;
      wtlist = wtlist->next;
    } else {
      break;
    }
  }

  scheduler();
  return 0; 
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
    log("-1");
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
    log("-1");
    return -1;
  }
  std::cout << "process " << rng_proc_i->value << " running" << std::endl;
  log(std::to_string(rng_proc_i->value));
  return 0; 
}

RC ExtendedManager::init(int n, int u0, int u1, int u2, int u3) {
  if (n < 2) {
    std::cerr << "Priority level must be at least 2" << std::endl;
    return -1;
  }
  
  if (this->init_status) {
    outFile << std::endl;
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

void ExtendedManager::print_resource(int i) {
  if (i < 0 || i >= MAX_RESRC) {
    std::cerr << "error" << std::endl;
  } else {
    rsrc* resource = RCB[i];
    std::cout << "[Resource " << i << "]" << std::endl;
    std::cout << "Inventory: " << resource->inventory << std::endl;
    std::cout << "State: " << resource->state << std::endl;
    std::cout << "Waitlist: ";
    Node<w_proc*>* waitlist = resource->waitlist;
    while (waitlist != nullptr) {
      w_proc* wt_proc = waitlist->value;
      std::cout << "(" << wt_proc->proc_id << ", " << wt_proc->units_requested;
      if (waitlist->next == nullptr) {
        std::cout << ")" << std::endl;
      } else {
        std::cout << "), " << std::endl;
      }
      waitlist = waitlist->next;
    }
  }
}

void ExtendedManager::openLogFile(const std::string &filename) {
    outFile.open(filename);
    if (!outFile) {
        std::cerr << "Failed to open " << filename << " for writing." << std::endl;
    }
}

void ExtendedManager::log(const std::string& message) {
  if (outFile.is_open()) {
    if (pid == 1) {
      outFile << message;
    } else {
      outFile << " " + message;
    }
  } else {
      std::cerr << "Logfile is not open for writing." << std::endl;
  }
}
}  // namespace PeterOS