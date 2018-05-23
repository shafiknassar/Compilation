Follow:
S = { EOF }
Structure = { EOF }
SItems = { ENDSTRUCT }
Collection = { ENDSTRUCT , LLIST , LDICT , KEY , COMMA , EOF }
List = { ENDSTRUCT , LLIST , LDICT , KEY , COMMA , EOF }
LItems = { RLIST }
Map = { ENDSTRUCT , LLIST , LDICT , KEY , COMMA , EOF }
MItems = { RDICT }
Key = { KEY }
Item = { COMMA }

