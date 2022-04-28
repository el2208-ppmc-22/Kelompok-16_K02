//----------------------------------
// Quine�McCluskey Algorithm
// =================================
// Kelompok 16 : Figo Akmal Munir (18320036), Farisa Aliya (18320038), Indira Mona Farhany (18320040)

#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define MAXVARS 4
#define MAX 2048

//Deklarasi Variabel Global
int minterm[MAX][MAX];
int mask[MAX][MAX];		// mask of minterm  
int used[MAX][MAX];		// minterm used  
int hasil[MAX];		//   
int primmask[MAX];		// mask for prime implicants  
int prim[MAX];			// prime implicant 
int wprim[MAX];			// essential prime implicant (TRUE/FALSE)  
int nwprim[MAX];		// needed not essential prime implicant 

//Count all set bits of the integer number  
int popCount(unsigned x) { // Taken from book "Hackers Delight" 
	x = x - ((x >> 1) & 0x55555555);
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	x = (x + (x >> 4)) & 0x0F0F0F0F;
	x = x + (x >> 8);
	x = x + (x >> 16);
	return x & 0x0000003F;
}

//Calculate hamming weight/distance of two integer numbers  
int hammingWeight(int v1, int v2) {
	return popCount(v1 ^ v2);
} 

//Output upper part of term in console  
void upperTerm(int bitfield, int mask, int num) {
	if (mask) {
		int z;
		for ( z = 0; z < num; z++) {
			if (mask & (1 << z)) {      
				if (bitfield & (1 << z))		
					printf("_");
				else
					printf(" ");
			}
		} 
	}
}

//Output lower part of term in console  
void lowerTerm(int mask, int num) {
	if (mask) {
		int z;
		for (z = 0; z < num; z++) {
			if (mask & (1 << z)) {
				printf("%c", 'z' - (num - 1) + z);
			} 
		} 
	}
}

//Output a term to console 
void outputTerm(int bitfield, int mask, int num) {
	upperTerm(bitfield, mask, num);
	if (mask) printf("\n");
	lowerTerm(mask, num);
}

//Determines whether "value" contains "part"  
int contains(value, mask, part, partmask) {
	if ((value & partmask) == (part & partmask)) {
		if ((mask & partmask) ==  partmask)
			return TRUE;
	}   
	return FALSE;
}

int main() {
	int num = 0; // Number of Variables  
	int pos = 0;
	int cur = 0;
	int reduction = 0; //reduction step  
	int maderedction = FALSE;
	int prim_count = 0;
	int term = 0;
	int termmask = 0;
	int found = 0;
	int x = 0;
	int y = 0;
	int z = 0;
	int count = 0;
	int lastprim = 0; 
	int res = 0; // actual result  

	// Fill all arrays with default values 
	while (x < MAX){
		primmask[x] = 0;
		prim[x] = 0;
		wprim[x] = FALSE;
		nwprim[x] = FALSE;
		hasil[x] = FALSE;
		nwprim[x] = TRUE; 
		while (y < MAX){
			mask[x][y] = 0;
			minterm[x][y] = 0;
			used[x][y] = FALSE;
			y = y + 1;
		}
		x = x + 1;
	}
	
	/*for (x = 0; x < MAX; x++) {
		primmask[x] = 0;
		prim[x] = 0;
		wprim[x] = FALSE;
		nwprim[x] = FALSE;
		hasil[x] = FALSE;
		nwprim[x] = TRUE; 
		for (y = 0; y < MAX; y++) {
			mask[x][y] = 0;
			minterm[x][y] = 0;
			used[x][y] = FALSE;
		}
	}*/

	printf("Enter number of variables: ");
	scanf(" %d", &num);
	if (num > MAXVARS) {
		fprintf(stderr, "ERROR: Number of variables too big!\n");
		return 0;
	}
	if (num < 1) {
		fprintf(stderr, "ERROR: Number of variables must be at least 1!\n");
		return 0;
	}

	pos = (1 << num);  // 2 ^ num
	printf("Please enter desired results: ( 0 or 1)\n\n");

	cur = 0; 
	for ( x=0; x < pos; x++) {
		int value = 0;
		outputTerm(x, pos - 1, num);
		printf(" = ");
		scanf(" %d", &value);
		if (value) {
			mask[cur][0] = ((1 << num)- 1);   
			minterm[cur][0] = x;  
			cur++;  
			hasil[x] = 1;
		}
		printf("\n");
	}

	for (reduction = 0; reduction < MAX; reduction++) {
		cur = 0; 
		maderedction = FALSE;
		for (y=0; y < MAX; y++) {
			for (x=0; x < MAX; x++) {   
				if ((mask[x][reduction]) && (mask[y][reduction])) {      
					if (popCount(mask[x][reduction]) > 1) { // Do not allow complete removal (problem if all terms are 1)  
						if ((hammingWeight(minterm[x][reduction] & mask[x][reduction], minterm[y][reduction] & mask[y][reduction]) == 1) && (mask[x][reduction] == mask[y][reduction])) { // Simplification only possible if 1 bit differs   
							term = minterm[x][reduction]; // could be mintern x or y 
							//e.g.:
							//1110
							//1111
							//Should result in mask of 1110  
							termmask = mask[x][reduction]  ^ (minterm[x][reduction] ^ minterm[y][reduction]); 
							term  &= termmask;

							found = FALSE;		
							for ( z=0; z<cur; z++) {
								if ((minterm[z][reduction+1] == term) && (mask[z][reduction+1] == termmask) ) {							
									found = TRUE;
								}
							}

							if (found == FALSE) {
								minterm[cur][reduction+1] = term;
								mask[cur][reduction+1] = termmask;
								cur++; 
							}
							used[x][reduction] = TRUE;
							used[y][reduction] = TRUE;  
							maderedction = TRUE;
						}
					}
				} 
			}    
		}
		if (maderedction == FALSE)
			break; //exit loop early (speed optimisation)  
	}

	prim_count = 0;
	//printf("\nprime implicants:\n");
	for ( reduction = 0 ; reduction < MAX; reduction++) {
		for ( x=0 ;x < MAX; x++) {		
			//Determine all not used minterms  
			if ((used[x][reduction] == FALSE) && (mask[x][reduction]) ) {
				//Check if the same prime implicant is already in the list  
				found = FALSE;
				for ( z=0; z < prim_count; z++) {
					if (((prim[z] & primmask[z]) == (minterm[x][reduction] & mask[x][reduction])) &&  (primmask[z] == mask[x][reduction]) )					
						found = TRUE;
				} 
				if (found == FALSE) {
					//outputTerm(minterm[x][reduction], mask[x][reduction], num);
					//printf("\n");
					primmask[prim_count] = mask[x][reduction];
					prim[prim_count] = minterm[x][reduction];
					prim_count++;
				}     
			} 
		} 
	}  

	//find essential and not essential prime implicants  
	//all alle prime implicants are set to "not essential" so far  
	for (y=0; y < pos; y++) { //for all minterms  
		count = 0;
		lastprim = 0;   
		if (mask[y][0]) {
			for (x=0; x < prim_count; x++ ) { //for all prime implicants    
				if (primmask[x]) {
					// Check if the minterm contains prime implicant  
					if (contains(minterm[y][0], mask[y][0], prim[x], primmask[x])) {					
						count++;
						lastprim = x;          
					}  
				} 		
			}
			// If count = 1 then it is a essential prime implicant 
			if (count == 1) {
				wprim[lastprim] = TRUE;
			}
		}
	}

	// successively testing if it is possible to remove prime implicants from the rest matrix  
	for ( z=0; z < prim_count; z++) {
		if (primmask[z] ) {
			if ((wprim[z] == FALSE)) { // && (rwprim[z] == TRUE))
				nwprim[z] = FALSE; // mark as "not essential" 
				for ( y=0; y < pos; y++) { // for all possibilities  
					res = 0;
					for ( x=0; x < prim_count; x++) {
						if ( (wprim[x] == TRUE) || (nwprim[x] == TRUE)) {  //essential prime implicant or marked as required  
							if ((y & primmask[x]) == (prim[x] & primmask[x])) { //All bits must be 1  
								res = 1; 
								break;
							}
						}
					}
					//printf(" %d\t%d\n", result, result[y]);
					if (res == hasil[y]) {  // compare calculated result with input value 		
						//printf("not needed\n"); //prime implicant not required  
					}
					else {
						//printf("needed\n");
						nwprim[z] = TRUE; //prime implicant required  
					}
				}
			}
		}
	}
	printf("\nResult:\n\n");
	// Output of essential and required prime implicants 
	count = 0;
	for ( x = 0 ; x < prim_count; x++) {
		if (wprim[x] == TRUE) {
			if (count > 0) printf("   ");
			upperTerm(prim[x], primmask[x], num);
			count++;
		}
		else if ((wprim[x] == FALSE) && (nwprim[x] == TRUE)) {
			if (count > 0) printf("   ");
			upperTerm(prim[x], primmask[x], num);
			count++;
		}
	}
	printf("\n");
	count = 0;
	for ( x = 0 ; x < prim_count; x++) {
		if (wprim[x] == TRUE) {
			if (count > 0) printf(" + ");
			lowerTerm(primmask[x], num);
			count++;
		}
		else if ((wprim[x] == FALSE) && (nwprim[x] == TRUE)) {
			if (count > 0) printf(" + ");
			lowerTerm(primmask[x], num);
			count++;
		}
	}
	printf("\n");
	return 0;
}