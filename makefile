BISONFlAGS = -d -v
CFLAGS = -Wall

cpq:	cpq.o lex.yy.o parser.tab.o quad1.o quad2.o
	g++ $(CFLAFS) -o cpq cpq.o lex.yy.o parser.tab.o quad1.o quad2.o -lfl
	
cpq.o:	cpq.c parser.tab.h quad1.h
	g++ $(CFLAGS) -c cpq.c
	
lex.yy.o:	lex.yy.c parser.tab.h quad2.h
	g++ -g -c lex.yy.c
	
lex.yy.c:	lexer.l parser.tab.h
	flex lexer.l
	
parser.tab.o:	parser.tab.c parser.tab.h quad1.h
	g++ $(CFLAGS) -c parser.tab.c
	
parser.tab.c:	parser.y
	bison $(BISONFlAGS) parser.y
	
parser.tab.h:	parser.y
	bison $(BISONFlAGS) parser.y
	
quad1.o:	quad1.c quad1.h quad2.h
	g++ $(CFLAGS) -c quad1.c

quad2.o:	quad2.c quad1.h quad2.h
	g++ $(CFLAGS) -c quad2.c

