# E20_Cache

This project, like E20 Processor, was developed using the object-oriented paradigm. Because cache is an item that the e20 CPU utilizes, it makes the most sense. The cache is located in the cache.h header file. Because it's in a header file, it'll be more structured and simpler to understand. Furthermore, putting it in class makes configuring the cache and implementing multiple caches more simple. As
previously stated in project 2, the benefits of abstraction and modification are critical for updating new features and not gaining exposure to design.


When I wrote the cache class, I intended that the cache will be replicated in the same way it would be in hardware where each block has a valid bit, a line, and a tag value. The main three functions are direct-cache, n_associated, and fully associated are configurations of different types of caches. 
With direct-cache, each line contains one block; fully-associated, there is only one line and the associated is the number of blocks; and n- associated, each line contains the number of blocks of the association. The implementation of the least-recently-used (LRU) replacement strategy made the 
n- associated function the most difficult task of the project. A two-dimensional vector was the data structure I used. One dimension is for the line, while the other is for the number of blocks per line. The algorithm I used to implement the LRU is for each line the least used block would be always 
pushed to the end so therefore, when the blocks are all full and I need to evict I would just pop the first block. However, utilizing a vector for this approach has a disadvantage in that when there is an eviction and the first block is popped, all remaining items must be moved/copied, resulting in a time complexity of O(n). Because we're just utilizing a maximum associativity of 16 and only evict if all of the blocks are occupied, there won't be much of a performance impact. For larger caches, a better data structure that not only have O(1) insertion but also O(1) deletion would be required.


After reading the instructions several times, I evaluated all of the edge cases and devised my own test in addition to the ones provided, all of which produced positive results. I did my best to minimize redundant code and change my sim.cpp file from project 2 to make it more structured and readable. Almost all functions are explained in detail, and all functions include detailed comments. If there is anything that is unclear, please contact me using the email address listed at the top of this readme file.
