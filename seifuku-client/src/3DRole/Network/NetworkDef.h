#ifndef _NETWORKDEF_H
#define _NETWORKDEF_H

const int _MAX_MSGSIZE		=1024;
const int _MAX_WORDSSIZE	=256;
const int _MAX_IDLETIME		=600;
const int _MAX_TASK_PER_PAGE = 10;

const int _PORT_ACCOUNTSERVER	= 9958;
const int _PORT_GAMESERVER		= 5816;

const unsigned short _MSG_NONE			=0;
const unsigned short _MSG_GENERAL		=1000;
const unsigned short _MSG_REGISTER		=_MSG_GENERAL+1;
const unsigned short _MSG_LOGIN			=_MSG_GENERAL+2;
const unsigned short _MSG_LOGOUT		=_MSG_GENERAL+3;
const unsigned short _MSG_TALK			=_MSG_GENERAL+4;
const unsigned short _MSG_WALK			=_MSG_GENERAL+5;
const unsigned short _MSG_USERINFO		=_MSG_GENERAL+6;
const unsigned short _MSG_ATTACK		=_MSG_GENERAL+7;
const unsigned short _MSG_ITEMINFO		=_MSG_GENERAL+8;
const unsigned short _MSG_ITEM			=_MSG_GENERAL+9;
const unsigned short _MSG_ACTION		=_MSG_GENERAL+10;
const unsigned short _MSG_ACCIDENT		=_MSG_GENERAL+11;
const unsigned short _MSG_TICK			=_MSG_GENERAL+12;
const unsigned short _MSG_ROOM			=_MSG_GENERAL+13;
const unsigned short _MSG_PLAYER		=_MSG_GENERAL+14;
const unsigned short _MSG_NAME			=_MSG_GENERAL+15;
const unsigned short _MSG_WEATHER		=_MSG_GENERAL+16;
const unsigned short _MSG_USERATTRIB	=_MSG_GENERAL+17;
const unsigned short _MSG_ROLE			=_MSG_GENERAL+18;
const unsigned short _MSG_FRIEND		=_MSG_GENERAL+19;
const unsigned short _MSG_EFFECT		=_MSG_GENERAL+20;
const unsigned short _MSG_QUIZGAME		=_MSG_GENERAL+21;
const unsigned short _MSG_INTERACT		=_MSG_GENERAL+22;
const unsigned short _MSG_TEAM			=_MSG_GENERAL+23;
const unsigned short _MSG_ALLOT			=_MSG_GENERAL+24;
const unsigned short _MSG_WEAPONSKILL	=_MSG_GENERAL+25;
const unsigned short _MSG_TEAMMEMBER	=_MSG_GENERAL+26;
const unsigned short _MSG_GEMEMBED		=_MSG_GENERAL+27;

const unsigned short _MSG_ACCOUNT		=_MSG_GENERAL+51;
const unsigned short _MSG_CONNECT		=_MSG_GENERAL+52;
const unsigned short _MSG_TRADE			=_MSG_GENERAL+56;

const unsigned short _MSG_LOGINREPLY	=_MSG_GENERAL+55;
const unsigned short _MSG_LOGINACCOUNTEX=_MSG_GENERAL+60;
const unsigned short _MSG_TYPE1028		=_MSG_GENERAL+28;
const unsigned short _MSG_TYPE1029		=_MSG_GENERAL+29;

const unsigned short _MSG_MAPITEM		=_MSG_GENERAL+101;
const unsigned short _MSG_PACKAGE		=_MSG_GENERAL+102;
const unsigned short _MSG_MAGICINFO		=_MSG_GENERAL+103;
const unsigned short _MSG_FLUSHEXP		=_MSG_GENERAL+104;
const unsigned short _MSG_MAGICEFFECT	=_MSG_GENERAL+105;
const unsigned short _MSG_SYNATTRINFO	=_MSG_GENERAL+106;
const unsigned short _MSG_SYNDICATE		=_MSG_GENERAL+107;

const unsigned short _MSG_ITEMINFOEX	=_MSG_GENERAL+108;
const unsigned short _MSG_NPCINFOEX		=_MSG_GENERAL+109;
const unsigned short _MSG_MAPINFO		=_MSG_GENERAL+110;
const unsigned short _MSG_MESSAGEBOARD	=_MSG_GENERAL+111;
const unsigned short _MSG_SYNMEMBERINFO	=_MSG_GENERAL+112;
const unsigned short _MSG_DICE			=_MSG_GENERAL+113;

const unsigned short _MSG_NPCINFO		=_MSG_GENERAL+1030;
const unsigned short _MSG_NPC			=_MSG_GENERAL+1031;
const unsigned short _MSG_TASKDIALOG	=_MSG_GENERAL+1032;
const unsigned short _MSG_FRIENDINFO	=_MSG_GENERAL+1033;

const unsigned short _MSG_TIME			=_MSG_GENERAL+1034;
const unsigned short _MSG_PETINFO		=_MSG_GENERAL+1035;
const unsigned short _MSG_DATAARRAY		=_MSG_GENERAL+1036;

const unsigned short _TXT_NORMAL	=0;
const unsigned short _TXT_SCROLL	=0x0001;
const unsigned short _TXT_FLASH		=0x0002;
const unsigned short _TXT_BLAST		=0x0008;

const unsigned short _TXTATR_NORMAL		=2000;
const unsigned short _TXTATR_PRIVATE	=_TXTATR_NORMAL+1;
const unsigned short _TXTATR_ACTION		=_TXTATR_NORMAL+2;
const unsigned short _TXTATR_TEAM		=_TXTATR_NORMAL+3;
const unsigned short _TXTATR_SYNDICATE	=_TXTATR_NORMAL+4;
const unsigned short _TXTATR_SYSTEM		=_TXTATR_NORMAL+5;
const unsigned short _TXTATR_FAMILY		=_TXTATR_NORMAL+6;
const unsigned short _TXTATR_TALK		=_TXTATR_NORMAL+7;
const unsigned short _TXTATR_YELP 		=_TXTATR_NORMAL+8;
const unsigned short _TXTATR_FRIEND 	=_TXTATR_NORMAL+9;
const unsigned short _TXTATR_GLOBAL 	=_TXTATR_NORMAL+10;
const unsigned short _TXTATR_GM 		=_TXTATR_NORMAL+11;
const unsigned short _TXTATR_WHISPER 	=_TXTATR_NORMAL+12;
const unsigned short _TXTATR_GHOST	 	=_TXTATR_NORMAL+13;
const unsigned short _TXTATR_SERVE		=_TXTATR_NORMAL+14;
const unsigned short _TXTATR_MY			=_TXTATR_NORMAL+15;

const unsigned short _TXTATR_REGISTER	=_TXTATR_NORMAL+100;
const unsigned short _TXTATR_ENTRANCE	=_TXTATR_NORMAL+101;
const unsigned short _TXTATR_SHOP		=_TXTATR_NORMAL+102;
const unsigned short _TXTATR_PETTALK	=_TXTATR_NORMAL+103;
const unsigned short _TXTATR_CRYOUT		=_TXTATR_NORMAL+104;
const unsigned short _TXTATR_WEBPAGE	=_TXTATR_NORMAL+105;

const unsigned short _TXTATR_NEWMESSAGE	=_TXTATR_NORMAL+106;
const unsigned short _TXTATR_TASK		=_TXTATR_NORMAL+107;

const unsigned short _TXT_SYNWAR_FIRST	=_TXTATR_NORMAL+108;
const unsigned short _TXT_SYNWAR_NEXT	=_TXTATR_NORMAL+109;

const unsigned short _TXTATR_LEAVEWORD		=_TXTATR_NORMAL+110;
const unsigned short _TXTATR_SYNANNOUNCEG	=_TXTATR_NORMAL+111;
const unsigned short _TXTATR_SYNANNOUNCEL	=_TXTATR_NORMAL+114;

const unsigned short _TXTATR_MESSAGEBOX		=_TXTATR_NORMAL+112;
const unsigned short _TXTATR_REJECT			=_TXTATR_NORMAL+113;

const unsigned short _TXTATR_TRACE			=_TXTATR_NORMAL+115;

const unsigned short _TXTATR_MSG_TRADE	=_TXTATR_NORMAL+201;
const unsigned short _TXTATR_MSG_FRIEND	=_TXTATR_NORMAL+202;
const unsigned short _TXTATR_MSG_TEAM	=_TXTATR_NORMAL+203;
const unsigned short _TXTATR_MSG_SYN	=_TXTATR_NORMAL+204;
const unsigned short _TXTATR_MSG_OTHER	=_TXTATR_NORMAL+205;

#endif