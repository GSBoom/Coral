CXX = g++
CXXFLAGS = -O2 -std=c++17 -fno-omit-frame-pointer
SRC_DIR = src
CORAL_DIR = coral

# Auxo/GSS common objects (OPENMP disabled for clean cache measurement)
COMMON_OBJS = $(SRC_DIR)/Auxo.o $(SRC_DIR)/AuxoPro.o $(SRC_DIR)/GSS_Chain.o $(SRC_DIR)/hashFunction.o

.PHONY: all clean

all: test_auxo_cache test_gss_cache test_coral_cache test_coral_mw4_cache test_auxopro_migrate gen_queries

# --- Auxo cache test ---
$(SRC_DIR)/Auxo.o: $(SRC_DIR)/Auxo.cpp $(SRC_DIR)/querysupportstruct.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I$(SRC_DIR)

$(SRC_DIR)/AuxoPro.o: $(SRC_DIR)/AuxoPro.cpp $(SRC_DIR)/querysupportstruct.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I$(SRC_DIR)

$(SRC_DIR)/GSS_Chain.o: $(SRC_DIR)/GSS_Chain.cpp $(SRC_DIR)/querysupportstruct.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I$(SRC_DIR)

$(SRC_DIR)/hashFunction.o: $(SRC_DIR)/hashFunction.cpp $(SRC_DIR)/querysupportstruct.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I$(SRC_DIR)

test_auxo_cache: $(COMMON_OBJS) $(SRC_DIR)/test_auxo_cache.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(COMMON_OBJS) $(SRC_DIR)/test_auxo_cache.cpp -I$(SRC_DIR)

# --- GSS cache test ---
test_gss_cache: $(COMMON_OBJS) $(SRC_DIR)/test_gss_cache.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(COMMON_OBJS) $(SRC_DIR)/test_gss_cache.cpp -I$(SRC_DIR)

# --- Coral cache test (header-only, single compilation unit) ---
test_coral_cache: $(CORAL_DIR)/test_coral_cache.cpp $(CORAL_DIR)/Coral.h $(CORAL_DIR)/hashfunctions.h
	$(CXX) $(CXXFLAGS) -o $@ $(CORAL_DIR)/test_coral_cache.cpp -I$(CORAL_DIR)

# --- Query generator ---
gen_queries: $(SRC_DIR)/gen_queries.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(SRC_DIR)/gen_queries.cpp -I$(SRC_DIR)

# --- Coral MW4 (MatrixWidth=4, 16 cells/basket) ---
test_coral_mw4_cache: coral_mw4/test_coral_cache.cpp coral_mw4/Coral.h coral_mw4/hashfunctions.h
	$(CXX) $(CXXFLAGS) -o $@ coral_mw4/test_coral_cache.cpp -Icoral_mw4

test_auxopro_migrate: $(COMMON_OBJS) $(SRC_DIR)/test_auxopro_migrate.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(COMMON_OBJS) $(SRC_DIR)/test_auxopro_migrate.cpp -I$(SRC_DIR)

clean:
	rm -f $(SRC_DIR)/*.o test_auxo_cache test_gss_cache test_coral_cache test_coral_mw4_cache gen_queries
