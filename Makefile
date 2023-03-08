CC = g++
CFLAGS  = -g -Wall
TARGET = VideoToAscii

all: $(TARGET)

$(TARGET): Image.o Main.cpp 
		$(CC) $(CFLAGS) -o $(TARGET) Main.cpp Image.o

Image.o: Image.cpp Image.h
	$(CC) $(CFLAGS) -c Image.cpp -I./Include

clean:
	del *.o *~ $(TARGET).exe

run:
	./$(TARGET)

