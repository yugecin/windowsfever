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

float eq_out_quad(float t) // fast
{
	return -1.0f * t * (t - 2.0f);
}

float eq_out_cubic(float t) // faster
{
	t = t - 1.0f;
	return t = t * t * t + 1.0f;
}

float eq_out_quart(float t) // fastest
{
	t = t - 1.0f;
	return t = -1.0f * (t * t * t * t - 1.0f);
}

float eq_out_quint(float t) // fasterest
{
	t = t - 1.0f;
	return t * t * t * t * t + 1.0f;
}
