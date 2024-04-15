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

namespace PeterOS {
    typedef int RC;                                                       // return code: 0 for success, -1 for error

    template <typename Value>                                             // a custom Node struct that takes any value type
    struct Node {
        Value value;
        struct Node* next = nullptr;
        
        Node() = default;                                                 // default constructor
        Node(Value val, Node* nxt = nullptr) : value(val), next(nxt) {};  // construct by value
    };

    class ExtendedManager {
    public:
        static ExtendedManager &instance();                               // Access to the singleton instance

        RC create(int p);                                                 // creates a process i of p priority
        RC destroy(int proc_id);                                          // recursively destroy
        RC request(int resrc_id, int k);
        RC release(int resrc_id, int k);
        RC timeout();                                                     // moves proccess i from head of RL to end
        RC scheduler();                                                   // prints head of RL
        RC init(int n, int u0, int u1, int u2, int u3);                   // initialize system with given parameters
        RC init_default();                                                // initialize system with default parameters
        int pid = 0;                                                      // process id is not reused
        int RL_levels = 0;

        struct proc {
            int state;                                                    // ready or blocked: 1 or 0
            int parent;                                                   // index of parent process i. -1 if no parent
            int p;                                                        // priority on ready list
            Node<int>* children = nullptr;                                // head of child process linked list
            Node<int>* resources = nullptr;                               // head of resources held by current process
        };
        
        struct rsrc {
            int inventory;                                                // initial number of units
            int state;                                                    // number of available units
            Node<proc*>* waitlist = nullptr;                              // a linked list of processes
        };
        
        private:
            ExtendedManager();                                            // Prevent construction
            ~ExtendedManager();                                           // Prevent unwanted destruction
            ExtendedManager(const ExtendedManager &);                     // Prevent construction by copying
            ExtendedManager &operator=(const ExtendedManager &);          // Prevent assignment

            proc* PCB[MAX_PROC];                                          // No reuse process control block
            Node<rsrc*> RCB[MAX_RESRC];                                   // A static array of pointers
            Node<int>** RL = nullptr;                                     // pointer to an array of integer linked lists

    };
}

#endif // _exm_h_