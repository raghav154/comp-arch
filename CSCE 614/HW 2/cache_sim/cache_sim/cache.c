#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Function definition hex2dec().
Convert the incoming hex 64-bit address string into a decimal number */
unsigned long long int hex2dec(char input[])
{
	unsigned long long int Addr=0;
	int len,i=0,num=0;
	len=strlen(input)-1;
	/* Reading the address alone without the first character */
	while (len != 1)
	{
		/* Switch case to check for the hexadecimal entry and convert to decimal */
		switch(input[--len])
		{
			case '0':num=0;
			break;
			case '1':num=1;
			break;
			case '2':num=2;
			break;
			case '3':num=3;
			break;
			case '4':num=4;
			break;
			case '5':num=5;
			break;
			case '6':num=6;
			break;
			case '7':num=7;
			break;
			case '8':num=8;
			break;
			case '9':num=9;
			break;
			case 'a':num=10;
			break;
			case 'b':num=11;
			break;
			case 'c':num=12;
			break;
			case 'd':num=13;
			break;
			case 'e':num=14;
			break;
			case 'f':num=15;
			break;
		}
		/* Converting the 64-bit Address to a decimal number */
		Addr+=(unsigned long long int) pow(16.0,i)*num;
		i++;
	}
	/* Return the Decimal Address */
	return Addr;
}

/* Initialization of the LRU pointer  */
unsigned long long int **Lru;

/* Function definition LruTagChange().
Change the tag order in the Lru array and implement the LRU policy */
void LruTagChange(unsigned long long int set, unsigned long long int assoc, unsigned long long int x)
{
    unsigned long long int i,pos=0;
	/* Run a loop to check for the position of the block within the set */
    for(i=0;i<assoc;i++)
	{
		/*If there is a match, store the position of the block */
        if(Lru[set][i] == x)
            pos = i;
	}
	/* Run a loop from the position to the second last block within the set */
	for(i=pos;i<assoc-1;i++)
    {
		/* Swap every position of the block with the next one */
		Lru[set][i] = Lru[set][i+1];
	}
	/* Store the block at position pos at the very end, so as to mark as recently used */
	Lru[set][assoc-1] = x;
}

/* Main function taking command line arguments */
int main(int argc, char* argv[])
{    
	/* Initialization of variables */
    unsigned long long int Address,i,j,pos,r,set,tag,CacheCap,Assoc,BlockSize,NumSets,RChecks=0,WChecks=0,RMiss=0,WMiss=0,length=0;
	/* Initialization of the Cache pointer */
	unsigned long long int **Cache;
	/* Initialization of variables to compute various % parameters*/
	long double MissPer,RMissPer,WMissPer;
	int found;
	/* Initialization of character buffer to store incoming addresses */
	char Repl,buffer[20];
	
	/* If number of command line arguments not 4, return */
	if(argc!=5)
	{
		printf("Wrong Number of Parameters \n");
		return 0;
	}

	/* Convert command line arguments (strings) into corresponding integers */
	CacheCap=atoi(argv[1])*1024;
	Assoc=atoi(argv[2]);
	BlockSize=atoi(argv[3]);
	Repl=argv[4][0];
	/* Compute the Number of Sets in the Cache based on the parameters */
	NumSets=CacheCap/(BlockSize*Assoc);
	
	/* Dynamic allocation fo the Cache and LRU arrays based on Number of Sets & Associativity */
	Cache=(unsigned long long int **)malloc(sizeof(unsigned long long int*)*NumSets);
	Lru=(unsigned long long int **)malloc(sizeof(unsigned long long int*)*NumSets);
	for(i=0;i<NumSets;i++)
	{
		Cache[i]=(unsigned long long int *)malloc(sizeof(unsigned long long int)*Assoc);
		Lru[i]=(unsigned long long int *)malloc(sizeof(unsigned long long int)*NumSets);
	}

	/* Initialization of values of the Cache and LRU arrays */
	for(i=0;i<NumSets;i++)
	{
		for(j=0;j<Assoc;j++)
		{
			Cache[i][j] = 0;
			Lru[i][j] = j; 
		}
	}

	/* Run loop to obtain incoming strings' of addresses until end of file is reached */
	while (fgets(buffer,20,stdin)!= NULL)
    {
			/* Obtain Decimal address value */
       		Address=hex2dec(buffer);
			/* Computation of the Set & Tag component of the address */
			set = (Address / BlockSize) % NumSets;
			tag = Address / (BlockSize * NumSets);
			/* If reading into cache */
			if(buffer[0]=='r')
			{
				/* Increment Read Checks to the Cache */
				RChecks++;
				found=0;
				for(i=0;i<Assoc;i++)
				{
					/* If the tag of the address matches the one in Cache, set flag & store the position */
					if(Cache[set][i] == tag)
					{
						found = 1;
						pos = i;
						break;
					}
				}
				/* If block found i.e. a hit */
				if(found)
				{
					/* If LRU policy chosen, change tag position of the block in the LRU array */
					if(Repl == 'l')
					{
						LruTagChange(set,Assoc,pos);
					}
				}
				/* In case of Miss */
				else 
				{
					/* Increment value of Read Misses */
					RMiss++;
					/* If LRU policy chosen, remove the Least Recently used block and store the block in Cache & update LRU Array*/
					if(Repl == 'l')
					{
						i = Lru[set][0];
						Cache[set][i] = tag;
						LruTagChange(set,Assoc,i);
					}
					/* If Random  policy chosen, generate random position and store the block in that position in Cache*/
					else
					{
						r = rand() % Assoc;
						Cache[set][r] = tag;
					}
				}
			}
			/* If writing into cache */
			else if(buffer[0]=='w')
			{
				/* Increment Write Checks to the Cache */
				WChecks++;
				found=0;
				for(i=0;i<Assoc;i++)
				{
					/* If the tag of the address matches the one in Cache, set flag & store the position */
					if(Cache[set][i] == tag)
					{
						found = 1;
						pos = i;
					}
				}
				/* If block found i.e. a hit */
				if(found)
				{
					/* If LRU policy chosen, change tag position of the block in the LRU array */
					if(Repl == 'l')
						{
							LruTagChange(set,Assoc,pos);
						}
				}
				/* In case of Miss */
				else if(!found)
				{
					/* Increment value of Write Misses */
					WMiss++;
					/* If LRU policy chosen, remove the Least Recently used block and store the block in Cache & update LRU Array*/
					if(Repl == 'l')
					{
						i = Lru[set][0];
						Cache[set][i] = tag;
						LruTagChange(set,Assoc,i);
					}
					/* If Random  policy chosen, generate random position and store the block in that position in Cache*/
					else
					{
						r = rand() % Assoc;
						Cache[set][r] = tag;
					}
				}
			}
	}
	/* Calculate Miss percentages*/	
	MissPer=(long double)(WMiss+RMiss)*100/(WChecks+RChecks);
	RMissPer=(long double)RMiss*100/RChecks;
	WMissPer=(long double)WMiss*100/WChecks;
	/* Display the paramters on the screen */
	printf("Total Number of Misses = %d\n",(WMiss+RMiss));
	printf("Percentage of Misses = %lf percent\n",MissPer);
	printf("Total Number of Read Misses = %d\n",(RMiss));
	printf("Percentage of Read Misses = %lf percent\n",RMissPer);
	printf("Total Number of Write Misses = %d\n",(WMiss));
	printf("Percentage of Write Misses = %lf percent\n",WMissPer);
	getchar();
	return 0;
}