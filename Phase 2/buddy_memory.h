//#include<bits/stdc++.h>
//using namespace std;
#include <math.h>
#include "map.h"
//#include "vector.h"
#include "vector_of_vectors.h"
#include "headers.h"
// Size of vector of pairs
int size;

// Global vector of pairs to track all
// the free nodes of various sizes
//vector<pair<int, int>> arr[100000];
struct vector_of_vectors arr = {NULL, NULL, 0 };

// Map used as hash map to store the
// starting address as key and size
// of allocated segment key as value
//map<int, int> mp;
struct map mp = {NULL};


typedef short bool;
#define true 1
#define false 0

void Buddy(int s)
{

	// Maximum number of powers of 2 possible
	int n = ceil(log(s) / log(2));

	size = n + 1;

	
	initialize_vov(&arr, n);
	
	struct vector* iterator = arr.tail_vov;
	push_back(iterator, 0, s-1);
	printf("Memory with size %d bytes has been initialized!\n", s);
	
}

int allocate(int s)
{

	// Calculate index in free list
	// to search for block if available
	int x = ceil(log(s) / log(2));
	

	struct vector* iterator = arr.head_vov;
	int counter = 0;
	while (iterator->next != NULL)
	{
		if (counter == x)
		{
			break;
		}
		counter ++ ;
		iterator = iterator->next;
	}
	if(counter < x)
	{
		
		iterator = NULL;
		printf("Sorry, failed to allocate memory\n");
		return - 1;
	}
	
	// Block available
	if(iterator->head != NULL)
	{
	    
        int temp_first = back_first(iterator);
		int temp_second = back_second(iterator);
		pop_back(iterator);
		
		printf("Memory from %d to %d allocated\n", temp_first, temp_second);

		
		insert_to_map(&mp, temp_first, temp_second -temp_first + 1); //my defined map
	}
	else
	{
		int i;

		
		// If not, search for a larger block
		iterator = iterator->next;
		for(i = x + 1; i < size; i++)
		{
			
			
			if(iterator->head != NULL)
				break;
			iterator = iterator->next;	
			
		}
		//printf("debug i %d\n", i);
		// If no such block is found
		// i.e., no memory block available
		if (i >= size)
		{
			printf("Sorry, failed to allocate memory\n");
			return -1;
		}

		
		else
		{

			int temp_first = back_first(iterator);
			int temp_second = back_second(iterator);
			pop_back(iterator);
			
			
			i--;

			for(;i >= x; i--)
			{

				// Divide block into two halves
				
				int pair1_first = 	temp_first;
				int pair1_second = temp_first +(temp_second -temp_first) / 2;			
				int pair2_first = 	temp_first +(temp_second -temp_first + 1) / 2;
				int pair2_second = temp_second;

				iterator = iterator->previous;

				
				push_back(iterator, pair1_first, pair1_second);
				
				push_back(iterator, pair2_first, pair2_second);
				
				
				temp_first = begin_first(iterator);
				temp_second = begin_second(iterator);
				
				erase_begin(iterator);
			}

			
			printf("Memory from %d to %d allocated\n", temp_first, temp_second);

			
			insert_to_map(&mp, temp_first, temp_second -temp_first + 1); //my defined map
		}
	}
	return 0;
}

void deallocate(int id)
{

	// If no such starting address available
	int check_exist = get_value_map(&mp, id);
	if(check_exist == -1)
	{
		//cout << "Sorry, invalid free request\n";
		printf("Sorry, invalid free request\n");
		return;
	}

	// Size of block to be searched
	int temp_log = get_value_map(&mp, id);
	int n = ceil(log(temp_log) / log(2));

	int i, buddyNumber, buddyAddress;
	int shadow_id = id;

	struct vector* iterator = arr.head_vov;
	int counter = 0;
	while (iterator->next != NULL)
	{
		if (counter == n)
		{
			break;
		}
		counter ++ ;
		iterator = iterator->next;
	}
	// Add the block in free list

	push_back(iterator, id, id + pow(2, n) - 1);						
	printf("Memory block from %d to %.0f freed\n", id, id + pow(2, n) - 1);

	// Calculate buddy number
	temp_log = get_value_map(&mp, id);

	buddyNumber = id / temp_log;

    int temp_size = temp_log;

	bool flag = true;
	while(n <= size && flag) // may be modified to less than only
    {
        flag = false;


        if (buddyNumber % 2 != 0)
            buddyAddress = id - pow(2, n);
        else
            buddyAddress = id + pow(2, n);

		printf("%d\t%d\n", buddyAddress, buddyNumber);

        // Search in free list to find it's buddy
		int  my_size = iterator->size;
		struct vector_element * v_element = iterator->head;
		for(i = 0; i < my_size; i++)
        {
			
			if (v_element->first == buddyAddress)
            {
                flag = true;
                
                if (buddyNumber % 2 == 0)
                {
                    
					iterator = iterator->next;
					push_back(iterator, id, id + 2 * (pow(2, n)) -1);

					printf("Coalescing of blocks starting at %d and %d was done\n", id, buddyAddress);
                }
                else
                {
					iterator = iterator->next;
					push_back(iterator, buddyAddress, buddyAddress +2 * (pow(2, n)) - 1);

					printf("Coalescing of blocks starting at %d and %d was done\n", buddyAddress, id);
                }
				iterator = iterator->previous;
				
                erase_at_index(iterator, i);
                erase_at_index(iterator,  iterator->size-1);
				
				break;
            }
			v_element = v_element->next;
        }
       
        if (buddyNumber % 2 == 0)
        {
            id = id;
        }
        else
        {
           id = buddyAddress;
        }
        temp_size = temp_size * 2;

        buddyNumber = id / (temp_size);

        n += 1;
		//printf("Deboo\n");
		//print_vector(iterator);
		iterator = iterator->next;

    }
    // Remove the key existence from map
    //mp.erase(shadow_id);
	remove_from_map(&mp, shadow_id);
}

void print()
{
	
	struct vector* iterator = arr.head_vov;
	int counter = 0;
	while (iterator != NULL)
	{
		printf("i: %d\n", counter);
		print_vector(iterator);
		iterator = iterator->next;
		counter ++;
	}
	

}

