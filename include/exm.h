#ifndef _exm_h_
#define _exm_h_

#define MAX_PROC 16
#define MAX_RESRC 4
#define DEFAULT_N_UNITS 3
#define DEFAULT_U0 1
#define DEFAULT_U1 1
#define DEFAULT_U2 2
#define DEFAULT_U3 3
#define INIT_PROC 0
#define NULL_PROC -1

namespace PeterOS {
    typedef int RC;                                                       // return code: 0 for success, -1 for error

    template <typename Value>                                             // a custom Node struct that takes any value type
    struct Node {
        Value value;
        struct Node* next = nullptr;
        
        Node() : value(), next(nullptr) {};                               // default constructor
        Node(Value val, Node* nxt = nullptr) : value(val), next(nxt) {};  // construct by value
    };

    class ExtendedManager {
    public:
        static ExtendedManager &instance();                               // Access to the singleton class instance

        RC create(int p);                                                 // creates a process i of p priority
        RC destroy(int proc_id, int &rec);                                // recursively destroy. rec flag indicates recursive status
        RC destroy(int proc_id) {                                         // overload destroy() to handle
            int defaultRec = 0;
            return destroy(proc_id, defaultRec);
        }
        RC request(int resrc_id, int k);                                  // requests k units of resource i
        RC release(int resrc_id, int k);                                  // releases k units of resource i
        RC timeout();                                                     // moves proccess i from head of RL to end
        RC scheduler();                                                   // prints head of RL
        RC init(int n, int u0, int u1, int u2, int u3);                   // initialize system with given parameters
        RC init_default();                                                // initialize system with default parameters
        int pid = 0;                                                      // process id is not reused
        int RL_levels = -1;                                               // levels of priority
        int init_status = 0;                                              // enables reset() after the first system startup
        void reset();                                                     // reset PCB, RCB, RL and delete all process
        bool verbose = false;



        struct rsrc_unit;
        struct w_proc;

        // represents a process
        struct proc {
            int ready;                                                    // ready or blocked: 1 or 0
            int parent;                                                   // index of parent process i. -1 if no parent
            int p;                                                        // priority on ready list
            Node<int>* children = nullptr;                                // head of child process linked list (of integers)
            Node<rsrc_unit*>* resources = nullptr;                        // head of resources linked list (of rsrc_unit*)
        };
        
        // represents resource at each priority level
        struct rsrc {
            int inventory;                                                // initial number of units
            int state;                                                    // number of available units
            Node<w_proc*>* waitlist = nullptr;                            // a linked list of waiting processes
        };

        // represents resource being requested
        struct rsrc_unit {
            int index;
            int units_requested;
        };

        // represents waiting processes
        struct w_proc {
            int proc_id;
            int units_requested;
        };
        
        private:
            ExtendedManager();                                            // Prevent construction
            ~ExtendedManager();                                           // Prevent unwanted destruction
            ExtendedManager(const ExtendedManager &);                     // Prevent construction by copying
            ExtendedManager &operator=(const ExtendedManager &);          // Prevent assignment

            proc* PCB[MAX_PROC];                                          // No reuse process control block
            rsrc* RCB[MAX_RESRC];                                         // A static array of rsrcs
            Node<int>** RL = nullptr;                                     // pointer to an array of integer linked lists

    };
}

#endif // _exm_h_