// Seed operations

typedef struct {
	unsigned color; // = ZERO_COLOR;
	double weight; // = DEFAULT_SEED_WEIGHT;
} Rasteron_Seed;

#define MAX_COLOR_SEEDS 2046
#define DEFAULT_SEED_WEIGHT 0.0

typedef struct {
    Rasteron_Seed seeds[MAX_COLOR_SEEDS];
	unsigned seedCount; // = 0;
} Rasteron_SeedTable;

void addSeed(Rasteron_SeedTable* table, unsigned color);
void addWeightedSeed(Rasteron_SeedTable* table, unsigned color, double weight);