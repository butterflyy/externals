//rc4.h  
#ifndef _RC4_H
#define _RC4_H

struct rc4_state
{
    int x, y, m[256];
};

static void rc4_setup(struct rc4_state *s, unsigned char *key, int length)
{
	int i, j, k, *m, a;

	s->x = 0;
	s->y = 0;
	m = s->m;

	for (i = 0; i < 256; i++)
	{
		m[i] = i;
	}

	j = k = 0;

	for (i = 0; i < 256; i++)
	{
		a = m[i];
		j = (unsigned char)(j + a + key[k]);
		m[i] = m[j]; m[j] = a;
		if (++k >= length) k = 0;
	}
}

static void rc4_crypt(struct rc4_state *s, unsigned char *data, int length, unsigned char *outdata)
{
	int i, x, y, *m, a, b;

	x = s->x;
	y = s->y;
	m = s->m;

	for (i = 0; i < length; i++)
	{
		x = (unsigned char)(x + 1);
		a = m[x];
		y = (unsigned char)(y + a);
		m[x] = b = m[y];
		m[y] = a;
		outdata[i] = data[i] ^ m[(unsigned char)(a + b)];
	}

	s->x = x;
	s->y = y;
}

#endif /* rc4.h */

 
