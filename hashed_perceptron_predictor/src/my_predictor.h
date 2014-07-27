/* my_predictor.h */
/* HASHED PERCEPTRON BRANCH PREDICTOR WITH MERGING PATH AND GSHARE PREDICTOR */
/* Author: Raghavan S V */
/* UIN: 722009006 */
/* Hardware budget provided: 16384 Bytes + 256 bits = 16416 bytes */
/* Hardware budget used: 16416 bytes */
/* Assumption: Hardware budget does not include variables for loops and temporary variables in functions */

#include <math.h>
#define HASH_TABLES       	8  /* Number of Hash tables used */
#define WEIGHTS_PER_TABLE 	3001 /* Total number of weights per table */
#define MASK 		  	0xFF /* MASK for obtaining history segment */
#define HIST_LEN		2 /* Number of array elements for global history */
#define path /* Define for including path history */

#ifdef path
	#define P_MASK 		0x7FF /* Mask for path/branch history */
	#define P_HIST		1349 /* The number of path history values indexed */
#endif

class my_update : public branch_update {
public:
	unsigned short index[HASH_TABLES]; /*Index for each hash table: Max value = WEIGHTS_PER_TABLE = 3001
										Space = 12/16 * HASH_TABLES * 2= 12 bytes */   
};

class my_predictor : public branch_predictor {
public:
	my_update u; /*Object of type My_update */ 
	branch_info bi; /* object of type branch info */
	float theta;	/* Threshold: 4 bytes */
#ifdef path
	unsigned short path_indx; /* Path history index: 2 bytes */
	unsigned char path_history[P_HIST]; /* Path history table: 1349 bytes */
#endif
	unsigned long history[HIST_LEN];/* Global History: 4*2 = 8 bytes */
	float bias_table[HASH_TABLES]; /* BIAS table: 4*8 = 32 bytes */
	char W[HASH_TABLES][WEIGHTS_PER_TABLE]; /* Table of Weights - Using only 5-bit counters of the 8 bits
                                                Space = WEIGHTS_PER_TABLE * HASH_TABLES * (5/8) = WEIGHTS_PER_TABLE * 5 bytes 
									                        = 15005 bytes	*/
	float outcome;/* Outcome: 4 bytes */

	/* Total hardware budget used: 15005 + 1349 + 4 + 4 + 32 + 8 + 2 + 12 = 16416 bytes */

	/* Constructor - Initialization of tables/variables */
	my_predictor (void) : outcome(0) { 
		for(int i=0;i<HIST_LEN;i++) {
			history[i] = 0;		
		}
		for(int i=0;i<HASH_TABLES;i++) {
			bias_table[i] = 1.0;
			for(int j=0;j<WEIGHTS_PER_TABLE;j++) {
				W[i][j] = 0;
			}
		}
#ifdef path
		for(int i=0;i<P_HIST;i++) {
			path_history[i] = 0;
		}
#endif		
		theta = 1.27 * HASH_TABLES + HASH_TABLES/2;  /* Threshold */	
	}

	/* Predict Function */
	branch_update *predict (branch_info & b) {
		bi = b;
		if (b.br_flags & BR_CONDITIONAL) {
#ifdef path
            path_indx = (bi.address & P_MASK) % P_HIST; /* Computing path table index (Branch History) */
#endif
			outcome = 0; /*Initializing the outcome to 0 */  			
			for(int i=0; i<HASH_TABLES; i++) {
                u.index[i] = hash(b.address,i); /* Calling the hash function for computing index */
				outcome += (W[i][u.index[i]] * bias_table[i]);	
            }
			if(outcome > 0) {
				u.direction_prediction (true);
			}
			else {
				u.direction_prediction (false);
			}
		} 
		else {
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
	}

	/* Update Function */
	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {
			if(( u->direction_prediction() != taken ) || ((abs(outcome)) <= theta)) {
				for(int i=0;i<HASH_TABLES;i++) {
					/* Weight Counters use only 5 of the 8 bits */
					if(taken) { /* Saturating Counter at (2^(5-1)-1 = 15) */
						if(W[i][((my_update *)u)->index[i]] >= 15)
							W[i][((my_update *)u)->index[i]] = 15;
						else
							W[i][((my_update *)u)->index[i]] += 1;
					}
					else { /* Saturating Counter at (-2^(5-1) = -16) */
						if(W[i][((my_update *)u)->index[i]] <= -16)
                                                        W[i][((my_update *)u)->index[i]] = -16;
                                                else
                                                        W[i][((my_update *)u)->index[i]] -= 1;
					}
				}
				/* Dynamic Weight Boosting of tables, by calculating bias for each table, in case of misprediction*/
				calculate_bias();
			}
			/* Updating global history */
			for(int i=HIST_LEN-1;i>0;i--) {
				history[i] <<= 1;
				history[i] |= (history[i-1]>>31);
			}
			history[0] <<= 1;
			history[0] |= taken; 
			/* Updating Path(branch) history */
#ifdef path
			path_history[path_indx] <<= 1;
			path_history[path_indx] |= taken;
#endif
		}
	}
	/* Computing the hashed index */
	unsigned short hash(unsigned long addr,int num) {
		unsigned short indx,hst_indx,msk_sft;
		hst_indx = num/(HASH_TABLES/HIST_LEN); /* Computing index for global history table */
		msk_sft = (HIST_LEN*32)/HASH_TABLES-1; /* Mask Shift offset value to be used */
		/* Computing INDEX of weigth table */
		indx = (((((history[hst_indx]<<8) | (path_history[path_indx]))) & (MASK<<((num-(hst_indx*4))*msk_sft))) ^ (addr<<4)) % (WEIGHTS_PER_TABLE);
		return indx;
	}
	/* Calculating bias for each hash table */
	void calculate_bias() {
		/* Compute the bias to be given to eaach HASH table */
		for(int i=0; i<HASH_TABLES; i++) {
			if(outcome > 0) {
				if((outcome-W[i][u.index[i]]) <= 0) {
					bias_table[i] += 0.025;
				}
				else {
					bias_table[i] -= 0.005;
				}
			}
			else {
				if((outcome-W[i][u.index[i]]) > 0) {
					bias_table[i] += 0.025;
				}
				else {
                    bias_table[i] -= 0.005;
                }
			}
			/* Lower bias saturating at 0.75 */
			if(bias_table[i] <= 0.75)
				bias_table[i] = 0.75;
			/* Upper bias saturating at 2.5 */
			else if (bias_table[i] >= 2.5)
				bias_table[i] = 2.5;
		}
	}		
};

