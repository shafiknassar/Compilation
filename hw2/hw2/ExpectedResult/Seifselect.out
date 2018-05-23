Select:
( S -> Structure ) = { STARTSTRUCT }
( S -> Collection ) = { LLIST , LDICT }
( Structure -> STARTSTRUCT SItems ENDSTRUCT ) = { STARTSTRUCT }
( SItems -> Collection SItems ) = { LLIST , LDICT }
( SItems ->  ) = { ENDSTRUCT }
( Collection -> List ) = { LLIST }
( Collection -> Map ) = { LDICT }
( List -> LLIST LItems RLIST ) = { LLIST }
( LItems -> Item COMMA LItems ) = { LLIST , LDICT , TRUE , FALSE , INTEGER , REAL , STRING , VAL }
( LItems ->  ) = { RLIST }
( Map -> LDICT MItems RDICT ) = { LDICT }
( MItems -> Key KEY Item COMMA MItems ) = { COMPLEXKEY , VAL }
( MItems ->  ) = { RDICT }
( Key -> VAL ) = { VAL }
( Key -> COMPLEXKEY Collection ) = { COMPLEXKEY }
( Item -> STRING ) = { STRING }
( Item -> INTEGER ) = { INTEGER }
( Item -> REAL ) = { REAL }
( Item -> Collection ) = { LLIST , LDICT }
( Item -> TRUE ) = { TRUE }
( Item -> FALSE ) = { FALSE }
( Item -> VAL ) = { VAL }

