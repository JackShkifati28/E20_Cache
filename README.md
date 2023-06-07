# **E20_Cache**

## **Overview**
___
This project, akin to the E20 Processor, was sculpted employing the _object-oriented paradigm_. In the realm of the e20 CPU, the cache serves an indispensable role, hence our decision to instantiate a `cache` object. 

## **File Organization**
___
The `cache` class has been meticulously positioned in a unique header file named `cache.h`. The advantages of this layout are numerous:

- Promotes structural cohesion.
- Simplifies comprehension.
- Facilitates multiple cache implementations and configurations.

As previously explained in Project 2, the importance of abstraction and modifiability cannot be overstated when it comes to updating new features and preserving the integrity of the design.

## **Function Design**
___
During the formulation of the `cache` class, it was envisioned to echo the hardware scenario where each block owns a _valid bit_, a _line_, and a _tag value_. The three primary functions - `direct-cache`, `n_associated`, and `fully_associated` - constitute different configurations of caches. 

- `direct-cache`: Each line houses a single block.
- `fully-associated`: A single line exists, the association denotes the number of blocks.
- `n-associated`: Each line contains a number of blocks equal to the association.

The most challenging task in this project was the incorporation of the _Least Recently Used (LRU)_ replacement strategy in the `n-associated` function. A two-dimensional vector was the chosen data structure, where one dimension corresponds to the line, and the other to the number of blocks per line. 

## **LRU Implementation and Performance Considerations**
___
The LRU algorithm pushes the least used block to the end for each line. Hence, during an eviction scenario, the first block is simply popped out. However, there's a performance caveat with this approach when using vectors - all remaining items need to be shifted or copied, leading to a time complexity of O(n). This won't impact the performance significantly for our project due to the maximum associativity being limited to 16 and evictions only happening when all blocks are filled. For larger caches, a data structure offering O(1) insertion and deletion would be required.

## **Edge Cases, Testing, and Code Quality**
___
After several rounds of scrutinizing the instructions, various edge cases were evaluated and additional tests, apart from the provided ones, were run, all yielding successful results. Efforts were made to minimize redundant code and restructure the `sim.cpp` file from Project 2 for enhanced readability. Almost all functions are meticulously explained, and each function is equipped with detailed comments.

Should any ambiguities arise, feel free to reach out using the email address provided at the top of this readme file.
