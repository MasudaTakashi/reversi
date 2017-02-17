CXX = g++
TARGET = reversi
CXXFLAGS = -Wall -g -O3
OBJS = main.o move.o

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS)

clean:
	rm -f $(TARGET) $(OBJS) 
