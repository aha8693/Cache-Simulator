#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <sstream>
#include <cmath>
#include <cstdint>

using namespace std;

/*
* Data structure for a slot/block
*/
struct Slot {
    uint32_t tag;
    bool valid, dirty;
    int load_ts, access_ts;

    
    // Default constructor for filling out a set used later. 
    Slot() {
        this->tag = 0;
        this->valid = false;
        this->dirty = false;
        this->load_ts = 0;
        this->access_ts = 0;
    }
    
    // Default constructor to create a slot. 
    Slot(uint32_t tag) {
        this->tag = tag;
        this->valid = true;
        this->dirty = false;
        this->load_ts = 0;
        this->access_ts = 0;
    }

};

/*
* A class for a set.  
* Set stores `bytesNumb` number of blocks. 
* For each set, it conducts various manipulations that are block related exclusively. 
*/

class Set {
    vector<Slot> set;
    int size;
    int setsNumb, blocksNumb, bytesNumb, cyclesAdded;
    bool writeThrough, evictions;

    public:
        // Default constructor. It adds `blocksNumb` number of empty (invalid) blocks. 
        Set(int setsNumb, int blocksNumb, int bytesNumb, bool writeThrough, bool evictions): size(0), cyclesAdded(0) {
            this->setsNumb = setsNumb;
            this->blocksNumb = blocksNumb;
            this->bytesNumb = bytesNumb;
            this->writeThrough = writeThrough;
            this->evictions = evictions;

            for (int i = 0; i < blocksNumb; i++) { 
                set.push_back(Slot());
            }
        }

        // Getter method for a slot in this set
        Slot get_slot(int index) {
            return set[index];
        }

        // Setter method for a slot in this set
        void set_slot(int index, Slot new_slot) {
            set[index] = new_slot;
        }

        // Adding slot when store/load miss
        // It evicts a block when needed. Cycles are also calculated when the evicting block is dirty. 
        // Adding a slot impacts the other blocks' timestamps. 
        void add_slot(Slot slot_new) { 
            int index_to_update = size;
            if (size == blocksNumb) { // target set is full, need to evict 
                index_to_update = discard(); // discard the highest order slot and update the rest of orders
            } 

            update(); // update the ordering
            
            this->set[index_to_update] = slot_new;
            size++;
        }

        // Finding a slot with given tag. 
        // It returns a corresponding index with the given tag, otherwise return -1. 
        int find_slot(uint32_t tag) {
            for (int i = 0; i < blocksNumb; i++) {
                if (set[i].valid && set[i].tag == tag) {
                    return i;
                }
            }
            return -1;
        }

        // When the block is full, then set needs to evict a block. 
        // Depending on parameters (lru and fifo), this method discard the block with an appropriate measurement of the highest order (based on access_ts or load_ts)
        int discard() { 
            int discard_index = -1;
            int order_max = -1;
            if (evictions) { // lru
                for (int i = 0; i < blocksNumb; i++) {
                    if ((set[i].access_ts > order_max) && set[i].valid) { // find a slot with the highest order
                        discard_index = i;
                        order_max = set[i].access_ts;
                    } 
                }
            } else { // fifo
                for (int i = 0; i < blocksNumb; i++) {
                    if ((set[i].load_ts > order_max) && set[i].valid) { // find a slot with the highest order
                        discard_index = i;
                        order_max = set[i].load_ts;
                    }
                }
            }

            if (set[discard_index].dirty && !writeThrough) { // If this slot is dirty, we need to write back to memory before replacing
                this->cyclesAdded += 100 * (bytesNumb/4);
            }
            size--;

            return discard_index;
        }

        // This method updates the rest of the existing blocks
        void update() {
            if (evictions) { // lru
                for (int i = 0; i < blocksNumb; i++) {
                    if (set[i].valid){
                        set[i].access_ts++;
                    }   
                }
            } else { // fifo
                for (int i = 0; i < blocksNumb; i++) {
                    if (set[i].valid){
                        set[i].load_ts++;
                    } 
                }
            }
        }

        // This method is used when the command-line argument is lru
        // LRU also updates its order when it is 'accessed' (i.e. when load/store hit). 
        // It does not update the order when the accessed block is the recent one. 
        void lru_hit_update(uint32_t found_index) { // If Hit, only lru updates access_ts
            if (set[found_index].access_ts == 0) {
                return;
            }
            for (int i = 0; i < blocksNumb; i++) {
                if (set[i].valid) {
                        set[i].access_ts++;
                }
            }
            set[found_index].access_ts = 0;
        }

        // This is the helper method for returning the number of cycles that take place during evicting dirty blocks. 
        // This returns the value to the Cache object, and the total cycles are calculated in the Cache object. 
        int cycles_added_output() {
            int final_cyclesAdded = cyclesAdded;
            this->cyclesAdded = 0;
            return final_cyclesAdded;
        }
};


/*
* A class for a Cache.  
* Set stores `setNumbs` number of sets. 
* This class computes the core manipulations of Cache store/load. 
*/
class Cache {
    vector<Set> cache;
    int setsNumb, blocksNumb, bytesNumb;

    bool writeAllocate = false;
    bool writeThrough = false;
    bool evictions = false;

    private:
        int store_hit, store_miss, load_hit, load_miss, total_cycles;

    public:
        // Default constructor
        // It pushes 'setsNumb' number of sets in this cache
        Cache(int setsNumb, int blocksNumb, int bytesNumb, bool writeAllocate, bool writeThrough, bool evictions): store_hit(0), store_miss(0), load_hit(0), load_miss(0), total_cycles(0) {
            this->setsNumb = setsNumb;
            this->blocksNumb = blocksNumb;
            this->bytesNumb = bytesNumb;
            this->writeAllocate = writeAllocate;
            this->writeThrough = writeThrough;
            this->evictions = evictions;

            for (int i = 0; i < setsNumb; i++) { // Cache having setsNumb of Set objects
                cache.push_back(Set(setsNumb, blocksNumb, bytesNumb, writeThrough, evictions));
            }   
        }

        // It adds a slot by calling set's add_slot method. 
        // This is called during loading and storing 
        // Cycles taken place during add_slot is also counted. 
        void add_slot(uint32_t setIndex, Slot slot_new) {
            cache[setIndex].add_slot(slot_new);
            int dirty_cycle = cache[setIndex].cycles_added_output();
            total_cycles += dirty_cycle;
            
            total_cycles++;
        }

        // It finds a slot with a tag and a set index by calling set's find_slot method. 
        int find_slot(uint32_t tag, int setIndex) {
            return cache[setIndex].find_slot(tag);
        }

        // Core function of Cache
        // Whenever a load is called, this method is used. 
        // If load hits, it counts up the load hits and update ts if needed. 
        // IF load misses, it adds a slot into a corresponding set. 
        void load(uint32_t tag, int setIndex) {
            int found_index = cache[setIndex].find_slot(tag);
            if (found_index != -1) { // hit
                if (evictions) { // if lru, update orders
                    cache[setIndex].lru_hit_update(found_index);
                }
                load_hit++;
            } else { // miss
                load_miss++;
                total_cycles += 100 * (bytesNumb / 4);
                Slot slot_new(tag);
                add_slot(setIndex, slot_new);
            }
            total_cycles++;
        }

        // Core function of Cache
        // Whenever a store is called, this method is used. 
        // If store hits, it counts up store hits
        // If needed, it updates the time stamp, and/or it sets the accessed slot as dirty. 
        // It also counts up the total_cycle depending on the command-line parameter  (write-back)
        // If store misses, it adds a slot and calculates the total_cycle depending on the command-line parameters. 
        void store(uint32_t tag, int setIndex) {
            int found_index = cache[setIndex].find_slot(tag);
            if (found_index != -1) { // hit
                if (evictions) { // if lru, update orders
                    cache[setIndex].lru_hit_update(found_index);
                }
                if (!writeThrough) {
                    Slot target_slot = cache[setIndex].get_slot(found_index);
                    target_slot.dirty = true; // this slot needs to be written back to memory when being evicted
                    cache[setIndex].set_slot(found_index, target_slot);
                } else {
                    total_cycles += 100;
                }
                store_hit++;
                total_cycles++;
            } else { // miss
                store_miss++;
                total_cycles++;
                Slot slot_new(tag);

                if (writeAllocate && writeThrough) { // there is no difference between slot and memory (writeThrough), so add a new slot with dirty = false;
                    add_slot(setIndex, slot_new);
                    total_cycles += 100 * (bytesNumb / 4); // load the block from main memory
                    total_cycles += 100; // write 4 byte value through to main memory

                } else if (writeAllocate && !writeThrough) { // there IS difference between slot and memory (write-back)
                    slot_new.dirty = true;
                    
                    total_cycles += 100 * (bytesNumb / 4); // load the block from main memory
                    add_slot(setIndex, slot_new);

                } else { // !writeAllocate && writeThrough , !writeAllocate &&  !writeThrough not possible
                    /* Only writes to the memory */
                    total_cycles += 100;
                } 
            }
        }
        
        // This method outputs the final result of this cache simulator. 
        void print_result() {
            std::cout << "Total loads: " << load_hit+load_miss << std::endl;
            std::cout << "Total stores: " << store_hit+store_miss << std::endl;
            std::cout << "Load hits: " << load_hit << std::endl;
            std::cout << "Load misses: " << load_miss << std::endl;
            std::cout << "Store hits: " << store_hit << std::endl;
            std::cout << "Store misses: " << store_miss << std::endl;
            std::cout << "Total cycles: " << total_cycles << std::endl;
        }
    
};


// This is a helper method to identify whether a number is not a power of 2. 
bool isNotPowerOf2(int n) {
    if (n <= 0) {
        return true;  // Negative numbers and zero cannot be powers of 2.
    }
    return (n & (n - 1)) != 0;
}

// This method checks whether the command line parameters are valid or not
// Valid includes arguments 1, 2, and 3 should be power of 2 (and arg 3 should be more than 4)
// And arguments 4, 5, 6 should be among the selected strings (e.g. write-allocate, etc. )
bool checkArgValidity(int setsInCache, int blocksInSet, int bytesInBlock, string writeAllocate, string writeThrough, string evictions) {
    if (isNotPowerOf2(setsInCache) || isNotPowerOf2(blocksInSet) || isNotPowerOf2(bytesInBlock)) {
        cerr << "Invalid argument: not a positive power-of-2" << endl;
        return false;
    }

    if (bytesInBlock < 4) {
        cerr << "Block size should be at least 4 bytes" << endl;
        return false;
    }

    bool arg4, arg5, arg6 = false;

    arg4 = ((writeAllocate == "write-allocate") || (writeAllocate == "no-write-allocate"));
    arg5 = ((writeThrough == "write-through") || (writeThrough == "write-back"));
    arg6 = ((evictions == "lru") || (evictions == "fifo"));

    if (!arg4 || !arg5 || !arg6) {
        cerr << "Invalid argument 4, 5, or 6" << endl;
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    int setsInCache = atoi(argv[1]);
    int blocksInSet = atoi(argv[2]);
    int bytesInBlock = atoi(argv[3]);
    string allocate = argv[4];
    string through = argv[5];
    string evict = argv[6];

    /* START OF Checking Command-line Arguments Validity */
    if (argc < 7) {
        cerr << "Missing arguments" << endl;
        return 1;
    }

    if (argc > 7) {
        cerr << "Extra arguments" << endl;
        return 2;
    }

    if (!checkArgValidity(setsInCache, blocksInSet, bytesInBlock, allocate, through, evict)) {
        cerr << "Invalid arguments" << endl;
        return 3;
    }

    bool writeAllocate = (allocate == "write-allocate"); // write-allocate(1) or no-write-allocate(0)
    bool writeThrough = (through == "write-through"); // write-through(1) or write-back(0)
    bool evictions = (evict == "lru");  // lru (1) or fifo (0)

    if (!writeAllocate && !writeThrough) {
        cerr << "Invalid arguments combinations: no-write-allocate, write-back";
        return 4;
    }
    /* END OF Checking Command-line Arguments Validity */

    Cache cache(setsInCache, blocksInSet, bytesInBlock, writeAllocate, writeThrough, evictions);


    /* START OF reading the input trace file */
    string line;
    while (getline(cin, line)) {
        char operation;
        string hexAddress;
        string thirdField;

        // Reading the input stream 
        istringstream iss(line);
        iss >> operation >> hexAddress >> thirdField;

        // Slicing the address into index and tag 
        uint32_t address = stoul(hexAddress, nullptr, 16);
        int offsetBits = log(bytesInBlock) / log(2);
        int indexBits = log(setsInCache) / log(2);

        //uint32_t offset = hexAddress & ((1u << offsetBits) - 1); // Extract the last 'offsetBits' bits
        uint32_t index = (address & ((1u << indexBits) - 1) << offsetBits) >> offsetBits; // Extract indexBits up to offsetBits
        uint32_t tag = address >> (offsetBits + indexBits); // Remaining is a tag

        if (iss) {
            // Process the extracted information
            if (operation == 'l') {
                cache.load(tag, index);
            } else if (operation == 's') {
                cache.store(tag, index);
            }
        } else {
            cerr << "Invalid line: " << line << endl;
            return -1;
        }
    }

    cache.print_result();


    return 0;
}