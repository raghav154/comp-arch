// my_predictor.h
// This file contains a sample my_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.
// Note that this predictor doesn't use the whole 32 kilobytes available
// for the CBP-2 contest; it is just an example.

#define HASH_TABLES       8 
#define WEIGHTS_PER_TABLE 2039	
#define MASK 0xF0000000

class my_update : public branch_update {
public:
	unsigned int index[WEIGHTS_PER_TABLE];
};

class my_predictor : public branch_predictor {
public:
	my_update u;
	branch_info bi;
	unsigned long long history;
	char W[HASH_TABLES][WEIGHTS_PER_TABLE]; /* Table of Weights */
	char outcome;

	my_predictor (void) : history(0),outcome(0) { 
		for(int i=0;i<HASH_TABLES;i++) {
			for(int j=0;j<WEIGHTS_PER_TABLE;j++) {
				W[i][j] = 0;
			}
		}	
	}

	branch_update *predict (branch_info & b) {
		bi = b;
		if (b.br_flags & BR_CONDITIONAL) {
			u.index[0] = (history ^ b.address) % (WEIGHTS_PER_TABLE);
			outcome = W[0][u.index[0]]; 			
			for(int i=1;i<HASH_TABLES;i++) {
                                u.index[i] = ((history & (MASK>>(i*5))) ^ (b.address)) % (WEIGHTS_PER_TABLE);
				outcome += W[i][u.index[i]];	
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

	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {
			if( u->direction_prediction() != taken ) {
				for(int i=0;i<HASH_TABLES;i++) {
					if(taken) {
						W[i][((my_update *)u)->index[i]] += 1;
					}
					else {
						W[i][((my_update *)u)->index[i]] -= 1;
					}
				}
			}
			history <<= 1;
			history |= taken;
		}
	}
};
