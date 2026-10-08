
typedef unsigned long long size_t;

void *memcpy(void * __restrict dest, const void * __restrict src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *dest, int c, size_t n);
int   memcmp(const void *lhs, const void *rhs, size_t n);

void *memcpy(void * __restrict dest, const void * __restrict src, size_t n)
{
	unsigned char *d = (unsigned char *)dest;
	const unsigned char *s = (const unsigned char *)src;

	while (n--)
		*d++ = *s++;

	return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
	unsigned char *d = (unsigned char *)dest;
	const unsigned char *s = (const unsigned char *)src;

	if (d == s || n == 0)
		return dest;

	if (d < s)
	{
		while (n--)
			*d++ = *s++;
	}
	else
	{
		d += n;
		s += n;

		while (n--)
			*--d = *--s;
	}

	return dest;
}

void *memset(void *dest, int c, size_t n)
{
	unsigned char *d = (unsigned char *)dest;
	unsigned char value = (unsigned char)c;

	while (n--)
		*d++ = value;

	return dest;
}

int memcmp(const void *lhs, const void *rhs, size_t n)
{
	const unsigned char *a = (const unsigned char *)lhs;
	const unsigned char *b = (const unsigned char *)rhs;

	while (n--)
	{
		if (*a != *b)
			return (*a > *b) ? 1 : -1;

		++a;
		++b;
	}

	return 0;
}