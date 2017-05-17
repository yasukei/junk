ANTLR=java -jar /usr/local/lib/antlr-4.7-complete.jar
JAVAC=javac
GRUN=java org.antlr.v4.gui.TestRig

all:
	$(ANTLR) Expr.g4
	$(JAVAC) Expr*.java
	$(GRUN) Expr prog -gui

clean:
	rm -f *.java *.class *.tokens
