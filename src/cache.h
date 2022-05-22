#pragma once
#include <vector>
#include <list>
#include <unordered_map>
#include <cmath>

using namespace std;

/**
 * @brief An cache object is represented by the cache class. It contains three differnt types of configrations
 * direct-cache, n-assicated, and fully-associate. For associative caches, it uses the least-recently-used (LRU) 
 * replacement policy.
 */
class cache
{

public:
    string status;              // the status if miss or hit
    vector<int> valBit;         // the valid bit for each block  
    vector<vector<int>> blocks; // blocks cotaining the tags
    int blockSize, line, blockID, tagVal, numLine, associate;

    /**
     * @brief The cache contractor initializes all blocks to zero, the block-size, the number of lines in the cache, the
     * number of associates (direct, n-associated, or fully-associate), and the number of blocks per line.
     * @param lines Number of lines the cache contain.
     * @param blockS The blocksize of the cache
     * @param assoc The associate value
     */
    cache(int lines, int blockS, int assoc)
    {

        valBit.assign(lines, 0);  

        blocks.assign(lines, vector<int>(assoc, 0)); 

        status = "";
        blockSize = blockS;
        numLine = lines;
        associate = assoc;
    }

    /**
     * @brief The config_cache methods sets the cache blockID, which line, and the tag value.
     * @param address A poniter that points to a value in the cache.
     */
    void config_Cache(int address)
    {
        blockID = floor(address / blockSize);
        line = floor(blockID % numLine);
        tagVal = floor(blockID / numLine);
    }

    /**
     * @brief The blocks full function detects whether or not all of the blocks for a given line are occupied.
     * If it is, it pops the first entry of the first block, which is arranged from least recently used.
     * @param blocks the blocks of the cache
     */
    void blocks_full(vector<vector<int>> &blocks)
    {
        if (blocks[line].size() == associate)
        {
            blocks[line].erase(blocks[line].begin());
        }
    }

    /**
     * @brief The cache type function checks if the cache is a direct, n assoicated , or full associated.
     */
    void cacheType()
    {
        if (associate == 0)
            fully_associated();

        else if (associate == 1)
            direct();

        else
            n_associated();
    }

    /**
     * @brief The direct fuction is short for "Direct-Cache" where the associate value is one and each line
     * contains one block.
     */
    void direct()
    {

        if (valBit.at(line) == 0) // if the vald bit has not been set
        {
            status = "MISS";
            valBit.at(line) = 1;
            blocks[line][0] = tagVal; // store the tag value to the block.
        }

        else if (blocks[line][0] == tagVal) // If the block contains the tag
        {
            status = "HIT";
        }

        else // If the valid bit is not zero and the tags don't match miss with eviction.
        {
            status = "MISS";
            blocks[line][0] = tagVal;
        }
    }

    /**
     * @brief The n_associated cache is a configration that each line of the cache
     * contains the number blocks by the given assication.
     */
    void n_associated()
    {
        bool isHit = false;

        // Checks to see if any of the blocks contain the tag vlaue.
        for (int i = 0; i < associate && isHit == false; i++)
        {
            if (blocks[line][i] == tagVal && valBit[line] != 0) // If block contains a tag and valid bit is not zero.
            {
                isHit = true; 

                status = "HIT";

                blocks_full(blocks); // Removes the least resently used from if all the blocks of the line are full.

                blocks[line].push_back(tagVal); // insert the tag value to the last block.
            }
            else
                status = "MISS";
        }

        if (status == "MISS")
        {
            valBit[line] = 1;
            blocks_full(blocks);            // Removes the least resently used from if all the blocks of the line are full.
            blocks[line].push_back(tagVal); // insert the tag value to the last block.
        }
    }
   
   /**
    * @brief The fully associated cache contains only one line.
    */
    void fully_associated()
    {

        if (valBit.at(0) == 0)  // If the valid bit is not set.
        {
            status = "MISS";
            valBit.at(line) = 1;
            blocks[line][0] = tagVal; 
        }
        else if (blocks[line][0] == tagVal)  // If the block contains the tag.
        {
            status = "HIT";
        }
        else   // If the valid bit is not zero and the tags don't match miss with eviction.s
        {  
            status = "MISS";
            blocks[line][0] = tagVal;
        }
    }
};