CC = gcc
CFLAGS = 

SRC_DIR = src
OBJECT_DIR = obj


OUT_CLIENT = client
OUT_SERVER = server

VPATH = $(SRC_DIR)


all: $(OUT_CLIENT) $(OUT_SERVER)

$(OUT_CLIENT): $(OBJECT_DIR)/client.o $(OBJECT_DIR)/logger.o $(OBJECT_DIR)/utils.o
	$(CC) $(CFLAGS) $^ -o $@

$(OUT_SERVER): $(OBJECT_DIR)/server.o $(OBJECT_DIR)/logger.o $(OBJECT_DIR)/utils.o
	$(CC) $(CFLAGS) $^ -o $@



$(OBJECT_DIR)/client.o: $(SRC_DIR)/client.c $(SRC_DIR)/logger.h $(SRC_DIR)/types.h $(SRC_DIR)/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_DIR)/server.o: $(SRC_DIR)/server.c $(SRC_DIR)/logger.h $(SRC_DIR)/types.h $(SRC_DIR)/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_DIR)/logger.o: $(SRC_DIR)/logger.c $(SRC_DIR)/logger.h $(SRC_DIR)/ansi_colors.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_DIR)/utils.o: $(SRC_DIR)/utils.c $(SRC_DIR)/utils.h $(SRC_DIR)/ansi_colors.h $(SRC_DIR)/logger.h
	$(CC) $(CFLAGS) -c $< -o $@


$(OBJECT_DIR): ; mkdir -p $@


.PHONY: all clean

clean:
	rm -f $(OBJECT_DIR)/*.o $(BUILD_DIR)/*.exe
