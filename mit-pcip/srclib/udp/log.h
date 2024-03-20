/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */

/* log header and some constants */

struct logmsg {
	char	l_type;
	char	l_subtype;
	char	l_logname[8];
	char	l_id[2];
	char	l_sender[8];
	char	l_status[2];
	char	l_substatus[2];
	};

/* log types */
#define	LOG_MSG			1
#define	LOG_STATUS_REPLY	2
#define	LOG_ACK			2
#define	LOG_STATUS_REQUEST	3

/* subtypes */
#define	LOG_S_IGNORE		1
#define	LOG_S_LOG		2
#define	LOG_S_DIFF		3
#define	LOG_S_NO_ACK		4
