CC = g++
CPPFLAGS = -g
LINK = -lsfml-graphics -lsfml-window -lsfml-system
VPATH = src/
OUT = bin/
SRCS = main.cpp game.cpp entity.cpp map.cpp

include $(SRCS:.cpp=.d)

.DEFAULT_GOAL := nozoki

nozoki: $(SRCS:.cpp=.o)
	$(CC) $(CPPFLAGS) $(LINK) -o $(OUT)$@ $^ 

%.o : %.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $<

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	rm -rf bin/nozoki \
	rm -rf *.o
