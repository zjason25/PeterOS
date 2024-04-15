#ifndef _exm_h_
#define _exm_h_

#define MAX_PROC 16
#define MAX_RESRC 4
#define DEFAULT_N_UNITS 3
#define DEFAULT_U0 1
#define DEFAULT_U1 1
#define DEFAULT_U2 2
#define DEFAULT_U3 3

namespace PeterOS {
    typedef int RC;                                                       // return code: 0 for success, -1 for error

    template <typename Value>
    struct Node {
        Value value;
        struct Node* next = nullptr;
    };

    class ExtendedManager {
    public:
        static ExtendedManager &instance();                               // Access to the singleton instance

        RC create(int priority);
        RC destroy(int proc_id);                                          // recursively destroy
        RC request(int resrc_id, int k);
        RC release(int resrc_id, int k);
        RC timeout();                                                     // moves proccess i from head of RL to end
        RC scheduler();                                                   // prints head of RL
        RC init(int n, int u0, int u1, int u2, int u3);
        RC init_default();
        int pid = 0;                                                      // process id is not reused

        struct proc {
            int state;                                                    // ready or blocked: 1 or 0
            int parent;                                                   // index of parent process i
            int priority;                                                 // priority on ready list
            Node<int>* children = nullptr;                                // head of child process linked list
            Node<int>* resources = nullptr;                               // head of resources held by current process
        };
        
        struct rsrc {
            int inventory;                                                // initial number of units
            int state;                                                    // number of available units
            Node<proc*>* waitlist = nullptr;
        };
        
        private:
            ExtendedManager();                                            // Prevent construction
            ~ExtendedManager();                                           // Prevent unwanted destruction
            ExtendedManager(const ExtendedManager &);                     // Prevent construction by copying
            ExtendedManager &operator=(const ExtendedManager &);          // Prevent assignment

            proc PCB[MAX_PROC];                                           // No reuse process control block
            Node<rsrc*> RCB[MAX_RESRC];
            Node<int>** RL = nullptr;                                     // pointer to an array of integer linked lists

    };
}

#endif // _exm_h_