#ifndef _exm_h_
#define _exm_h_

#define MAX_PROC 16
#define MAX_RESRC 4

#include <vector>

namespace PeterOS {
    typedef int RC; // return code: 0 for success, -1 for error
    class ExtendedManager {
         

    struct proc {
        unsigned state; // ready or blocked: 0 or 1
        unsigned parent; // index of parent process i
        child_list* children; // head of child process linked list
        resrc_list* resources; // head of resources held by current process
    };

    struct RL {
        unsigned id;
        unsigned state; // 0 for free, 1 for allocated
        RL* next;
    };
    
    struct WL {
        unsigned proc_id;
        WL* next;
    };

    struct resrc {
        unsigned id;
        unsigned proc;
        WL* waitlist; // head of waitlist
    };

    struct child_list {
        proc* cur;
        proc* next;
    };

    struct resrc_list {
        unsigned id; // resource id
        resrc* next; // next id
    };



    public:
        ExtendedManager();
        ~ExtendedManager();
        RC create(unsigned priority);
        RC destroy(unsigned proc_id); // recursively destroy
        RC request(unsigned resrc_id, unsigned k);
        RC release(unsigned resrc_id, unsigned k);
        RC timeout(); // moves proccess i from head of RL to end
        RC scheduler(); // prints head of RL
        RC init();
        RC init_default();
        unsigned pid = 0; // process id is not reused

    private:
        proc PCB[MAX_PROC]; // No reuse process control block
        resrc RCB[MAX_RESRC];
        RL* RL; // a head to Ready List linked list
    };
}

#endif // _exm_h_