#include <iostream>
#include  <iomanip>
#include <time.h>
#include <string>


using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(32*1024)

struct Line
{
	unsigned int index;
	unsigned int tag;
	unsigned int v;
	//int usage;			//How often or how recently is the cache item used (from fully associative)
};

struct Line cacheLine[1024];

enum cacheResType {MISS=0, HIT=1};
char *msg[2] = {"Miss","Hit"};

unsigned int m_w = 0xABABAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05080902;    /* must not be zero, nor 0x9068ffff */

 
unsigned int rand_()
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;  /* 32-bit result */
}

unsigned int memGen1()
{
	static unsigned int addr=0;
	return (addr++)%(DRAM_SIZE);
}

unsigned int memGen2()
{
	static unsigned int addr=0;
	return  rand_()%(8*1024);
}

unsigned int memGen3()
{
	return rand_()%(DRAM_SIZE);
}

unsigned int memGen4()
{
	static unsigned int addr=0;
	return (addr++)%(1024);
}

unsigned int memGen5()
{
	static unsigned int addr=0;
	return (addr++)%(1024*64);
}

unsigned int memGen6()
{
	static unsigned int addr=0;
	return (addr+=512)%(DRAM_SIZE);
}


// Direct Mapped Cache Simulator
cacheResType cacheSimDM(unsigned int addr)
{	
	unsigned int Tag;
	unsigned int Index;

	Index = addr & 0xffc;
	Index = Index >> 2;
	//cout << Index << endl;
	Tag = addr & 0xfffff000;	
	Tag = Tag >> 12;
	
	//cout << "Index: " << Index << "   Tag: " << Tag << endl;

	if(cacheLine[Index].v == 0)
	{
		cacheLine[Index].v = 1;
		cacheLine[Index].tag = Tag;
		return MISS;
	}
	else
		if (cacheLine[Index].tag != Tag)
		{
			cacheLine[Index].tag = Tag;
			return MISS;
		}
		else
			return HIT;
}

// Fully Associative Cache Simulator
cacheResType cacheSimFA(unsigned int addr)
{	
	unsigned int Tag;
	int cacheLocation;
	bool found = false, flag = false;

	Tag = addr & 0xFFFFFFE0;	
	Tag = Tag >> 5;
	
	for(int i=0; i<1024; i++)				//Searching for the tag in all the cache
		if( cacheLine[i].tag == Tag)
		{
			found = true;
			break;
		}
	
	if (found == true)
		return HIT;
	else
	{
		//Random
		srand ((unsigned) time(NULL));			//set the random number to the pc time

		for(int i = 0; i<1024; i++)
		{
			cacheLocation = rand() % 1024;			//Random Location

			if(	cacheLine[cacheLocation].v == 0)
			{
				cacheLine[cacheLocation].tag = Tag;
				cacheLine[cacheLocation].v = 1;
				flag = true;
				break;
			}
		}

		if (flag == false)
			cacheLine[cacheLocation].tag = Tag;


		//LRU

		return MISS;
	}

}

void Simulate(const int nReferences, int memGen, string simType)
{
	int inst, Hits, Misses;
	cacheResType r;
	unsigned int addr;

	inst=0; Hits = 0; Misses = 0;		//Initializing the values

	cout << "Memory Generator " << memGen << endl;

	//Forming the cache, validity = 0 (cache empty), numbering the indecies
	for (int i=0; i<1024; i++)
	{
		cacheLine[i].index = i;
		cacheLine[i].v = 0;

	}

	for(;inst<nReferences;inst++)
	{
		switch(memGen)
		{
			case 1:
				addr = memGen1();
				break;
			case 2:
				addr = memGen2();
				break;
			case 3:
				addr = memGen3();
				break;
			case 4:
				addr = memGen4();
				break;
			case 5:
				addr = memGen5();
				break;
			case 6:
				addr = memGen6();
				break;
		}

		if(simType == "Fully Associative")			//Fully Associative
			r = cacheSimFA(addr);
		else
			if(simType == "Direct Mapped")		//Direct Mapped
				r = cacheSimDM(addr);

		
		cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[r] <<")\n";
		
		if( r == 0 )
			Misses++;
		else
			Hits++;
	}

	cout << "Number of Misses: " << dec << Misses << endl;
	cout << "Miss ratio: " << Misses/(float)nReferences * 100.0 << "%" << endl;
	cout << "Number of Hits: " << Hits << endl;
	cout << "Hit ratio: " << Hits/(float)nReferences * 100.0 << "%" << endl << endl;
	
}

int main()
{
	int memGen;
	char terminate;

	while(true)
	{
		cout << "For which memGen function? (if done enter '0') ";
		cin >> memGen;

		if(memGen == 0)
			break;

		cout << "Direct Mapped Cache Simulator\n\n";

		Simulate(100, memGen, "Direct Mapped");
	
		cout << "Fully Associative Cache Simulator\n\n";

		Simulate(100, memGen, "Fully Associative");	
	}

	system("pause");
}


