#ifndef _exm_h_
#define _exm_h_

#define MAX_PROC 16
#define MAX_RESRC 4

#include <vector>

namespace PeterOS {
    typedef int RC; // return code: 0 for success, -1 for error
    template <typename Value>
        struct Node {
            Value value;
            struct Node* next{};
    };

    class ExtendedManager {
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

            struct proc {
                unsigned state; // ready or blocked: 1 or 0
                unsigned parent; // index of parent process i
                Node<int>* children{}; // head of child process linked list
                Node<int>* resources{}; // head of resources held by current process
            };
            
            struct rsrc {
                unsigned state;
                Node<proc> waistList;
            };

        private:
            proc PCB[MAX_PROC]; // No reuse process control block
            Node<rsrc> RCB[MAX_RESRC];
            Node<proc> RL;
            // RL* RL; // a head to Ready List linked list
    };
}

#endif // _exm_h_