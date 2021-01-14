//#include<bits/stdc++.h>
//using namespace std;
#include <math.h>
#include "map.h"
//#include "vector.h"
#include "vector_of_vectors.h"
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

	/*for(int i = 0; i <= n; i++)
		arr[i].clear();
	*/
	initialize_vov(&arr, n);
	// Initially whole block of specified
	// size is available
	//arr[n].push_back(make_pair(0, s - 1));
	struct vector* iterator = arr.tail_vov;
	push_back(iterator, 0, s-1);
	
	
}

int allocate(int s)
{

	// Calculate index in free list
	// to search for block if available
	int x = ceil(log(s) / log(2));
	
	//printf("X = %d\n", x);

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
		/*
		iterator->head = NULL;
		iterator->next = NULL;
		iterator->previous = NULL;
		iterator->size = 0;
		iterator ->tail = NULL;*/
		iterator = NULL;
		printf("Sorry, failed to allocate memory\n");
		return - 1;
	}
	//printf("Counter = %d\n",counter);
	
	// Block available
	//if (arr[x].size() > 0)
	if(iterator->head != NULL)
	{
	    /*
	    if(s == 30)
            cout << "Hey\n";
		*/
		//pair<int, int> temp = arr[x][0];
		//pair<int, int> temp = arr[x].back();
        //arr[x].pop_back();
        int temp_first = back_first(iterator);
		int temp_second = back_second(iterator);
		pop_back(iterator);
		/*
        if(s == 30)
            cout << temp.first << "\t" << temp.second << endl;
        */
		// Remove block from free list
		//arr[x].erase(arr[x].begin());

		//cout << "Memory from " << temp.first<< " to " << temp.second<< " allocated" << "\n";
		//printf("Memory from %d to %d allocated\n", temp.first, temp.second);
		printf("Memory from %d to %d allocated\n", temp_first, temp_second);

		// Map starting address with
		// size to make deallocating easy
		//mp[temp.first] = temp.second -temp.first + 1;
		insert_to_map(&mp, temp_first, temp_second -temp_first + 1); //my defined map
	}
	else
	{
		int i;

		
		// If not, search for a larger block
		iterator = iterator->next;
		for(i = x + 1; i < size; i++)
		{
			//printf("Inside:\n");
			//print_vector(iterator);
			// Find block size greater
			// than request
			//if (arr[i].size() != 0)
			
			if(iterator->head != NULL)
				break;
			iterator = iterator->next;	
			
		}
		//printf("debug i %d\n", i);
		// If no such block is found
		// i.e., no memory block available
		if (i >= size)
		{
			//cout << "Sorry, failed to allocate memory\n";
			printf("Sorry, failed to allocate memory\n");
			return -1;
		}

		// If found
		else
		{
			//pair<int, int> temp;
			//temp = arr[i].back();
            //arr[i].pop_back();

			int temp_first = back_first(iterator);
			int temp_second = back_second(iterator);
			pop_back(iterator);
			// Remove first block to split
			// it into halves
			//arr[i].erase(arr[i].begin());
			i--;

			for(;i >= x; i--)
			{

				// Divide block into two halves
				//pair<int, int> pair1, pair2;
				//pair1 = make_pair(temp.first,temp.first +(temp.second -temp.first) / 2);
				int pair1_first = 	temp_first;
				int pair1_second = temp_first +(temp_second -temp_first) / 2;			
				//pair2 = make_pair(temp.first +(temp.second -temp.first + 1) / 2,temp.second);
				int pair2_first = 	temp_first +(temp_second -temp_first + 1) / 2;
				int pair2_second = temp_second;

				iterator = iterator->previous;

				//arr[i].push_back(pair1);
				push_back(iterator, pair1_first, pair1_second);
				// Push them in free list
				//arr[i].push_back(pair2);
				push_back(iterator, pair2_first, pair2_second);
				
				//temp = arr[i][0];
				temp_first = begin_first(iterator);
				temp_second = begin_second(iterator);
				// Remove first free block to
				// further split
				//arr[i].erase(arr[i].begin());
				erase_begin(iterator);
			}

			//cout << "Memory from " << temp.first << " to " << temp.second<< " allocate" << "\n";
			//printf("Memory from %d to %d allocated\n", temp.first, temp.second);
			printf("Memory from %d to %d allocated\n", temp_first, temp_second);

			//mp[temp.first] = temp.second -temp.first + 1;
			//insert_to_map(&mp, temp.first, temp.second -temp.first + 1); //my defined map
			insert_to_map(&mp, temp_first, temp_second -temp_first + 1); //my defined map
		}
	}
	return 0;
}

void deallocate(int id)
{

	// If no such starting address available
	//if(mp.find(id) == mp.end())
	int check_exist = get_value_map(&mp, id);
	if(check_exist == -1)
	{
		//cout << "Sorry, invalid free request\n";
		printf("Sorry, invalid free request\n");
		return;
	}

	// Size of block to be searched
	//int n = ceil(log(mp[id]) / log(2));
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
	//arr[n].push_back(make_pair(id,id + pow(2, n) - 1));

	push_back(iterator, id, id + pow(2, n) - 1);						
	//cout << "Memory block from " << id << " to "<< id + pow(2, n) - 1<< " freed\n";
	printf("Memory block from %d to %.0f freed\n", id, id + pow(2, n) - 1);

	// Calculate buddy number
	temp_log = get_value_map(&mp, id);

	//buddyNumber = id / mp[id];
	buddyNumber = id / temp_log;

	//int temp_size = mp[id];
    int temp_size = temp_log;

	bool flag = true;
	while(n <= size && flag) // may be modified to less than only
    {
        //printf("loop\n");
        flag = false;


        if (buddyNumber % 2 != 0)
            buddyAddress = id - pow(2, n);
        else
            buddyAddress = id + pow(2, n);

        //cout << buddyAddress << "\t" << buddyNumber<< endl;
		printf("%d\t%d\n", buddyAddress, buddyNumber);

        // Search in free list to find it's buddy
		int  my_size = iterator->size;
        //for(i = 0; i < arr[n].size(); i++)
		struct vector_element * v_element = iterator->head;
		for(i = 0; i < my_size; i++)
        {
			//printf("Hellozzz\n");
            // If buddy found and is also free
            //if (arr[n][i].first == buddyAddress)
			if (v_element->first == buddyAddress)
            {
                flag = true;
                //cout << buddyAddress << "\t" << buddyNumber<< "\t"<<i << endl;
                // Now merge the buddies to make
                // them one large free memory block
                if (buddyNumber % 2 == 0)
                {
                    //cout << "Here\n";
                    //arr[n + 1].push_back(make_pair(id,id + 2 * (pow(2, n)) -1));
					iterator = iterator->next;
					push_back(iterator, id, id + 2 * (pow(2, n)) -1);

                    //cout << "Coalescing of blocks starting at "<< id << " and " << buddyAddress<< " was done" << "\n";
					printf("Coalescing of blocks starting at %d and %d was done\n", id, buddyAddress);
                }
                else
                {
                    //arr[n + 1].push_back(make_pair(buddyAddress, buddyAddress +2 * (pow(2, n)) - 1));
					iterator = iterator->next;
					push_back(iterator, buddyAddress, buddyAddress +2 * (pow(2, n)) - 1);

                    //cout << "Coalescing of blocks starting at "<< buddyAddress << " and "<< id << " was done" << "\n";
					printf("Coalescing of blocks starting at %d and %d was done\n", buddyAddress, id);
                }
				iterator = iterator->previous;
				//printf("Debuggg###\n");
                //print_vector(iterator);
				//printf("Debuggg###\n");
				//arr[n].erase(arr[n].begin() + i);
                erase_at_index(iterator, i);
				//arr[n].erase(arr[n].begin() +arr[n].size() - 1);
                erase_at_index(iterator,  iterator->size-1);
				//printf("mm\t%d\n", iterator->size);
				//printf("Debo\n");
				//print_vector(iterator);
				//printf("Debo\n");
				//print_vector(iterator);
				break;
            }
			v_element = v_element->next;
        }
        //cout<<"Boom\n";


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
	/*
    for(int i = 0; i < size ; i ++)
    {
        for(int j = 0; j < (int)arr[i].size(); j ++)
        {
            //cout << arr[i][j].first << "\t" <<arr[i][j].second << "\t" << i << "\t" << j <<endl;
			printf("%d\t%d\t%d\t%d\n", arr[i][j].first, arr[i][j].second, i, j);

        }

    }*/
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

// Driver code
int main()
{

	int result;
    
	Buddy(128);
	
	print();
	/*
	allocate(200);
	printf("dehk\n");
	*/
	
	result = allocate(20);
	print();
	if(result == -1)
	{
		printf("Bad\n");
	}
	else
	{
		printf("GOOOD\n");
	}
	
	result = allocate(15);
	print();
	if(result == -1)
	{
		printf("Bad\n");
	}
	else
	{
		printf("GOOOD\n");
	}
	
	result = allocate(10);
	print();
	if(result == -1)
	{
		printf("Bad\n");
	}
	else
	{
		printf("GOOOD\n");
	}
	
	result = allocate(25);
	print();
	if(result == -1)
	{
		printf("Bad\n");
	}
	else
	{
		printf("GOOOD\n");
	}
	
	result = allocate(80);
	if(result == -1)
	{
		printf("Bad\n");
	}
	else
	{
		printf("GOOOD\n");
	}
	
	deallocate(0);
	print();
	deallocate(48);
	print();
	result = allocate(8);
	print();
	if(result == -1)
	{
		printf("Bad\n");
	}
	else
	{
		printf("GOOOD\n");
	}
	
	result = allocate(30);
	print();
	if(result == -1)
	{
		printf("Bad\n");
	}
	else
	{
		printf("GOOOD\n");
	}
	
	deallocate(32);
	print();
	result = allocate(15);
	print();
	if(result == -1)
	{
		printf("Bad\n");
	}
	else
	{
		printf("GOOOD\n");
	}
	
	deallocate(48);
    print();
    deallocate(0);
    print();
    deallocate(32);
    print();
	deallocate(64);
	print();
    //allocate(200);
	//print();
	
    /*
    //case 2
    Buddy(128);
	print();
	allocate(20);
	print();
	allocate(15);
	print();
	deallocate(0);
	print();
	deallocate(32);
	print();
	allocate(125);
    //
	*/
    /*
    //case 3
    Buddy(128);
	print();
	allocate(20);
	print();
	allocate(15);
	print();
	allocate(10);
	print();
	deallocate(32);
	print();
	deallocate(0);
	print();
	deallocate(48);
	print();
	//allocate(125);
    //
    */

	return 0;
}

// This code is contributed by sarthak_eddy
