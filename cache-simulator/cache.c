#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

unsigned long long int hex2dec(char input[])
{
	unsigned long long int Addr=0;
	int len,i=0,num=0;
	len=strlen(input)-1;
	while (len != 1)
	{
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
		Addr+=(unsigned long long int) pow(16.0,i)*num;
		i++;
	}
	return Addr;
}

unsigned long long int **Lru;
//Function to change the tag order in the lru algo
void bringtotop(unsigned long long int set, unsigned long long int assoc, unsigned long long int x)
{
    unsigned long long int i,pos=0;
    for(i=0;i<assoc;i++)
	{
        if(Lru[set][i] == x)
            pos = i;
	}
	for(i=pos;i<assoc-1;i++)
    {
		Lru[set][i] = Lru[set][i+1];
	}
	Lru[set][assoc-1] = x;
}

int main(int argc, char* argv[])
{    
    unsigned long long int Address,i,j,pos,r,set,tag,CacheCap,Assoc,BlockSize,NumSets,RChecks=0,WChecks=0,RMiss=0,WMiss=0,length=0;
	unsigned long long int **Cache;
	double MissPer,RMissPer,WMissPer;
	int found;
	char Repl,buffer[20];
	
	if(argc!=5)
	{
		printf("Wrong Number of Parameters \n");
		return 0;
	}

	CacheCap=atoi(argv[1])*1024;
	Assoc=atoi(argv[2]);
	BlockSize=atoi(argv[3]);
	Repl=argv[4][0];
	NumSets=CacheCap/(BlockSize*Assoc);
	
	
	Cache=(unsigned long long int **)malloc(sizeof(unsigned long long int*)*NumSets);
	Lru=(unsigned long long int **)malloc(sizeof(unsigned long long int*)*NumSets);
	for(i=0;i<NumSets;i++)
	{
		Cache[i]=(unsigned long long int *)malloc(sizeof(unsigned long long int)*Assoc);
		Lru[i]=(unsigned long long int *)malloc(sizeof(unsigned long long int)*NumSets);
	}

	for(i=0;i<NumSets;i++)
	{
		for(j=0;j<Assoc;j++)
		{
			Cache[i][j] = 0;
			Lru[i][j] = j; 
		}
	}


	while (fgets(buffer,20,stdin)!= NULL)
    	{
       			Address=hex2dec(buffer);
			set = (Address / BlockSize) % NumSets;
			tag = Address / (BlockSize * NumSets);
			if(buffer[0]=='r')
			{
				RChecks++;
				found=0;
				for(i=0;i<Assoc;i++)
				{
					if(Cache[set][i] == tag)
					{
						found = 1;
						pos = i;
						break;
					}
				}
				if(found)
				{
					if(Repl == 'l')
					{
						bringtotop(set,Assoc,pos);
					}
				}
				else 
				{
					RMiss++;
					if(Repl == 'l')
					{
						i = Lru[set][0];
						Cache[set][i] = tag;
						bringtotop(set,Assoc,i);
					}
					else
					{
						r = rand() % Assoc;
						Cache[set][r] = tag;
					}
				}
			}
			else if(buffer[0]=='w')
			{
				WChecks++;
				found=0;
				for(i=0;i<Assoc;i++)
				{
					if(Cache[set][i] == tag)
					{
						found = 1;
						pos = i;
					}
				}
				if(found)
				{
					if(Repl == 'l')
						{
							bringtotop(set,Assoc,pos);
						}
				}
				else if(!found)
				{
					WMiss++;
					if(Repl == 'l')
					{
						i = Lru[set][0];
						Cache[set][i] = tag;
						bringtotop(set,Assoc,i);
					}
					else
					{
						r = rand() % Assoc;
						Cache[set][r] = tag;
					}
				}
			}
	}
	
	MissPer=(double)(WMiss+RMiss)*100/(WChecks+RChecks);
	RMissPer=(double)RMiss*100/RChecks;
	WMissPer=(double)WMiss*100/WChecks;
	printf("Total Number of Misses = %d\n",(WMiss+RMiss));
	printf("Percentage of Misses = %lf percent\n",MissPer);
	printf("Total Number of Read Misses = %d\n",(RMiss));
	printf("Percentage of Read Misses = %lf percent\n",RMissPer);
	printf("Total Number of Write Misses = %d\n",(WMiss));
	printf("Percentage of Write Misses = %lf percent\n",WMissPer);
	getchar();
	return 0;
}
