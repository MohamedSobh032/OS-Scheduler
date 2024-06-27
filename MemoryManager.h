/**
 * @file MemoryManager.h
 * @brief Header file for memory manager functions.
 */

#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Define the total memory size
#define TOTAL_MEMORY_SIZE 1024
#define MINIMUM_BLOCK_SIZE 8

unsigned char arr[TOTAL_MEMORY_SIZE];  // NOLINT

// Structure to represent a node in the binary tree
typedef struct BuddyNode {
  size_t size;
  bool free;
  struct BuddyNode* left;
  struct BuddyNode* right;
  size_t offset;
} BuddyNode;

/**
 * @struct BuddyAllocator
 * @brief Structure to represent the buddy allocator.
 */
typedef struct {
  BuddyNode* root;
} BuddyAllocator;

/**
 * @brief Global instance of the buddy allocator.
 */
BuddyAllocator globalAllocator = {.root = NULL};  // NOLINT

/**
 * @brief Creates a new BuddyNode.
 *
 * @param size The size of the memory block represented by this node.
 * @param parent Pointer to the parent node.
 * @param offset Offset of the memory block represented by this node.
 * @return Pointer to the newly created BuddyNode.
 */
BuddyNode* createBuddyNode(size_t size, size_t offset) {
  BuddyNode* node = (BuddyNode*)malloc(sizeof(BuddyNode));
  node->size = size;
  node->free = true;
  node->left = NULL;
  node->right = NULL;
  node->offset = offset;
  return node;
}

/**
 * @brief Initializes the buddy allocator.
 */
void initializeBuddyAllocator(void) {
  globalAllocator.root = createBuddyNode(TOTAL_MEMORY_SIZE, 0);
}

/**
 * @brief Splits a node into two buddies.
 *
 * @param node Pointer to the node to split.
 */
void splitNode(BuddyNode* node) {
  size_t newSize = node->size / 2;
  node->left = createBuddyNode(newSize, node->offset);
  node->right = createBuddyNode(newSize, node->offset + newSize);
  node->free = true;
}

/**
 * @brief Finds and allocates memory.
 *
 * @param node Pointer to the node to start allocation from.
 * @param size The size of memory to allocate.
 * @return Pointer to the allocated memory block, or NULL if allocation fails.
 */
void* allocateMemory(BuddyNode* node, size_t size) {
  if (node->size == size && node->free && !(node->right) &&  // NOLINT
      !(node->left)) {                                       // NOLINT
    node->free = false;
    return (void*)&arr[node->offset];
  } else if (node->size < size || !(node->free)) {
    return NULL;
  } else {
    if (node->left == NULL && node->right == NULL) {
      splitNode(node);
    }
    void* left_result = allocateMemory(node->left, size);
    if (left_result != NULL) {
      return left_result;
    } else {
      return allocateMemory(node->right, size);
    }
  }
}

/**
 * @brief Merges free blocks in the binary tree.
 *
 * @param node Pointer to the root node of the subtree to merge.
 */
void mergeFreeBlocks(BuddyNode* node) {
  if (node == NULL || node->left == NULL || node->right == NULL) {
    return;
  }
  // Recursively merge free blocks in the left and right subtrees
  mergeFreeBlocks(node->left);
  mergeFreeBlocks(node->right);
  // Merge adjacent free blocks
  if (node->left->free && node->right->free) {
    if (node->left->left == NULL && node->right->left == NULL) {
      node->free = true;
      free(node->left);
      free(node->right);
      node->left = NULL;
      node->right = NULL;
    }
  }
}

/**
 * @brief Finds the BuddyNode corresponding to a memory block.
 *
 * @param node Pointer to the root node of the subtree to search.
 * @param block Pointer to the memory block to find.
 * @return Pointer to the BuddyNode corresponding to the memory block, or NULL
 * if not found.
 */
BuddyNode* findBuddyNode(BuddyNode* node, void* block) {
  if (node == NULL) {
    return NULL;
  }
  if ((void*)&arr[node->offset] == block && !(node->left) && !(node->right)) {
    return node;
  }
  BuddyNode* left_result = findBuddyNode(node->left, block);
  if (left_result != NULL) {
    return left_result;
  }
  return findBuddyNode(node->right, block);
}

/**
 * @brief Prints the memory layout.
 *
 * @param node Pointer to the root node of the binary tree to print.
 */
void printMemoryLayout(BuddyNode* node) {
  if (node == NULL) {
    return;
  }
  if (node->left == NULL && node->right == NULL) {
    if (!node->free) {
      printf("[%zu:Allocated] ", node->size);
    } else {
      printf("[%zu:Free] ", node->size);
    }
  } else {
    printMemoryLayout(node->left);
    printMemoryLayout(node->right);
  }
}

/**
 * @brief Wrapper Functions
 *
 * These functions provide an interface for memory allocation and deallocation,
 * as well as utilities for retrieving memory block addresses and printing the
 * memory structure.
 */

/**
 * @brief Allocates memory of the specified size.
 *
 * @param size The size of memory to allocate.
 * @return A pointer to the allocated memory block, or NULL if allocation fails.
 */
void* allocate(size_t size) {
  size_t rounded_size = (size_t)pow(2, ceil(log2(size)));  // NOLINT
  return allocateMemory(globalAllocator.root, rounded_size);
}

/**
 * @brief Deallocates memory previously allocated by the allocate function.
 *
 * @param block Pointer to the memory block to deallocate.
 */
void deallocate(void* block) {
  BuddyNode* node = findBuddyNode(globalAllocator.root, block);
  if (node != NULL) {
    node->free = true;
    mergeFreeBlocks(globalAllocator.root);
  }
}

/**
 * @brief Gets the starting address of the memory block.
 *
 * @param block Pointer to the memory block.
 * @return The starting address of the memory block.
 */
size_t getStartAddress(void* block) {
  BuddyNode* node = findBuddyNode(globalAllocator.root, block);
  return node->offset;
}

/**
 * @brief Gets the ending address of the memory block.
 *
 * @param block Pointer to the memory block.
 * @return The ending address of the memory block.
 */
size_t getEndAddress(void* block) {
  BuddyNode* node = findBuddyNode(globalAllocator.root, block);
  return (node->offset + node->size - 1);
}

/**
 * @brief Prints the memory structure.
 */
void printMemoryStructure(void) {
  printMemoryLayout(globalAllocator.root);
  printf("\n");
}

#endif