struct map
{
	mem_t	m_size;
	mem_t	m_addr;
};

struct map coremap[CMAPSIZ];	/* space for core allocation */
struct map swapmap[SMAPSIZ];	/* space for swap allocation */
