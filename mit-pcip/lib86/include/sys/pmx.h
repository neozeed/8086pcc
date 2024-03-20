#define	NINDEX	16

struct group {
	short	g_state;
	char	g_index;
	char	g_rot;
	struct	group	*g_group;
	struct	inode	*g_inode;
	short	g_rotmask;
	short	g_datq;
	struct	chan *g_chans[NINDEX];
};
