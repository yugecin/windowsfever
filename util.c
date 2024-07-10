unsigned long int next = 1;

/*rand & srand copied from "the c (ansi c) programming language second edition"*/
#define RAND_MAX 32768
int rand(void)
{
	next = next * 1103515245 + 12345;
	return (unsigned int) (next/65536) % 32768;
}
void srand(unsigned int seed)
{
	next = seed;
}

int randn(int maxexcl)
{
	return rand() / (RAND_MAX / maxexcl + 1);
}
