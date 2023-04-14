CXX = g++
CXXFLAGS = -std=c++11

SRC_DIR = src
OBJ_DIR = obj


OUT_CAS = ClubsAgeStats.out
OUT_COUNTRY = country.out
OUT_CLUB = club.out
OUT_POS = position.out

VPATH = $(SRC_DIR)


all: $(OUT_CAS) $(OUT_COUNTRY) $(OUT_CLUB) $(OUT_POS)

$(OUT_CAS): $(OBJ_DIR)/ClubsAgeStats.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OUT_COUNTRY): $(OBJ_DIR)/country.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OUT_CLUB): $(OBJ_DIR)/club.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OUT_POS): $(OBJ_DIR)/position.o
	$(CXX) $(CXXFLAGS) $^ -o $@



$(OBJ_DIR)/ClubsAgeStats.o: $(SRC_DIR)/main.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/country.o: $(SRC_DIR)/country.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/club.o: $(SRC_DIR)/club.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/position.o: $(SRC_DIR)/position.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/logger.o: $(SRC_DIR)/logger.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/csv.o: $(SRC_DIR)/csv.c
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)



.PHONY: all clean

clean:
	rm -f $(OBJ_DIR)/*.o ./*.out
